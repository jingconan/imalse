try:
    from Experiment import ImalseNetnsExperiment
    from Node import ImalseNetnsNode
except Exception as e:
    import inspect
    filename, linenum, funcname = inspect.getframeinfo(inspect.currentframe())[:3]
    print('[%s %s %s] Netns mode not supported'%(filename, linenum, funcname))

from Experiment import ImalsePureSimExperiment
from Node import ImalseNetnsSimNode
from Topology import TopologyNet
