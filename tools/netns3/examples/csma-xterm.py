#!/usr/bin/env python

# Create a N node CSMA network and launch xterms for shell access to
# the first and last nodes.

from netns3 import *

class XtermCsmaExperiment(CsmaExperiment):

    def setup(self):
        CsmaExperiment.setup(self)
        if len(self.nodes) > 0:
            self.event(0, self.nodes[0].xterm, "bash")
        if len(self.nodes) > 1:
            self.event(0, self.nodes[-1].xterm, "bash")

if __name__ == "__main__":
    x = XtermCsmaExperiment()
    x.main()
