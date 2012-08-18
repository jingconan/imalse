from core.ns3.netns3 import NetnsExperiment
from core.ns3.Node import ImalseNetnsNode
from Experiment import ImalseExperiment

NODE_NUM = 0
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
