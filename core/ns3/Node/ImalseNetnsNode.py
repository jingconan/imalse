import sys

from core.real import PhyNode
from core.ns3.netns3 import NetnsNode
class ImalseNetnsNode(NetnsNode, PhyNode):
    """Imalse Netns Node, it will use linux namespace to construct virtual machine.
    And applications will run in these virtual machines.
    """
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


