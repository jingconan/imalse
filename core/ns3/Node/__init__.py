#!/usr/bin/env python
"""
This module defined the for NS3 node.
two types of nodes are defined:
    1. **ImalseNetnsNode**, used for netns3 simulation.
    2. **ImalseNetnsSimNode**, used for ns3 pure simulation.
"""
try:
    from ImalseNetnsNode import ImalseNetnsNode
except:
    print('ImalseNetnsNode cannot be imported')

from ImalseNetnsSimNode import ImalseNetnsSimNode


