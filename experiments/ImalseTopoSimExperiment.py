"""
The experiment is purely simulated, namely all nodes are simulated in ns3.
It demos the process of loading a predefined topology file.
"""
# from core.ns3.Experiment import *
from core.ns3.Node import ImalseNetnsSimNode, ImalseNetnsNode
from ImalseSimExperiment import ImalseSimExperiment
from core.ns3.Topology import TopologyNet
import os
NODE_NUM = 0

ADDR_BASE = "10.0.0.0"
SERVER_ADDR = "10.0.0.1"
IP_MASK = "255.255.255.0"

# class ImalseTopoSimExperiment(ImalseExperiment):
class ImalseTopoSimExperiment(ImalseSimExperiment):
    """This is pure ns-3 topology Experiment without emulated node"""
    server_id_set = [0]
    botmaster_id_set = [1]
    client_id_set = [2, 3, 4, 5, 6]
    # client_id_set = [2]


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

        self.net = TopologyNet(
                os.path.abspath(self.options.topology_file),
                self.options.topology_type,
                ImalseNetnsSimNode,
                ipv4AddrBase= ADDR_BASE,
                ipv4Mask = IP_MASK
                )

        self._install_cmds(srv_addr = SERVER_ADDR)
        self.print_srv_addr()
        self._set_server_info()
        self.start_nodes()
