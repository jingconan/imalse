#!/usr/bin/env python
# node_type = 'Server'
node_type = 'client'
scenario = 'simple_ddos'

# exec( 'import scenario.%s as scen'%(scenario) )
import sys
__import__('scenario.%s'%(scenario))
scen = sys.modules['scenario.%s'%(scenario)]

# class Server(Node):
#     def __init__(self):
#         scmd = scen.ServerCommand()
#         scmd.install(self)
#         attack = scen.Attack('simple_ddos.adesc')
#         attack.install(self)
#         self.start_server()

# class Client(Node):
#     def __init__(self):
#         ccmd = scen.ClientCommand()
#         ccmd.install(self)
#         self.start_client()

class Node:
    def start(self):
        if 'cmd' not in self.__dict__:
            print "cmd doesn't exist"
            print self.__dict__
            exit()
        print 'self.cmd', self.cmd
        print 'start'

def run_server():
    server = Node()
    scmd = scen.ServerCommand()
    scmd.install(server)
    attack = scen.Attack('simple_ddos.adesc')
    attack.install(server)
    server.start()

def run_client():
    client = Node()
    ccmd = scen.ClientCommand()
    ccmd.install(client)
    client.start()

exec('run_%s()'%(node_type))
