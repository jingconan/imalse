try:
    from core.ns3 import ImalseNetnsExperiment
except:
    print 'ImalseNetnsExperiment cannot be imported'
    class ImalseNetnsExperiment:
        def print_help(self):
            print 'Netns mode is not support'

from core.ns3 import ImalsePureSimExperiment

class Basic(object):
    def initparser(self, parser): pass
    def _install_cmds(srv_addr): pass

mode_map = {
        'netns3':ImalseNetnsExperiment,
        'sim':ImalsePureSimExperiment,
        'basic':Basic,
        }

import settings

def experiment_factory(experiment, mode):
    if isinstance(mode, str):
        BaseClass = mode_map[mode]
    else:
        BaseClass = mode
    exper_fname = settings.ROOT + '/experiments/' + experiment + '.py'
    execfile(exper_fname, locals())
    return locals()[experiment]
