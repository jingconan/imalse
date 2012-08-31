#!/usr/bin/env python
"""
This is a command line tool which let NS3 load fs dot format
configuration file and generate pcap data file
"""
from __future__ import print_function

import argparse
from NS3Config import run_ns3
import settings
parser = argparse.ArgumentParser(description='dotns3 \n usage ./sim <dot file>')
parser.add_argument('-t', '--simtime', type=int, nargs='+', default=1000,
                           help='total simulation time')
parser.add_argument('-r', '--trace', help='file specify the trace', default='{}')
parser.add_argument('-v', '--visual', action='store_true', help='visualization switcher')

args, res = parser.parse_known_args()
if len(res) == 0:
    print('usage ./sim <dot file>')
    import sys
    sys.exit(1)
if args.trace.endswith('.py'):
    fid = open(settings.ROOT + '/' + args.trace, 'r')
    text = fid.read()
    fid.close
else:
    text = args.trace
trace_config = eval(text)

# print('trace_config, ', trace_config)
run_ns3(res[0], trace_config, args.simtime, args.visual)
