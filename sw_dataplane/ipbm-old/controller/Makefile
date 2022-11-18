
HOST_SYSTEM = $(shell uname | cut -f 1 -d_)

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++14
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
			-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
			-ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ./proto

vpath %.proto $(PROTOS_PATH)

all: client server

client: controller_dataplane_api.pb.o controller_dataplane_api.grpc.pb.o client.o
		$(CXX) $^ $(LDFLAGS) -o $@

server: controller_dataplane_api.pb.o controller_dataplane_api.grpc.pb.o server.o
		$(CXX) $^ $(LDFLAGS) -o $@

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
		$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
		$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
		rm -f *.o *.pb.cc *.pb.h server client