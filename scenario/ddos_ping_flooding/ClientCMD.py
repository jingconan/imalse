#!/usr/bin/env python
# ClientCommand
from jedie_python_ping.ping import verbose_ping
import sys
sys.path.append("../..")
import core
import threading

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



class ClientCMD(core.ClientCMD):
    def __init__(self, client_fsm):
        core.ClientCMD.__init__(self, client_fsm)
        self.pings = dict()

    def send_ping(self, sock, data):
        ping_th = PingThread(data)
        self.pings[sock] = ping_th
        ping_th.run()

    def stop_ping(self, sock):
        self.pings[sock].stop()
        del self.pings[sock]

if __name__ == "__main__":
    cmd = ClientCMD(client_fsm)
    node = core.PhyNode()
    cmd.install(node)
    node.start()
    pass
