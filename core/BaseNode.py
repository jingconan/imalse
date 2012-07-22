from util import abstract_method
import logging

class BaseNode(object):
    """ Virtual Base Class For all nodes"""
    name = 'base_node'
    def __init__(self):
        self._set_logger()

    def _set_logger(self):
        logging.basicConfig()
        self.logger = logging.getLogger(self.name)
        self.logger.setLevel(logging.DEBUG)


    def start(self): abstract_method()
    #################################
    ###  Some Utility Function    ###
    #################################
    @property
    def now(self): abstract_method()
    def sleep(self, t, call_back=None): abstract_method()
    def create_timer(self, t, call_back): abstract_method()
    #################################
    ###  Network Related          ###
    #################################
    def set_master_sock(self, sock):
        self.sockets[sock]['type'] = 'master'

    # @property
    # def client_socks(self): abstract_method()
    @property
    def client_socks(self):
        return [sock for sock, v in self.sockets.iteritems() if v['type'] == 'client']
    #### Socket API ####
    def create_sock(self, desc): abstract_method()
    def bind(self, sock, port): abstract_method()
    def listen(self, sock, backlog): abstract_method()
    def accept(self, sock): abstract_method()
    def recv(self, sock, bufsize, dispatcher, threaded=False): abstract_method()
    def send(self, sock, data): abstract_method()
    def connect(self, sock, addr_port): abstract_method()
    def sendto(self, sock, data, addr, port): abstract_method()
    def close(self, sock): abstract_method()
    def close_socks(self, socks_seq_set, _type='client'):
        """sock_seq_set is the sequence of socks in this type of sock"""
        socks_set = [self.client_socks[i] for i in socks_seq_set]
        for sock in socks_set:
            if self.sockets[sock]['type'] == _type:
                del self.sockets[sock]
    #################################
    ###  Application Layer    #######
    #################################
    def ping(self, sock, data, threaded=False): abstract_method()
    def ftp(self, sock, data, threaded=False): abstract_method()
    def icmp(self, sock, data, threaded=False): abstract_method()
    def http(self, sock, data, threaded=False): abstract_method()
    def stop_app(self, sock, app_name): abstract_method()

