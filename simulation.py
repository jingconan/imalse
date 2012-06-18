#!/usr/bin/env python
"""
This is the main file of the
Integrated MALware Simulator & Emulator
imalse
prou: `I'm sir`
"""
# import settings
import core

net = Network('./net_config/x.dot')
net.install(core)

# install the command set to core
scmd = ServerCMD()
scmd.install(core)

ccmd = ClientCMD()
ccmd.install(core)


# install the attack to core
attack = Attack('simple_ddos.adesc')
attack.install(core)


core.simulate()
