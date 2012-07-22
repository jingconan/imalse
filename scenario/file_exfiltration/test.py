#!/usr/bin/env python
import sys
sys.path.insert(0, '../../')
import unittest

from ClientCMD import *
class ClientCMDTestCase(unittest.TestCase):
    @unittest.skip("demonstrating skipping")
    def test_search_files(self):
        ftp = {
            'host':'thales.bu.edu',
            'user':'imalse-ftp',
            'password':'imalse',
            },
        file_filter = {
            'suffix':['.txt', '.py'],
            'pattern':'assword',
            'directory':'.'
            }
        cmd = ClientCMD(client_desc)
        node = core.real.PhyNode()
        cmd.install(node)
        # node.start()
        cmd.set_file_filter([], file_filter)
        cmd.set_ftp_info([], ftp)
        interesting_files = cmd._search_files(**file_filter)
        print 'interesting_files', interesting_files

    def test_search_and_upload(self):
        ftp = {
            'host':'thales.bu.edu',
            'user':'imalse-ftp',
            'password':'imalse',
            }
        file_filter = {
            'suffix':['.txt', '.py'],
            'pattern':'assword',
            'directory':'.'
            }
        cmd = ClientCMD(client_desc)
        node = core.real.PhyNode()
        cmd.install(node)
        cmd.set_file_filter([], file_filter)
        cmd.set_ftp_info([], ftp)
        cmd.search_and_upload([], [])

if __name__ == "__main__":
    unittest.main()
