//
// Created by xilinx_0 on 1/13/22.
//

#include <iostream>

#include "../dataplane/global.cpp"

using namespace std;

int main() {
    int data = 511;
    auto res = get_uints(data, 9);
    for(int i = 0 ; i < 2; i++) {
        std::cout << (int) res[i] << std::endl;
    }

    PHV * phv = new PHV();
    memset(phv->packet, 0, sizeof(uint8_t) * 2);

    phv->parsed_headers[0]->hdr_offset = 0;
    phv->parsed_headers[0]->hdr_id = 0;

    FieldInfo fd {0, 0, 9};
    Data d {9, res};
    modify_phv_with_data(fd, d, phv);

    std::cout << get_field_value(fd, phv) << std::endl;

//    for(int i = 0 ; i < 2; i++) {
//        std::cout << (int) phv->packet[i] << std::endl;
//    }

    return 0;
}