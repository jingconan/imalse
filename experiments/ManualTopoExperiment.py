"""
It demos the process of loading a predefined topology file.
"""
import settings
from core.ns3.Topology import ManualTopologyNet
from experiments import experiment_factory
import os

SERVER_ADDR = "10.0.1.1"

TopoExperiment = experiment_factory('TopoExperiment', BaseClass)
from util import Namespace

class ManualTopoExperiment(TopoExperiment):
    """In Manual Topo Experiment,  You can specify and ip address for each indexing"""
    def initparser(self, parser):
        TopoExperiment.initparser(self, parser)

        parser.set_defaults(net_settings=settings.ROOT+"/net_config/net_settings.py",
                )
        parser.add_option('--net_settings', dest="net_settings",
                help='net settings file',
                )

    def load_net_settings(self):
        s = {}
        execfile(self.options.net_settings, s)
        return Namespace(s)

    def setup(self):
        BaseClass.setup(self)
        self.net = ManualTopologyNet(
                os.path.abspath(self.options.topology_file),
                self.options.topology_type,
                self.NodeCreator,
                self.load_net_settings(),
                )
        self.net.set_trace()

        self._install_cmds(srv_addr = SERVER_ADDR)
        self.print_srv_addr()
        self._set_server_info()
        self.start_nodes()

