#!/bin/bash
entryDir=entry/multi_tenant
fromDir=p4/rp4/multi_tenant
toDir=../sw_dataplane/temp

rm -f ${toDir}/*

for i in $(seq 1 3)
do
		cp ${fromDir}/2${i}.update ${toDir}/2${i}.update
done

sleep 1

for i in $(seq 1 3)
do
		cp ${entryDir}/new/2${i}.json ${toDir}/2${i}.json
done

