# Miscellaneous helpers

import ns.core
import ns.csma
import ns.internet
import ns.mobility
import ns.network
import ns.tap_bridge
import ns.wifi

import optparse
from netnsbase import Netns
from threading import Thread

ns.core.GlobalValue.Bind("SimulatorImplementationType",
                         ns.core.StringValue("ns3::RealtimeSimulatorImpl"))

ns.core.GlobalValue.Bind("ChecksumEnabled", ns.core.BooleanValue("true"))

class Netns3(Netns):

    def addnetif(self, ifname, ipaddrs = [],
                 rename = "eth0", up = True, now = False):
        """\
        Add a network interface to the netns and do basic
        configuration.  By default, the given network interface is
        renamed eth0, given IP address(es), and brought up at when the
        simulation starts.  Waiting until the simulation runs ensures
        tap devices created by ns-3 exist.
        """
        def doaddnetif():
            self.acquire_netif(ifname, rename)
            if rename:
                name = rename
            else:
                name = ifname
            if up:
                self.ifup(name)
            for ipaddr in ipaddrs:
                self.add_ipaddr(name, ipaddr)
        if now:
            doaddnetif()
        else:
            # schedule when simulation starts
            ns.core.Simulator.Schedule(ns.core.Time("0"), doaddnetif)

class NetnsNode(ns.network.Node, Netns3):

    def __init__(self, *args, **kwds):
        ns.network.Node.__init__(self)
        Netns3.__init__(self, *args, **kwds)

def parseprefix(prefixstr):
    tmp = prefixstr.split("/")
    assert len(tmp) == 2, "invalid prefix: %s" % prefixstr
    mask = ns.network.Ipv4Mask("/" + tmp[1])
    addrhelper = \
        ns.internet.Ipv4AddressHelper(ns.network.Ipv4Address(tmp[0]), mask)
    return addrhelper, mask

class NetnsExperiment(object):

    def __init__(self):
        self.nodes = []

    @staticmethod
    def simthread(sec):
        def run():
            ns.core.Simulator.Stop(ns.core.Seconds(sec))
            ns.core.Simulator.Run(signal_check_frequency = -1)
        t = Thread(target = run)
        t.daemon = True
        t.start()
        return t

    @staticmethod
    def event(time, func, *args, **kwds):
        """\
        Spawn a detached thread that calls func with args and kwds.
        Use separate threads for ns-3 callbacks to avoid blocking the
        real-time scheduler.
        """
        def run():
            t = Thread(target = func, args = args, kwargs = kwds)
            t.daemon = True
            t.start()
        ns.core.Simulator.Schedule(ns.core.Time(str(time)), run)

    def createnet(self, devhelper, nodecontainer,
                  addrhelper, mask, ifnames = []):
        tapbridge = ns.tap_bridge.TapBridgeHelper()
        devices = devhelper.Install(nodecontainer)
        for i in xrange(nodecontainer.GetN()):
            n = nodecontainer.Get(i)
            dev = devices.Get(i)
            tap = tapbridge.Install(n, dev)
            tap.SetMode(ns.tap_bridge.TapBridge.CONFIGURE_LOCAL)
            tapname = "ns3tap%d" % i
            tap.SetAttribute("DeviceName", ns.core.StringValue(tapname))
            addr = addrhelper.NewAddress()
            tap.SetAttribute("IpAddress", ns.network.Ipv4AddressValue(addr))
            tap.SetAttribute("Netmask", ns.network.Ipv4MaskValue(mask))
            if ifnames:
                ifname = ifnames[i]
            else:
                ifname = "eth0"
            n.addnetif(tapname,
                       ipaddrs = ["%s/%s" % (addr, mask.GetPrefixLength())],
                       rename = ifname)
            n.ipaddr = str(addr)

    def createnodes(self, numnodes, devhelper, prefix = "10.0.0.0/8",
                    nodenum = 0):
        addrhelper, mask = parseprefix(prefix)
        nc = ns.network.NodeContainer()
        for i in xrange(numnodes):
            name = "n%s" % nodenum
            n = NetnsNode(name, logfile = "/tmp/%s.log" % name)
            self.nodes.append(n)
            nc.Add(n)
            nodenum += 1
        self.createnet(devhelper, nc, addrhelper, mask)

    def run(self):
        self.setup()
        print "running simulator for %s sec" % self.options.simtime
        t = self.simthread(self.options.simtime)
        while True:
            t.join(60.0)
            if not t.isAlive():
                break
        print "simulator done"
        self.cleanup()

    def cleanup(self):
        ns.core.Simulator.Destroy()
        while self.nodes:
            n = self.nodes.pop()
            n.stop()

    def usagestr(self):
        return "[-n <numnodes>] [-t <simtime>]"

    def initparser(self, parser):
        parser.set_defaults(numnodes = 2, simtime = 30)
        parser.add_option("-n", "--numnodes", dest = "numnodes", type = int,
                          help = "number of nodes; default = %s" %
                          parser.defaults["numnodes"])
        parser.add_option("-t", "--simtime", dest = "simtime", type = float,
                          help = "simulation run time; default = %s" %
                          parser.defaults["simtime"])

    def main(self, run = True):
        usage = "%prog [-h] " + self.usagestr() + " ..."
        parser = optparse.OptionParser(usage = usage)
        self.initparser(parser)
        self.options, self.args = parser.parse_args()
        if run:
            self.run()

class CsmaExperiment(NetnsExperiment):

    @staticmethod
    def csmahelper(rate, delay):
        helper = ns.csma.CsmaHelper()
        if rate is not None:
            helper.SetChannelAttribute("DataRate",
                                       ns.core.StringValue(str(rate)))
        if delay is not None:
            helper.SetChannelAttribute("Delay",
                                       ns.core.StringValue(str(delay)))
        return helper

    def initparser(self, parser):
        NetnsExperiment.initparser(self, parser)
        parser.set_defaults(rate = "10Mbps", delay = "1ms")
        parser.add_option("-r", "--rate", dest = "rate",
                          help = "csma channel datarate; default = %s" %
                          parser.defaults["rate"])
        parser.add_option("-d", "--delay", dest = "delay",
                          help = "csma channel delay; default = %s" %
                          parser.defaults["delay"])

    def setup(self):
        devhelper = self.csmahelper(rate = self.options.rate,
                                    delay = self.options.delay)
        self.createnodes(self.options.numnodes, devhelper)

class WifiAdhocExperiment(NetnsExperiment):

    def initparser(self, parser):
        NetnsExperiment.initparser(self, parser)

    def setup(self):
        wifi = ns.wifi.WifiHelper.Default()
        wifi.SetStandard(ns.wifi.WIFI_PHY_STANDARD_80211a)
        wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                     "DataMode",
                                     ns.core.StringValue("OfdmRate54Mbps"))

        wifiMac = ns.wifi.NqosWifiMacHelper.Default()
        wifiMac.SetType("ns3::AdhocWifiMac")

        wifiChannel = ns.wifi.YansWifiChannelHelper.Default()
        wifiPhy = ns.wifi.YansWifiPhyHelper.Default()
        wifiPhy.SetChannel(wifiChannel.Create())

        nodes = ns.network.NodeContainer()
        for i in xrange(self.options.numnodes):
            name = "n%s" % i
            n = NetnsNode(name, logfile = "/tmp/%s.log" % name)
            self.nodes.append(n)
            nodes.Add(n)

        mobility = ns.mobility.MobilityHelper()
        positionAlloc = ns.mobility.ListPositionAllocator()
        for i in xrange(self.options.numnodes):
            positionAlloc.Add(ns.core.Vector(1.0 * i, 0.0, 0.0))
        mobility.SetPositionAllocator(positionAlloc)
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel")
        mobility.Install(nodes)

        devices = wifi.Install(wifiPhy, wifiMac, nodes)

        tapbridge = ns.tap_bridge.TapBridgeHelper()
        addrhelper, mask = parseprefix("10.0.0.0/8")

        for i in xrange(nodes.GetN()):
            n = nodes.Get(i)
            dev = devices.Get(i)
            tap = tapbridge.Install(n, dev)
            tap.SetMode(ns.tap_bridge.TapBridge.CONFIGURE_LOCAL)
            tapname = "ns3tap%d" % i
            tap.SetAttribute("DeviceName", ns.core.StringValue(tapname))
            addr = addrhelper.NewAddress()
            tap.SetAttribute("IpAddress", ns.network.Ipv4AddressValue(addr))
            tap.SetAttribute("Netmask", ns.network.Ipv4MaskValue(mask))

            n.addnetif(tapname,
                       ipaddrs = ["%s/%s" % (addr, mask.GetPrefixLength())],
                       rename = "ath0")
            n.ipaddr = str(addr)
