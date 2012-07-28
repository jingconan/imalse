from util import get_experiment_option
files = get_experiment_option()
for f in files:
    exec('from %s import *'%(f))

