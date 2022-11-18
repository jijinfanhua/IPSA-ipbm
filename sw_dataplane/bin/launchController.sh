#!/bin/bash
Config=/home/xilinx_0/walker/walker_mininet/config
Entry=/home/xilinx_0/walker/walker_mininet/entry


L2Switch1Config=$Config/L2Switch.json
L2Switch1Entry=$Entry/L2Switch1Entry.json
L2Switch1Port=50052
L2Switch2Config=$Config/L2Switch.json
L2Switch2Entry=$Entry/L2Switch2Entry.json
L2Switch2Port=50053
RouterConfig=$Config/Router.json
RouterEntry=$Entry/RouterEntry.json
RouterPort=50054

sudo ./controller $RouterPort    $RouterConfig    $RouterEntry
sudo ./controller $L2Switch1Port $L2Switch1Config $L2Switch1Entry
sudo ./controller $L2Switch2Port $L2Switch2Config $L2Switch2Entry
