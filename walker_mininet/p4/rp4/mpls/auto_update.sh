#!/bin/bash
entryDir=entry/mpls
fromDir=p4/rp4/mpls
toDir=../sw_dataplane/temp

rm -f ${toDir}/*

for i in $(seq 4 6)
do
  	cp ${fromDir}/${i}.update ${toDir}/${i}.update
done

sleep 1

# cp ${entryDir}/new/1.json ${toDir}/1.json
for i in $(seq 4 7)
do
		cp ${entryDir}/new/${i}.json ${toDir}/${i}.json
done

