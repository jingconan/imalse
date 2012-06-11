import socket
import time
# import Timer
import threading
class Node:
    """Node can maintain a lot of sockets
    abstraction for physical
    """
    def __init__(self):
        self.sockets = dict()

    def start(self):
        self.cmd.start()

    def set_state(self, val):
        lock = threading.Lock()
        lock.acquire()
        self.state = val
        lock.release()

    def set_master_sock(self, sock):
        self.sockets[sock]['type'] = 'master'

    @property
    def client_socks(self):
        # print self.soc
        # return [v for sock, v in self.sockets.iteritems()]
        return [sock for sock, v in self.sockets.iteritems() if v['type'] == 'client']

    # physical
    @property
    def now(self): return time.time()

    def sleep(self, t, call_back=None):
        time.sleep(t)
        if call_back: call_back()

    def create_timer(self, t, call_back):
        t = threading.Timer(t, call_back)
        t.start()

    # Network Interface
    def create_server_socket(self):
        server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sockets[server_sock] =  {'type':'server'}
        return server_sock

    def create_client_socket(self):
        client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sockets[client_sock] =  {'type':'client'}
        return client_sock

    # for stateless socket
    def create_udp_socket(self):
        client_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock = { client_sock : {'type':'udp'} }
        self.sockets.append(sock)
        return sock

    def sock_bind(self, sock, port):
        assert( self.sockets[sock]['type'] == 'server')
        sock.bind(("", port))

    def sock_listen(self, sock, backlog):
        assert( self.sockets[sock]['type'] == 'server')
        sock.listen(backlog)

    def sock_accept(self, sock):
        assert( self.sockets[sock]['type'] == 'server')
        client_sock, address = sock.accept()
        self.sockets[client_sock] = {
                'addr': address,
                'type':'client'
                }
        return client_sock, address

    def sock_recv(self, sock, bufsize, dispatcher):
        while True:
            data = sock.recv(bufsize)
            if not data: continue
            if dispatcher(sock, data) == 'fail':
                print 'sock_recv dispater fail, data, ', data
                break

    def sock_thread_recv(self, sock, bufsize, dispatcher):
        recv_th = threading.Thread(target = self.sock_recv,
                args=(sock, bufsize, dispatcher))
        recv_th.daemon = False
        recv_th.start()

    def sock_send(self, sock, data):
        sock.send(data)

    def sock_connect(self, sock, addr_port):
        try:
            sock.connect(addr_port)
            data = sock.recv(512)
            if data == 'connect_ack':
                return 'recv_ack'
            else:
                raise Exception('unknow info from server')
        except socket.error as e:
            self.sock_close(sock)
            print e
            return 'connection_refused'
        except socket.timeout as e:
            print e
            return 'request_timeout'

    def sock_sendto(self, sock, data, addr, port):
        sock.sendto(data, (addr, port))

    def sock_close(self, sock):
        sock.close()
        del self.sockets[sock]

