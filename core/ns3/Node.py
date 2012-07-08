#!/usr/bin/env python
from core.ns3.netns3 import *
from core.BaseNode import BaseNode
from core.real import PhyNode

# class ImalseNetnsNode(NetnsNode, BaseNode):
class ImalseNetnsNode(NetnsNode, PhyNode):
    def __init__(self, *args, **kwargs):
        self.cmd_set = None
        NetnsNode.__init__(self, *args, **kwargs)
        PhyNode.__init__(self)

    def ping(self, sock, data, threaded=False):
        dst = data['hostname']
        count = 3
        cmd = ["ping", "-b", "-c", str(count), str(dst)]
        print "%s: ping: %s" % (self.name, " ".join(cmd))
        p, stdin, stdout, stderr = self.popen(cmd)
        err = stderr.read()
        out = stdout.read()
        stderr.write(err)
        stdout.write(out)

    def stop_ping(self, sock):
        print 'stop_ping'
        # self.node.stop_app(sock, 'ping')

nodenum = 0
from ns.network import Socket
from ns.internet import TcpSocketFactory
from ns.core import TypeId
class ImalseNetnsSimNode(NetnsNode, BaseNode):
    def __init__(self, *args, **kwargs):
        global nodenum
        name = "sim_n%s" %(nodenum)
        NetnsNode.__init__(self, name, logfile = "/tmp/%s.log" % name)
        nodenum += 1

    def create_sock(self, desc):
        sock = Socket.CreateSocket(self, TcpSocketFactory.GetTypeId())
        print 'sock, ', sock
        import pdb;pdb.set_trace()
