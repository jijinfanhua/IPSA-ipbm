#!/bin/bash
entryDir=entry/mpls
fromDir=p4/rp4/mpls
toDir=../sw_dataplane/temp

rm -f ${toDir}/*

#cp ${fromDir}/1.rp4 ${toDir}/1.rp4
for i in $(seq 4 7)
do
		cp ${fromDir}/${i}.rp4 ${toDir}/${i}.rp4
done

sleep 1

#cp ${entryDir}/1.json ${toDir}/1.json
for i in $(seq 4 7)
do
		cp ${entryDir}/${i}.json ${toDir}/${i}.json
done

