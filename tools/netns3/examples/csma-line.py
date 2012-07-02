#!/usr/bin/env python

# Create a N node line connected by a series of CSMA networks.  Enable
# IP forwarding, add static routes, and launch xterms for shell access
# to the first and last nodes.

from netns3 import *

class LineCsmaExperiment(CsmaExperiment):

    def initparser(self, parser):
        CsmaExperiment.initparser(self, parser)
        parser.set_defaults(numnodes = 3, simtime = 120)

    def createnodes(self, numnodes, devhelper, nodenum = 0):
        for i in xrange(numnodes):
            name = "n%s" % nodenum
            n = NetnsNode(name, logfile = "/tmp/%s.log" % name)
            self.nodes.append(n)
            nodenum += 1
        if numnodes < 2:
            return
        addrhelper, mask = parseprefix("10.0.0.0/30")
        for i in xrange(numnodes - 1):
            nc = ns3.NodeContainer()
            nc.Add(self.nodes[i])
            nc.Add(self.nodes[i + 1])
            ifnames = ["eth1", "eth0"]
            CsmaExperiment.createnet(self, devhelper, nc,
                                     addrhelper, mask, ifnames)
            addrhelper.NewNetwork()

    def config(self):
        "enable ip forwarding and set static routes"
        print "adding routes"

        numnodes = len(self.nodes)

        for i in xrange(2):
            self.nodes[i].cmd(["sysctl", "-w", "net.ipv4.ip_forward=1"])
            nexthop = "10.0.0.%s" % (4 * i + 2)
            rt = ["ip", "route", "add", "10.0.0.0/8", "via", nexthop]
            self.nodes[i].cmd(rt)

        for i in xrange(2, numnodes):
            self.nodes[i].cmd(["sysctl", "-w", "net.ipv4.ip_forward=1"])

            prevhop = "10.0.0.%s" % (4 * i - 3)
            net = "10.0.0.0"
            addrhelper = ns3.Ipv4AddressHelper(ns3.Ipv4Address(net),
                                               ns3.Ipv4Mask("/30"))
            for j in xrange(i - 1):
                rt = ["ip", "route", "add", "%s/30" % net, "via", prevhop]
                self.nodes[i].cmd(rt)
                net = addrhelper.NewNetwork()

            if i < numnodes - 1:
                nexthop = "10.0.0.%s" % (4 * i + 2)
                rt = ["ip", "route", "add", "10.0.0.0/8", "via", nexthop]
                self.nodes[i].cmd(rt)

        print "done adding routes"

    def setup(self):
        CsmaExperiment.setup(self)
        self.event(0, self.config)
        if len(self.nodes) > 0:
            self.event(0, self.nodes[0].xterm, "bash")
        if len(self.nodes) > 1:
            self.event(0, self.nodes[-1].xterm, "bash")

if __name__ == "__main__":
    x = LineCsmaExperiment()
    x.main()
