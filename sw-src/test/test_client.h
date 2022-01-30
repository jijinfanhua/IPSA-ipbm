//
// Created by xilinx_0 on 1/14/22.
//

#ifndef RECONF_SWITCH_IPSA_TEST_CLIENT_H
#define RECONF_SWITCH_IPSA_TEST_CLIENT_H

#include <unistd.h>

struct Cup {
    int a = 0;
    int b = 0;
};

Cup * cup;

void Run(Cup * c) {
    cup = c;
    sleep(5);
    cup->a = 6;
}

#endif //RECONF_SWITCH_IPSA_TEST_CLIENT_H
