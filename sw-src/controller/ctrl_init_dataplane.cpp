//
// Created by xilinx_0 on 1/26/22.
//

#include "ctrl_init_dataplane.h"

#define MAIN

int main (int argc, char * argv[]) {

#ifndef NO_CFG
    api::CfgClient cfg(grpc::CreateChannel("localhost:50051",
                                           grpc::InsecureChannelCredentials()));
    #ifdef MAIN
        extract(argv[1], cfg);
    #else
        extract("../../sw-src/controller/basic.json", cfg);
    #endif
#else
    extract("../../sw-src/controller/basic.json");
#endif

    return 0;
}