def get_net(ipaddress, netmask):
    ip = ipaddress.split(".")
    netm = netmask.split(".")
    network = str(int(ip[0])&int(netm[0]))+"."+str(int(ip[1])&int(netm[1]))+"."+str(int(ip[2])&int(netm[2]))+"."+str(int(ip[3])&int(netm[3]))
    return network

def get_net_addr(ipaddress, netmask):
    ip = ipaddress.split(".")
    netm = netmask.split(".")
    addr = str(int(ip[0])&(~int(netm[0])))+"."+str(int(ip[1])&(~int(netm[1])))+"."+str(int(ip[2])& (~int(netm[2])))+"."+str(int(ip[3])&(~int(netm[3])))
    return addr

def CIDR_to_subnet_mask(s_addr):
    """change CIDR format to address, net address and mask address"""
    addr, prefix_len = s_addr.rsplit('/')
    mask = len2mask(int(prefix_len))
    net = get_net(addr, mask)
    return addr, net, mask

# CIDR to Netmask conversion
# (c) 2010 Peter Bittner, http://linux.bittner.it/blog
#
# Tip for an excellent background read: "IP subnetting made easy"
# http://articles.techrepublic.com.com/5100-10878_11-6089187.html

def len2mask(len):
    """Convert a bit length to a dotted netmask (aka. CIDR to netmask)"""
    mask = ''
    if not isinstance(len, int) or len < 0 or len > 32:
        print "Illegal subnet length: %s (which is a %s)" % (str(len), type(len).__name__)
        return None

    for t in range(4):
        if len > 7:
            mask += '255.'
        else:
            dec = 255 - (2**(8 - len) - 1)
            mask += str(dec) + '.'
        len -= 8
        if len < 0:
            len = 0

    return mask[:-1]
