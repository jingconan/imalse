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
        }

class ServerCMD(core.ServerCMD):
    def __init__(self, desc=server_desc):
        core.ServerCMD.__init__(self, desc)

    def forward_to_bots(self, sock, data):
        print 'forward to bots has been excuted!'
        self.logger.info('start to echo_bots' )
        new_data = copy.deepcopy(data)
        new_data['event'] = data['bot_event']
        del new_data['bot_event']
        for client_sock in self.node.client_socks:
            print 'send to socket, ', new_data
            self.node.send(client_sock, self._dump_json(new_data))
