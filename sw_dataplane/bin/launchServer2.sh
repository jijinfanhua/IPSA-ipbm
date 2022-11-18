#!/bin/bash
Interface=/home/xilinx_0/walker/walker_mininet/interface/L2Switch_Router

L2Switch1Interface=$Interface/L2Switch1.yml
L2Switch2Interface=$Interface/L2Switch2.yml
Router3Interface=$Interface/Router3.yml

L2Switch1RPCPort=50052
L2Switch2RPCPort=50053
Router3RPCPort=50054

sudo ./main $L2Switch2Interface $L2Switch2RPCPort
