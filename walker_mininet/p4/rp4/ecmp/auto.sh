#!/bin/bash

entryDir=entry/ecmp
fromDir=p4/rp4/ecmp
toDir=../sw_dataplane/temp

rm -f ${toDir}/*

for i in $(seq 2 4)
do
		cp ${fromDir}/1${i}.rp4 ${toDir}/1${i}.rp4
done
cp ${fromDir}/16.rp4 ${toDir}/16.rp4

sleep 1

for i in $(seq 2 4)
do
		cp ${entryDir}/1${i}.json ${toDir}/1${i}.json
done
cp ${entryDir}/16.json ${toDir}/16.json
