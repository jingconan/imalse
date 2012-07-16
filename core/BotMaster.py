#!/usr/bin/env python
from Client import ClientCMD
from util import abstract_method

botmaster_info = {
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
    }

botmaster_fsm = {
        # 'waiting -> send_ack' : 'request_conn',
        # 'waiting -> send_cmd' : 'get_cmd',
        'initial' : 'waiting',
        'start_action' : 'request_connect',
        'info' : botmaster_info,
        }
keywords = ['initial', 'info']


class BotMaster(ClientCMD):
    def recv_ack(self): abstract_method()

    def echo(self, sock, data):
        print 'recv from server: ', data

class BotMasterManInput(BotMaster):
    def recv_ack(self):
        print 'connection constructed'
        while True:
            cmd = raw_input('hi master, please input your command: ')
            print 'cmd, ', cmd
            if cmd == 'q':
                self.node.send(self.sock, 'master exit')
                self.node.close_sock(self.sock)
                break

            self.node.send(self.sock, self._cmd_to_json(cmd))
            self.node.recv(self.sock, 512, self.echo, threaded=True)
        print 'finish recv_ack'

# class BotMasterShowExistence(ClientCMD):
#     def recv_ack(self):
#         while True:
#             self.node.sock_send(self.sock, self._cmd_to_json('event=verify_master;password=1234;'))
#             self.node.sleep(2)
#             self.node.sock_send(self.sock,
#                     self._cmd_to_json('event=echo_bots;msg=I am the master, I have controlled you;') )
#             self.node.sleep(2)

class BotMasterOneCMD(BotMaster):
    def __init__(self, client_fsm, master_password, interval, num, cmd_str):
        ClientCMD.__init__(self, client_fsm)
        self.master_password = master_password
        self.cmd_str = cmd_str
        self.interval = interval
        self.num = num

    def recv_ack(self):
        self.node.send(self.sock,
                self._cmd_to_json('event=verify_master;password=%s;'%(self.master_password)))
        print 'self.node.send(self.sock) finishe'
        self.node.sleep(self.interval)
        print 'sleep has been called'
        idx = self.num
        while True:
            if idx == 0: break
            idx -= 1
            print 'send command out'
            self.node.send(self.sock,
                    self._cmd_to_json(self.cmd_str) )
            self.node.sleep(self.interval)

class BotMasterTest(BotMasterOneCMD):
    def __init__(self, client_fsm=botmaster_fsm):
        BotMasterOneCMD.__init__(self, client_fsm,
            '1234',
            2,
            -1,
            'event=forward_to_bots;bot_event=send_ping;hostname=127.0.0.1')
            # 'event=forward_to_bots;bot_event=send_ping;hostname=www.google.com;')

# if __name__ == "__main__":
    # cmd = BotMasterOneCMD(client_fsm,
            # '1234',
            # 2,
            # -1,
            # 'event=echo_bots;msg=I am the master, I have controlled you;')
    # cmd = BotMasterManInput(client_fsm)
    # cmd = BotMasterOneCMD(client_fsm,
    #         '1234',
    #         2,
    #         -1,
    #         'event=forward_to_bots;bot_event=send_ping;hostname=www.google.com;')
    # node = PhyNode()
    # cmd.install(node)
    # node.start()
