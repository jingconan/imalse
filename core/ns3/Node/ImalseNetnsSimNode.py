from core.BaseNode import BaseNode
import ns3

nodenum = 0

#############################################
# NS3 python binding missing some important
# APIs, MSG_MAP and MSG_RE_MAP are used
# to hack the API. Need to be fixed later
#############################################
## Been Fixed at[2012-08-08 23:20:52], MSG_MAP and MSG_RE_MAP now becomes useless.

MSG_MAP = {
        'connect_ack': 21,
        '{"password": ["1234"], "event": ["verify_master"]}':122,
        '{"msg": ["verifed, hi master, what you want to do?"], "event": ["echo"]}':123,
        '{"hostname": ["127.0.0.1"], "event": ["forward_to_bots"], "bot_event": ["send_ping"]}':124,
        '{"event": ["file_exfiltration"]}':125,
        '{"host": "thales.bu.edu", "password": "imalse", "user": "imalse-ftp", "event": "set_ftp_info"}':126,
        '{"directory": ".", "pattern": "assword", "suffix": [".txt"], "event": "set_file_filter"}':127,
        '{"event": "search_and_upload"}':128,
        '{"hostname": ["127.0.0.1"], "event": ["send_ping"]}':129,
        }
MSG_RE_MAP = dict( [ (v,k) for k,v in MSG_MAP.iteritems() ] )

def call_method(method, *args, **kwargs):
    method(*args, **kwargs)

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
    def __init__(self, name=None, *args, **kwargs):
        BaseNode.__init__(self)
        if name is None:
            global nodenum
            self.name = "sim_n%s" %(nodenum)
            nodenum += 1
        else:
            self.name = name

        ns3.Node.__init__(self)
        self.sockets = SocketDict()
        self.sleep_delay = 0

    @property
    def client_socks(self):
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
        self.logger.debug('Node [%s] start to bind to %s'%(self.name, addr_port) )
        addr = self._search_server_addr(addr_port[0])
        # import pdb;pdb.set_trace()
        dst = ns3.InetSocketAddress (addr, addr_port[1])
        # dst = ns3.InetSocketAddress (ns3.Ipv4Address('10.0.1.1'), addr_port[1])
        sock.Bind(dst);
        # sock.Bind();

    def _search_server_addr(self, addr):
        if addr:
            return ns3.Ipv4Address(addr)
        else:
            return self.server_addr_set[0].GetLocal()

    def listen(self, sock, backlog):
        self.logger.debug('Node [%s] start to listen'%(self.name) )
        sock.Listen()

    def recv(self, sock, bufsize, dispatcher=None, threaded=False):
        # print('Node [%s] has set recv dispatcher [%s] for sock [%s]'%(self.name, dispatcher, str(sock)) )
        self.logger.debug('Node [%s] has set recv dispatcher [%s] for sock [%s]'%(self.name, dispatcher, str(sock)) )
        sock.SetRecvCallback(dispatcher)
        self.logger.debug('Node [%s] has finish set recv dispatcher for sock [%s]'%(self.name, str(sock)) )
        sock.Recv()
        self.logger.debug('sock.Recv finished')
        # sock.Recv(bufsize, 0)

    def dispatcher(self, sock):
        """node level dispatcher, it will read data from the socket,
        parse the data into approariate format and handle the data to
        cmd.dispatcher()
        """
        print 'dispatcher'
#FIXME this function was not exculated in pure simulation mode
        # if self.name == "sim_n%s" %(2):
            # import pdb;pdb.set_trace()

        _from = ns3.Address()
        packet = sock.RecvFrom (_from)
        # import pdb;pdb.set_trace()
        try:
            msg = self.get_msg(packet)
        except:
            self.logger.error('Probably you are not using NS3 with imalse customaization?' +
                    ' Use deprec API will suppport only very limitted msgs')
            msg = self.get_msg_deprec(packet)

        self.logger.debug('Node [%s] has receive message %s from sock [%s] and node [%s]'%(self.name, msg, sock, _from) )

        if msg == 'connect_ack':
            self.logger.debug('Node [%s] call self.recv_ack s'%(self.name) )
            self.cmd_set.recv_ack()
        else:
            self.logger.debug('Node [%s] call cmd_set.dispatcher'%(self.name) )
            self.cmd_set.dispatcher(sock, msg)

        # self.recv(sock, 512, self.dispatcher)
        self.after(0.1, self.recv, sock, 512, self.dispatcher)
        # self.after(0, self.recv, sock, 512, self.dispatcher)

    def sleep(self, t, call_back=None):
        if call_back:
            self.after(t, call_back)
            return
        self.sleep_delay = t #FIXME Just to Make Botmaster API Unchanged

    def connect(self, sock, addr_port):
        """Will set Connect callback function. If succeeded, self.recv will be called. otherwise
        the sock will be closed"""
        server_addr = self._search_server_addr(addr_port[0])
        print 'addr_port, ', addr_port
        print 'server_addr, ', server_addr
        print 'serval local, ', self.server_addr_set[0].GetLocal()
        # import pdb;pdb.set_trace()
        assert(str(server_addr) == str(self.server_addr_set[0].GetLocal()))
        # import pdb;pdb.set_trace()

        inetAddr = ns3.InetSocketAddress(
                server_addr,
                # self.server_addr_set[0].GetLocal(), # connect to first server
                addr_port[1]
                )

        def connect_succeeded(sock):
            print 'Node [%s] connect succeeded'%(self.name)
            self.logger.debug('Node [%s] connect succeeded'%(self.name) )
            # self.recv(sock, 512, self.dispatcher)
            self.after(0, self.recv, sock, 512, self.dispatcher)

        def connect_failed(sock):
            self.logger.debug('Node [%s] connect failed'%(self.name) )
            self.close_sock(sock)

        sock.SetConnectCallback(connect_succeeded, connect_failed)
        ret = sock.Connect(inetAddr)
        if ret == -1:
            print 'node [%s] cannot connect to server'%(self.name)
            # raise Exception('node [%s] cannot connect to server'%(self.name) )
        print 'node %s connected to server'%(self.name)

        # self.recv(sock, 512, self.dispatcher)
        # x = inetAddr.GetIpv4()
        # from inspect import getmembers
        # for a, b in getmembers(x): print a, b
        # print sock
        # print ns3.InetSocketAddress.IsMatchingType(inetAddr)
        # ns3.TcpSocketFactory.GetTypeId()
        # import pdb;pdb.set_trace()
        # self.after(0, sock.Connect, inetAddr)
        # import pdb;pdb.set_trace()

    @staticmethod
    def get_msg(p):
        """get message from the pacetk"""
        h = ns3.ImalseHeader()
        p.RemoveHeader(h)
        msg = h.GetData()
        return msg

    @staticmethod
    def add_msg(p, msg):
        """add message to pacekt"""
        h = ns3.ImalseHeader()
        h.SetData(msg)
        p.AddHeader(h)
        return p

    @staticmethod
    def get_msg_deprec(p):
        """get_msg and add_msg are two hack function"""
        # import pdb;pdb.set_trace()
        return MSG_RE_MAP[p.GetSize()]
        # return MSG_RE_MAP[21]

    @staticmethod
    def add_msg_deprec(p, msg):
        """encode the message to the packet"""
        msg_id = MSG_MAP.get(msg, None) # FIXME use padding length to present msg, a wordround for python bug
        if not msg_id:
            raise Exception('Unknown Message %s'%(msg))
        # p.AddPaddingAtEnd(msg_id)
        # FIXME due to the bug of pyviz, we cannot AddPading to existing packets, instead
        # a new packet is created
        p2 = ns3.Packet(msg_id)
        return p2

    def send(self, sock, data):
        self.logger.debug('Node [%s] send data [%s] on socket [%s]'%(self.name, str(data), str(sock)) )
        p = ns3.Packet()
        try:
            p = self.add_msg(p, data)
        except:
            self.logger.error('Probably you are not using NS3 with imalse customaization?' +
                    ' Use deprec API will suppport only very limitted msgs')
            p = self.add_msg_deprec(p, data)

        self.after(self.sleep_delay, sock.Send, p)
        # self.after(self.sleep_delay, self.send, sock, data)
        # value = sock.Send(p)
        # self.logger.debug('Node [%s] send out [%i] bytes'%(self.name, value) )
        # if value == -1:
            # raise Exception('Fail in Send traffic')
        self.sleep_delay = 0

    def stop(self):
        pass

    ####################################
    ##    File System              #####
    ####################################
    def get_file_list(self, *args, **kwargs):
        return ['pesudo_file.txt']

    def load_file(self, *args, **kwargs):
        return 'password'


    def ftp_upload(self, f, host, user, password):
        print 'File [%s] has been uploaded to ftp %s'%(f, host)

    ####################################
    ##    File System              #####
    ####################################
    def ping(self, sock, data, threaded):
        """The attributes you can customize

        **StartTime**
        **StopTime**
        **Remote**: The address of the machine we want to ping.
        **Verbose**: Produce usual output.
        **Interval**: Wait interval seconds between sending each packet.
        **Size**: The number of data bytes to be sent, real packet will
                be 8 (ICMP) + 20 (IP) bytes longer.
        """
        # print 'ImalseNetnsSimNode.ping: this is just a stub, please finish it'
        print 'data, ', data
        print 'here, '
        remote = data.get('remote')[0]
        remote_addr = ns3.Ipv4Address(remote)

        helper = ns3.V4PingHelper(remote_addr)

        start_time = data.get('start_time')[0]
        stop_time = data.get('stop_time')[0]
        helper.SetAttribute("StartTime", ns3.TimeValue(ns3.Time(str(start_time)+'s')))
        helper.SetAttribute("StopTime", ns3.TimeValue(ns3.Time(str(stop_time)+'s')))

       # helper.SetAttribute("Remote", ns3.AddressValue(remote_inet))

        # interval = data.get('Interval')
        # helper.SetAttribute("Interval", ns3.TimeValue(str(interval) + 's'))
        helper.Install(self)
