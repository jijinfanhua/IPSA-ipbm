#!/usr/bin/env python
import argparse
import sys
import socket
import random
import struct

from scapy.all import sendp, send, get_if_list, get_if_hwaddr, hexdump
from scapy.all import Packet
from scapy.layers.inet import IP
from scapy.layers.l2 import Ether
from scapy.packet import Raw
from time import sleep

def get_if():
    ifs = get_if_list()
    iface = None  # "h1-eth0"
    for i in get_if_list():
        if i != "lo":
            iface = i
            break
    if not iface:
        print("Cannot find interface")
        exit(1)
    return iface


def main():

    iface = get_if()
    # print("sending on interface %s to %s" % (iface, str(addr)))
    pkt = Ether(src=get_if_hwaddr(iface), dst='00:00:00:00:01:01') / IP(src="10.1.3.2", dst="10.1.4.2", proto=202)
    # print(pkt)
    body = struct.pack('>i i i i i i i i', 1, 2, 3, 4, 8, 16, 32, 128)
    # pkt.show2()
    # pkt = raw(pkt)
    pkt = bytes(pkt) + body
    print(hexdump(pkt))
    sendp(pkt, iface=iface, verbose=False)


if __name__ == '__main__':
    while True:
        main()
        sleep(1)
