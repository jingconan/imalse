from __future__ import print_function
print('run here 11')
from Experiment import ImalseExperiment
print('run here 12')
from ImalsePureSimExperiment import ImalsePureSimExperiment
print('run here 13')

try:
    from ImalseNetnsExperiment import ImalseNetnsExperiment
except:
    print('NetnsExperiment cannot be imported')
