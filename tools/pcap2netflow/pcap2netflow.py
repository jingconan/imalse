#!/usr/bin/env python
from subprocess import call, Popen
import os, signal
import time

SOFT_FLOWD = '../softflowd-0.9.8-revised-for-ns3-pcap/softflowd'

def start_flowd(host_port, log_file, daemon=True):
    # create tmp conf file
    conf_file_name = './tmp_conf_file.conf'
    fid = open(conf_file_name, 'w')
    fid.write('logfile "%s"\nlisten on %s\npidfile "/var/run/flowd.pid"\nstore ALL\n'%(log_file, host_port))
    fid.close()
    print '*' * 100
    print '--> configure file has been written'
    call(['cat', conf_file_name])
    print '*' * 100
    # create flowd collector process
    # flowd_pid = Popen(['flowd', '-f', conf_file_name, '-d']).pid
    if daemon:
        flowd_pid = Popen(['sudo', 'flowd', '-f', conf_file_name, '-d']).pid
    else:
        flowd_pid = call(['sudo', 'flowd', '-f', conf_file_name, '-d']).pid
    print '--> flowd process has been started, its pid is %d'%(flowd_pid)
    print '*' * 100
    return flowd_pid

def pcap2flow(pcap_file_name, host_port, log_file=None, flowd=True):
    # start flowd process
    if not log_file:
        log_file = pcap_file_name.rsplit('.pcap')[0] + '.log'

    flowd_pid = None
    if flowd:
        flowd_pid = start_flowd(host_port, log_file)

    time.sleep(1)

    # start softflowd process
    softflowd_pid = Popen([SOFT_FLOWD, '-r', pcap_file_name, '-n', host_port]).pid

    # create flowd-reader process
    # flowreader_pid = Popen(['flowd-reader', log_file]).pid
    # flowreader_pid = call(['flowd-reader', log_file]).pid
    # flowreader_pid = Popen(['flowd-reader', log_file]).pid
    time.sleep(1)
    output_file_name = pcap_file_name.rsplit('.pcap')[0] + '.flow'
    print "--> run 'flowd-reader %s > %s'%(log_file, output_file_name)"
    # call('sudo flowd-reader %s > %s'%(log_file, output_file_name), shell=True)
    flowreader_pid = Popen('sudo flowd-reader %s > %s'%(log_file, output_file_name), shell=True).pid
    # flowreader_pid = None

    return flowd_pid, softflowd_pid, flowreader_pid

    # print 'softflowd pid: %s \n flowd pid: %s \n flowreader pid: %s \n' %(softflowd_pid, flowd_pid, flowreader_pid)

def loop_folder(folder_name):
    """is not quite sucessful right now"""
    import glob
    i = 0
    start_port = 40035
    for pcap_file_name in glob.glob( os.path.join(folder_name, '*.pcap') ):
        print "--> start to process pcap_file_nam: " + pcap_file_name
        i += 1
        # flowd = True if i == 1 else False
        flowd_pid, softflowd_pid, flowreader_pid = pcap2flow(pcap_file_name, "127.0.0.1:%d"%(start_port + 30*i))
        time.sleep(1)

    # os.system('sudo killall flowd')

HOST_PORT_DEFAULT = '127.0.0.1:12345'
if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='pcap2netflow')
    parser.add_argument('-p', '--pcap', default=None,
            help='specify the pcap file you want to process')
    parser.add_argument('-f', '--folder', default=None,
            help='specify the folder you want to loop through')
    parser.add_argument('-n', '--host_port', default=HOST_PORT_DEFAULT,
            help='host:port')
    parser.add_argument('--start_flowd', default=False, action="store_true",
            help='start flowd damon process')
    parser.add_argument('-l', '--log', default='tmp.log',
            help='log file')



    args, remains = parser.parse_known_args()
    if args.host_port:
        host_port = args.host_port
    else:
        host_port = HOST_PORT_DEFAULT

    if args.start_flowd:
        start_flowd(args.host_port, args.log, False)
        import sys
        sys.exit()

    if args.pcap:
        pcap2flow(args.pcap, host_port)
    elif args.folder:
        loop_folder(args.folder)
    else:
        parser.print_help()

    # pcap_file_name = '/home/wangjing/LocalResearch/CyberData/pcap2netflow/best_malware_protection.pcap'
    # host_port = '127.0.0.1:12345'
    # logfile =  '/home/wangjing/LocalResearch/CyberData/flowd.log'
    # pcap2flow(pcap_file_name, host_port)
    # loop_folder('/home/wangjing/LocalResearch/CyberData/OpenPacket/')
    # loop_folder('/home/wangjing/LocalResearch/CyberData/ppa-capture-files/')

