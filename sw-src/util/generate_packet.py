
import scapy
from scapy.layers.inet import TCP, IP, UDP
from scapy.layers.l2 import Ether
import binascii

from scapy.utils import hexdump

data = "hello"

pkt = Ether(src='ac:1f:6b:f8:7f:12', dst='11:11:11:11:11:11')/IP(src='192.168.0.1', dst='192.168.0.2')/TCP(sport=12345, dport=54321)/data
hexdump(pkt)
# hexdump(pkt.getlayer(IP))
# pkt.show()

# binascii.unhexlify(pkt)