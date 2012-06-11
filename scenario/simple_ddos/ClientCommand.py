# ClientCommand
from scenario import base
class ClientCommand(base.ClientCommand):
    def install(self, core):
        self.core = core
        core.cmd = self

    def send_ping(self):
        pass

    def stop_ping():
        pass

