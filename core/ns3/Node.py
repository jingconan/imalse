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
        }
MSG_RE_MAP = dict( [ (v,k) for k,v in MSG_MAP.iteritems() ] )

class SocketDict(object):
    def __init__(self):
        self.data = dict()

    def hash(self, sock):
        node = sock.GetNode()
        typeId = sock.GetTypeId()
        return hash((node.GetSystemId(), typeId.GetName()))

    def __getitem__(self, sock):
        return self.data[self.hash(sock)]

    def __setitem__(self, sock, value):
        self.data[self.hash(sock)] = value


class ImalseNetnsSimNode(ns3.Node, BaseNode):
    """Simulated Node in NS3 Adapted for Imalse Simulation"""
    proto_map = {
            'udp':ns3.UdpSocketFactory.GetTypeId(),
            'tcp':ns3.TcpSocketFactory.GetTypeId(),
            }
    NODE_TYPE = 'sim_ns3'
    def __init__(self, *args, **kwargs):
        global nodenum
        name = "sim_n%s" %(nodenum)
        ns3.Node.__init__(self)
        nodenum += 1
        self.sockets = SocketDict()

    def create_sock(self, desc):
        sock = ns3.Socket.CreateSocket(self, self.proto_map[desc['proto']])
        print 'create sock, ', sock
        self.sockets[sock] = desc
        return sock

    def after(self, t, method, *args, **kwargs):
        """schedue a even after t seconds"""
        if t == 0:
            return ns3.Simulator.ScheduleNow(method, *args, **kwargs)
        return ns3.Simulator.Schedule(ns3.Simulator.Now()+ns3.Seconds(t),
                method,
                *args,
                **kwargs)

    def bind(self, sock, addr_port):
        print 'start to bind'
        print 'addr_port, ', addr_port
        # dstaddr = ns3.Ipv4Address (addr_port[0])
        dstaddr = ns3.Ipv4Address ("10.1.1.1")
        dst = ns3.InetSocketAddress (dstaddr, addr_port[1])
        sock.Bind(dst);

    def listen(self, sock, backlog):
        print 'start to listen'
        sock.Listen()

    def recv(self, sock, bufsize, dispatcher=None, threaded=False):
        print 'has set dispatcher'
        sock.SetRecvCallback(dispatcher)
        print 'finish set dispatcher'
        sock.Recv(bufsize, 0)

    def dispatcher(self, sock):
        _from = ns3.Address()
        packet = sock.RecvFrom (_from)
        msg = self.get_msg(packet)
        print 'receive message, ', msg

        if msg == 'connect_ack':
            print 'call cmd_set.recv_ack'
            self.cmd_set.recv_ack()
            return
        print 'cmd_set dispatcher wil be called'
        self.cmd_set.dispatcher(sock, msg)

        # print_members(packet)
        # packet.RemoveAllPacketTags ()
        # packet.RemoveAllByteTags ()
        # print 'has recive, size[%i]'%(packet.GetSize())
        # msg_id = packet.GetSize() #FIXME
        # print 'the message is: ', MSG_RE_MAP[msg_id]


        # pt = packet.GetPacketTagIterator()
        # print 'get tag, ', pt
        # item = pt.Next()
        # print 'item ', item
        # print 'item tag, ', item.GetTypeId()
        # print_members(pt.Item)

    def connect(self, sock, addr_port):
        """Will set Connect callback function. If succeeded, self.recv will be called. otherwise
        the sock will be closed"""

        # print 'addr_port, ', addr_port
        print 'connect to server [%s]'%(self.server_addr_set[0].GetLocal())
        inetAddr = ns3.InetSocketAddress(
                # ns3.Ipv4Address(addr_port[0]),
                # ns3.Ipv4Address("10.1.1.1"),
                self.server_addr_set[0].GetLocal(),
                addr_port[1]
                )

        def connect_succeeded(sock):
            print 'connect succeeded'
            data = self.recv(sock, 512, self.dispatcher)

        def connect_failed(sock):
            print 'connect_failed'
            self.close_sock(sock)

        sock.SetConnectCallback(connect_succeeded, connect_failed)
        sock.Connect(inetAddr)

    @staticmethod
    def get_msg(p):
        """get_msg and add_msg are two hack function"""
        return MSG_RE_MAP[p.GetSize()]

    @staticmethod
    def add_msg(p, msg):
        msg_id = MSG_MAP.get(msg, None) # FIXME use padding length to present msg, a wordround for python bug
        if not msg_id:
            raise Exception('Unknown Message ')
        p.AddPaddingAtEnd(msg_id)
        return p
        # if data == "connect_ack":
            # tag = ConnectACKTag()
        # p.AddPacketTag(tag)

    def send(self, sock, data):
        print 'data, ', data
        print 'send function is called'
        p = ns3.Packet()
        p = self.add_msg(p, data)
        r = sock.Send(p)
        print 'send status, ', r

    def stop(self):
        pass

