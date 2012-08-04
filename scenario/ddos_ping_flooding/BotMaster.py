import core.BotMaster

botmaster_desc = {
        'initial' : 'waiting',
        'start_action' : 'request_connect',
        'srv_addr':'127.0.0.1',
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
        }

class BotMaster(core.BotMaster.BotMasterOneCMD):
    def __init__(self, desc=botmaster_desc):
        core.BotMaster.BotMasterOneCMD.__init__(self,
            desc = desc,
            master_password = '1234',
            interval = 2,
            num = 11,
            cmd_str = 'event=forward_to_bots;bot_event=send_ping;hostname=127.0.0.1')
            # 'event=forward_to_bots;bot_event=send_ping;hostname=10.0.0.0')
            # 'event=forward_to_bots;bot_event=send_ping;hostname=www.google.com;')


