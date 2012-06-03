#!/usr/bin/env python
"""
This is the main file of the
Integrated MALware Simulator & Emulator
imalse
prou: `I'm sir`
"""
import settings
from scenario.simple_ddos import ServerCommand, ClientCommand

class Core:
    pass

core = Core()
scmd = ServerCommand(core)
ccmd = ClientCommand(core)

scmd.get_client_id()

