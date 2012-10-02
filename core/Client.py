#!/usr/bin/env python
"""This file defines the Client Command Meta Description"""
from CMD import CMD

# ClientCommand
# Default Parameter Setting for Client
client_fsm = {
        'start_action' : 'request_connect',
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
        }

class ClientCMD(CMD):
    """Client Command Meta Description
    define the handle of each transition
    each CMD should claim the basic function set a node much provide
    """
    name = 'client_cmd'
    def __init__(self, desc=None):
        """initialization of ClientCMD"""
        CMD.__init__(self, desc)

    @property
    def srv_addr(self): return self.desc['srv_addr']
    @property
    def srv_port(self): return self.desc['srv_port']

    def _is_okay(self, node):
        """check whether this command set is compmatible with the node"""
        return True

    def request_connect(self):
        """request a connection to server"""
        sock = self.node.create_sock({'type':'server', 'proto':'tcp'})
        self.sock = sock
        if self.node.NODE_TYPE.startswith('real'):
            event = self.node.connect(sock, (self.srv_addr, self.srv_port))
            self._trigger(event)
        elif self.node.NODE_TYPE.startswith('sim'):
            event = self.node.connect(sock, (self.srv_addr, self.srv_port))

    def connection_refused(self):
        """call back when the connection request has been refused
        by server"""
        self.logger.info("connection_refused, try 2 seconds later")
        self.node.sleep(2, self.request_connect)

    def request_time_out(self):
        """call back when the connection request has time out"""
        self.logger.info("request_time_out, try 2 seconds later")
        self.node.sleep(2, self.request_connect)

    def recv_disconn_req(self, sock):
        """call back when receive the disconnect request from server"""
        self.logger.info("recv_disconn_req")
        self.node.close_sock(sock)

    def recv_ack(self):
        """call back when receive ack message from server"""
        self.logger.info("connection constructed")
        print 'connection constructed'
        self.node.recv(self.sock, 512, self.dispatcher)

    def echo(self, sock, data):
        """echo the message"""
        self.logger.info("receive echo message, [%s]"%(data['msg'][0]))
        print '-->', data['msg'][0]

    def start(self):
        self.request_connect()
