#!/usr/bin/env python
import sys
import struct
import os
from scapy.all import sniff, sendp, hexstr, get_if_list, get_if_hwaddr
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

def p(pkt):
    print(type(hexstr(pkt)))

def t(s):
    x = ''
    for i in [hex(ord(c)).replace('0x','') for c in s]:
        x += '%02x' % int(i, 16)
    return x

def main():
    iface = get_if()
    print("sniffing on %s" % iface)
    sys.stdout.flush()
    while True:
        res = sniff(iface=iface, count=1)
        protocol = int(t(str(res[0][1]))[18:20], 16)
        s = t(str(res[0][2]))
        if protocol == 200:
            print('\033[0;{};40m'.format(protocol-197+30) + \
                  ' + '.join(['c' + str(i) for i in range(0, 8)]) + \
                  (' = {}'.format(int(s[0:8], 16))) + \
                  '\033[0m'
                  )
        elif protocol == 201:
            print('\033[0;{};40m'.format(protocol-197+30) + \
                  ' & '.join(['c' + str(i) for i in range(0, 8)]) + \
                  (' = {}'.format(int(s[0:8], 16))) + \
                  '\033[0m'
                  )
        elif protocol == 202:
            print('\033[0;{};40m'.format(protocol-197+30) + \
                  ' | '.join(['c' + str(i) for i in range(0, 8)]) + \
                  (' = {}'.format(int(s[0:8], 16))) + \
                  '\033[0m'
                  )
        else:
            continue



if __name__ == '__main__':
    main()
