#!/usr/bin/env python
"""test all experiments and scenarios"""
from subprocess import check_call, Popen
from util import get_experiment_option, get_scenario_option

exper_options = get_experiment_option()
scenario_option =  get_scenario_option()
skip_expr = ['ImalseTopoHybridExperiment', 'ImalseHybridExperiment']
skip_scen = ['ddos_ping_flooding', 'infecting']

fid = open ('test.out', 'w')
for exper in exper_options:
    for scenario in scenario_option:
        if exper in skip_expr or scenario in skip_scen:
            print >>fid, 'skipping experiment: [%s], scenario:[%s]'%(exper, scenario)
            continue
        print >>fid, '-' * 100
        print >>fid, 'testing experiment: [%s], scenario:[%s]'%(exper, scenario)
        print >>fid, '-' * 100
        check_call(['./simulate.py', '-e', exper, '-s', scenario])
        # Popen(['./simulate.py', '-e', exper, '-s', scenario])
fid.close()
