#!/usr/bin/env python
# ClientCommand
import core

client_desc = {
        'initial' : 'disconn',
        'start_action' : 'request_connect',
        'srv_addr':'127.0.0.1',
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
        }

class ClientCMD(core.ClientCMD):
    def __init__(self, desc=client_desc):
        core.ClientCMD.__init__(self, desc)

    def send_ping(self, sock, data):
        print 'send_ping in ClientCMD'
        self.node.ping(sock, data, threaded=True)

    def stop_ping(self, sock):
        self.node.stop_app(sock, 'ping')

if __name__ == "__main__":
    cmd = ClientCMD(client_fsm)
    node = core.PhyNode()
    cmd.install(node)
    node.start()
    pass
