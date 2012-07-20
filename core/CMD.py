#!/usr/bin/env python
import logging
import json
from urlparse import parse_qs

def augment_bracket(string):
    if not string.endswith('}'):
        return string + '}'
    if not string.startswith('{'):
        return '{' + string
    return string

def split_data(data):
    split = data.rsplit('}{')
    if len(split) == 1: return split
    return [augment_bracket(s) for s in split]

class CMD:
    """Command Meta Description"""
    def __init__(self, desc=None):
        self.desc = desc
        self._set_logger()

    def _set_logger(self):
        logging.basicConfig()
        self.logger = logging.getLogger(self.name)
        self.logger.setLevel(logging.DEBUG)

    def _trigger(self, event_name, *argv, **kwargv):
        """trigger an event, event hander is a class member function"""
        self.logger.debug('%s has been triggered'%(event_name))
        getattr(self, event_name)(*argv, **kwargv)

    def dispatcher(self, sock, data):
        """data may contain on or more command"""
        self.logger.debug('dispatcher recv data' + data)
        print 'data, ', data
        print 'data, type', type(data)
        if not data:
            return
        s_data = split_data(data)
        print 's_data, ', s_data
        if len(s_data) > 1:
            print 'length of s_data, ', len(s_data)
            for one_data in s_data:
                self.dispatcher(sock, one_data)
        else:
            data = s_data[0]

        dt_data = self._load_json(data)
        if isinstance(dt_data['event'], list):
            event_name = dt_data['event'][0]
        elif isinstance(dt_data['event'], str) or isinstance(dt_data['event'], unicode):
            event_name = dt_data['event']
        else:
            print type(dt_data['event'])
            raise Exception('Unknown event type')
        del dt_data['event']
        print 'event_name', event_name
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

if __name__ == "__main__":
    docs = """{"host": "thales.bu.edu", "password": "", "user": "anonymous", "event": "set_ftp_info"}{"pattern": "assword", "suffix": [".txt"], "event": "set_file_filter"}"""
    s_data = split_data(docs)
    print s_data

