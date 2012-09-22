from __future__ import print_function
try:
    from Experiment import ImalseNetnsExperiment
    from Node import ImalseNetnsNode
except Exception as e:
    import inspect
    filename, linenum, funcname = inspect.getframeinfo(inspect.currentframe())[:3]
    print('[%s %s %s] Netns mode not supported'%(filename, linenum, funcname))

print('run here 1')
from Experiment import ImalsePureSimExperiment
print('run here 2')
from Node import ImalseNetnsSimNode
print('run here 3')
from Topology import TopologyNet
print('run here 4')
