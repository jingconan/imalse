#!/usr/bin/env python
# ServerCommand
import sys
sys.path.append("../..")
import core
import copy

class ServerCMD(core.ServerCMD):
    # def get_client_id(self):
        # print 'get_client_id'
        # pass
    def forward_to_bots(self, sock, data):
        self.logger.info('start to echo_bots' )
        new_data = copy.deepcopy(data)
        new_data['event'] = data['bot_event']
        del new_data['bot_event']
        for client_sock in self.node.client_socks:
            self.node.sock_send(client_sock, self._dump_json(new_data))

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


