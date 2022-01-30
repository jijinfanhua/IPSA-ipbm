#!/usr/bin/env python
import sys
import os

from scapy.all import *

def handle_pkt(pkt):
    pkt.show2()
    print("===================================")

def get_if():
    ifs = get_if_list()
    for i in ifs:
        print(i)
        if(i == "r3r2"):
            sniff(iface=i, prn=lambda x: handle_pkt(x))

get_if()