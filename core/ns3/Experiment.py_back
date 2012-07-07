from netns3 import *
from Node import *
from csma_ping import *

def load_module(scenario):
    __import__('scenario.%s'%(scenario))
    scen = sys.modules['scenario.%s'%(scenario)]
    return scen

# for test, just select the first node as server, the second node as botmaster, all other nodes
# are clients
# class ImalseExperiment(NetnsExperiment):
class ImalseExperiment(PingCsmaExperiment):
    # def createnet(self, devhelper, nodecontainer, addrhelper, mask, ifnames = []):
        # super(ImalseExperiment, self).createnet(devhelper,
                # nodecontainer, addrhelper,
                # mask, ifnames )
    def initparser(self, parser):
        CsmaExperiment.initparser(self, parser)
        parser.set_defaults(simtime = 10, pingcount = 3)
        parser.add_option("-c", "--pingcount", dest = "pingcount",
                          help = "ping count; default = %s" %
                          parser.defaults["pingcount"])
        parser.add_option('-s', '--scenario', dest="scenario",
                default='None',
                # help='specify the scenario you want to execute. Scenearios availiable are: %s'%(scenario_ops),
                )
        # parser.add_option("-r", '--role', default='None',
                # help='specify the role you want to emulate, 1.[server], 2.[client], 3.[botmaster]',
                # )

    def _install_cmds(self):
        scen = load_module(self.options.scenario)
        botmaster_id_set = [1]
        server_id_set = [0]
        for i in xrange(len(self.nodes)):
            if i in botmaster_id_set:
                cmd = scen.ClientCMD()
            elif i in server_id_set:
                cmd = scen.ServerCMD()
            else:
                cmd = scen.BotMaster()
            cmd.install(self.nodes[i])

    def createnodes(self, numnodes, devhelper, prefix = "10.0.0.0/8",
                    nodenum = 0):
        addrhelper, mask = parseprefix(prefix)
        nc = ns.network.NodeContainer()
        for i in xrange(numnodes):
            name = "n%s" % nodenum
            n = ImalseNetnsNode(name, logfile = "/tmp/%s.log" % name)
            self.nodes.append(n)
            nc.Add(n)
            nodenum += 1
        self.createnet(devhelper, nc, addrhelper, mask)

        self._install_cmds()

    def node_run(self, node, *args, **kwargs):
        node.cmd_set._trigger(*args, **kwargs)

    def setup(self):
        print 'this is setup of experiment'
        CsmaExperiment.setup(self)
        for n in self.nodes:
            self.event(0, self.config, n)
        if not self.nodes:
            return
        for i in xrange(len(self.nodes)):
            print 'node [%i] start at [%f]s'%(i, i)
            self.event(i+1, self.node_run, self.nodes[i], 'start') # start server
            # self.event(1, self.nodes[0], 'start') # start server
            # self.event(1, self.nodes[1], 'start') # start botmaster
            # self.event(1, self.nodes[1], 'start') # start botmaster

        # if self.nodes:
            # self.event(1, self.ping, self.nodes[0],
                       # "255.255.255.255", self.options.pingcount)


