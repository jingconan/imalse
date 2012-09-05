""" This file will accept
    **topo** and **srv_node_list** as parameters
"""
#################################
##   topology ##
#################################
link_attr = {'weight':'10', 'capacity':'10000000', 'delay':'0.01'} # link Attribute


#################################
##   Parameter For Normal Case ##
#################################
sim_t = 8000
start = 0
DEFAULT_PROFILE = ((sim_t,),(1,))


# gen_desc = {'TYPE':'harpoon', 'flow_size_mean':'4e5', 'flow_size_var':'100', 'flow_arrival_rate':'0.5'}
gen_desc = {'TYPE':'harpoon', 'flow_size_mean':'4e3', 'flow_size_var':'100', 'flow_arrival_rate':'1'}
NORM_DESC = dict(
        TYPE = 'NORMAl',
        start = '0',
        node_para = {'states':[gen_desc]},
        profile = DEFAULT_PROFILE,
        )

ANO_LIST = []

NET_DESC = dict(
        topo=topo,
        # size=topo.shape[0],
        size=len(topo),
        srv_list=srv_node_list,
        link_attr=link_attr,
        node_type='NNode',
        node_para={},
        )
