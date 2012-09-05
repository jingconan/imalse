ipv4_net_addr_base = '10.7.0.1/24'
# link_to_interface_map
link_to_ip_map = {
        (0, 1):['10.0.1.1/24', '10.0.1.2/24'],
    }

link_attr_default = ['2ms','5Mbps']
link_attr = {
        (0, 1):['2ms','5Mbps'],
        }

# all links in pcap_links will export pcap data.
pcap_links = [(0, 1)]

# all nodes in pcap_nodes will export pcap data.
pcap_nodes = [0]


# botnet related configuration
botmaster_id_set = [1]
client_id_set = [2, 3, 4]
# server_addr = ["10.0.12.4/24"]
# you just need to specify either server_id_set or server_addr
server_id_set = [0]
server_addr = []
