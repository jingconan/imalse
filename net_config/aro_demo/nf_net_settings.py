Ipv4NetAddressBase = '10.7.0.1/24'
link_attr_default = ['2ms','5Mbps']
nets = {
        'PointToPoint': {
            'ChannelAttributeDefault':{'Delay':'2ms'},
            'DeviceAttributeDefault':{'DataRate':'5Mbps'},
            'IpMap': {
                    (1, 0):['10.200.1.1/24', '10.200.1.3/24'],
                    (6, 0):['10.1.1.1/24', '10.1.1.5/24'],
                    (7, 6):['1.1.1.1/24', '2.3.5.10/24'],
                    (8, 6):['2.3.5.6/24', '1.1.1.10/24'],
                    (6, 9):['5.6.8.10/24', '5.6.8.9/24'],
                },
            'ChannelAttribute':{},
            'DeviceAttribute':{},
            },
        'Csma': {
            'ChannelAttributeDefault':{'Delay':'2ms', 'DataRate':'5Mbps'},
            'IpMap': {
                (0, 2, 3, 4, 5, 10, 11, 12, 13): [
                    '10.20.30.9/24',
                    '10.20.30.1/24',
                    '10.20.30.2/24',
                    '10.20.30.3/24',
                    '10.20.30.4/24',
                    '10.20.30.6/24',
                    '10.20.30.7/24',
                    '10.20.30.8/24',
                    '10.20.30.5/24',
                ]
                },
            'ChannelAttribute':{},
            'DeviceAttribute':{},
            },
        }

pcap_nodes = [ 0 ]
pcap_links = [  ]
botmaster_id_set = [ 7 ]
client_id_set = [ 2, 3, 4, 5, 6 ]
server_id_set = [ 8 ]
server_addr = [  ]
