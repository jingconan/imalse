#!/usr/bin/env python
keywords = ['initial', 'info']

def fsm_get_states(fsm_desc):
     return list( set( state.strip()  for k in fsm_desc.keys() for state in k.rsplit('->') ) - set(keywords) )

class CMD:
    """Command Meta Description"""
    def __init__(self, name='cmd', fsm_desc=None):
        self._load_fsm(fsm_desc)
        self.name = name

    def _load_fsm(self, fsm_desc):
        self.fsm_desc = fsm_desc
        self.state_set = fsm_get_states(fsm_desc)
        self.state = self.fsm_desc['initial']

    def _trigger(self, event_name, *argv, **kwargv):
        """trigger an event, event hander is a class member function"""
        getattr(self, event_name)(*argv, **kwargv)

    def install(self, node):
        if self._is_okay(node):
            pass
        self.node = node
        node.cmd = self

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

