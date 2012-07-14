#!/usr/bin/env python
from core.ns3.netns3 import *
from core.BaseNode import BaseNode
from core.real import PhyNode
import sys

# class ImalseNetnsNode(NetnsNode, BaseNode):
class ImalseNetnsNode(NetnsNode, PhyNode):
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
        # self.node.stop_app(sock, 'ping')

nodenum = 0
import ns3
class ImalseNetnsSimNode(ns3.Node, BaseNode):
    """Simulated Node in NS3 Adapted for Imalse Simulation"""
    proto_map = {'udp':ns3.UdpSocketFactory.GetTypeId(), 'tcp':ns3.TcpSocketFactory.GetTypeId()}
    NODE_TYPE = 'sim_ns3'
    def __init__(self, *args, **kwargs):
        global nodenum
        name = "sim_n%s" %(nodenum)
        ns3.Node.__init__(self)
        nodenum += 1

    def create_sock(self, desc):
        sock = ns3.Socket.CreateSocket(self, self.proto_map[desc['proto']])
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
        # sock.Bind(dst);
        def BindSock(sock, dst):
            print 'BindSock'
            sock.Bind(dst)

        self.after(0, BindSock, sock, dst)

        def srcSocketRecv (socket):
            _from = Address()
            packet = socket.RecvFrom (_from)
            packet.RemoveAllPacketTags ()
            packet.RemoveAllByteTags ()
            if (socket.GetBoundNetDevice ()):
                ns3.NS_LOG_INFO ("Socket was bound")
            else:
                ns3.NS_LOG_INFO ("Socket was not bound")

        sock.SetRecvCallback (srcSocketRecv);
        # def socket_recv(sock):
            # print 'socket_recv called '
            # pass

        # sock.SetRecvCallback (socket_recv);
        # print 'RecvCallBack has ben set'

    def listen(self, sock, backlog):
        print 'start to listen'
        def ListenSock(sock):
            sock.Listen()
        self.after(0, ListenSock, sock)
        # sock.Listen()

    def accept(self, sock):
        return sock, None

    def recv(self, sock, bufsize, dispatcher=None, threaded=False):
        print 'has set dispatcher'
        sock.SetRecvCallback(dispatcher)
        print 'finish set dispatcher'
        def SockRecv(sock):
            sock.Recv(bufsize, 0)
        self.after(0, SockRecv, sock)
        # data = sock.Recv(bufsize, 0) #FIXME, flag set to be 0, don't know the meaning

    def connect(self, sock, addr_port):
        """Will set Connect callback function. If succeeded, self.recv will be called. otherwise
        the sock will be closed"""

        # print 'addr_port, ', addr_port
        inetAddr = ns3.InetSocketAddress(
                # ns3.Ipv4Address(addr_port[0]),
                ns3.Ipv4Address("10.1.1.1"),
                addr_port[1]
                )
        def connect_succeeded(sock):
            print 'connect_succeded'
            data = self.recv(sock, 512, self.cmd_set.dispatcher)

        def connect_failed(sock):
            print 'connect_failed'
            self.close_sock(sock)

        sock.SetConnectCallback(connect_succeeded, connect_failed)
        print 'server Address is, ', self.server_addr_set[0].GetLocal()
        def SockConnect(self, sock):
            sock.Connect(inetAddr)

        self.after(0, SockConnect, self, sock)

        def WriteUntilBufferFull (localSocket, txSpace):
            print 'WriteUntilBufferFull'
            while (currentTxBytes < totalTxBytes and localSocket.GetTxAvailable () > 0):
                left = totalTxBytes - currentTxBytes
                dataOffset = currentTxBytes % writeSize
                toWrite = writeSize - dataOffset
                toWrite = min (toWrite, left)
                toWrite = min (toWrite, localSocket.GetTxAvailable ())
                amountSent = localSocket.Send (data[dataOffset], toWrite, 0)
                if(amountSent < 0):
                    # we will be called again when new tx space becomes available.
                    return;
                currentTxBytes += amountSent
            localSocket.Close ()
        # sock.SetSendCallback (WriteUntilBufferFull)

        def SendStuff (sock, dstaddr, port):
            print 'Send some packets out'
            p = ns3.Packet()
            p.AddPaddingAtEnd (100);
            r = sock.SendTo (p, 0, ns3.InetSocketAddress(dstaddr,port));
            print 'r, ', r
            return;
        self.after(0, SendStuff, sock, self.server_addr_set[0].GetLocal(), 3333)


    def send(self, sock, data):
        sock.Send()



