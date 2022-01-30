#!/usr/bin/env bash

set -ex

for NS in R1 R2 R3
do
  ip netns delete $NS 2>/dev/null || true
  ip netns add $NS
  ip netns exec $NS ip link set lo up
done

# R1 <-> R2
ip netns exec R1 ip link add r1r2 type veth peer name r2r1 netns R2
ip netns exec R1 ip addr add 10.0.1.50/24 dev r1r2
ip netns exec R1 ip link set dev r1r2 address 00:00:00:00:01:50
ip netns exec R1 ip link set r1r2 up
ip netns exec R1 ip route add default via 10.0.1.1
ip netns exec R1 arp -i r1r2 -s 10.0.1.1 00:00:00:00:01:01
ip netns exec R1 ethtool -K r1r2 tx off

ip netns exec R2 ip link set dev r2r1 address 00:00:00:00:01:01
ip netns exec R2 ip link set r2r1 up
ip netns exec R2 ethtool -K r2r1 tx off

# R2 <-> R3
ip netns exec R3 ip link add r3r2 type veth peer name r2r3 netns R2
ip netns exec R3 ip addr add 10.0.2.50/24 dev r3r2
ip netns exec R3 ip link set dev r3r2 address 00:00:00:00:02:50
ip netns exec R3 ip link set r3r2 up
ip netns exec R3 ip route add default via 10.0.2.1
ip netns exec R3 arp -i r3r2 -s 10.0.2.1 00:00:00:00:02:01
ip netns exec R3 ethtool -K r3r2 tx off

ip netns exec R2 ip link set dev r2r3 address 00:00:00:00:02:01
ip netns exec R2 ip link set r2r3 up
ip netns exec R2 ethtool -K r2r3 tx off

#ip netns exec R1 ip link add r1r4 type veth peer name r4r1 netns R4
#ip netns exec R1 ip link set r1r4 up
#ip netns exec R4 ip link set r4r1 up
#ip netns exec R4 ip addr add 10.0.1.40/24 dev r4r1
#ip netns exec R1 ip link set dev r1r4 address 00:00:00:00:01:60
#ip netns exec R4 ip link set dev r4r1 address 00:00:00:00:01:40
#ip netns exec R4 ip route add default via 10.0.1.1