"""
In Pure Sim experiments, all nodes are simulated. So we don't need
the real-time schedular.
"""
from Experiment import ImalseExperiment
import ns.core

ns.core.GlobalValue.Bind("SimulatorImplementationType",
        ns.core.StringValue("ns3::DefaultSimulatorImpl"))
class ImalsePureSimExperiment(ImalseExperiment):
    def run(self):
        self.setup()
        print "running simulator for %s sec" % self.options.simtime
        ns.core.Simulator.Stop(ns.core.Seconds(self.options.simtime))
        ns.core.Simulator.Run(signal_check_frequency = -1)
        print "simulator done"
        # self.cleanup()
    @staticmethod
    def event(time, func, *args, **kwds):
        def run():
            func(*args, **kwds)
        ns.core.Simulator.Schedule(ns.core.Time(str(time)), run)
