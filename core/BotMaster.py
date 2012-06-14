#!/usr/bin/env python
from Client import ClientCMD, client_fsm
from Node import PhyNode
from util import abstract_method

class BotMaster(ClientCMD):
    def recv_ack(self): abstract_method()

class BotMasterManInput(BotMaster):
    def recv_ack(self):
        print 'connection constructed'
        while True:
            cmd = raw_input('hi master, please input your command: ')
            print 'cmd, ', cmd
            if cmd == 'q':
                self.node.sock_send(self.sock, 'master exit')
                self.node.sock_close(self.sock)
                break

            self.node.sock_send(self.sock, cmd)
            self.node.sock_thread_recv(self.sock, 512, self.show)
        print 'finish recv_ack'

class BotMasterShowExistence(ClientCMD):
    def show(self, sock, data):
        print 'recv from server: ', data
    def recv_ack(self):
        while True:
            self.node.sock_send(self.sock, 'verify_master > password:1234')
            # self.node.sock_thread_recv(self.sock, 512, self.show)
            self.node.sleep(2)
            self.node.sock_send(self.sock, 'send_message_to_all > I am the master, I have controlled you')
            self.node.sleep(2)

if __name__ == "__main__":
    cmd = BotMasterShowExistence(client_fsm)
    node = PhyNode()
    cmd.install(node)
    node.start()
