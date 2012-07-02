#!/usr/bin/env python

# Create a N node CSMA network and broadcast ping from the first node.

from netns3 import *
import sys

class PingCsmaExperiment(CsmaExperiment):

    @staticmethod
    def ping(node, dst, count = 3):
        cmd = ["ping", "-b", "-c", str(count), str(dst)]
        print "%s: ping: %s" % (node.name, " ".join(cmd))
        p, stdin, stdout, stderr = node.popen(cmd)
        err = stderr.read()
        out = stdout.read()
        sys.stderr.write(err)
        sys.stdout.write(out)

    @staticmethod
    def config(node):
        node.cmd(["sysctl", "-w", "net.ipv4.icmp_echo_ignore_broadcasts=0"])
        node.cmd(["ip", "route", "add", "default", "dev", "eth0"])

    def initparser(self, parser):
        CsmaExperiment.initparser(self, parser)
        parser.set_defaults(simtime = 10, pingcount = 3)
        parser.add_option("-c", "--pingcount", dest = "pingcount",
                          help = "ping count; default = %s" %
                          parser.defaults["pingcount"])

    def setup(self):
        CsmaExperiment.setup(self)
        for n in self.nodes:
            self.event(0, self.config, n)
        if self.nodes:
            self.event(1, self.ping, self.nodes[0],
                       "255.255.255.255", self.options.pingcount)


if __name__ == "__main__":
    x = PingCsmaExperiment()
    x.main()
