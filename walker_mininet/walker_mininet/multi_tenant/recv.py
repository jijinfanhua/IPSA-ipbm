#!/usr/bin/env python
import sys
import struct
import os

from scapy.all import sniff, sendp, hexdump, get_if_list, get_if_hwaddr
from scapy.all import Packet
from scapy.all import ShortField, IntField, LongField, BitField, FieldListField, FieldLenField


def get_if():
    ifs = get_if_list()
    iface = None  # "h1-eth0"
    for i in get_if_list():
        if i != 'lo':
            iface = i
            break
    if not iface:
        print("Cannot find eth0 interface")
        exit(1)
    return iface


def handle_pkt(pkt):
    print(pkt)
    pkt.show()


def main():
    sniff(iface='h4-eth0',
          count=0,
          prn=lambda x: handle_pkt(x))


if __name__ == '__main__':
    main()
