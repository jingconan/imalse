from core.ns3.Experiment import *
from core.ns3.Topology import TopologyNet
import os
NODE_NUM = 0
class ImalseTopoSimExperiment(ImalseExperiment):
    """This is pure ns-3 topology Experiment without emulated node"""

    def initparser(self, parser):
        super(ImalseTopoSimExperiment, self).initparser(parser)
        parser.set_defaults(topology_file="./net_config/Inet_small_toposample.txt",
                topology_type = 'Inet',
                )
        parser.add_option("-f", "--topology_file", dest = "topology_file",
                          help = "topology file name, default = %s" %(parser.defaults["topology_file"]))
        parser.add_option('-p', '--topology_type', dest="topology_type",
                help='type of topology file',
                )
        scenario_ops = get_scenario_option()
        parser.add_option('-s', '--scenario', dest="scenario",
                default='None',
                help='specify the scenario you want to execute. Scenearios availiable are: %s'%(scenario_ops),
                )


    def get_node(self, i):
        return self.net.nodes.Get(i)
    @property
    def node_num(self):
        return self.net.nodes.GetN()

    def setup(self):
        def ImalseNetnsNodeCreator():
            global NODE_NUM
            NODE_NUM += 1
            name = "n%s" %NODE_NUM
            return ImalseNetnsNode(name, logfile = "/tmp/%s.log" % name)

        scen = load_module(self.options.scenario)
        cmd = scen.ClientCMD()
        self.net = TopologyNet(os.path.abspath(self.options.topology_file),
                self.options.topology_type,
                # ImalseNetnsNodeCreator)
                ImalseNetnsSimNode,
                # ipv4AddrBase=cmd.srv_addr,
                ipv4Mask="255.255.255.0",
                ipv4AddrBase="10.0.0.0",
                # ipv4Mask="255.255.255.0"
                )
        self._install_cmds()
        # self.print_srv_addr()
        self._set_server_info()

        if self.node_num == 0: return
        # for i in xrange(self.node_num):
            # print 'node [%i] will start at [%f]s'%(i, i)
            # self.event(i+1, self.node_run, self.get_node(i), 'start')
        # start server
        for i in self.server_id_set:
            self.event(0, self.node_run, self.get_node(i), 'start')

        # start clients
        t = 0
        for i in self.client_id_set:
            t += 1
            self.event(t, self.node_run, self.get_node(i), 'start')

        # start botmaster
        # for i in self.botmaster_id_set:
            # self.event(t+1, self.node_run, self.get_node(i), 'start')


class ImalsePingCsmaExperiment(PingCsmaExperiment, ImalseExperiment):
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

    def create_topology(self):
        pass

    def setup(self):
        print 'this is setup of experiment'
        CsmaExperiment.setup(self)
        for n in self.nodes:
            self.event(0, self.config, n)
        if not self.nodes:
            return
        for i in xrange(len(self.nodes)):
            print 'node [%i] start at [%f]s'%(i, i)
            self.event(i+1, self.node_run, self.nodes[i], 'start') # start server
            # self.event(1, self.nodes[0], 'start') # start server
            # self.event(1, self.nodes[1], 'start') # start botmaster
            # self.event(1, self.nodes[1], 'start') # start botmaster

        # if self.nodes:
            # self.event(1, self.ping, self.nodes[0],
                       # "255.255.255.255", self.options.pingcount)
