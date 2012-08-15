"""
It demos the process of loading a predefined topology file.
"""
import settings
from core.ns3.Topology import ManualTopologyNet
from experiments import experiment_factory
import os

# SERVER_ADDR = "10.0.1.1"

TopoExperiment = experiment_factory('TopoExperiment', BaseClass)
from util import Namespace

class ManualTopoExperiment(TopoExperiment):
    """In Manual Topo Experiment,  You can specify and ip address for each indexing"""
    def initparser(self, parser):
        TopoExperiment.initparser(self, parser)

        parser.set_defaults(net_settings="net_config/net_settings.py",
                )
        parser.add_option('--net_settings', dest="net_settings",
                help='net settings file',
                )

    def load_net_settings(self):
        s = {}
        execfile(settings.ROOT + '/' + self.options.net_settings, s)
        return Namespace(s)

    def load_exper_settings(self, ns):
        from util import CIDR_to_subnet_mask
        # botnet related configuration
        self.server_id_set = ns.server_id_set
        self.botmaster_id_set = ns.botmaster_id_set
        self.client_id_set = ns.client_id_set
        self.SERVER_ADDR, self.NETWORK_BASE, self.IP_MASK = CIDR_to_subnet_mask(ns.server_addr[0]);

    def setup(self):
        BaseClass.setup(self)
        net_settings = self.load_net_settings()
        self.load_exper_settings(net_settings)

        self.net = ManualTopologyNet(
                # os.path.abspath(self.options.topology_file),
                settings.ROOT + '/' + self.options.topology_file,
                self.options.topology_type,
                self.NodeCreator,
                net_settings,
                )
        self.net.set_trace()

        self._install_cmds(srv_addr = self.SERVER_ADDR)
        self.print_srv_addr()
        self._set_server_info()
        self.start_nodes()

