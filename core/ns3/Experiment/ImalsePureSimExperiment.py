"""
In Pure Sim experiments, all nodes are simulated. So we don't need
the real-time schedular.
"""
import optparse
from core.ns3.Node import ImalseNetnsSimNode
import ns.core
from Experiment import ImalseExperiment

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

