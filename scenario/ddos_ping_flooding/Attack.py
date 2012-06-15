""" Attack Description
"""
class Attack:
    def __init__(self, path):
        print 'attack description file [%s] loading...'%(path)

    def install(self, core):
        self.core = core
        core.attack = self
        print 'attack installing...'
