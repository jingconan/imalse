"""
Complex Network Experiment with Background Traffic

This experiment will deal with heterogeneous network
that consists of not only PointToPointLink but also
CsmaNetwork, etc
"""
from __future__ import print_function, division
import settings
from core.ns3.NS3Config import BackgroundTrafficConfig
from core.ns3.Topology import ComplexNet
from experiments import experiment_factory
from core.configure import gen_anomaly_dot
import ns3

ManualTopoExperiment = experiment_factory('ManualTopoExperiment', BaseClass)

zeros = lambda s:[[0 for i in xrange(s[1])] for j in xrange(s[0])]
import copy

class ComplexNetExperiment(ManualTopoExperiment):
    """This experiment deals with hetergenous network
    """
    # routing protocol list, 'type':priority
    routing_helper_list = {
            'static':0,
            # 'nix':5,
            'olsr':10,
            }

    def initparser(self, parser):
        ManualTopoExperiment.initparser(self, parser)

        # parser.set_defaults(back_traf="net_config/back_traf.py",
                # )
        parser.add_option('--back_traf', dest="back_traf", default=None,
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

    @staticmethod
    def transform_to_net_desc(net_settings):
        """ Transform the net_setting used by ComplextNet to net_desc paramter used by fs configure

        The net specification parameter in *Imalse* and *fs* configure module
        are quite different. This function tranform a net_setting parameter to
        a net_desc parameter used by fs/configure module
        we don't need topology in the dot file to be exactly the same.
        we just need the ip address for each interface.

        """
        net_desc = copy.copy(net_settings) # shallow copy
        net_desc['node_type'] = 'NNode'
        net_desc['node_para'] = {}

        # Create a Suitable Topology. For CSMA network. It will simply
        # create circle topology.
        net_desc['link_to_ip_map'] = {}
        for type_, desc in net_desc['nets'].iteritems():
            if type_ == 'PointToPoint':
                net_desc['link_to_ip_map'].update(desc['IpMap'])
            elif type_ == "Csma":
                for nodes, ips in desc['IpMap'].iteritems():
                    for i in xrange(len(nodes) - 1):
                        net_desc['link_to_ip_map'].update({ (nodes[i], nodes[i+1]):(ips[i], ips[i+1]) })

        pairs = net_desc['link_to_ip_map'].keys()
        g_size = max(max(pairs)) + 1
        topo = zeros((g_size, g_size))
        for x, y in pairs:
            topo[x][y] = 1
        net_desc['topo'] = topo

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

        self.net = ComplexNet(
                self.NodeCreator,
                net_settings,
                routing_helper_list = self.routing_helper_list,
                )

        if self.options.back_traf:
            # Generate dot file that describe the background traffic.
            dot_file = self.gen_back_traf_dot(net_settings)
            ns3.LogComponentEnable("OnOffApplication", ns3.LOG_LEVEL_INFO)
            bg_cofig = BackgroundTrafficConfig(dot_file, self.net)
            bg_cofig.config_onoff_app()

        self.net.set_trace()
        print('finish running set_trace')
        self._install_cmds(srv_addr = self.SERVER_ADDR)
        # print('finish running install_cmds')
        self.print_srv_addr()
        # print('finish running print_srv_addr')
        self._set_server_info()
        # print('finish running set_server_info')
        self.start_nodes()
        # print('finish running start nodes')

