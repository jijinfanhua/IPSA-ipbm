from mininet.topo import Topo


class WalkerTopo(Topo):
    """ The mininet topology class for the P4 tutorial exercises.
        A custom class is used because the exercises make a few topology
        assumptions, mostly about the IP and MAC addresses.
    """
    def __init__(self, hosts, switches, links, **opts):
        Topo.__init__(self, **opts)
        host_links = []
        switch_links = []
        self.sw_port_mapping = {}

        for link in links:
            if link['node1'][0] == 'h':
                host_links.append(link)
            else:
                switch_links.append(link)

        link_sort_key = lambda x: x['node1'] + x['node2']
        # Links must be added in a sorted order so bmv2 port numbers are predictable
        host_links.sort(key=link_sort_key)
        switch_links.sort(key=link_sort_key)

        for sw in switches:
            self.addSwitch(sw['name'])

        for host in hosts:
            self.addHost(host['name'], ip=host['ip'], mac=host['mac'])


        for link in host_links:
            
            self.addLink(
                    link['node1'], 
                    link['node2'],
                    delay = link['latency'], 
                    bw    = link['bandwidth']
                )
            self.add_switch_port(link['node2'], link['node1'])

        for link in switch_links:
            self.addLink(
                    link['node1'], 
                    link['node2'],
                    delay = link['latency'], 
                    bw    = link['bandwidth']
                )
            self.add_switch_port(link['node1'], link['node2'])
            self.add_switch_port(link['node2'], link['node1'])

        self.print_port_mapping()

    def add_switch_port(self, sw, node2):
        if sw not in self.sw_port_mapping:
            self.sw_port_mapping[sw] = []
        portno = len(self.sw_port_mapping[sw])+1
        self.sw_port_mapping[sw].append((portno, node2))


    def print_port_mapping(self):
        print "Switch port mapping:"
        for sw in sorted(self.sw_port_mapping.keys()):
            print "%s: " % sw,
            for portno, node2 in self.sw_port_mapping[sw]:
                print "%d:%s\t" % (portno, node2),
            print
