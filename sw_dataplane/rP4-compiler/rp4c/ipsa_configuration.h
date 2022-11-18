#pragma once

namespace ipsa_configuration {
    const int SRAM_WIDTH = 128;
    const int TCAM_WIDTH = 64;
    const int SRAM_DEPTH = 1024;
    const int TCAM_DEPTH = 1024;
    const int MAX_LEVEL = 4;
    const int CLUSTER_COUNT = 4;
    const int CLUSTER_PROC_COUNT = 4;
    const int CLUSTER_SRAM_COUNT = 256;
    const int CLUSTER_TCAM_COUNT = 128;
    const int PROC_COUNT = CLUSTER_PROC_COUNT * CLUSTER_COUNT;
}