//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_ENUMS_H
#define GRPC_TEST_ENUMS_H

enum class MatchType {
    EXACT, TERNARY, LPM, RANGE, NONE
};

enum class MemType {
    MEM_SRAM, MEM_TCAM, NONE
};

#endif //GRPC_TEST_ENUMS_H
