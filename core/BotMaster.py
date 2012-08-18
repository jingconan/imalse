#!/usr/bin/env python
if __name__ == "__main__":
    import sys; sys.path.insert(0, "..")

from Client import ClientCMD
from util import abstract_method

botmaster_desc= {
        'start_action' : 'request_connect',
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
        }

class BotMaster(ClientCMD):
    """Base Class for Bot Master"""
    name = 'bot_master_cmd'
    def recv_ack(self): abstract_method()

    def echo(self, sock, data):
        print 'recv from server: ', data

class BotMasterManInput(BotMaster):
    """After connection is constructed, enter into an interactive command line"""
    @staticmethod
    def print_help():
        docs = """
        A typical command consists of a series of equality sentences
        for example:
            event=forward_to_bots;bot_event=send_ping;hostname=127.0.0.1;
        which means:
            event is the command that will run on server, "forward_to_bots" will
            be executed this time.
            bot_event is the command that will executed on bots
            hostname is the parameter for the send_ping command
        """
        print docs

    def recv_ack(self):
        print 'connection constructed'
        while True:
            cmd = raw_input('hi master, please input your command(h for help): ')
            if cmd == 'h':
                self.print_help()
                continue
            print 'the following command will to send to execute: ', cmd
            if cmd == 'q':
                self.node.send(self.sock, 'master exit')
                self.node.close_sock(self.sock)
                break

            self.node.send(self.sock, self._cmd_to_json(cmd))
            self.node.recv(self.sock, 512, self.echo, threaded=True)
        print 'finish recv_ack'

class BotMasterOneCMD(BotMaster):
    """
    >>> cmd = BotMasterOneCMD(botmaster_desc, '1234', 2, -1, \
            'event=echo_bots;msg=I am the master, I have controlled you;')
    >>> cmd = BotMasterOneCMD(botmaster_desc, '1234', 2, -1, \
            'event=forward_to_bots;bot_event=send_ping;hostname=www.google.com;')

    """
    def __init__(self, desc, master_password, interval, num, cmd_str):
        """
        Bot Master that verify itself and send command. One command wil be
        sent automatically.
        - **desc** specify the server information
        - **master_password**
        - **interval** the interval between two consequent command.
        - **num** the total number this command will be sent
        - **cmd_str** the command string. An example of string can be
            event=forward_to_bots;bot_event=send_ping;hostname=127.0.0.1;
        """
        ClientCMD.__init__(self, desc)
        self.master_password = master_password
        self.cmd_str = cmd_str
        self.interval = interval
        self.num = num

    def recv_ack(self):
        self.logger.debug('botmaster one command recv_ack has been recorded')
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
    def __init__(self, desc=botmaster_desc):
        BotMasterOneCMD.__init__(self, desc,
            '1234',
            2,
            -1,
            'event=forward_to_bots;bot_event=send_ping;hostname=127.0.0.1')
            # 'event=forward_to_bots;bot_event=send_ping;hostname=10.0.0.0')
            # 'event=forward_to_bots;bot_event=send_ping;hostname=www.google.com;')

if __name__ == "__main__":
    import doctest
    doctest.testmod()

# if __name__ == "__main__":
   # cmd = BotMasterManInput(client_fsm)
    # cmd = BotMasterOneCMD(client_fsm,
    #         '1234',
    #         2,
    #         -1,
    #         'event=forward_to_bots;bot_event=send_ping;hostname=www.google.com;')
    # node = PhyNode()
    # cmd.install(node)
    # node.start()
