#!/usr/bin/env python
"""This is python implementation of NS3 topology-reader module
   pybindgen is far from mature and cannot generate usable python binding
   for this module, so I simply reimplement it in python.
"""
from __future__ import division, print_function

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


def argsort(seq):
    return sorted(range(len(seq)), key=seq.__getitem__)

class InetTopologyReader(TopologyReader):
    """reader for `inet topology generator file <http://topology.eecs.umich.edu/inet/>`_
    """
    def Read(self):
        print('Start to read InetTopologyReader file...')
        nodes = ns.network.NodeContainer()
        fid = open(self.m_fileName, 'r')
        i = -1
        node_name_list = []
        while True:
            i += 1
            line = fid.readline()
            if not line: break
            if i == 0:
                totnode, totlink = [int(s.strip()) for s in line.rsplit()]
                continue
            if i <= totnode: # ignore the position information
                continue

            _from, _to, _lineBuffer = [s.strip() for s in line.rsplit()]

            def get_or_create(name):
                try:
                    node = self.m_nodeMap[name]
                except KeyError:
                    node = self.NodeCreator()
                    node_name_list.append(name)
                    nodes.Add(node)
                    self.m_nodeMap[name] = node

                return node

            _fromNode = get_or_create(_from)
            _toNode = get_or_create(_to)

            # link = Link(_from, _fromNode, _to, _toNode)
            link = Link(_fromNode, _from, _toNode, _to)
            self.AddLink(link)

        fid.close()

        print( 'finish scanning topology, there are [%i] nodes'%(nodes.GetN()) )

        # can only deal with case that node name is string that can be changed int, like '1', '2' ...
        node_id_list = [int(name) for name in node_name_list]
        assert( len(node_id_list) ==  max(node_id_list) + 1) # must be continous
        # sort the node according to node id
        sort_idx = argsort(node_id_list)
        order_nodes = ns.network.NodeContainer()
        for idx in sort_idx:
            order_nodes.Add(nodes.Get(idx))

        return order_nodes

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
# from ns.network import NetDeviceContainer
from ns.point_to_point import PointToPointHelper
from ns.network import Ipv4Address, Ipv4Mask
from ns.network import NodeContainer
# from ns.internet import Ipv4InterfaceContainer, Ipv4InterfaceAddress
import ns3
import sys
class TopologyNet():
    """Load Topology File and Contruct the Network Accordingly"""
    routing_helper_list = {
            'static':0,
            'nix':5,
            'olsr':10,
            }
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
        """Load Topology File"""
        self.inFile, self.nodes = self._load_file(self._input, self._format, self.NodeCreator)

    def install_stack(self):
        """Install Internet Stack"""
        stack = ns.internet.InternetStackHelper()
        nix = Ipv4NixVectorHelper()
        static = ns.internet.Ipv4StaticRoutingHelper()
        olsr= ns3.OlsrHelper()

        listRH = ns.internet.Ipv4ListRoutingHelper()
        for k, v in self.routing_helper_list.iteritems():
            listRH.Add(locals()[k], v)

        stack.SetRoutingHelper(listRH)
        stack.Install(self.nodes)

    def init_link(self):
        """Construct Point to Point Link in the Network"""
        self.linksC = self._init_link(self.inFile)

    def init_net_device(self, *args, **kwargs):
        """Initial the ip address and network devices"""
        self.ndc, self.ipic = self._init_net_device(self.inFile, self.linksC, *args, **kwargs)

    def set_pcap_trace():
        pass

    @staticmethod
    def _load_file(_input, _format, NodeCreator):
        topoHelp = TopologyReaderHelper()
        topoHelp.SetFileName(_input)
        topoHelp.SetFileType(_format)
        topoHelp.SetNodeCreator(NodeCreator)
        inFile = topoHelp.GetTopologyReader()
        nodes = inFile.Read()
        if inFile.LinksSize() == 0:
            print("fail to read file")
            sys.exit(1)
        return inFile, nodes

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

        ipic = [] #ip interface container
        for i in xrange(totlinks):
            ipic.append( address.Assign(ndc[i]) )
            address.NewNetwork()

        # from ns.core import ofstream
        # _ascii = ofstream("wifi-ap.tr")
        # p2p.EnableAsciiAll("test")

        return ndc, ipic


from util import get_net_addr, CIDR_to_subnet_mask
import settings
# import ns3
class ManualTopologyNet(TopologyNet):
    """Topology network with manual ip settings"""
    def get_link_name(self, i):
        link = self.inFile.m_linksList[i]
        link_name = (int(link.GetFromNodeName()), int(link.GetToNodeName()) )
        return link_name

    def get_link_attr(self, i):
        default = self.net_settings.link_attr_default
        return self.net_settings.link_attr.get(self.get_link_name(i), default)

    def set_trace(self):
        for n in self.net_settings.pcap_nodes:
            self.p2p.EnablePcap(settings.ROOT + "/res/trace-node", ns3.NodeContainer(self.nodes.Get(n)), 0)

        for l in self.net_settings.pcap_links:
            self.p2p.EnablePcap(settings.ROOT + "/res/trace-link", self.get_link_ndc(l))

    def get_link_ndc(self, i):
        if isinstance(i, tuple):
            return self.link_ndc_map[i]
        return self.link_ndc_map[self.get_link_name(i)]

    def search_ipi(self, addr):
        """search the ip interface from addr"""
        for ipi_1, ipi_2 in self.ipic:
            if str(ipi_1.GetAddress(0)) == addr:
                return ipi_1

            if str(ipi_2.GetAddress(0)) == addr:
                return ipi_2

            # print '-----------------'
            # print ipi_1.GetAddress(0)
            # print ipi_2.GetAddress(0)
            # print '-----------------'

    def search_node(self, addr):
        addr = addr.rsplit('/')[0]
        for i in xrange(self.nodes.GetN()):
            node = self.nodes.Get(i)
            ipv4 = node.GetObject(ns3.Ipv4.GetTypeId())
            # local_addr = ipv4.GetAddress (1, 0).GetLocal ();
            # import pdb;pdb.set_trace()
            for a in xrange(ipv4.GetNInterfaces()):
                for b in xrange(ipv4.GetNAddresses(a)):
                    local_addr = ipv4.GetAddress (a, b).GetLocal ();
                    if str(local_addr) == addr:
                        return node



    def init_net_device(self, net_settings, *args, **kwargs):
        """Initial the ip address and network devices"""
        self.net_settings = net_settings
        totlinks = self.inFile.LinksSize()
        p2p = ns3.PointToPointHelper()

        self.link_ndc_map = dict()
        for i in xrange(totlinks):
            Delay, DataRate = self.get_link_attr(i)
            p2p.SetChannelAttribute("Delay", ns.core.StringValue(Delay))
            p2p.SetDeviceAttribute("DataRate", ns.core.StringValue(DataRate))
            self.link_ndc_map[self.get_link_name(i)] = p2p.Install( self.linksC[i] )

        # Create little subnets, one for each couple of nodes
        defaultAddressHelper = ns3.Ipv4AddressHelper()
        defaultAddr, defaultNetBase, defaultMask = CIDR_to_subnet_mask(net_settings.ipv4_net_addr_base)
        netAddr = get_net_addr(defaultAddr, defaultMask)
        defaultAddressHelper.SetBase(
                network=ns3.Ipv4Address(defaultNetBase),
                mask = ns3.Ipv4Mask(defaultMask),
                base = ns3.Ipv4Address(netAddr),
                )

        addressHelper = ns3.Ipv4AddressHelper()
        ipic = [] #ip interface container
        for i in xrange(totlinks):
            ips = self.net_settings.link_to_ip_map.get(self.get_link_name(i), None)
            if not ips:
                # ipic.append( defaultAddressHelper.Assign(ndc[i]) )
                ipic.append( defaultAddressHelper.Assign(self.get_link_ndc(i)) )
                addressHelper.NewNetwork()
                continue

            addr, netBase, mask = CIDR_to_subnet_mask(ips[0])
            net_addr = get_net_addr(addr, mask)
            addressHelper.SetBase(
                    network=ns3.Ipv4Address(netBase),
                    mask = ns3.Ipv4Mask(mask),
                    base = ns3.Ipv4Address(net_addr),
                    )
            # ip1 = addressHelper.Assign(ns3.NetDeviceContainer(ndc[i].Get(0)))
            ip1 = addressHelper.Assign(ns3.NetDeviceContainer(self.get_link_ndc(i).Get(0)))

            addr, netBase, mask = CIDR_to_subnet_mask(ips[1])
            net_addr = get_net_addr(addr, mask)
            addressHelper.SetBase(
                    network=ns3.Ipv4Address(netBase),
                    mask = ns3.Ipv4Mask(mask),
                    base = ns3.Ipv4Address(net_addr)
                    )
            # ip2 = addressHelper.Assign(ns3.NetDeviceContainer(ndc[i].Get(1)))
            ip2 = addressHelper.Assign(ns3.NetDeviceContainer(self.get_link_ndc(i).Get(1)))
            ipic.append((ip1, ip2))

        self.p2p = p2p

        # from ns.core import ofstream
        # _ascii = ofstream("wifi-ap.tr")
        # p2p.EnableAsciiAll("test")

        self.ipic = ipic

        return ipic


def main():
    topoHelper = TopologyReaderHelper()
    topoHelper.SetFileType('Inet')
    # topoHelper.SetFileName('../../net_config/Inet_small_toposample.txt')
    topoHelper.SetFileName('../../net_config/Inet_toposample.txt')
    topoReader = topoHelper.GetTopologyReader()
    nodes = topoReader.Read()
    print('nodes, ', nodes)
    print('node number, ', nodes.GetN())

if __name__ == "__main__":
    main()
