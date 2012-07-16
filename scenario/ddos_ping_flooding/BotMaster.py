import core.BotMaster

botmaster_info = {
        'srv_addr':'127.0.0.1',
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
    }

botmaster_fsm = {
        # 'waiting -> send_ack' : 'request_conn',
        # 'waiting -> send_cmd' : 'get_cmd',
        'initial' : 'waiting',
        'start_action' : 'request_connect',
        'info' : botmaster_info,
        }
keywords = ['initial', 'info']


class BotMaster(core.BotMaster.BotMasterTest):
    def __init__(self, fsm=botmaster_fsm):
        core.BotMaster.BotMasterTest.__init__(self, fsm)

