#!/usr/bin/env sh
# This script will load the imalse configuration script in the gui/imals_config/ 
# automatically and run the simulation in the visualization mode. 
# It is basicially a shortcut. If you want to run with other parameters, 
# you can modify the command correspondingly.
./sim -e ManualTopoExperiment --mode sim -s file_exfiltration -f \
    gui/imalse_config/topology.inet \
    --net_settings=gui/imalse_config/net_settings.py \
    -t 100 --SimulatorImpl=Visual
