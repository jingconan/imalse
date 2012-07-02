#!/usr/bin/env python
import logging
import json
from urlparse import parse_qs
keywords = ['initial', 'info']

def fsm_get_states(fsm_desc):
     return list( set( state.strip()  for k in fsm_desc.keys() for state in k.rsplit('->') ) - set(keywords) )

class CMD:
    """Command Meta Description"""
    def __init__(self, fsm_desc=None):
        self._load_fsm(fsm_desc)
        self._set_logger()

    def _set_logger(self):
        logging.basicConfig()
        self.logger = logging.getLogger(self.name)
        self.logger.setLevel(logging.DEBUG)

    def _load_fsm(self, fsm_desc):
        self.fsm_desc = fsm_desc
        self.state_set = fsm_get_states(fsm_desc)
        self.state = self.fsm_desc['initial']

    def _trigger(self, event_name, *argv, **kwargv):
        """trigger an event, event hander is a class member function"""
        self.logger.debug('%s has been triggered'%(event_name))
        getattr(self, event_name)(*argv, **kwargv)

    def dispatcher(self, sock, data):
        self.logger.debug('dispatcher recv data' + data)
        dt_data = self._load_json(data)
        event_name = dt_data['event'][0]
        del dt_data['event']
        self._trigger(event_name, sock, dt_data)


    def _cmd_to_json(self, cmd_str): return json.dumps(parse_qs(cmd_str))
    def _dump_json(self, data): return json.dumps(data)
    def _load_json(self, js): return json.loads(js)

    def install(self, node):
        if self._is_okay(node):
            pass
        self.node = node
        # node.cmd = self
        node.cmd_set = self

    def start(self):
        self._trigger(self.fsm_desc['start_action'])

    def get_state(self): return self.node.state
    def set_state(self, val): self.node.set_state(val)
    state = property(get_state, set_state)

    def export(self, pic_name):
        from fsm import State, FiniteStateMachine, get_graph
        cmd = FiniteStateMachine(self.name)

        # create states
        states_name_set = fsm_get_states(self.fsm_desc)
        states = dict()
        for name in states_name_set:
            initial = True if ( name == self.fsm_desc['initial'] ) else False
            states[name] = State(name, machine=cmd, initial=initial)

        # create transitions
        for k, v in self.fsm_desc.iteritems():
            if k in keywords:
                continue
            s, ns = [ state.strip() for state in k.rsplit('->') ]
            states[s][v] = states[ns]

        get_graph(cmd).draw(pic_name, prog='dot')

