from core.ns3 import ImalseNetnsExperiment, ImalsePureSimExperiment

mode_map = {
        'netns3':ImalseNetnsExperiment,
        'sim':ImalsePureSimExperiment,
        }

import settings

def experiment_factory(experiment, mode):
    BaseClass = mode_map[mode]
    exper_fname = settings.ROOT + '/experiments/' + experiment + '.py'
    execfile(exper_fname, locals())
    return locals()[experiment]
