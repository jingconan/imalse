from core.ns3.netns3 import *
class Network:
    def __init__(self, path):
        print 'network config file [%s] loading...'%(path)
        pass

    def install(self, core):
        print 'network installing...'
        self.core = core
        core.network = self
