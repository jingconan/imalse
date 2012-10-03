#!/usr/bin/env sh
# ./sim -e ManualTopoBTExperiment --net_settings=net_config/net_settings.py --topology_file=net_config/Inet_small_toposample.txt --Simulator=Visual -s file_exfiltration
# ./sim -e ManualTopoBTExperiment --net_settings=net_config/aro_demo/net_settings.py --topology_file=net_config/aro_demo/topology.inet --Simulator=Visual -s file_exfiltration
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py --Simulator=Visual -s file_exfiltration
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s file_exfiltration -t 100
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s ddos_ping_flooding -t 1000 
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s file_exfiltration -t 1000 --Simulator=Visual
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s file_exfiltration -t 100

# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s file_exfiltration -t 100 --back_traf=net_config/aro_demo/back_traf.py
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s file_exfiltration -t 100
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s ddos_ping_flooding -t 100
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s ddos_ping_flooding -t 1000 --Simulator=Visual
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s ddos_ping_flooding -t 1000 --Simulator=Visual --back_traf=net_config/aro_demo/back_traf.py
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s ddos_ping_flooding -t 1000 --back_traf=net_config/aro_demo/back_traf.py --Simulator=Visual
./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s ddos_ping_flooding -t 1000 --Simulator=Visual
# ./sim -e ComplexNetExperiment --net_settings=net_config/aro_demo/nf_net_settings.py -s file_exfiltration -t 100


