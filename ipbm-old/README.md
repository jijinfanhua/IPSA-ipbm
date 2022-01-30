# Reconfigurable Switch

**Recent Aim** (up to 2020/2/28):

1. Implement basic forwarding (`ethernet->vlan->ipv4->tcp`);
2. Software device can support 8 processors/stages/threads, 8 SRAM slices, 8 TCAM slices, memory size depends;
3. User can add a new protocol and corresponding protocol processing logic -> *protocol modification at run-time*;
4. User can add ip tunneling or mpls routing -> *processing logic modification at run-time*;
5. User can add network measurement:
    - controller can add/delete/modify flow entries;
    - controller can modify parser dynamically (**add protocol** first, and then delete/modify)
    - controller can modify matcher logic (**add** first, and then delete/modify);
    - controller can modify action logic and parameters (**add** first, and then delete/modify)
6. Implement rP4 parser;
7. Design commands to data plane -> further GUI.

**Supported commands**:
1. init_mem `[mem file]` ([sram](https://github.com/li-plus/reconf-switch/blob/main/controller/config/sram_config.txt) and [tcam](https://github.com/li-plus/reconf-switch/blob/main/controller/config/tcam_config.txt) capacity, and the corresponding logical table clusters, [sram spec](https://github.com/li-plus/reconf-switch/blob/main/controller/config/sram.json), [tcam spec](https://github.com/li-plus/reconf-switch/blob/main/controller/config/tcam.json), **before run-time**)
2. init_header `[header file]` (protocol specs, **before run-time**)
3. init_flow_table `[flow table file]` (protocol specs, **before run-time**, below are at run-time)
4. add_entry ****
5. update_entry ****
6. delete_entry ****
7. add_table `[flow table spec file]`
8. add_header `[protocol spec file]`
9. add_matcher (optional)
10. add_executer `[executer spec file (multiple actions)]`
11. add_action `[action spec, like add_executer]`
12. print_parser (parser graph)

## Build

Install cmake, [pcap](https://www.tcpdump.org/), [yaml-cpp](https://github.com/jbeder/yaml-cpp), [glog](https://github.com/google/glog), [gflags](https://github.com/gflags/gflags)

```shell
sudo apt install -y cmake libpcap-dev libyaml-cpp-dev libgoogle-glog-dev libgflags-dev
```

Compile the project.

```shell
mkdir -p build && cd build
cmake ..
make -j
```

## Run

Run router

```shell
sudo bash ../script/router.sh
sudo ip netns exec R2 ./bin/main -logtostderr=1 -ifconf ../conf/router.yml
# connectivity test
sudo ip netns exec R1 ping 10.0.2.50 -c 4
sudo ip netns exec R3 ping 10.0.1.50 -c 4
# speed test
sudo ip netns exec R3 iperf3 -s
sudo ip netns exec R1 iperf3 -c 10.0.2.50 -O 5 -P 10
```

Start controller

```shell
sudo ip netns exec R2 ./bin/ctrl_flow_entry
```

Run switch

```shell
sudo bash ../script/switch.sh 
sudo ip netns exec BRD1 ./bin/main -logtostderr=1 -ifconf ../conf/switch.yml
# test
sudo ip netns exec P12 ping 10.0.1.3 -c 4
sudo ip netns exec P12 ping 10.0.1.1 -c 4
sudo ip netns exec P12 ping 10.0.1.4 -c 4
sudo ip netns exec P11 ping 10.0.1.3 -c 4
```

Start controller

```shell
sudo ip netns exec BRD1 ./bin/ctrl_vlan
```

Run tunnel

```shell
cd ../script/
sudo bash router.sh
sudo ip netns exec R2 ../build/bin/main -logtostderr=1 -ifconf ../conf/router.yml
sudo ip netns exec R3 python3 tunnel_recv.py
sudo ip netns exec R1 python3 tunnel_send.py 10.0.2.50 "hello world" --dst_id 1
```

Counter controller 

```shell
sudo ip netns exec R2 ./bin/ctrl_counter
```

### Complex

```sh
sudo bash ../script/complex.sh
# start switch
sudo ip netns exec BRD1 ./bin/main -logtostderr=1 -ifconf ../conf/complex.yml
# CONTROLLER: L2 & L3 forwarding
sudo ip netns exec BRD1 ./bin/ctrl_complex_fwd
# mac forward
sudo ip netns exec P12 ping 10.0.1.3 -c 4
sudo ip netns exec P14 ping 10.0.1.5 -c 4
sudo ip netns exec P13 ping 10.0.1.4 -c 4
# ip forward
sudo ip netns exec P14 ping 10.0.2.2 -c 4
# CONTROLLER: Add VLAN
sudo ip netns exec BRD1 ./bin/ctrl_complex_vlan
# vlan
sudo ip netns exec P14 ping 10.0.1.5 -c 4   # L2 forward
sudo ip netns exec P12 ping 10.0.1.3 -c 4   # L2 forward
sudo ip netns exec P13 ping 10.0.1.4 -c 4   # L3 forward
sudo ip netns exec P14 ping 10.0.2.2 -c 4   # L3 forward
# CONTROLLER: Add tunnel
sudo ip netns exec BRD1 ./bin/ctrl_complex_tunnel
# tunnel
sudo ip netns exec P22 python3 ../script/tunnel_recv.py
sudo ip netns exec P13 python3 ../script/tunnel_send.py 10.0.2.2 "hello world" --dst_id 4
# CONTROLLER: Add counter
sudo ip netns exec BRD1 ./bin/ctrl_complex_counter
# counter
sudo ip netns exec P12 ping 10.0.2.2 -c 4
sudo ip netns exec P22 ping 10.0.1.2 -c 4
sudo ip netns exec BRD1 ./bin/ctrl_complex_counter_monitor
```
