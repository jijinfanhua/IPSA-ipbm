//
// Created by xilinx_0 on 12/30/21.
//

#include <iostream>
#include <cstring>

#include "../dataplane/matcher_c.h"
#include "../dataplane/parser.h"

using namespace std;

//void test_sram_insert() {
//    Matcher * matcher = new Matcher();
//    uint8_t key_config[] = {0, 1, 4, 5};
//    uint8_t value_config[]= {2, 3};
//    matcher->set_sram_config(2, 1, 2, key_config, value_config);
//
//    uint8_t key[] = "\xff\xff\xff\xff\xff\xff\xff\xff"
//                    "\xff\xff\xff\xff\xff\xff\xff\xff"
//                    "\xff\xff"; // 3 bytes
//    uint8_t value[] = "\x11\x11"; // 2 byte
//    matcher->insert_entry(key, value, 18, 2);
//
////    matcher->print_entry(3);
//
//    uint8_t * out_key = new uint8_t[16];
//    uint8_t * out_value = new uint8_t[16];
//    matcher->get_match_res(key, 18 * 8, out_key, out_value);
//    for(int i = 0; i < 16; i++) {
//        std::cout << (int)out_value[i] << " ";
//    }
////    std::cout << out_value;
//    std::cout << std::endl;
//
//    int tag = 0;
//    for(int i = 0; i < 18; i++) {
//        if(key[i] != out_key[i]) {
//            std::cout << "No entry!" << std::endl;
//            tag = 1;
//            break;
//        }
//    }
//
//    if (tag == 0) {
//        std::cout << "Match success!" << std::endl;
//    }
//
//}

void insert_entry(Parser * parser) {
    FieldInfo * fi0 = new FieldInfo{0, 96, 16};
    std::vector<FieldInfo*> fis0;
    fis0.push_back(fi0);

    parser->modify_parser_entry_direct(0, 0, 0xffff, 0xffffffff, 0, 112, 1, 1, fis0);

    FieldInfo * fi1 = new FieldInfo{1, 72, 8};
    std::vector<FieldInfo*> fis1;
    fis1.push_back(fi1);
    parser->modify_parser_entry_direct(1, 1, 0x0800, 0xffffffff, 1, 160, 2, 1, fis1);

    std::vector<FieldInfo*> fis2;
    parser->modify_parser_entry_direct(2, 2, 0x6, 0xffffffff, 2, 160, 3, 0, fis2);

}

void test_matcher_thread() {
    init_mem();
    MatcherThread * mt = new MatcherThread(0); // sram[0] sram[1]

    uint8_t key_config[] = {0};
    uint8_t value_config[]= {1};
    mt->set_mem_config(1,1,1,key_config,value_config);

    uint8_t key0[] = "\x11\x11\x22\x22";
    uint8_t value0[] = "\x33\x44\x00";
    mt->insert_sram_entry(key0, value0, 4, 3);

//    mt->print_entry(1);
}

void test_get_field(PHV * phv) {
    MatcherThread * mt = new MatcherThread(0);

    FieldInfo * fd = new FieldInfo{0, 6, 12};
    KeyInfo * ki = mt->get_field(fd, phv);
    std::cout << ki->tail_offset << " ~ " << ki->byte_len << " ~ " << ki->field_len << std::endl;
    for(int i = 0; i < ki->byte_len; i++) {
        std::cout << (int)ki->field[i] << std::endl;
    }
}

void test_combine(PHV * phv) {
    MatcherThread * mt = new MatcherThread(0);

    FieldInfo * fd0 = new FieldInfo{0, 3,12};
    FieldInfo * fd1 = new FieldInfo{0, 17, 10};
    std::vector<KeyInfo*> keyInfos;
    keyInfos.push_back(mt->get_field(fd0, phv));
    keyInfos.push_back(mt->get_field(fd1, phv));

    uint8_t * key = mt->combine_to_single_key(keyInfos, 22);

    int len = 22 / 8 + ((22 % 8) > 0);
    for(int i = 0; i < len; i++) {
        std::cout << (int) key[i] << std::endl;
    }
}


int main() {

    uint8_t packet[] = "\x11\x11\x11\x11\x11\x11\xAC\x1F\x6B\xF8\x7F\x12\x08\x00"
                       "\x45\x00\x00\x2D\x00\x01\x00\x00\x40\x06\xF9\x76\xC0\xA8\x00\x01\xC0\xA8\x00\x02"
                       "\x30\x39\xD4\x31\x00\x00\x00\x00\x00\x00\x00\x00\x50\x02\x20\x00\xC6\x4C\x00\x00";

    PHV *phv = new PHV();
    memcpy(phv->packet, packet, 54 * sizeof(uint8_t));
    phv->cur_trans_fd_num = 1;
    phv->cur_state = 0;
    phv->cur_trans_key = 0xffff;

    auto parser = new Parser();
    parser->init_parser_level(3);
    insert_entry(parser);
    parser->parse(phv);

    test_combine(phv);

    // 2.
//    test_get_field(phv);

//    int begin = 14;
//    int end = 16;
//
//    uint8_t field[end - begin + 1];
//
//    memcpy(field, packet + begin, end - begin + 1);
//
//    std::cout << (int)field[0] << std::endl;

//    uint8_t packet[] = "\xff\x00\xff\x00\xff\x00";
//    // 11111111 00000000 11111111 00000000 11111111 00000000
//    // 11111 000 00000 111 11
//    // 11 00000000 11111111 0000
//    // 11000000 00111111 110000
//


//    std::vector<KeyInfo*> keyInfos;
//
//    uint8_t fd0[] = "\xff\xff\xa0";
//    KeyInfo * k0 = new KeyInfo(3, fd0, 3, 19);
//    keyInfos.push_back(k0);
//
//    uint8_t fd1[] = "\x80";
//    KeyInfo * k1 = new KeyInfo(2, fd1, 1, 2);
//    keyInfos.push_back(k1);
//
//    uint8_t fd2[] = "\xff\x54";
//    KeyInfo * k2 = new KeyInfo(6, fd2, 2, 14);
//    keyInfos.push_back(k2);
//
//    LEN fd_len_sum = 35;
//
//    Matcher * m = new Matcher();
//    uint8_t * result = m->combine_to_single_key(keyInfos, fd_len_sum);
//
//    for(int i = 0; i < 5; i++) {
//        std::cout << (int)result[i] << std::endl;
//    }

//    test_sram_insert();

    // 1.
//    test_matcher_thread();

    return 0;
}