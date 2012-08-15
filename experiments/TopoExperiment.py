"""
It demos the process of loading a predefined topology file.
"""
import settings
from core.ns3.Topology import TopologyNet

class TopoExperiment(BaseClass):
    NETWORK_BASE = "10.0.0.0"
    SERVER_ADDR = "10.0.0.1"
    # SERVER_ADDR = "127.0.0.1"
    IP_MASK = "255.255.255.0"

    """This is pure ns-3 topology Experiment without emulated node"""
    def initparser(self, parser):
        # import pdb;pdb.set_trace()
        super(TopoExperiment, self).initparser(parser)

        parser.set_defaults(topology_file="net_config/Inet_small_toposample.txt",
                topology_type = 'Inet',
                )
        parser.add_option("-f", "--topology_file", dest = "topology_file",
                          help = "topology file name, default = %s" %(parser.defaults["topology_file"]))
        parser.add_option('-p', '--topology_type', dest="topology_type",
                help='type of topology file',
                )

    def get_node(self, i):
        return self.net.nodes.Get(i)
    @property
    def node_num(self):
        return self.net.nodes.GetN()


    def setup(self):
        super(TopoExperiment, self).setup()
        self.net = TopologyNet(
                # os.path.abspath(self.options.topology_file),
                settings.ROOT + '/' + self.options.topology_file,
                self.options.topology_type,
                self.NodeCreator,
                ipv4Mask = self.IP_MASK,
                ipv4NetworkBase= self.NETWORK_BASE,
                ipv4AddrBase = self.SERVER_ADDR
                )

        self._install_cmds(srv_addr = self.SERVER_ADDR)
        self.print_srv_addr()
        self._set_server_info()
        self.start_nodes()

    def start_nodes(self):
        # start servers
        for i in self.server_id_set:
            # print 'node [%i] type [%s] start at [%f]s'%(i, 'server', 0)
            print 'node [%s] type [%s] start at [%f]s'%(self.get_node(i).name, 'server', 0)
            self.event(0, self.node_run, self.get_node(i), 'start')

        # start clients
        t = 0
        for i in self.client_id_set:
            t += 2
            # print 'node [%i] type [%s] start at [%f]s'%(i, 'client', t)
            print 'node [%s] type [%s] start at [%f]s'%(self.get_node(i).name, 'client', t)
            self.event(t, self.node_run, self.get_node(i), 'start')

        # start botmaster
        for i in self.botmaster_id_set:
            # print 'node [%i] type [%s] start at [%f]s'%(i, 'botmaster', t+2)
            print 'node [%s] type [%s] start at [%f]s'%(self.get_node(i).name, 'botmaster', t+2)
            self.event(t+2, self.node_run, self.get_node(i), 'start')
