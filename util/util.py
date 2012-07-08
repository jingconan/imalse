def abstract_method():
    """ This should be called when an abstract method is called that should have been
    implemented by a subclass. It should not be called in situations where no implementation
    (i.e. a 'pass' behavior) is acceptable. """
    raise NotImplementedError('Method not implemented!')

class DataEndException(Exception):
    pass

import sys
def load_module(scenario):
    __import__('scenario.%s'%(scenario))
    scen = sys.modules['scenario.%s'%(scenario)]
    return scen

import os
def get_scenario_option(scen_dir = './scenario/'):
    return [f_name for f_name in os.listdir(scen_dir) if not \
            ( f_name.lower().endswith('py') or f_name.lower().startswith('.') or f_name.lower().endswith('pyc'))
            ]
