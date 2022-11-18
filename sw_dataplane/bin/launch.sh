#!/bin/bash

echo 'launch all server'
EXE_SH=/home/xilinx_0/walker/sw/bin
sudo sh $EXE_SH/test.sh &
child_pid=$!
second=1
echo $child_pid
echo wait $second seconds to kill server.
sleep $second
kill $child_pid

