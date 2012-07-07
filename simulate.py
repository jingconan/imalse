#!/usr/bin/env python
"""
This is the main file of the
Integrated MALware Simulator & Emulator
imalse
prou: `I'm sir`
"""
# Create a Experiment
import sys
import os
sys.path.append(os.getcwd())
from core.ns3.Experiment import *
from util import *

if len(sys.argv) == 1:
    os.system('./simulate.py -h')
    sys.exit(1)

x = ImalseExperiment()
x.main()

# Schedule all the events

