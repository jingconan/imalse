#!/usr/bin/env python
"""
This is the main file of the
simulator part of
Integrated MALware Simulator & Emulator
imalse
prou: `I'm sir`
"""
import sys
import argparse

try:
    import experiments
except ImportError as e:
    print 'error, ', e
    print 'You should use ns3 waf shell to run this code'
    sys.exit()
# import experiments

from util import get_experiment_option

exper_options = get_experiment_option()
parser = argparse.ArgumentParser(description='imalse')

parser.add_argument('-e', '--experiment', default='TopoExperiment',
        help='specify the experiment. Avaliable experiments are [%s]'%(" |").join(exper_options)
        )

parser.add_argument('--mode', default='sim',
        help="specify the mode, can be ['netns3' | 'sim'], default is 'sim'. "
        )
parser.add_argument('--ehelp', action='store_true',
        help="print the help of a experiment"
        )

# Print Help when there is no argument
if len(sys.argv) == 1:
    print '----------------Experment Selection Arguments------------------'
    parser.print_help()
    print '----------------Exeriment Options------------------------------'
    for experiment in exper_options:
        for mode in ['netns3','sim']:
            print '+++++++++Experiment:[%s] Mode:[%s]+++++++++++++++++++++++'%(experiment, mode)
            exper_class = experiments.experiment_factory(experiment, mode)
            exper = exper_class()
            exper.print_help()

    sys.exit(1)

args, exper_args = parser.parse_known_args()
exper_class = experiments.experiment_factory(args.experiment, args.mode)
exper = exper_class()

if args.ehelp:
    print '+++++++++Experiment:[%s] Mode:[%s]+++++++++++++++++++++++'%(args.experiment, args.mode)
    exper.print_help()
    sys.exit(1)





exper.main(exper_args)
