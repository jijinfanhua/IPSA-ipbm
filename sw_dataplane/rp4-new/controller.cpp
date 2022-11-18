//
// Created by xilinx_0 on 3/4/22.
//
#include "../sw-src/api_pb/controller_to_dataplane.grpc.pb.h"
#include "DataPlaneController.h"
#include <grpcpp/grpcpp.h>
#include <google/protobuf/text_format.h>
#include <fstream>
#include "ipsa_configuration.h"

DataPlaneController* controllers[30];

void InitializeController() {
    for (int i = 0 ; i < ipsa_configuration::MAX_PORT; i ++) {
        controllers[i] = new DataPlaneController(i + ipsa_configuration::PORT_INIT);
    }
}

void insertConfig(int port, std::string input_filename) {
    std::cout << port << " -> config " << input_filename << std::endl;
    json j;

    std::ifstream in(input_filename);
    in >> j;

    DataPlaneController* controller = controllers[port];
    controller->Extract(j);
}

void insertEntry(int port, std::string entry_path) {
    std::cout << port << " -> entry " << entry_path << std::endl;
    DataPlaneController* controller = controllers[port];
    controller->TestInsertEntry(entry_path);
}