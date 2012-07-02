from netns3 import *
# import sys
# sys.path.append("..")
from BaseNode import BaseNode
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


