from walker_switch import WalkerSwitch
from walker_host import WalkerHost
from walker_topo import WalkerTopo
import os, sys, json, subprocess, re, argparse
from time import sleep

from mininet.net import Mininet
from mininet.topo import Topo
from mininet.link import TCLink
from mininet.cli import CLI


class WalkerRunner:

    def __init__(self, topo_file, exe):

        print('Reading topology file.')
        with open(topo_file, 'r') as f:
            topo = json.load(f)
        self.hosts = topo['hosts']
        self.switches = topo['switches']
        self.links = self.parse_links(topo['links'])
        self.exe = exe

        print("Building mininet topology.")
        self.topo = WalkerTopo(self.hosts, self.switches, self.links)
        self.net = Mininet(topo=self.topo,
                           link=TCLink,
                           host=WalkerHost,
                           switch=WalkerSwitch,
                           controller=None)

    def run(self):
        self.net.start()
        sleep(1)
        self.program()
        sleep(1)
        self.do_net_cli()
        self.net.stop()

    def parse_links(self, unparsed_links):
        links = []
        for link in unparsed_links:
            # make sure each link's endpoints are ordered alphabetically
            s, t, = link[0], link[1]
            if s > t:
                s, t = t, s

            link_dict = {'node1': s,
                         'node2': t,
                         'latency': '0ms',
                         'bandwidth': None
                         }
            links.append(link_dict)
        return links

    def program(self):
        for host in self.hosts:
            h = self.net.get(host['name'])
            h.defaultIntf().rename('%s-eth0' % host['name'])
            cmds = host['cmd']
            for cmd in cmds:
                h.cmd(cmd)

    def do_net_cli(self):
        """ Starts up the mininet CLI and prints some helpful output.

                Assumes:
                    - A mininet instance is stored as self.net and self.net.start() has
                      been called.
        """
        # for s in self.net.switches:
        #    s.describe()
        for h in self.net.hosts:
             h.describe()
        print("Starting mininet CLI")
        # Generate a message that will be printed by the Mininet CLI to make
        # interacting with the mpls switch a little easier.
        print('')
        print('======================================================================')
        print('Welcome to Walker Mininet CLI!')
        print('======================================================================')
        print('')

        CLI(self.net)


def get_args():
    cwd = os.getcwd()
    parser = argparse.ArgumentParser()
    parser.add_argument('-t', '--topo', help='Path to topology json',
                        type=str, required=True)
    parser.add_argument('-e', '--exe', help='switch executable', type=str, required=True)
    # parser.add_argument('-c', '--ctrl', help='controller path', type=str, required=True)
    return parser.parse_args()


if __name__ == '__main__':
    args = get_args()
    runner = WalkerRunner(args.topo, args.exe)
    runner.run()
