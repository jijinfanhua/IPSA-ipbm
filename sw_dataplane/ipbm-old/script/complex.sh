#!/usr/bin/env bash

set -ex

# P12 <-----+ veth12    veth11 +-----> P15
#    veth   |                  |   veth
#  10.0.1.2 |                  | 10.0.1.5
#           +-----> BRD1 <-----+
#           |         |        |
#           |         |        |
# P13 <-----+ veth13  | veth14 +-----> P14
#    veth             |            veth
#  10.0.1.3           |          10.0.1.4
#                     | veth22
#                     v 10.0.2.2
#                    P22

for NS in P12 P13 P14 P15 BRD1 P22
do
    ip netns delete $NS 2>/dev/null || true
    ip netns add $NS
    ip netns exec $NS ip l set lo up
done

# P12 <-> BRD1
ip l add veth netns P12 type veth peer name veth12 netns BRD1
ip netns exec P12 ip l set dev veth address 00:00:00:00:01:02
ip netns exec P12 ip a add 10.0.1.2/24 dev veth
ip netns exec P12 ip l set veth up
ip netns exec P12 ethtool -K veth tx off
ip netns exec P12 ip r add default via 10.0.1.1
ip netns exec P12 arp -i veth -s 10.0.1.1 00:00:00:00:01:01

ip netns exec BRD1 ip l set veth12 up
ip netns exec BRD1 ethtool -K veth12 tx off

# P13 <-> BRD1
ip l add veth netns P13 type veth peer name veth13 netns BRD1
ip netns exec P13 ip l set dev veth address 00:00:00:00:01:03
ip netns exec P13 ip a add 10.0.1.3/24 dev veth
ip netns exec P13 ip l set veth up
ip netns exec P13 ethtool -K veth tx off
ip netns exec P13 ip r add default via 10.0.1.1
ip netns exec P13 arp -i veth -s 10.0.1.1 00:00:00:00:01:01

ip netns exec BRD1 ip l set veth13 up
ip netns exec BRD1 ethtool -K veth13 tx off

# P14 <-> BRD1
ip l add veth netns P14 type veth peer name veth14 netns BRD1
ip netns exec P14 ip l set dev veth address 00:00:00:00:01:04
ip netns exec P14 ip a add 10.0.1.4/24 dev veth
ip netns exec P14 ip l set veth up
ip netns exec P14 ethtool -K veth tx off
ip netns exec P14 ip r add default via 10.0.1.1
ip netns exec P14 arp -i veth -s 10.0.1.1 00:00:00:00:01:01

ip netns exec BRD1 ip l set veth14 up
ip netns exec BRD1 ethtool -K veth14 tx off

# P15 <-> BRD1
ip l add veth netns P15 type veth peer name veth15 netns BRD1
ip netns exec P15 ip l set dev veth address 00:00:00:00:01:05
ip netns exec P15 ip a add 10.0.1.5/24 dev veth
ip netns exec P15 ip l set veth up
ip netns exec P15 ethtool -K veth tx off
ip netns exec P15 ip r add default via 10.0.1.1
ip netns exec P15 arp -i veth -s 10.0.1.1 00:00:00:00:01:01

ip netns exec BRD1 ip l set veth15 up
ip netns exec BRD1 ethtool -K veth15 tx off

# P22 <-> BRD1
ip netns exec P22 ip l add veth type veth peer name veth21 netns BRD1
ip netns exec P22 ip a add 10.0.2.2/24 dev veth
ip netns exec P22 ip l set dev veth address 00:00:00:00:02:02
ip netns exec P22 ip l set veth up
ip netns exec P22 ip r add default via 10.0.2.1
ip netns exec P22 arp -i veth -s 10.0.2.1 00:00:00:00:02:01
ip netns exec P22 ethtool -K veth tx off

ip netns exec BRD1 ip l set dev veth21 address 00:00:00:00:02:01
ip netns exec BRD1 ip l set veth21 up
ip netns exec BRD1 ethtool -K veth21 tx off

# VLAN
ip netns exec P12 arp -i veth -s 10.0.1.4 00:00:00:00:01:04
ip netns exec P12 arp -i veth -s 10.0.1.5 00:00:00:00:01:05
ip netns exec P13 arp -i veth -s 10.0.1.4 00:00:00:00:01:04
ip netns exec P13 arp -i veth -s 10.0.1.5 00:00:00:00:01:05
ip netns exec P14 arp -i veth -s 10.0.1.2 00:00:00:00:01:02
ip netns exec P14 arp -i veth -s 10.0.1.3 00:00:00:00:01:03
ip netns exec P15 arp -i veth -s 10.0.1.2 00:00:00:00:01:02
ip netns exec P15 arp -i veth -s 10.0.1.3 00:00:00:00:01:03

# bridge
# ip netns exec BRD1 ip l add brd1 type bridge
# ip netns exec BRD1 ip l set brd1 up
# ip netns exec BRD1 ip l set veth12 master brd1
# ip netns exec BRD1 ip l set veth13 master brd1
# ip netns exec BRD1 ip l set veth14 master brd1
# ip netns exec BRD1 ip l set veth15 master brd1

# forward
# ip netns exec BRD1 sh -c "echo 1 > /proc/sys/net/ipv4/conf/all/forwarding"
