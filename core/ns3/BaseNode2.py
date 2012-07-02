from util import abstract_method
class BaseNode(object):
    """ Virtual Base Class For all nodes"""
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
    def set_master_sock(self, sock): abstract_method()
    @property
    def client_socks(self): abstract_method()
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
    #################################
    ###  Application Layer    #######
    #################################
    def ping(self, sock, data, threaded=False): abstract_method()
    def ftp(self, sock, data, threaded=False): abstract_method()
    def icmp(self, sock, data, threaded=False): abstract_method()
    def http(self, sock, data, threaded=False): abstract_method()
    def stop_app(self, sock, app_name): abstract_method()
