#pragma once

namespace ipsa_configuration {
    const int SRAM_WIDTH = 128;
    const int TCAM_WIDTH = 64;
    const int SRAM_DEPTH = 1024;
    const int TCAM_DEPTH = 1024;
    const int MAX_LEVEL = 2;
    const int CLUSTER_COUNT = 4;
    const int CLUSTER_PROC_COUNT = 4;
    const int PROC_STAGE_COUNT = 4;
    const int CLUSTER_SRAM_COUNT = 16;
    const int CLUSTER_TCAM_COUNT = 4;
    const int PROC_COUNT = CLUSTER_PROC_COUNT * CLUSTER_COUNT;

    const int MAX_STAGE = 128;
    const int MAX_HEADER = 32;

    const int BIT_SIZE = 8;

    const int PORT_INIT = 50010;

    const int MAX_PORT = 30;
}// namespace ipsa_configuration