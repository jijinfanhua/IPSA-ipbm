//
// Created by fengyong on 2021/2/22.
//

#ifndef GRPC_TEST_STANDARD_METADATA_H
#define GRPC_TEST_STANDARD_METADATA_H

#include <iostream>
#include <unordered_map>

class StandardMetadata {

    std::unordered_map<std::string, int> field_map = {
            {"ingress_port", 1},
            {"egress_spec", 2},
            {"egress_port", 3},
            {"instance_type", 4},
            {"packet_length", 5},
            {"enq_timestamp", 6},
            {"enq_qdepth", 7},
            {"deq_timedelta", 8},
            {"deq_qdepth", 9},
            {"ingress_global_timestamp", 10},
            {"egress_global_timestamp", 11},
            {"mcast_grp", 12},
            {"egress_rid", 13},
            {"checksum_error", 14},
            {"priority", 15}
    }
};

#endif //GRPC_TEST_STANDARD_METADATA_H
