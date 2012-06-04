#!/usr/bin/env python
# node_type = 'Server'
node_type = 'Client'

from core import Core
from scenario.simple_ddos import ServerCommand, ClientCommand, Attack
from net_adapter.real import Network

core = Core()
core.mode = 'emulate'
core.node_type = node_type

net = Network()
net.install(core)

scmd = ServerCommand()
scmd.install(core)

ccmd = ClientCommand()
ccmd.install(core)

if node_type == 'Server':
    attack = Attack('simple_ddos.adesc')
    attack.install(core)
    core.start_server()
elif node_type == 'Client':
    core.start_client()
