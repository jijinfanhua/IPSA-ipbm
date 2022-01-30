//
// Created by xilinx_0 on 1/14/22.
//

#include <iostream>

#include "../api/controller_client.h"

int main() {
    try{
        api::CfgClient cfg(grpc::CreateChannel("localhost:50051",
                                               grpc::InsecureChannelCredentials()));
        RC rc = cfg.init_parser_level(0, 3);
        std::cout << rc << std::endl;
        std::cout << cfg.clear_parser(0) << std::endl;
    } catch (const std::runtime_error & e) {
        std::cout << e.what() << std::endl;
    }
    sleep(20);
    return 0;
}