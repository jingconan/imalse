"""
this file describe the netns3 experiment for imalse
"""
import ns3
from util import load_module
from util import get_scenario_option, abstract_method

# for test, just select the first node as server, the second node as botmaster, all other nodes
# are clients
class ImalseExperiment(object):
    """ Base class for all ImalseExperiment
    """
    server_id_set = [0]
    botmaster_id_set = [1]
    client_id_set = [2, 3, 4]

    @property
    def id_set(self):
        return self.server_id_set + self.botmaster_id_set + self.client_id_set

    def initparser(self, parser):
        scenario_ops = get_scenario_option()
        parser.add_option('-s', '--scenario', dest="scenario",
                default='ddos_ping_flooding',
                help='specify the scenario you want to execute. Scenearios availiable are: %s'%(scenario_ops),
                )

    def _init(self): pass
    def setup(self): pass

    def get_node(self, i):
        """get ith nodes"""
        abstract_method()

    @property
    def node_num(self):
        abstract_method()

    def node_run(self, node, *args, **kwargs):
        """run a command on a specific node"""
        node.cmd_set._trigger(*args, **kwargs)

    def start_nodes(self):
        """specify when each node should start"""
        # start servers
        for i in self.server_id_set:
            print 'node [%i] type [%s] start at [%f]s'%(i, 'server', 0)
            self.event(0, self.node_run, self.get_node(i), 'start')

        # start clients
        t = 0
        for i in self.client_id_set:
            t += 2
            print 'node [%i] type [%s] start at [%f]s'%(i, 'client', t)
            self.event(t, self.node_run, self.get_node(i), 'start')

        # start botmaster
        for i in self.botmaster_id_set:
            print 'node [%i] type [%s] start at [%f]s'%(i, 'botmaster', t+2)
            self.event(t+2, self.node_run, self.get_node(i), 'start')

    def _install_cmds(self, srv_addr=None):
        """install different command set to nodes according to their type
        can manually set the server address
        """
        scen = load_module(self.options.scenario)
        botmaster_desc = scen.botmaster_desc
        server_desc = scen.server_desc
        client_desc = scen.client_desc
        if srv_addr is not None:
            botmaster_desc['srv_addr'] = srv_addr
            server_desc['srv_addr'] = srv_addr
            client_desc['srv_addr'] = srv_addr

        for i in xrange(self.node_num):
            if i in self.botmaster_id_set:
                cmd = scen.BotMaster(botmaster_desc)
            elif i in self.server_id_set:
                cmd = scen.ServerCMD(server_desc)
            elif i in self.client_id_set:
                cmd = scen.ClientCMD(client_desc)
            else:
                continue
            cmd.install(self.get_node(i))

    def _get_server_addr(self):
        """The all the server address"""
        addr_set = []
        # print 'self.server_id_set', self.server_id_set
        # for i in [0, 1, 2, 3, 4, 5]:
            # ipv4Addr = self.get_node(i).GetObject(ns3.TypeId.LookupByName("ns3::Ipv4")).GetAddress(1, 0)
            # print str(ipv4Addr.GetLocal())

        for i in self.server_id_set:
            ipv4Addr = self.get_node(i).GetObject(ns3.TypeId.LookupByName("ns3::Ipv4")).GetAddress(1, 0)
            addr_set.append(ipv4Addr)
        return addr_set

    def _get_server_nodes(self):
        return [self.get_node(i) for i in self.server_id_set]

    def _set_server_info(self):
        """Only Set Server information for simulated nodes"""
        server_addr_set = self._get_server_addr()
        server_set = self._get_server_nodes()
        for i in xrange(self.node_num):
            # if i in self.server_id_set or self.get_node(i).NODE_TYPE.startswith('real'):
                # continue
            if self.get_node(i).NODE_TYPE.startswith('real'):
                continue
            self.get_node(i).server_addr_set = server_addr_set
            self.get_node(i).server_set = server_set

    def print_srv_addr(self):
        """print out the address of the server"""
        for i in self.server_id_set:
            ipv4Addr = self.get_node(i).GetObject(ns3.TypeId.LookupByName("ns3::Ipv4")).GetAddress(1, 0)
            print 'The server addr of server [%i ]: '%(i), ipv4Addr

