#!/bin/bash
entryDir=entry/ecmp
fromDir=p4/rp4/ecmp
toDir=../sw_dataplane/temp

rm -f ${toDir}/*

for i in $(seq 2 3)
do
		cp ${fromDir}/1${i}.update ${toDir}/1${i}.update
done

sleep 1

for i in $(seq 2 3)
do
		cp ${entryDir}/new/1${i}.json ${toDir}/1${i}.json
done

