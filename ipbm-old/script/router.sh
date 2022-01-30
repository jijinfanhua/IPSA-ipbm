#!/usr/bin/env bash

set -ex

# Topology:
# R1 <-----------------> P2 <-----------------> P3
#    r1r2           r2r1    r2r3           r3r2
#  10.0.1.50     10.0.1.1  10.0.2.1    10.0.2.50

# netns, P2 is the router
for NS in R1 R2 R3
do
    ip netns delete $NS 2>/dev/null || true
    ip netns add $NS
    ip netns exec $NS ip l set lo up
done

# R1 <-> R2
ip netns exec R1 ip l add r1r2 type veth peer name r2r1 netns R2
ip netns exec R1 ip a add 10.0.1.50/24 dev r1r2
ip netns exec R1 ip l set dev r1r2 address 00:00:00:00:01:50
ip netns exec R1 ip l set r1r2 up
ip netns exec R1 ip r add default via 10.0.1.1
ip netns exec R1 arp -i r1r2 -s 10.0.1.1 00:00:00:00:01:01
ip netns exec R1 ethtool -K r1r2 tx off

ip netns exec R2 ip l set dev r2r1 address 00:00:00:00:01:01
ip netns exec R2 ip l set r2r1 up
ip netns exec R2 ethtool -K r2r1 tx off

# R2 <-> R3
ip netns exec R2 ip l add r2r3 type veth peer name r3r2 netns R3
ip netns exec R3 ip a add 10.0.2.50/24 dev r3r2
ip netns exec R3 ip l set dev r3r2 address 00:00:00:00:02:50
ip netns exec R3 ip l set r3r2 up
ip netns exec R3 ip r add default via 10.0.2.1
ip netns exec R3 arp -i r3r2 -s 10.0.2.1 2a:a4:1d:20:4d:fd
ip netns exec R3 ethtool -K r3r2 tx off

ip netns exec R2 ip l set dev r2r3 address 00:00:00:00:02:01
ip netns exec R2 ip l set r2r3 up
ip netns exec R2 ethtool -K r2r3 tx off
