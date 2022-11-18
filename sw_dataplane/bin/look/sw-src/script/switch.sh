#!/usr/bin/env bash

set -ex

# P12 <-----+ veth12    veth11 +-----> P11
#    veth   |                  |   veth
#  10.0.1.2 |                  | 10.0.1.1
#           +-----> BRD1 <-----+
#           |                  |
#           |                  |
# P13 <-----+ veth13    veth14 +-----> P14
#    veth                          veth
#  10.0.1.3                      10.0.1.4

for NS in P12 P13 P11 P14 BRD1
do
    ip netns delete $NS 2>/dev/null || true
    ip netns add $NS
    ip netns exec $NS ip l set lo up
done

# P11 <-> BRD1
ip l add veth netns P11 type veth peer name veth11 netns BRD1
ip netns exec P11 ip l set dev veth address 00:00:00:00:01:01
ip netns exec P11 ip a add 10.0.1.1/24 dev veth
ip netns exec P11 ip l set veth up
ip netns exec P11 ethtool -K veth tx off

ip netns exec BRD1 ip l set veth11 up
ip netns exec BRD1 ethtool -K veth11 tx off

# P12 <-> BRD1
ip l add veth netns P12 type veth peer name veth12 netns BRD1
ip netns exec P12 ip l set dev veth address 00:00:00:00:01:02
ip netns exec P12 ip a add 10.0.1.2/24 dev veth
ip netns exec P12 ip l set veth up
ip netns exec P12 ethtool -K veth tx off
ip netns exec P12 ip r add default via 10.0.1.1

ip netns exec BRD1 ip l set veth12 up
ip netns exec BRD1 ethtool -K veth12 tx off

# P13 <-> BRD1
ip l add veth netns P13 type veth peer name veth13 netns BRD1
ip netns exec P13 ip l set dev veth address 00:00:00:00:01:03
ip netns exec P13 ip a add 10.0.1.3/24 dev veth
ip netns exec P13 ip l set veth up
ip netns exec P13 ethtool -K veth tx off
ip netns exec P13 ip r add default via 10.0.1.1

ip netns exec BRD1 ip l set veth13 up
ip netns exec BRD1 ethtool -K veth13 tx off

# P14 <-> BRD1
ip l add veth netns P14 type veth peer name veth14 netns BRD1
ip netns exec P14 ip l set dev veth address 00:00:00:00:01:04
ip netns exec P14 ip a add 10.0.1.4/24 dev veth
ip netns exec P14 ip l set veth up
ip netns exec P14 ethtool -K veth tx off

ip netns exec BRD1 ip l set veth14 up
ip netns exec BRD1 ethtool -K veth14 tx off