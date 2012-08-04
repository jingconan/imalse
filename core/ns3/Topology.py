#!/usr/bin/env python
"""This is python implementation of NS3 topology-reader module
   pybindgen is far from mature and cannot generate usable python binding
   for this module, so I simply reimplement it in python.
"""
class Link(object):
    # def __init__(self, fromName, fromPtr, toName, toPtr):
    def __init__(self, fromPtr, fromName, toPtr, toName):
        self.m_fromName = fromName;
        self.m_fromPtr = fromPtr
        self.m_toName = toName
        self.m_toPtr = toPtr
        self.m_linkAttr = dict()

    def GetFromNode(self): return self.m_fromPtr
    def GetFromNodeName(self): return self.m_fromName
    def GetToNode(self): return self.m_toPtr
    def GetToNodeName(self): return self.m_toName
    def GetAttribute(self, name): pass
    def SetAttribute(self, name, value): pass

import ns.network
class TopologyReader(object):
    """base class for Topology Reader"""
    def __init__(self, fileName=None, NodeCreator=ns.network.Node):
        self.m_linksList= []
        self.m_nodeMap = dict()
        self.m_fileName = fileName
        self.NodeCreator = NodeCreator
    def LinksBegin(self):
        return self.m_linksList[0]
    def LinksEnd(self):
        return self.m_linksList[-1]
    def LinksEmpty(self):
        return True if not self.linkList else False
    def AddLink(self, link):
        self.m_linksList.append(link)
    def Read(self):
        raise Exception('NotImplemented')
    def SetFileName(self, fileName):
        self.m_fileName = fileName

    def LinksSize(self):
        return len(self.m_linksList)

class InetTopologyReader(TopologyReader):
    """reader for `inet topology generator file<http://topology.eecs.umich.edu/inet/>`_
    """
    def Read(self):
        print 'Start to read InetTopologyReader file...'
        nodes = ns.network.NodeContainer()
        fid = open(self.m_fileName, 'r')
        i = -1
        while True:
            i += 1
            line = fid.readline()
            if not line: break
            if i == 0:
                totnode, totlink = [int(s.strip()) for s in line.rsplit(' ')]
                continue
            if i <= totnode: # ignore the positive information
                continue

            _from, _to, _lineBuffer = [s.strip() for s in line.rsplit('\t')]

            def get_or_create(name):
                try:
                    node = self.m_nodeMap[name]
                except KeyError:
                    node = self.NodeCreator()
                    nodes.Add(node)
                    self.m_nodeMap[name] = node

                return node

            _fromNode = get_or_create(_from)
            _toNode = get_or_create(_to)

            # link = Link(_from, _fromNode, _to, _toNode)
            link = Link(_fromNode, _from, _toNode, _to)
            self.AddLink(link)

        print 'finish scanning topology, there are [%i] nodes'%(nodes.GetN())
        return nodes

class OrbisTopologyReader(TopologyReader):
    pass

class RocketfuelTopologyReader(TopologyReader):
    pass

topoMap = {
        'Inet': InetTopologyReader,
        'Orbis': OrbisTopologyReader,
        'Rocketfuel': RocketfuelTopologyReader,
        }

class TopologyReaderHelper(object):
    def SetFileName(self, fileName):
        self.m_fileName = fileName
        self.NodeCreator = ns.network.Node

    def SetFileType(self, fileType):
        self.m_fileType = fileType

    def SetNodeCreator(self, NodeCreator):
        self.NodeCreator = NodeCreator

    def GetTopologyReader(self):
        return topoMap[self.m_fileType](self.m_fileName, self.NodeCreator)

# from Topology import TopologyReaderHelper
from ns.nix_vector_routing import Ipv4NixVectorHelper
from ns.network import NetDeviceContainer
from ns.point_to_point import PointToPointHelper
from ns.network import Ipv4Address, Ipv4Mask
from ns.network import NodeContainer
from ns.internet import Ipv4InterfaceContainer, Ipv4InterfaceAddress
from ns.core import ofstream
import ns3
import sys
class TopologyNet():
    def __init__(self, _input, _format, NodeCreator, *args, **kwargs):
        self._input = _input
        self._format = _format
        self.NodeCreator = NodeCreator
        self.load_file()
        self.install_stack()
        self.init_link()
        self.init_net_device(*args, **kwargs)

        # create routing table
        ns3.Ipv4GlobalRoutingHelper.PopulateRoutingTables()

    def load_file(self):
        self.inFile, self.nodes = self._load_file(self._input, self._format, self.NodeCreator)

    def install_stack(self):
        self._install_stack(self.nodes)

    def init_link(self):
        self.linksC = self._init_link(self.inFile)

    def init_net_device(self, *args, **kwargs):
        self.ndc, self.ipic = self._init_net_device(self.inFile, self.linksC, *args, **kwargs)

    @staticmethod
    def _load_file(_input, _format, NodeCreator):
        topoHelp = TopologyReaderHelper()
        topoHelp.SetFileName(_input)
        topoHelp.SetFileType(_format)
        topoHelp.SetNodeCreator(NodeCreator)
        inFile = topoHelp.GetTopologyReader()
        nodes = inFile.Read()
        if inFile.LinksSize() == 0:
            print "fail to read file"
            sys.exit(1)
        return inFile, nodes

    @staticmethod
    def _install_stack(nodes):
        """install the network stack for all nodes"""
        stack = ns.internet.InternetStackHelper()
        nixRouting = Ipv4NixVectorHelper()
        staticRouting = ns.internet.Ipv4StaticRoutingHelper()
        # olsr = ns3.OlsrHelper()


        listRH = ns.internet.Ipv4ListRoutingHelper()
        listRH.Add(staticRouting, 0)
        listRH.Add(nixRouting, 10)
        # listRH.Add(olsr, 10)

        # stack.SetRoutingHelper(listRH)
        stack.Install(nodes)

    @staticmethod
    def _init_link(inFile):
        """Initialize the Network Link
        return the container contains all the links.
        """
        nc = []
        for link in inFile.m_linksList:
            con = NodeContainer()
            con.Add(link.GetFromNode())
            con.Add(link.GetToNode())
            nc.append( con )
        return nc

    @staticmethod
    def _init_net_device(inFile, linksC,
            Delay='2ms', DataRate='5Mbps',
            ipv4NetworkBase="10.0.0.0", ipv4Mask="255.255.255.252", ipv4AddrBase="10.0.0.1", *args, **kwargs):
        """ initialize the p2p link and assign the ip address.
        """
        totlinks = inFile.LinksSize()
        p2p = PointToPointHelper()
        ndc = [] # Net Device Container
        for i in xrange(totlinks):
            p2p.SetChannelAttribute("Delay", ns.core.StringValue(Delay))
            p2p.SetDeviceAttribute("DataRate", ns.core.StringValue(DataRate))
            ndc.append( p2p.Install( linksC[i]) )

        # Create little subnets, one for each couple of nodes
        mask = Ipv4Mask(ipv4Mask)
        base=Ipv4Address("0.0.0.1") # FIXME
        assert(ipv4AddrBase[-1]=='1')
        address = ns.internet.Ipv4AddressHelper()
        address.SetBase(
                network=Ipv4Address(ipv4NetworkBase),
                mask = mask,
                base = base
                # mask=Ipv4Mask(ipv4Mask),
                # base=Ipv4Address(ipv4AddrBase)
                # base=Ipv4Address('0.0.0.1')
                )
        # print ipv4NetworkBase
        # print ipv4Mask
        # print ipv4AddrBase

        ipic = [] #ip interface container
        for i in xrange(totlinks):
            ipic.append( address.Assign(ndc[i]) )
            address.NewNetwork()

        # _ascii = ofstream("wifi-ap.tr")
        # p2p.EnableAsciiAll("test")

        return ndc, ipic

def main():
    topoHelper = TopologyReaderHelper()
    topoHelper.SetFileType('Inet')
    # topoHelper.SetFileName('../../net_config/Inet_small_toposample.txt')
    topoHelper.SetFileName('../../net_config/Inet_toposample.txt')
    topoReader = topoHelper.GetTopologyReader()
    nodes = topoReader.Read()
    print 'nodes, ', nodes
    print 'node number, ', nodes.GetN()

if __name__ == "__main__":
    main()
