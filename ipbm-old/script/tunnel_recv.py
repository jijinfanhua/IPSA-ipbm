#!/usr/bin/env python
import sys
import os

from scapy.all import *
from tunnel_header import MyTunnel


def get_if():
    ifs = get_if_list()
    iface = None
    for i in get_if_list():
        if "eth0" in i:
            iface = i
            break
    if not iface:
        print("Cannot find eth0 interface")
        exit(1)
    return iface


def handle_pkt(pkt):
    if MyTunnel in pkt or (TCP in pkt and pkt[TCP].dport == 1234):
        print("got a packet")
        pkt.show2()
#        hexdump(pkt)
#        print "len(pkt) = ", len(pkt)
        sys.stdout.flush()


def main():
    ifaces = [x for x in os.listdir('/sys/class/net/') if 'lo' not in x]
    assert len(ifaces) == 1
    iface, = ifaces
    print("sniffing on %s" % iface)
    sys.stdout.flush()
    sniff(iface=iface,
          prn=lambda x: handle_pkt(x))


if __name__ == '__main__':
    main()
