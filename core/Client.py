#!/usr/bin/env python
from CMD import CMD
import sys
sys.path.append("../..")
from Node import PhyNode

# ClientCommand
# from fsm import FiniteStateMachine
info = {
        'srv_addr':'127.0.0.1',
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

class ClientCMD(CMD):
    """Client Command Meta Description
    define the handle of each transition
    each CMD should claim the basic function set a node much provide
    """
    name = 'client_cmd'
    def __init__(self, fsm_desc):
        CMD.__init__(self, fsm_desc)

    @property
    def srv_addr(self): return self.fsm_desc['info']['srv_addr']
    @property
    def srv_port(self): return self.fsm_desc['info']['srv_port']

    def _is_okay(self, node):
        """check whether this command set is compmatible with the node"""
        return True

    def request_connect(self):
        sock = self.node.create_sock({'type':'server', 'proto':'tcp'})
        event = self.node.sock_connect(sock, (self.srv_addr, self.srv_port))
        self.sock = sock
        self._trigger(event)

    def connection_refused(self):
        self.logger.info("connection_refused, try 2 seconds later")
        self.node.sleep(2, self.request_connect)

    def request_time_out(self):
        self.logger.info("request_time_out, try 2 seconds later")
        self.node.sleep(2, self.request_connect)

    def recv_disconn_req(self, sock):
        self.logger.info("recv_disconn_req")
        self.node.sock_close(sock)

    def recv_ack(self):
        self.logger.info("connection constructed")
        self.node.sock_recv(self.sock, 512, self.dispatcher)

    def echo(self, sock, data):
        self.logger.info("receive echo message, [%s]"%(data['msg'][0]))
        print '-->', data['msg'][0]


if __name__ == "__main__":
    cmd = ClientCMD(client_fsm)
    node = PhyNode()
    cmd.install(node)
    node.start()

