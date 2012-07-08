#!/usr/bin/env python
import unittest
import os, sys
sys.path.insert(0, os.path.abspath("../../"))
import core.ns3.Node
class NS3TestCase(unittest.TestCase):
    def setUp(self):
        self.node = core.ns3.Node.ImalseNetnsSimNode()

    # @unittest.skip("demonstrating skipping")
    def test_create_sockt(self):
        self.node.create_sock({'type':'server', 'proto':'tcp'});

if __name__ == "__main__":
    unittest.main()


