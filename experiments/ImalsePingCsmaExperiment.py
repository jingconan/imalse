from core.ns3.Experiment import *

class ImalsePingCsmaExperiment(PingCsmaExperiment, ImalseExperiment):
    """Ping Flooding Experiment On a Csma Network"""
    server_id_set = [0]
    botmaster_id_set = [1]
    client_id_set = [2, 3, 4]

    def __init__(self, *argv, **kwargv):
        PingCsmaExperiment.__init__(self, *argv, **kwargv)
        ImalseExperiment.__init__(self)

    def initparser(self, parser):
        CsmaExperiment.initparser(self, parser)
        parser.set_defaults(simtime = 10, pingcount = 3)
        parser.add_option("-c", "--pingcount", dest = "pingcount",
                          help = "ping count; default = %s" %
                          parser.defaults["pingcount"])
        scenario_ops = get_scenario_option()
        parser.add_option('-s', '--scenario', dest="scenario",
                default='None',
                help='specify the scenario you want to execute. Scenearios availiable are: %s'%(scenario_ops),
                )

    def createnodes(self, numnodes, devhelper, prefix = "10.0.0.0/8",
                    nodenum = 0):
        addrhelper, mask = parseprefix(prefix)
        nc = ns.network.NodeContainer()
        for i in xrange(numnodes):
            name = "n%s" % nodenum
            n = ImalseNetnsNode(name, logfile = "/tmp/%s.log" % name)
            self.nodes.append(n)
            nc.Add(n)
            nodenum += 1
        self.createnet(devhelper, nc, addrhelper, mask)

        self._install_cmds()

    def createnet(self, devhelper, nodecontainer,
                  addrhelper, mask, ifnames = []):
        tapbridge = ns.tap_bridge.TapBridgeHelper()
        devices = devhelper.Install(nodecontainer)
        for i in xrange(nodecontainer.GetN()):
            n = nodecontainer.Get(i)
            dev = devices.Get(i)
            # install a TapBridge on the specified Node and forms the
            # bridge with the net devices
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

    def setup(self):
        print 'this is setup of experiment'
        CsmaExperiment.setup(self)
        for n in self.nodes:
            self.event(0, self.config, n)
        if not self.nodes:
            return
        for i in xrange(len(self.nodes)):
            if i in self.id_set:
                print 'node [%i] start at [%f]s'%(i, i)
                self.event(i+1, self.node_run, self.nodes[i], 'start') # start server
            # self.event(1, self.nodes[0], 'start') # start server
            # self.event(1, self.nodes[1], 'start') # start botmaster
            # self.event(1, self.nodes[1], 'start') # start botmaster

        # if self.nodes:
            # self.event(1, self.ping, self.nodes[0],
                       # "255.255.255.255", self.options.pingcount)
