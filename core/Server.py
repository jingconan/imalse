#!/usr/bin/env python
from CMD import CMD
import copy

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

BOT_MASTER_PASSWORD = '1234'

class ServerCMD(CMD):
    name =  'server_cmd'
    def __init__(self, fsm_desc=None):
        # super(ServerCMD, self).__init__(fsm_desc)
        CMD.__init__(self, fsm_desc)

    def _is_okay(self, node): return True

    def initialize(self):
        srv_sock = self.node.create_sock({'type':'server', 'proto':'tcp'})
        self.node.sock_bind(srv_sock, 3333)
        self.node.sock_listen(srv_sock, 5)
        self.node.set_state('waiting')

        while True:
            client_sock, address = self.node.sock_accept(srv_sock)
            self._trigger('recv_request_conn', client_sock, address)

    def recv_request_conn(self, client_sock, address):
        self.logger.info('receive request from addr: %s'%(str(address)))
        self.node.sock_send(client_sock, 'connect_ack')
        self.node.sock_thread_recv(client_sock, 512, self.dispatcher)

    def dispatcher(self, sock, data):
        try:
            CMD.dispatcher(self, sock, data)
        except Exception as e:
            self.logger.error('unknow message: %s from %s'%(data, self.node.sockets[sock]))
            self.logger.exception(str(e))
            self.node.sock_send(sock, 'you have sent me a unknown message')

    def verify_master(self, sock, data):
        if data['password'][0] == BOT_MASTER_PASSWORD:
            self.logger.info( 'bot master password verfied' )
            self.node.set_master_sock(sock)
            self.node.sock_send(sock, 'verifed, hi master, what you want to do? ')

    def echo_bots(self, sock, data):
        self.logger.info('start to echo_bots' )
        new_data = copy.deepcopy(data)
        new_data['event'] = ['echo']
        for client_sock in self.node.client_socks:
            self.node.sock_send(client_sock, self._dump_json(new_data))

    def disconnect(self, sock):
        self.logger.info('receive disconnect request')
        return False

if __name__ == "__main__":
    from Node import PhyNode
    cmd = ServerCMD(client_fsm)
    node = PhyNode()
    cmd.install(node)
    node.start()


