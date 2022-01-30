//
// Created by xilinx_0 on 1/14/22.
//

#include <iostream>

#include <thread>
#include <unistd.h>
#include <cstring>

//#include "test_client.h"

struct Mem {
    int x = 8;
    int y = 10;
    Mem() = default;
};

struct Act {
    int a = 1;
    int b = 3;
    int c = 5;
    Mem mem{6,6};
    Act() = default;
};

struct HeaderInfo { // all of the offset and length are in bit
    uint8_t hdr_id;
    uint16_t hdr_offset; // should be larger than 32 bytes, e.g., uint16
    uint16_t hdr_len;
};

struct PHV {
    // the offset of metadata is (front_header_length), and the id can be set as 15
    uint8_t packet[100 + 50]{};
    // header_info
    HeaderInfo* parsed_headers[32]{};
    uint8_t valid_bitmap[32]{0};
    uint32_t bitmap = 0;

    int cur_state = 0;
    uint16_t cur_offset = 0;
    int cur_trans_fd_num = 0;
    uint32_t cur_trans_key; // derived from cur_trans_fds or initialized
    bool tcam_miss;

    uint8_t * match_value;
    int match_value_len;

    bool hit = false;

    int next_proc_id = 0;
    int next_matcher_id = 0;
    int next_action_id = 0;

    int ig_if = 0;
    int eg_if = 0;

    PHV() {
        for(auto & parsed_header : parsed_headers) {
            parsed_header = new HeaderInfo();
        }
        tcam_miss = true;
        cur_trans_key = 0xffff; // the first stage is to extract "Ethernet"
        bitmap = 0;
    }
};

int main() {

    uint8_t new_mod[] = "\x00\x00\x00\x01\x01";
    int mod_len = 48;

    uint8_t * mod;
    if(5 < 6) {
        mod = new uint8_t[6];
        memcpy(mod + 1, new_mod, 5);
    }

    uint8_t res[6] = {0};
    int right_empty = 0;

    // 0000 0001   1111 1111   1000 0000
    //             0000 0011   1111 1111

//    int idx = 1;
    for(int i = 5; i >= 1; i--) {
        res[i] += (mod[i] << right_empty);
        res[i-1] += (mod[i] >> (8 - right_empty));
//        if(idx >= 1) {
//            idx--;
//        } else {
//            break;
//        }
    }
    res[0] += (mod[0] << right_empty);

    for(int i = 0; i < 6; i++) {
        std::cout << (int)res[i] << std::endl;
    }


//    int a = 0xffffffff;
//
//    std::cout << uint32_t(a) << std::endl;

//    auto phv = new PHV();
//
//    memset(phv->packet, 1, 150);
//    std::cout << (int)phv->packet[0] << std::endl;
//
////    free(phv->packet);
//    for(int i = 0; i < 32; i++) {
//        delete phv->parsed_headers[i];
//    }
//    delete phv->match_value;
//    delete phv;
//    std::cout << (int)phv->packet[0] << std::endl;
//    delete phv;


//    char a[4] = {97, 98, 99, 100};
//    uint8_t b[4] = {0x61, 0x62, 0x63, 0x64};
//
//    char c[4];
//    for(int i = 0 ; i < 4; i++) {
//        c[i] = int(b[i]);
//    }
//
//    std::string d(a, 4);
//
//    std::cout << sizeof(a[0]) << std::endl;
//    std::cout << sizeof(b[0]) << std::endl;
//    std::cout << sizeof(c[0]) << std::endl;
//    std::cout << d.length() << " : " << d << std::endl;

    return 0;
}