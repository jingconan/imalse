#!/usr/bin/env python
from core.ns3.netns3 import *
from core.BaseNode import BaseNode
from core.real import PhyNode
import sys

class ImalseNetnsNode(NetnsNode, PhyNode):
    """Imalse Netns Node, it will use linux namespace to construct virtual machine.
    And applications will run in these virtual machines."""
    NODE_TYPE = 'real_ns3'
    def __init__(self, *args, **kwargs):
        self.cmd_set = None
        NetnsNode.__init__(self, *args, **kwargs)
        PhyNode.__init__(self)

    def ping(self, sock, data, threaded=False):
        dst = data['hostname'][0]
        count = 3
        cmd = ["ping", "-b", "-c", str(count), str(dst)]
        print "%s: %s" % (self.name, " ".join(cmd))
        p, stdin, stdout, stderr = self.popen(cmd)
        err = stderr.read()
        out = stdout.read()
        sys.stderr.write(err)
        sys.stdout.write(out)

    def stop_ping(self, sock):
        print 'stop_ping'

nodenum = 0
import ns3
# class ConnectACKTag(ns3.Tag):
#     def __init__(self):
#         pass
#     def GetSerializedSize (self):
#         pass
        # return 0
#     def Serialize (self, tag_buffer):
        # tag_buffer = "CONECT_ACK"
#         pass
#     def Deserialize (self, tag_buffer):
        # self.data = tag_buffer
#         pass
#     def Print (os):
#         pass

#############################################
# NS3 python binding missing some important
# APIs, MSG_MAP and MSG_RE_MAP are used
# to hack the API. Need to be fixed later
#############################################
MSG_MAP = {
        'connect_ack': 121,
        '{"password": ["1234"], "event": ["verify_master"]}':122,
        '{"msg": ["verifed, hi master, what you want to do?"], "event": ["echo"]}':123,
        '{"hostname": ["127.0.0.1"], "event": ["forward_to_bots"], "bot_event": ["send_ping"]}':124,
        '{"event": ["file_exfiltration"]}':125,
        '{"host": "thales.bu.edu", "password": "imalse", "user": "imalse-ftp", "event": "set_ftp_info"}':126,
        '{"directory": ".", "pattern": "assword", "suffix": [".txt"], "event": "set_file_filter"}':127,
        '{"event": "search_and_upload"}':128,
        }
MSG_RE_MAP = dict( [ (v,k) for k,v in MSG_MAP.iteritems() ] )

def call_method(method, *args, **kwargs):
    method(*args, **kwargs)

import itertools
class SocketDict(dict):
    def __init__(self):
        self.sockets = dict()
        super(SocketDict, self).__init__()

    def hash(self, sock):
        node = sock.GetNode()
        typeId = sock.GetTypeId()
        _from = ns3.Address()
        sock.GetSockName(_from)
        return hash((node.GetId(), typeId.GetName(), str(_from), str(sock)))

    def __getitem__(self, sock):
        return super(SocketDict, self).__getitem__(self.hash(sock))

    def __setitem__(self, sock, value):
        super(SocketDict, self).__setitem__(self.hash(sock), value)
        self.sockets[self.hash(sock)] = sock

    def iteritems(self): #FIXME this is not iterator actually
        return zip([self.sockets[k] for k in self.keys()] , self.values())

    def items(self):
        return zip([self.sockets[k] for k in self.keys()] , self.values())


class ImalseNetnsSimNode(ns3.Node, BaseNode):
    """Simulated Node in NS3 Adapted for Imalse Simulation"""
    proto_map = {
            'udp':ns3.UdpSocketFactory.GetTypeId(),
            'tcp':ns3.TcpSocketFactory.GetTypeId(),
            }
    NODE_TYPE = 'sim_ns3'
    name = 'ImalseNetnsSimNode'
    def __init__(self, *args, **kwargs):
        BaseNode.__init__(self)
        global nodenum
        self.name = "sim_n%s" %(nodenum)
        ns3.Node.__init__(self)
        nodenum += 1
        self.sockets = SocketDict()
        self.sleep_delay = 0

    @property
    def client_socks(self):
        # print '----------------------'
        # for sock, v in self.sockets.iteritems():
        #     print 'sock, ', type(sock)
        #     print 'v, ', v
        # print '----------------------'
        return [sock for sock, v in self.sockets.iteritems() if v['type'] == 'client']

    def create_sock(self, desc):
        sock = ns3.Socket.CreateSocket(self, self.proto_map[desc['proto']])
        self.logger.debug('node [%s] create sock [%s] with desc [%s]'%(self.name, str(sock), str(desc)))
        self.sockets[sock] = desc
        return sock

    def after(self, t, method, *args, **kwargs):
        """schedue a even after t seconds"""
        def void_return_method(*args, **kwargs):
            method(*args, **kwargs)
        return ns3.Simulator.Schedule(ns3.Simulator.Now()+ns3.Seconds(t),
                void_return_method,
                *args,
                **kwargs)

    def bind(self, sock, addr_port):
        self.logger.debug('Node [%s] start to bind'%(self.name) )
        addr = self._search_server_addr(addr_port[0])
        dst = ns3.InetSocketAddress (addr, addr_port[1])
        sock.Bind(dst);

    def _search_server_addr(self, addr):
        if addr:
            return ns3.Ipv4Address(addr)
        else:
            return self.server_addr_set[0].GetLocal(),

    def listen(self, sock, backlog):
        self.logger.debug('Node [%s] start to listen'%(self.name) )
        sock.Listen()

    def recv(self, sock, bufsize, dispatcher=None, threaded=False):
        sock.SetRecvCallback(dispatcher)
        self.logger.debug('Node [%s] has set recv dispatcher'%(self.name) )
        sock.Recv(bufsize, 0)

    def dispatcher(self, sock):
        _from = ns3.Address()
        packet = sock.RecvFrom (_from)
        msg = self.get_msg(packet)
        self.logger.debug('Node [%s] has receive message %s'%(self.name, msg) )

        if msg == 'connect_ack':
            self.logger.debug('Node [%s] call self.recv_ack s'%(self.name) )
            self.cmd_set.recv_ack()
            return
        self.logger.debug('Node [%s] call cmd_set.dispatcher'%(self.name) )
        self.cmd_set.dispatcher(sock, msg)

    def sleep(self, t, call_back=None):
        if call_back:
            self.after(t, call_back)
            return
        self.sleep_delay = t #FIXME Just to Make Botmaster API Unchanged

    def connect(self, sock, addr_port):
        """Will set Connect callback function. If succeeded, self.recv will be called. otherwise
        the sock will be closed"""
        # print 'connect to server [%s]'%(self.server_addr_set[0].GetLocal())
        server_addr = self._search_server_addr(addr_port[0])
        # print 'connect to server [%s]'%(server_addr)
        inetAddr = ns3.InetSocketAddress(
                # server_addr,
                self.server_addr_set[0].GetLocal(),
                addr_port[1]
                )

        def connect_succeeded(sock):
            self.logger.debug('Node [%s] connect succeeded'%(self.name) )
            data = self.recv(sock, 512, self.dispatcher)

        def connect_failed(sock):
            self.logger.debug('Node [%s] connect failed'%(self.name) )
            self.close_sock(sock)

        sock.SetConnectCallback(connect_succeeded, connect_failed)
        sock.Connect(inetAddr)

    @staticmethod
    def get_msg(p):
        """get_msg and add_msg are two hack function"""
        return MSG_RE_MAP[p.GetSize()]

    @staticmethod
    def add_msg(p, msg):
        """encode the message to the packet"""
        msg_id = MSG_MAP.get(msg, None) # FIXME use padding length to present msg, a wordround for python bug
        if not msg_id:
            raise Exception('Unknown Message %s'%(msg))
        p.AddPaddingAtEnd(msg_id)
        return p
        # if data == "connect_ack":
            # tag = ConnectACKTag()
        # p.AddPacketTag(tag)

    def send(self, sock, data):
        # print 'data, ', data
        self.logger.debug('Node [%s] send data [%s] on socket [%s]'%(self.name, str(data), str(sock)) )
        p = ns3.Packet()
        p = self.add_msg(p, data)
        # print 'sock.Send', sock.Send
        self.after(self.sleep_delay, sock.Send, p)
        self.sleep_delay = 0

    def stop(self):
        pass

