"""
In Pure Sim experiments, all nodes are simulated. So we don't need
the real-time schedular.
"""
from Experiment import ImalseExperiment
import ns.core
from util import get_scenario_option
class ImalsePureSimExperiment(ImalseExperiment):
    """Pure Sim Experiemtn Doesn't depend on netns3 can run in simulated time"""
    def initparser(self, parser):
        super(ImalsePureSimExperiment, self).initparser(parser)
        parser.set_defaults(SimulatorImplementationType='Default')
        parser.add_option("", "--SimulatorImpl", dest = "SimulatorImplementationType",
                help="the simulator implementation Type, available options are['Realtime', 'Default', 'Visual']")


        scenario_ops = get_scenario_option()
        parser.add_option('-s', '--scenario', dest="scenario",
                default='ddos_ping_flooding',
                help='specify the scenario you want to execute. Scenearios availiable are: %s'%(scenario_ops),
                )

    def setup(self):
        super(ImalsePureSimExperiment, self).setup()
        # ns.core.GlobalValue.Bind("SimulatorImplementationType",
                # ns.core.StringValue("ns3::VisualSimulatorImpl"))
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
        def run():
            func(*args, **kwds)
        ns.core.Simulator.Schedule(ns.core.Time(str(time)), run)

    def start_nodes(self):
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

