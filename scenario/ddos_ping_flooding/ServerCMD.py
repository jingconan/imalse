#!/usr/bin/env python
# ServerCommand
import core
import copy

info = {
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
    }

server_fsm = {
        'initial' : 'waiting',
        'start_action' : 'initialize',
        'info' : info,
        }
keywords = ['initial', 'info']

class ServerCMD(core.ServerCMD):
    def __init__(self):
        core.ServerCMD.__init__(self, server_fsm)

    def forward_to_bots(self, sock, data):
        self.logger.info('start to echo_bots' )
        new_data = copy.deepcopy(data)
        new_data['event'] = data['bot_event']
        del new_data['bot_event']
        for client_sock in self.node.client_socks:
            self.node.send(client_sock, self._dump_json(new_data))

if __name__ == "__main__":
    info = {
            'srv_addr':'127.0.0.1',
            'srv_port':3333,
            'request_timeout':10,
        }

    client_fsm = {
            # 'waiting -> send_ack' : 'request_conn',
            # 'waiting -> send_cmd' : 'get_cmd',
            'initial' : 'waiting',
            'start_action' : 'initialize',
            'info' : info,
            }
    keywords = ['initial', 'info']

    from core.Node import PhyNode
    cmd = ServerCMD(client_fsm)
    node = PhyNode()
    cmd.install(node)
    node.start()


