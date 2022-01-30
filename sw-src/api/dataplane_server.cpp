//
// Created by xilinx_0 on 1/14/22.
//
#include "api/dataplane_server.h"

int main(int argc, char * argv[]) {
//    google::InitGoogleLogging(argv[0]);
    auto pipe = new Pipeline(PROC_NUM);

    init_mem();
    api::RunServer(pipe);

    return 0;
}

