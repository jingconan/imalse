#!/usr/bin/env python

# Create a N node CSMA network and run an iperf TCP throughput test
# between the first and last nodes.  Multiple tests are run for a
# range of configured CSMA data rates unless a single rate is given
# from the command line.

from netns3 import *
import sys

class IperfCsmaExperiment(CsmaExperiment):

    def iperf(self, server, client, runtime = 20):
        srvcmd = ["iperf", "-s"]
        print "iperf server: %s" % server.ipaddr
        sp = server.spawn(srvcmd)
        clcmd = ["iperf", "-f", "m", "-t", str(runtime), "-c", server.ipaddr]
        print "iperf client: %s: %s" % (client.ipaddr, " ".join(clcmd))
        p, stdin, stdout, stderr = client.popen(clcmd)
        bw = None
        for line in stdout:
            tmp = line.split()
            if len(tmp) == 8:
                bw = " ".join(tmp[-2:])
        p.wait()
        server.kill(sp, 9)
        sp.wait()
        print "iperf done"
        print "csma rate:", self.options.rate
        print "iperf client tcp throughput:", bw

    def setup(self):
        CsmaExperiment.setup(self)
        if len(self.nodes) > 1:
            self.event(1, self.iperf, self.nodes[0], self.nodes[-1])

    def main(self):
        CsmaExperiment.main(self, run = False)
        if len(sys.argv) > 1:
            self.run()
        else:
            rates = map(lambda x: "%sMbps" % x, xrange(1, 11))
            for r in rates:
                self.options.rate = r
                self.run()
                print

if __name__ == "__main__":
    x = IperfCsmaExperiment()
    x.main()
