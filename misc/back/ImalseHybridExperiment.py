"""
This file is experiment showing the simulation of hybrid ns3 nodes and
real nodes.
"""
import ns.core
# from ImalseTopoExperiment import ImalseTopoExperiment
from ImalseTopoNetnsExperiment import ImalseTopoNetnsExperiment


# class ImalseHybridExperiment(ImalseTopoExperiment):
class ImalseHybridExperiment(ImalseTopoNetnsExperiment):
    """This is Hybrid Experiment with some nodes are simulated and some other nodes are real nodes
    """

    def _init(self):
        ns.core.GlobalValue.Bind("SimulatorImplementationType",
                                 ns.core.StringValue("ns3::RealtimeSimulatorImpl"))
        ns.core.GlobalValue.Bind("ChecksumEnabled", ns.core.BooleanValue("true"))



