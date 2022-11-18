#!/usr/bin/env python
import argparse
import sys
import socket
import random
import struct

from scapy.all import sendp, send, get_if_list, get_if_hwaddr
from scapy.all import Packet
from scapy.layers.inet import IP
from scapy.layers.l2 import Ether
from scapy.packet import Raw


def get_if():
    ifs = get_if_list()
    iface = None  # "h1-eth0"
    for i in get_if_list():
        if i == "lo":
            iface = i
            break
    if not iface:
        print("Cannot find interface")
        exit(1)
    return iface


def main():
    # if len(sys.argv) < 3:
    #     print('pass 2 arguments: <destination> "<message>"')
    #     exit(1)

    # addr = socket.gethostbyname(sys.argv[1])
    # iface = get_if()
    iface = 'h1-eth0'
    # print("sending on interface %s to %s" % (iface, str(addr)))
    pkt = Ether(src=get_if_hwaddr(iface), dst='00:00:00:00:01:01') / IP(src="10.1.1.2", dst="10.1.4.2", proto=200)
    # print(pkt)
    body = struct.pack('>i i i i i i i i', 128, 64, 32, 32, 8, 4, 2, 1)
    # pkt.show2()
    # pkt = raw(pkt)
    sendp(bytes(pkt) + body, iface=iface, verbose=False)


if __name__ == '__main__':
    while True:
        main()
        sleep(1)
