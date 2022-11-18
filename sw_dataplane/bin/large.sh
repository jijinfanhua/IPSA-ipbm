#!/bin/bash


Config=/home/xilinx_0/walker/walker_mininet/config
Entry=/home/xilinx_0/walker/walker_mininet/entry/Large

echo ./controller 50011 $Config/L2Switch.json $Entry/L2Switch11Entry.json
/home/xilinx_0/walker/sw/bin/controller 50011 /home/xilinx_0/walker/walker_mininet/config/L2Switch.json /home/xilinx_0/walker/walker_mininet/entry/Large/L2Switch11Entry.json &
