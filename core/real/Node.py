import socket, time, threading
from jedie_python_ping.ping import verbose_ping

class StoppableThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.stop_event = threading.Event()

    def stop(self):
        if self.isAlive() == True:
            # set event to signal thread to terminate
            self.stop_event.set()
            # block calling thread until thread really has terminated
            self.join()

class PingThread(StoppableThread):
    def __init__(self, para):
        self.para = para
        StoppableThread.__init__(self)

    def run(self):
        # verbose_ping(**self.para)
        ping_para = dict()
        for k, v in self.para.iteritems():
            ping_para[k] = v[0]
        verbose_ping(**ping_para)


class PhyNode(object):
    """Node can maintain a lot of sockets
    abstraction for physical Node. Will use socket to send real data to the network
    """
    proto_map = {'udp':socket.SOCK_STREAM, 'tcp':socket.SOCK_STREAM}
    NODE_TYPE = 'real'

    def __init__(self):
        self.sockets = dict()
        self.pings = dict()

    def start(self):
        # self.cmd.start()
        self.cmd_set.start()

    # def set_state(self, val):
    #     lock = threading.Lock()
    #     lock.acquire()
    #     self.state = val
    #     lock.release()

    #################################
    ###  Some Utility Function    ###
    #################################
    @property
    def now(self): return time.time()

    def sleep(self, t, call_back=None):
        time.sleep(t)
        if call_back: call_back()

    def create_timer(self, t, call_back):
        t = threading.Timer(t, call_back)
        t.start()

    #################################
    ###  Network Related          ###
    #################################
    def set_master_sock(self, sock):
        self.sockets[sock]['type'] = 'master'

    @property
    def client_socks(self):
        return [sock for sock, v in self.sockets.iteritems() if v['type'] == 'client']

    def create_sock(self, desc):
        sock = socket.socket(socket.AF_INET, self.proto_map[desc['proto']])
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sockets[sock] = desc
        return sock

    # def bind(self, sock, port):
    def bind(self, sock, addr_port):
        sock.bind(addr_port)
        # sock.bind(("", port))

    def listen(self, sock, backlog):
        sock.listen(backlog)

    def accept(self, sock):
        assert( self.sockets[sock]['type'] == 'server')
        client_sock, address = sock.accept()
        self.sockets[client_sock] = {
                'addr': address,
                'type':'client'
                }
        return client_sock, address

    def recv(self, sock, bufsize, dispatcher, threaded=False):
        if threaded:
            self._thread_recv(sock, bufsize, dispatcher)
        else:
            self._block_recv(sock, bufsize, dispatcher)

    def _block_recv(self, sock, bufsize, dispatcher):
        while True:
            data = sock.recv(bufsize)
            if not data: break
            if dispatcher(sock, data) == 'fail':
                print 'sock_recv dispater fail, data, ', data
                break

    def _thread_recv(self, sock, bufsize, dispatcher):
        try:
            recv_th = threading.Thread(target = self._block_recv,
                    args=(sock, bufsize, dispatcher))
            recv_th.daemon = True
            recv_th.start()
            # while True: time.sleep(100)
        except KeyboardInterrupt, SystemExit:
            print '\n! Receive Keyboard interrup, quitiing threads. \n'

    def send(self, sock, data):
        sock.send(data)

    def connect(self, sock, addr_port):
        try:
            sock.connect(addr_port)
            data = sock.recv(512)
            if data == 'connect_ack':
                return 'recv_ack'
            else:
                raise Exception('unknow info from server')
        except socket.error as e:
            self.close_sock(sock)
            print e
            return 'connection_refused'
        except socket.timeout as e:
            print e
            return 'request_timeout'

    def sendto(self, sock, data, addr, port):
        sock.sendto(data, (addr, port))

    def close_sock(self, sock):
        sock.close()
        del self.sockets[sock]

    def ping(self, sock, data, threaded=False):
        if threaded:
            ping_th = PingThread(data)
            self.pings[sock] = ping_th
            ping_th.run()
        else:
            ping_para = dict()
            for k, v in data.iteritems():
                ping_para[k] = v[0]
            verbose_ping(**ping_para)

    def stop_app(self, sock, app_name):
        if app_name == 'ping':
            self.pings[sock].stop()
            del self.pings[sock]

    def dispatcher(self, sock, data):
        self.cmd_set.dispatcher(sock, data)
        pass

