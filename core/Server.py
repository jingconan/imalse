#!/usr/bin/env python
from CMD import CMD, FSM
from Node import PhyNode

info = {
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
    }
client_fsm = {
        'waiting -> send_ack' : 'request_conn',
        'waiting -> send_cmd' : 'get_cmd',
        'initial' : 'waiting',
        'start_action' : 'initialize',
        'info' : info,
        }
keywords = ['initial', 'info']

BOT_MASTER_PASSWORD = '1234'

# class Connection(object):
#     def __init__(self, node, desc):
#         self.desc = desc
#         self.node = node

class ServerConnection(FSM):
    def __init__(self, sock, fsm_desc, node):
        FSM.__init__(self, 'serve_connection',fsm_desc)
        self.node = node
        self.node.sock_send(client_sock, 'connect_ack')
        self.node.sock_thread_recv(client_sock, 512, self.dispatcher)

class ServerCMD(CMD):
    def __init__(self, fsm_desc):
        # CMD.__init__(self, 'server_cmd', fsm_desc)
        self.fsm_desc = fsm_desc
        self.connections = dict()

    def _is_okay(self, node): return True

    def initialize(self):
        srv_sock = self.node.create_sock({'type':'server', 'proto':'tcp'})
        self.node.sock_bind(srv_sock, 3333)
        self.node.sock_listen(srv_sock, 5)
        self.node.set_state('waiting')

        while True:
            client_sock, addr = self.node.sock_accept(srv_sock)
            print 'receive request from addr: ', addr
            self.create_server_connection(client_sock)

    def create_server_connection(self, sock):
        self.connections[sock] = ServerConnection(sock, self.fsm_desc, self.node)

    def dispatcher(self, sock, data):
        print 'dispatcher, recv data', data
        try:
            event = data.rsplit(' > ')[0]
            user_data =  data.rsplit(' > ')[1]
            # self._trigger(event, sock, user_data)
            self.connections[sock]._trigger(event, user_data)
        except Exception as e:
            import traceback
            traceback.print_stack()
            print e
            print 'unknow message: %s from %s '%(data, self.node.sockets[sock])
            self.node.sock_send(sock, 'you have sent me a unknown message')

    def verify_master(self, sock, data):
        password = data.rsplit('password:')[1]
        if password == BOT_MASTER_PASSWORD:
            print 'bot master password verfied'
            self.node.set_master_sock(sock)
            self.node.sock_send(sock, 'verifed, hi master, what you want to do? ')

    def send_message_to_all(self, sock, data):
        print 'start to send_message_to_all'
        for client_sock in self.node.client_socks:
            print 'client_sock', client_sock
            self.node.sock_send(client_sock, data)

    def disconnect(self, sock):
        print 'receive disconnect request'
        return False

if __name__ == "__main__":
    cmd = ServerCMD(client_fsm)
    node = PhyNode()
    cmd.install(node)
    node.start()


