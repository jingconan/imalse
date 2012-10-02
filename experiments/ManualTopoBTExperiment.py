"""
Manual Topology Experiment with Background Traffic

"""
from __future__ import print_function, division
import settings
from core.ns3.NS3Config import BackgroundTrafficConfig
from core.ns3.Topology import ManualTopologyNet
from experiments import experiment_factory
from core.configure import gen_anomaly_dot
import ns3
import copy

ManualTopoExperiment = experiment_factory('ManualTopoExperiment', BaseClass)

zeros = lambda s:[[0 for i in xrange(s[1])] for j in xrange(s[0])]

def get_inet_adj_mat(fname):
    fid = open(fname, 'r')
    i = -1
    while True:
        i += 1
        line = fid.readline()
        if not line: break
        if i == 0:
            totnode, totlink = [int(s.strip()) for s in line.rsplit()]
            adj_mat = zeros([totnode, totnode])
            continue
        if i <= totnode: # ignore the position information
            continue

        _from, _to, _lineBuffer = [s.strip() for s in line.rsplit()]
        adj_mat[int(_from)][int(_to)] = 1
    fid.close()

    return adj_mat

class ManualTopoBTExperiment(ManualTopoExperiment):
    """ This is a extension of Manual topology experiment it
    add background traffic to the network.
    """
    # routing protocol list, 'type':priority
    routing_helper_list = {
            'static':0,
            'nix':5,
            # 'olsr':10,
            }

    def initparser(self, parser):
        ManualTopoExperiment.initparser(self, parser)

        parser.set_defaults(back_traf="net_config/back_traf.py",
                )
        parser.add_option('--back_traf', dest="back_traf",
                help='confgiuration files for back ground traffic',
                )
        parser.add_option('--dot_file', default='net_config/ManualTopoBTConf.dot',
                help='the configured dot file')


    @staticmethod
    def load_para(f_name, encap=None, **kwargs):
        """load parameters.

        - **f_name**: is the path of the configuration file
        - **encap**: is the additional operation done to the data, for example,
            the default value encap=Namespace is to change parameters from dict
            to Namespace class.
        - **kwargs**: contains some additional parameters
        """
        s = kwargs
        execfile(f_name, s)
        return s if encap is None else encap(s)

    def transform_to_net_desc(self, net_settings):
        """add topology from topoloy file"""
        net_desc = copy.copy(net_settings) # shallow copy
        net_desc['node_type'] = 'NNode'
        net_desc['node_para'] = {}
        net_desc['topo'] = get_inet_adj_mat(settings.ROOT + '/' + self.options.topology_file)
        return net_desc

    def load_exper_settings(self, ns):
        """ load experiment related settings

        **server_id_set**:
        **botmaster_id_set**:
        **client_id_set**:
        **SERVER_ADDR**:
        **NETWORK_BASE**:
        **IP_MASK**:

        """
        from util import CIDR_to_subnet_mask
        # botnet related configuration
        self.server_id_set = ns['server_id_set']
        self.botmaster_id_set = ns['botmaster_id_set']
        self.client_id_set = ns['client_id_set']

        # parse the server address
        if len(ns['server_addr']) == 0:
            self.SERVER_ADDR = ''
            self.NETWORK_BASE = ''
            self.IP_MASK = ''
        else:
            self.SERVER_ADDR, self.NETWORK_BASE, self.IP_MASK = CIDR_to_subnet_mask(ns['server_addr'][0]);

    def gen_back_traf_dot(self, net_settings):
        """generate background traffic DOT file

        *input*: is the a dict of net_settings
        *output*: DOT_FILE_PATH
        """

        # get back_traf parameter
        back_traf = self.load_para(
                f_name = settings.ROOT + '/' + self.options.back_traf,
                sim_t = self.options.simtime,
                )

        # call the SADIT/Configure module to generate the dot file specifying the background
        # traffic pattern.
        dot_file = settings.ROOT + '/' + self.options.dot_file
        gen_anomaly_dot(
                back_traf['ANO_LIST'],
                self.transform_to_net_desc(net_settings),
                back_traf['NORM_DESC'],
                dot_file
                )
        return dot_file

    def setup(self):
        BaseClass.setup(self)

        net_settings = self.load_para(f_name = settings.ROOT + '/' + self.options.net_settings)
        self.load_exper_settings(net_settings)

        # Generate dot file that describe the background traffic.
        dot_file = self.gen_back_traf_dot(net_settings)

        self.net = ManualTopologyNet(
                # os.path.abspath(self.options.topology_file),
                settings.ROOT + '/' + self.options.topology_file,
                self.options.topology_type,
                self.NodeCreator,
                net_settings,
                )


        bg_cofig = BackgroundTrafficConfig(dot_file, self.net)
        bg_cofig.config_onoff_app()

        self.net.set_trace()
        self._install_cmds(srv_addr = self.SERVER_ADDR)
        self.print_srv_addr()
        self._set_server_info()
        self.start_nodes()

    def setup(self):
        BaseClass.setup(self)
        # net_settings = self.load_net_settings()
        net_settings = self.load_para(
                f_name = settings.ROOT + '/' + self.options.net_settings,
                )
        self.load_exper_settings(net_settings)

        # Generate dot file that describe the background traffic.
        dot_file = self.gen_back_traf_dot(net_settings)

        ns3.LogComponentEnable("OnOffApplication", ns3.LOG_LEVEL_INFO)
        self.load_exper_settings(net_settings)

        self.net = ManualTopologyNet(
                # os.path.abspath(self.options.topology_file),
                settings.ROOT + '/' + self.options.topology_file,
                self.options.topology_type,
                self.NodeCreator,
                net_settings,
                )
        bg_cofig = BackgroundTrafficConfig(dot_file, self.net)
        bg_cofig.config_onoff_app()

        self.net.set_trace()
        self._install_cmds(srv_addr = self.SERVER_ADDR)
        self.print_srv_addr()
        self._set_server_info()
        self.start_nodes()
