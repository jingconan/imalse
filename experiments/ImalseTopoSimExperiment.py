from core.ns3.Experiment import *
from ImalseSimExperiment import ImalseSimExperiment
from core.ns3.Topology import TopologyNet
import os
NODE_NUM = 0

SERVER_ADDR = "10.0.0.0"
IP_MASK = "255.255.255.0"

# class ImalseTopoSimExperiment(ImalseExperiment):
class ImalseTopoSimExperiment(ImalseSimExperiment):
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
        # scenario_ops = get_scenario_option()
        # parser.add_option('-s', '--scenario', dest="scenario",
        #         default='None',
        #         help='specify the scenario you want to execute. Scenearios availiable are: %s'%(scenario_ops),
        #         )


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

        self.net = TopologyNet(os.path.abspath(self.options.topology_file),
                self.options.topology_type,
                # ImalseNetnsNodeCreator)
                ImalseNetnsSimNode,
                # ipv4AddrBase= SERVER_ADDR,
                # ipv4Mask = IP_MASK,
                ipv4Mask="255.255.255.0",
                ipv4AddrBase="10.0.0.0",
                )

        self._install_cmds()
        # self._install_cmds(srv_addr = "10.0.0.0")
        self.print_srv_addr()
        self._set_server_info()

        if self.node_num == 0: return
        for i in self.server_id_set:
            self.event(0, self.node_run, self.get_node(i), 'start')

        # start clients
        t = 0
        for i in self.client_id_set:
            t += 1
            self.event(t, self.node_run, self.get_node(i), 'start')

        # start botmaster
        for i in self.botmaster_id_set:
            self.event(t+1, self.node_run, self.get_node(i), 'start')
