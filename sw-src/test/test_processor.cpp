//
// Created by xilinx_0 on 1/6/22.
//

#include <iostream>

#include "../dataplane/global.h"

int main() {
    uint8_t a[] = "\x11\x11";
    PHV * phv = new PHV();

    phv->match_value = a;
    phv->match_value_len = 2;

    for(int i = 0 ; i < 2; i++) {
        std::cout << (int)phv->match_value[i] << std::endl;
    }

    return 0;
}