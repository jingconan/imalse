#!/usr/bin/env python
# ClientCommand
import core

info = {
        'srv_addr':'127.0.0.1',
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
    }
client_fsm = {
        # 'conn -> disconn' : 'disconn_req | cmd_time_out',
        # 'disconn -> conn' : 'recv_ack',
        # 'disconn -> disconn' : 'request_time_out | connection_refused',
        # 'conn -> exe_cmd' : 'recv_cmd',
        # 'exe_cmd -> conn' : 'cmd_done',
        'initial' : 'disconn',
        'start_action' : 'request_connect',
        'info' : info,
        }

class ClientCMD(core.ClientCMD):
    def __init__(self, fsm=client_fsm):
        core.ClientCMD.__init__(self, fsm)

    def send_ping(self, sock, data):
        self.node.ping(sock,data, threaded=True)

    def stop_ping(self, sock):
        self.node.stop_app(sock, 'ping')

if __name__ == "__main__":
    cmd = ClientCMD(client_fsm)
    node = core.PhyNode()
    cmd.install(node)
    node.start()
    pass
