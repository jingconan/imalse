#!/usr/bin/env python
import sys
sys.path.append("..")
from scenario.base import *

from lumpy import Lumpy
lumpy = Lumpy.Lumpy()
lumpy.make_reference()

cmd = ClientCMD(client_fsm)
node = Node()
cmd.install(node)

lumpy.object_diagram()
lumpy.class_diagram()


