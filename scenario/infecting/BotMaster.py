import core.BotMaster

botmaster_desc = {
        'initial' : 'waiting',
        'start_action' : 'request_connect',
        'srv_addr':'127.0.0.1',
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
        }

class BotMaster(core.BotMaster.BotMasterTest):
    def __init__(self, desc=botmaster_desc):
        core.BotMaster.BotMasterTest.__init__(self, desc)

