"""
This file is experiment showing the simulation of hybrid ns3 nodes and
real nodes.
"""
from core.ns3.Node import ImalseNetnsNode, ImalseNetnsSimNode
from core.ns3.Topology import TopologyNet
# from core.ns3.Experiment import ImalseExperiment
from util import get_scenario_option
import ns.core
from ImalseTopoExperiment import ImalseTopoExperiment

import os
NODE_NUM = 0

SERVER_ADDR = "10.0.0.0"
IP_MASK = "255.255.255.0"
REAL_NODE_SET = [1, 2, 3]

ns.core.GlobalValue.Bind("SimulatorImplementationType",
                         ns.core.StringValue("ns3::RealtimeSimulatorImpl"))
ns.core.GlobalValue.Bind("ChecksumEnabled", ns.core.BooleanValue("true"))

class ImalseHybridExperiment(ImalseTopoExperiment):
    """This is Hybrid Experiment with some nodes are simulated and some other nodes are real nodes
    """
    def setup(self):
        def ImalseHybridNodeCreator():
            global NODE_NUM
            NODE_NUM += 1
            name = "n%s" %NODE_NUM
            if NODE_NUM in REAL_NODE_SET:
                return ImalseNetnsNode(name, logfile = "/tmp/%s.log" % name)
            else:
                return ImalseNetnsSimNode(name)

        self.net = TopologyNet(os.path.abspath(self.options.topology_file),
                self.options.topology_type,
                ImalseHybridNodeCreator,
                ipv4AddrBase= SERVER_ADDR,
                ipv4Mask = IP_MASK
                )

        self._install_cmds()
        # self._install_cmds(srv_addr = "10.0.0.0")
        self.print_srv_addr()
        self._set_server_info()
        self.start_nodes()
