# ServerCommand
from scenario import base
class ServerCommand(base.ServerCommand):
    def __init__(self, core):
        self.core = core

    def get_client_id(self):
        print 'get_client_id'
        pass

    def initiate_attack(self):
        pass

    def stop_attack(self):
        pass
