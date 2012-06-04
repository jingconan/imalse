class Network:
    def install(self, core):
        print 'network installing...'
        self.core = core
        core.network = self
