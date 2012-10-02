from util import abstract_method
import logging

class BaseNode(object):
    """ Virtual Base Class For all nodes
    All Type of Nodes Should Implment API in this abstract first
    """
    name = 'base_node'
    def __init__(self):
        self._set_logger()

    def _set_logger(self):
        logging.basicConfig()
        self.logger = logging.getLogger(self.name)
        # self.logger.setLevel(logging.DEBUG)
        self.logger.setLevel(logging.WARNING)

    def start(self):
        """start the node"""
        abstract_method()

    #################################
    ###  Some Utility Function    ###
    #################################
    @property
    def now(self):
        """get current time, either simulation time or real time."""
        abstract_method()

    def sleep(self, t, call_back=None):
        """sleep the node for a **t** seconds, if **call_back** is specified,
        it will be called after node wake from sleep"""
        abstract_method()

    def create_timer(self, t, call_back): abstract_method()
    #################################
    ###  Network Related          ###
    #################################
    def set_master_sock(self, sock):
        """set the **sock** to be the master sock. If a sock is master sock,
        then node will for commands from the master socks"""
        self.sockets[sock]['type'] = 'master'

    # @property
    # def client_socks(self): abstract_method()
    @property
    def client_socks(self):
        """all sockets whose type is client"""
        return [sock for sock, v in self.sockets.iteritems() if v['type'] == 'client']

    #### Socket API ####
    def create_sock(self, desc):
        """create_sock with parameter specified by **desc**, **desc** can be the
        address and port it tries to connect and the type of the connection"""
        abstract_method()

    def bind(self, sock, port):
        """bind **sock** to **port**"""
        abstract_method()

    def listen(self, sock, backlog):
        """listen to the **sock**. **backlog** is the max number of connection
        it can maintain"""
        abstract_method()

    def accept(self, sock):
        """accept from the **sock**, will return the accepted socket"""
        abstract_method()

    def recv(self, sock, bufsize, dispatcher, threaded=False):
        """receive some data from **sock**,
        - **bufsize** is the max size for the buffer.
        - **dispatecher** is a function handler that will manipulate the data
        - **threaded** if it is true a new thread wiill be created to avoid
            blocking the main process. Only real node suppport this. in sim
            Node and netns3 node, threading is not allowed.
        """
        abstract_method()

    def send(self, sock, data):
        """set **data** from sock."""
        abstract_method()

    def connect(self, sock, addr_port):
        """connect the **sock** to the server with address and port specfied by
        **addr_port** """
        abstract_method()

    def sendto(self, sock, data, addr, port):
        """send **data** from **sock** to **addr** and **port**, use UDP
        protocol"""
        abstract_method()

    def close(self, sock):
        """close the **sock**"""
        abstract_method()

    def close_socks(self, socks_seq_set, _type='client'):
        """**sock_seq_set** is the sequence of socks in this type of sock.
        for example, if _type = 'client', socks_seq_set = [0], then the first
        client sock need to be closed.
        """
        socks_set = [self.client_socks[i] for i in socks_seq_set]
        for sock in socks_set:
            if self.sockets[sock]['type'] == _type:
                del self.sockets[sock]

    #################################
    ###  Application Layer    #######
    #################################
    def ping(self, sock, data, threaded=False):
        """send ping traffic using **sock** and the server information is
        stored in **data**. A new thread will be created if the **threaded**
        is true
        """
        abstract_method()

    def ftp(self, sock, data, threaded=False):
        abstract_method()

    def icmp(self, sock, data, threaded=False):
        abstract_method()

    def http(self, sock, data, threaded=False):
        abstract_method()

    def stop_app(self, sock, app_name):
        """stop the app run on **sock** with app_name"""
        abstract_method()

