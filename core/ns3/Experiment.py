"""
this file describe the netns3 experiment for imalse
"""
from core.ns3.netns3 import NetnsExperiment
from core.ns3.Node import ImalseNetnsNode
import ns3
from util import load_module
from util import get_scenario_option, abstract_method

NODE_NUM = 0

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
        # botmaster_desc['srv_addr'] = "10.1.1.1"
        print 'srv_addr, ', srv_addr
        if srv_addr:
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
            if i in self.server_id_set or self.get_node(i).NODE_TYPE.startswith('real'):
                continue
            self.get_node(i).server_addr_set = server_addr_set
            self.get_node(i).server_set = server_set

    def print_srv_addr(self):
        """print out the address of the server"""
        for i in self.server_id_set:
            ipv4Addr = self.get_node(i).GetObject(ns3.TypeId.LookupByName("ns3::Ipv4")).GetAddress(1, 0)
            print 'The server addr of server [%i ]: '%(i), ipv4Addr

# for test, just select the first node as server, the second node as botmaster, all other nodes
# are clients
import optparse
class ImalseNetnsExperiment(ImalseExperiment, NetnsExperiment):
    """Base Class for Imalse Experiment in netns3 mode
    """
    def __init__(self, *args, **kwargs):
        # super(ImalseNetnsExperiment, self).__init__(*args, **kwargs)
        NetnsExperiment.__init__(self, *args, **kwargs)
        ImalseExperiment.__init__(self)
        self._init()

    def initparser(self, parser):
        ImalseExperiment.initparser(self, parser)
        NetnsExperiment.initparser(self, parser)

    def get_node(self, i):
        """get ith nodes"""
        return self.nodes[i]

    @property
    def node_num(self):
        return len(self.nodes)

    @staticmethod
    def NodeCreator():
        global NODE_NUM
        NODE_NUM += 1
        name = "n%s" %NODE_NUM
        return ImalseNetnsNode(name, logfile = "/tmp/%s.log" % name)


"""
In Pure Sim experiments, all nodes are simulated. So we don't need
the real-time schedular.
"""
from core.ns3.Node import ImalseNetnsSimNode
import ns.core

class ImalsePureSimExperiment(ImalseExperiment):
    """Pure Sim Experiemtn Doesn't depend on netns3 can run in simulated time"""
    def initparser(self, parser):
        super(ImalsePureSimExperiment, self).initparser(parser)
        parser.set_defaults(SimulatorImplementationType='Default')
        parser.add_option("", "--SimulatorImpl", dest = "SimulatorImplementationType",
                help="the simulator implementation Type, available options are['Realtime', 'Default', 'Visual']")


        parser.set_defaults(numnodes = 2, simtime = 30)
        parser.add_option("-n", "--numnodes", dest = "numnodes", type = int,
                          help = "number of nodes; default = %s" %
                          parser.defaults["numnodes"])
        parser.add_option("-t", "--simtime", dest = "simtime", type = float,
                          help = "simulation run time; default = %s" %
                          parser.defaults["simtime"])

    def setup(self):
        super(ImalsePureSimExperiment, self).setup()
        ns.core.GlobalValue.Bind("SimulatorImplementationType",
                ns.core.StringValue("ns3::%sSimulatorImpl"%(self.options.SimulatorImplementationType)))

    def run(self):
        self.setup()
        print "running simulator for %s sec" % self.options.simtime
        ns.core.Simulator.Stop(ns.core.Seconds(self.options.simtime))
        # ns.core.Simulator.Run(signal_check_frequency = -1)
        ns.core.Simulator.Run()
        self.cleanup()
        ns.core.Simulator.Destroy()
        print "simulator done"

    @staticmethod
    def event(time, func, *args, **kwds):
        """schedule an event to simulator"""
        def run():
            func(*args, **kwds)
        ns.core.Simulator.Schedule(ns.core.Time(str(time)), run)

    @staticmethod
    def NodeCreator():
        return ImalseNetnsSimNode()

    def main(self, args, run = True):
        usage = "%prog [-h] " + self.usagestr() + " ..."
        parser = optparse.OptionParser(usage = usage)
        self.initparser(parser)
        self.options, self.args = parser.parse_args(args)
        if run:
            self.run()

    def usagestr(self):
        return "[-n <numnodes>] [-t <simtime>]"

    def print_help(self):
        parser = optparse.OptionParser(usage = '')
        self.initparser(parser)
        parser.print_help()

    def cleanup(self):
        ns.core.Simulator.Destroy()
        for i in xrange(self.node_num):
            n = self.get_node(i)
            n.stop()
