#!/usr/bin/env python
""" Load the flowmax dot syntax flow description file.
use on-off application to simulate the flows. All packets
from one node during one segment of on-time are considered
to belongs to the same flow.
    **OnTime** is calcualted by *flow size / data rate*
    **OffTime** is calculated by *1 / flow arrival rate*
        (approximation, assume OnTime is small)
"""
from __future__ import division, print_function

import settings
from Node import ImalseNetnsSimNode
from Topology import ManualTopologyNet
from DotConfig import DotConfig
from util import Namespace
import ns3
from random import randint
from RandomVariable import RV

#####################################
####              API            ####
#####################################
def TopologyNetBT(dot_file, trace_config):
    """return a TopologyNet with Background traffic already configured
        - **dotfile** is the path of dot configuration file relative to ROOT directory
        - **trace_config** is a dictionary contains the trace and bot server client information
    """
    ns3_config = NS3Config(dot_file, trace_config)
    ns3_config.setup()
    ns3_config.config_onoff_app()
    return ns3_config.net

def run_ns3(dot_file, trace_config, sim_time, visual):
    """run ns3 simulation based on configuration in **dot_file** and **trace_flag**"""
    if visual:
        ns3.GlobalValue.Bind("SimulatorImplementationType",
                ns3.StringValue("ns3::VisualSimulatorImpl"))

    ns3.LogComponentEnable("OnOffApplication", ns3.LOG_LEVEL_INFO)
    ns3.LogComponentEnable("OnOffApplication", ns3.LOG_LEVEL_INFO)

    ns3_config = NS3Config(settings.ROOT + '/' + dot_file, trace_config)
    ns3_config.setup()
    ns3_config.config_onoff_app()
    ns3.Simulator.Stop(ns3.Seconds(sim_time))
    ns3.Simulator.Run()
    ns3.Simulator.Destroy()

##################################################################
####    Not recommend to call functions in this part directly ####
##################################################################
def short_num_to_standard(s):
    """change short text notation to standard numerica value"""
    replace_map = {
            'kbps':'1000',
                }
    for k, v in replace_map.iteritems():
        s = s.replace(k, v)
    return s

from RandomVarLib import *

class NS3Config(object):
    """config NS3 according to fs dot file"""
    TOPOLOGY_FILE = settings.ROOT + '/share/bg_topology.inet'
    TOPOLOGY_TYPE = 'Inet'
    NET_SETTINGS_FILE = settings.ROOT + '/share/bg_net_settings_tmp.py'
    def __init__(self, dot_file, trace_config):
        self.dot_file = dot_file
        self.dot_config = DotConfig(self.dot_file, trace_config)
        self.dot_config.export_inet(self.TOPOLOGY_FILE)
        self.dot_config.export_net_settings(self.NET_SETTINGS_FILE)

    def load_net_settings(self):
        s = {}
        execfile(self.NET_SETTINGS_FILE, s)
        return Namespace(s)

    @staticmethod
    def NodeCreator():
        return ImalseNetnsSimNode()

    def setup(self):
        """set up the network"""
        net_settings = self.load_net_settings()
        self.net = ManualTopologyNet(
                self.TOPOLOGY_FILE,
                self.TOPOLOGY_TYPE,
                self.NodeCreator,
                net_settings,
                )
        self.net.set_trace()

    def add_onoff_app(self, start_time, end_time, local, remote, on_time, off_time, data_rate, sport, dport):
        """add one ns3 onoff application to the network
        """
        ### Install OnOff Application ###
        socketType = "ns3::UdpSocketFactory"
        helper = ns3.OnOffHelper(socketType,
                ns3.InetSocketAddress(remote, dport))

        helper.SetAttribute("StartTime", ns3.TimeValue(ns3.Time(str(start_time)+'s')))
        helper.SetAttribute("StopTime", ns3.TimeValue(ns3.Time(str(end_time)+'s')))

        # helper.SetAttribute("Remote", ns3.AddressValue(ns3.Ipv4Address(remote)))
        local_inet = ns3.InetSocketAddress(local, sport)
        helper.SetAttribute("Local", ns3.AddressValue(local_inet))

        helper.SetAttribute("DataRate", ns3.StringValue(str(data_rate) + 'b/s'))
        helper.SetAttribute("OnTime", ns3.RandomVariableValue(on_time.to_ns3()))
        helper.SetAttribute("OffTime", ns3.RandomVariableValue(off_time.to_ns3()))

        local_node = self.net.search_node(local)
        helper.Install(local_node)

        ### Install Sink Application ####
        sinkLocalAddress = ns3.Address(ns3.InetSocketAddress (ns3.Ipv4Address.GetAny (), dport))
        sinkHelper = ns3.PacketSinkHelper(socketType, sinkLocalAddress)
        helper.SetAttribute("StartTime", ns3.TimeValue(ns3.Time(str(start_time)+'s')))
        helper.SetAttribute("StopTime", ns3.TimeValue(ns3.Time(str(end_time)+'s')))
        remote_node = self.net.search_node(remote)
        sinkHelper.Install (remote_node);

        print("""add an onoff application with
                start_time: %f
                end_time: %f
                local: %s,
                remote: %s,
                on_time: %s,
                off_time: %s"""%(start_time, end_time, local, remote, on_time, off_time))

    @staticmethod
    def transform_para(flowsize, flowstart, ipsrc, ipdst, sport, dport, **argv):
        """transform the parameter for generator desription to the NS3 onoff application parameters"""
        # data_rate = StringValue('1kbps') # constant data rate
        data_rate = 1000
        on_time = RV(flowsize) / data_rate
        off_time = RV(flowstart)
        return ipsrc, ipdst, on_time, off_time, data_rate, eval(sport).next(), eval(dport).next()

    def config_onoff_app(self):
        """ config on off application according to the modulator profile"""
        mod_prof = self.dot_config.load_mod_prof()
        for node, mod_profs in mod_prof.iteritems(): # for each node
            for start_time, end_time, num, generator in mod_profs: # for each modulator
                gen = self.dot_config.get_gen(node, generator)
                for i in xrange(num):
                    self.add_onoff_app(start_time, end_time, *self.transform_para(**gen))

if __name__ == "__main__":
    run_ns3('res.dot', [], 3000, False)
