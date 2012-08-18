from Experiment import ImalseExperiment
from ImalsePureSimExperiment import ImalsePureSimExperiment

try:
    from ImalseNetnsExperiment import ImalseNetnsExperiment
except:
    print('NetnsExperiment cannot be imported')
