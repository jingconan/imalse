from sadit_configure.Node import NNode
from core.BaseNode import BaseNode
from util import abstract_method

class Node(NNode, BaseNode):
    def __init__(self, ipdests, node_seq):
        self.sock_num = 0
        self.socks = []

    def start(self): abstract_method()
    #################################
    ###  Some Utility Function    ###
    #################################
    @property
    def now(self): return self.network.now()
    def sleep(self, t, call_back=None):
        pass
    def create_timer(self, t, call_back): abstract_method()
    #################################
    ###  Network Related          ###
    #################################
    def set_master_sock(self, sock): abstract_method()
    @property
    def client_socks(self): abstract_method()
    #### Socket API ####
    def create_sock(self, desc):
        sid = self.sock_num
        self.socks.append(desc)
        self.sock_num += 1
        return sid

    def bind(self, sock, port):
        self.socks[sock]['ipdests'] = self.ipdests
        self.socks[sock]['port'] = self.ipdests

    def listen(self, sock, backlog): pass
    def accept(self, sock): pass
    def recv(self, sock, bufsize, dispatcher, threaded=False): abstract_method()

    def send(self, sock, data): abstract_method()
    def connect(self, sock, addr_port): abstract_method()
    def sendto(self, sock, data, addr, port): abstract_method()
    def close_sock(self, sock): abstract_method()
    #################################
    ###  Application Layer    #######
    #################################
    def ping(self, sock, data, threaded=False): abstract_method()
    def ftp(self, sock, data, threaded=False): abstract_method()
    def icmp(self, sock, data, threaded=False): abstract_method()
    def http(self, sock, data, threaded=False): abstract_method()
    def stop_app(self, sock, app_name): abstract_method()

    #################################
    ##   New Func                  ##
    #################################
    # def install(self, network):
        # self.network = network
        # network.add_node(self)
