#!/usr/bin/env python
from base import CMD
from Node import Node


# ClientCommand
# from fsm import FiniteStateMachine
info = {
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
    }
client_fsm = {
        'conn -> disconn' : 'disconn_req | cmd_time_out',
        'disconn -> conn' : 'recv_ack',
        'disconn -> disconn' : 'request_time_out | connection_refused',
        'conn -> exe_cmd' : 'recv_cmd',
        'exe_cmd -> conn' : 'cmd_done',
        'initial' : 'disconn',
        'start_action' : 'request_connect',
        'info' : info,
        }


class ClientCMD(CMD):
    """Client Command Meta Description
    define the handle of each transition
    each CMD should claim the basic function set a node much provide
    """
    def __init__(self, fsm_desc):
        CMD.__init__(self, 'client_cmd', fsm_desc)

    @property
    def srv_addr(self): return self.fsm_desc['info']['srv_addr']
    @property
    def srv_port(self): return self.fsm_desc['info']['srv_port']

    def _is_okay(self, node):
        """check whether this command set is compmatible with the node"""
        return True

    def request_connect(self):
        sock = self.node.create_client_socket()
        event = self.node.sock_connect(sock, (self.srv_addr, self.srv_port))
        self.sock = sock
        self._trigger(event)

    def connection_refused(self):
        print 'connection_refused'
        print 'try 2 seconds later'
        # self.node.sleep(10, self._trigger('request_connect'))
        self.node.sleep(2, self.request_connect)

    def disconn_req(self):
        pass

    def request_time_out(self):
        print 'request_time_out'
        pass

    def recv_ack(self):
        print 'connection constructed'
        self.node.sock_recv(self.sock, 512, self.dispatcher)

    def dispatcher(self, sock, data):
        print 'client recive message,', data
        return True

    def recv_cmd(self):
        pass

    def cmd_done(self):
        pass

if __name__ == "__main__":
    cmd = ClientCMD(client_fsm)
    node = Node()
    cmd.install(node)
    node.start()

