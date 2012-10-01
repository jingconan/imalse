#!/usr/bin/env python
import networkx
import itertools
from util import get_net, CIDR_to_subnet_mask

class DotConfig(object):
    """load dot configurationn. """
    # DEFAULT_ROUTING_PREFIX = 8
    DEFAULT_ROUTING_PREFIX = 24

    # the fs dot file doesn't have the botnet and trace information.
    pcap_nodes = [ 0, 1, 2 ]
    pcap_links = [ (0, 2) ]
    botmaster_id_set = [  ]
    client_id_set = [ ]
    server_id_set = [  ]
    server_addr = [ ]

    def __init__(self, f_name):
        self._loadconfig(f_name)
        # self.__dict__.update(trace_config)

    def _loadconfig(self, config):
        self.graph = networkx.nx_pydot.read_dot(config)

    @staticmethod
    def nid(node_name):
        """get node id from **node_name**, node_name should start
        with n and follows with a digit"""
        return int(node_name[1:])
    @staticmethod
    def nname(nid):
        return 'n' + str(nid)


    def _load_link_attr(self):
        """create **link_attr** map, with the key to be tuple (src_id, dst_id)
        the value is defined by **link_handler_list**.
        """
        link_handler_list =(
                ('delay', lambda x: x + 's'),
                ('capacity', lambda x: x + 'bps'),
                )
        def handle(d, h):
            """h[0] is the attribute name, h[1] is the preprocessor"""
            return h[1](d[h[0]])

        self.link_attr = dict()
        for src_node, dst_node, d in self.graph.edges(data=True):
            self.link_attr[(self.nid(src_node), self.nid(dst_node))] = [handle(d, h) for h in link_handler_list]

    def load_mod_prof(self):
        """return a dictionary. for each node return the
           (start, end, num, generator): start time of onoff application, end time of onoff application and the number and source.
        """
        self.mod_prof = dict()
        for node_name, node_attr in self.graph.node.iteritems():
            self.mod_prof[node_name] = []
            for mod in node_attr['traffic'].rsplit():
                mod_name = mod.strip('"')
                mod_info_str = node_attr.get(mod_name, None)
                # modulator profile=((8000,),(1,)) start=0 generator=s1_1
                if not mod_info_str:
                    continue

                mod_info_list = mod_info_str.strip('"').rsplit()
                assert(mod_info_list[0] == 'modulator')
                mod_para = dict(val_pair.rsplit('=') for val_pair in mod_info_list[1:])
                start = float(mod_para['start'])
                cum_time = start
                for dur, num in zip(*eval(mod_para['profile'])):
                    self.mod_prof[node_name].append( (cum_time, cum_time+dur, num, mod_para['generator']) )

        return self.mod_prof

    def get_gen(self, node_name, gen_name):
        """get generator from **node_name** with name **gen_name**
        """
        gen_string = self.graph.node[node_name][gen_name]
        gen_info_list = gen_string.strip('"').rsplit()
        gen = dict()
        gen['type'] = gen_info_list[0]
        for info in gen_info_list[1:]:
            kvpair = info.rsplit("=")
            gen[kvpair[0]] = kvpair[1]
        return gen

    def export_net_settings(self, fname):
        """export **link_attr**, **link_to_ip_map**, from the dot configuration
        get **link_attr** first, then for each link, search the ipdests for
        the node. Find two ipaddress share with the same prefix, add corresponding
        item to link_to_ip_map. if the net
        length in the CIDR format is not specified, use the default value of
        24.
        """
        self._load_link_attr()

        def get_ips(nid):
            ipstr = self.graph.node[self.nname(nid)]['ipdests']
            return [ip.strip('"') for ip in ipstr.rsplit(' ')]

        def find_addr_in_same_net(a, b):
            """find the two addresses in the same network from the address list a and b
            """
            # routing_prefix_len = lambda a: int(a.rsplit('/')[1]) if '/' in a else self.DEFAULT_ROUTING_PREFIX
            aug_routing_prefix_len = lambda a: a if '/' in a else (a + '/' + str(self.DEFAULT_ROUTING_PREFIX))

            def same_net(au, bu):
                """return True if au and bu are in the same network"""
                a_addr, a_net, a_mask = CIDR_to_subnet_mask(au)
                b_addr, b_net, b_mask = CIDR_to_subnet_mask(bu)
                # print 'a_net, ', a_net
                # print 'b_net, ', b_net
                return get_net(a_net, a_mask) == get_net(b_net, b_mask)

            for addr1, addr2 in itertools.product(a, b):
                x = aug_routing_prefix_len(addr1)
                y = aug_routing_prefix_len(addr2)
                if same_net(x, y):
                    return x, y

        self.link_to_ip_map = dict()
        for (src_nid, dst_nid), attr in self.link_attr.iteritems():
            src_ips = get_ips(src_nid)
            dst_ips = get_ips(dst_nid)
            # print 'src_ips, ', src_ips
            # print 'dst_ips, ', dst_ips
            # import pdb;pdb.set_trace()
            res = find_addr_in_same_net(src_ips, dst_ips)
            print 'res, ', res
            if not res:
                raise Exception("The two nodes don't have two address in the same net, \
                        maybe you specify the wrong ip address?")
            self.link_to_ip_map[(src_nid, dst_nid)] = res

        fid = open(fname, 'w')
        import pprint
        fid.write("ipv4_net_addr_base = '10.7.0.1/24'\n")
        fid.write("link_attr_default = ['2ms','5Mbps']\n")

        fid.write('link_attr = ' + pprint.pformat(self.link_attr, indent=4)+'\n')
        fid.write('link_to_ip_map = ' + pprint.pformat(self.link_to_ip_map, indent=4)+'\n')
#         fid.write("""
# pcap_nodes = %s
# pcap_links = %s
# botmaster_id_set = %s
# client_id_set = %s
# server_id_set = %s
# server_addr = %s"""%(self.pcap_nodes, self.pcap_links,
#         self.botmaster_id_set,
#         self.client_id_set,
#         self.server_id_set,
#         self.server_addr))
#         fid.close()
        return self.link_attr, self.link_to_ip_map

    def export_inet(self, fname):
        """export the topology to inet format,
        since fs dot file doesn't have location information
        location will be set as (0, 0), the link weight is set
        as 1."""
        fid = open(fname, 'w')

        node_num = len(self.graph.node)
        link_num = len(self.graph.edges(data=True))
        fid.write('%i\t%i\n'%(node_num, link_num))
        for node in self.graph.node:
            # ignore the geographic location
            fid.write('%i\t%f\t%f\n'%(self.nid(node), 0, 0))

        for src_node, dst_node, d in self.graph.edges(data=True):
            # ignore the weight
            fid.write('%i\t%i\t1\n'%(self.nid(src_node), self.nid(dst_node)))

        fid.close()

if __name__ == "__main__":
    dot = DotConfig('./res.dot')
    # mod_prof = dot.load_mod_prof()
    # gen = dot.get_gen('n1', 's1_1')
    # print gen
    # dot.load_generator()
    # print dot.node['n1']['s1_1']
    dot.export_net_settings('./net_settings_tmp.py')
    dot.export_inet('./topology.inet')
    # ns3_config = NS3Config(settings.ROOT + '/conf.dot')
    # ns3_config.setup()
