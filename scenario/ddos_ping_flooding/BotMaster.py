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
    cmd_attr = ['event=forward_to_bots',
            'bot_event=send_ping',
            'remote=10.200.1.1',
            'start_time=300',
            'stop_time=500',
            'r_port=12345',
            ]
    def __init__(self, desc=botmaster_desc):
        core.BotMaster.BotMasterOneCMD.__init__(self,
            desc = desc,
            master_password = '1234',
            interval = 2,
            # num = 11,
            num = 1,
            cmd_str = ';'.join(self.cmd_attr))
            # cmd_str = 'event=forward_to_bots;bot_event=send_ping;hostname=127.0.0.1')
            # cmd_str = 'event=forward_to_bots;bot_event=send_ping;hostname=127.0.0.1')
            # 'event=forward_to_bots;bot_event=send_ping;hostname=10.0.0.0')
            # 'event=forward_to_bots;bot_event=send_ping;hostname=www.google.com;')


