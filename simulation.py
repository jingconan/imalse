#!/usr/bin/env python
"""
This is the main file of the
Integrated MALware Simulator & Emulator
imalse
prou: `I'm sir`
"""
# import settings
import sys; sys.path.append(".")
from core import Core
from scenario.simple_ddos import ServerCommand, ClientCommand, Attack
from net_adapter.ns3 import Network

core = Core()
core.mode = 'simulate'

net = Network('./net_config/x.dot')
net.install(core)

# install the command set to core
scmd = ServerCommand()
scmd.install(core)

ccmd = ClientCommand()
ccmd.install(core)


# install the attack to core
attack = Attack('simple_ddos.adesc')
attack.install(core)


core.simulate()
