from ClientCMD import ClientCMD
from ServerCMD import ServerCMD
import sys; sys.path.append("../..")
from core.BotMaster import BotMasterTest as BotMaster
from core.Node import PhyNode as Node

client_info = {
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
    }

client_fsm = {
        # 'waiting -> send_ack' : 'request_conn',
        # 'waiting -> send_cmd' : 'get_cmd',
        'initial' : 'waiting',
        'start_action' : 'request_connect',
        'info' : client_info,
        }
keywords = ['initial', 'info']


server_info = {
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
    }

server_fsm = {
        # 'waiting -> send_ack' : 'request_conn',
        # 'waiting -> send_cmd' : 'get_cmd',
        'initial' : 'waiting',
        'start_action' : 'initialize',
        'info' : server_info,
        }
keywords = ['initial', 'info']



botmaster_info = {
        'srv_addr':'127.0.0.1',
        'srv_port':3333,
        'request_timeout':10,
    }

botmaster_fsm = {
        # 'waiting -> send_ack' : 'request_conn',
        # 'waiting -> send_cmd' : 'get_cmd',
        'initial' : 'waiting',
        'start_action' : 'request_connect',
        'info' : botmaster_info,
        }
keywords = ['initial', 'info']



