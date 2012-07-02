#!/usr/bin/env python

# Create a N node line connected by a series of CSMA networks.  Enable
# IP forwarding, run OSPFv2 for routing, and launch xterms for shell
# access to all nodes.

from netns3 import *
import os
import subprocess

# these are the default ubuntu/debian paths
QUAGGA_CONF = "/etc/quagga"
QUAGGA_RUN = "/var/run/quagga"
QUAGGA_BIN = "/usr/lib/quagga/"

os.environ["VTYSH_PAGER"] = "more"

class QuaggaLineCsmaExperiment(CsmaExperiment):

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
            nc = ns.network.NodeContainer()
            nc.Add(self.nodes[i])
            nc.Add(self.nodes[i + 1])
            ifnames = ["eth1", "eth0"]
            CsmaExperiment.createnet(self, devhelper, nc,
                                     addrhelper, mask, ifnames)
            addrhelper.NewNetwork()

    def create_zebra_conf(self, dirname):
        fname = os.path.join(dirname, "zebra.conf")
        # print "creating", fname
        f = open(fname, "w")
        f.write("ip forwarding\n")
        f.close()

    def create_ospfd_conf(self, dirname, netprefixes):
        fname = os.path.join(dirname, "ospfd.conf")
        # print "creating", fname
        f = open(fname, "w")
        f.write("router ospf\n")
        for p in netprefixes:
            f.write("  network %s area 0\n" % p)
        f.close()

    def config(self):
        "set up quagga routing"
        print "configuring and running zebra and ospfd"

        net = "10.0.0.0"
        addrhelper = ns.internet.Ipv4AddressHelper(ns.network.Ipv4Address(net),
                                                   ns.network.Ipv4Mask("/30"))
        for n in self.nodes:
            rundirname = n.netnsdir(QUAGGA_RUN)
            subprocess.check_call(["chown", "quagga:quagga", rundirname])

            confdirname = n.netnsdir(QUAGGA_CONF)
            subprocess.check_call(["chown", "quagga:quagga", confdirname])

            self.create_zebra_conf(confdirname)
            n.cmd([os.path.join(QUAGGA_BIN, "zebra"), "-d"])

            netlist = ["%s/30" % net]
            if n != self.nodes[0] and n != self.nodes[-1]:
                net = addrhelper.NewNetwork()
                netlist.append("%s/30" % net)
            self.create_ospfd_conf(confdirname, netlist)
            n.cmd([os.path.join(QUAGGA_BIN, "ospfd"), "-d"])

        print "done configuring and running zebra and ospfd"

    def setup(self):
        CsmaExperiment.setup(self)
        self.event(0, self.config)
        for n in self.nodes:
            self.event(0, n.xterm, "bash")

if __name__ == "__main__":
    x = QuaggaLineCsmaExperiment()
    x.main()
