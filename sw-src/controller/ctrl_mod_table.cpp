//
// Created by xilinx_0 on 1/27/22.
//

#include "ctrl_mod_table.h"

#define MAIN

int main (int argc, char * argv[]) {

#ifndef NO_CFG
    api::CfgClient cfg(grpc::CreateChannel("localhost:50051",
                                           grpc::InsecureChannelCredentials()));

    #ifdef MAIN
    extract(argv[1], cfg);
    #else
    extract("../../sw-src/controller/update.json", cfg);
    #endif

#else
    extract("../../sw-src/controller/update.json");
#endif

    return 0;
}