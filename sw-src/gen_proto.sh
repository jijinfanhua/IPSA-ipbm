#!/usr/bin/env bash

# please "source ~/.bashrc" before run this script

rm ./api_pb/*
mkdir -p api_pb
protoc -I ./controller/proto --grpc_out=./api_pb --cpp_out=./api_pb --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
      ./controller/proto/controller_to_dataplane.proto