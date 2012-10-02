#!/usr/bin/env sh
# ./sim -e ManualTopoBTExperiment --net_settings=net_config/net_settings.py --topology_file=net_config/Inet_small_toposample.txt --Simulator=Visual -s file_exfiltration
# ./sim -e ManualTopoBTExperiment --net_settings=net_config/aro_demo/net_settings.py --topology_file=net_config/aro_demo/topology.inet --Simulator=Visual -s file_exfiltration
./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py --topology_file=net_config/aro_demo/topology.inet --Simulator=Visual -s file_exfiltration
