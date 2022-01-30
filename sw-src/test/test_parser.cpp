//
// Created by xilinx_0 on 12/24/21.
//

#include "../dataplane/parser.h"
#include <iostream>
#include <cstring>
using namespace std;

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

// [][][][][][][1][1][1]  [1][1][1][1][][][][]
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

//    uint8_t key[] = "\x00\x00\x00\xff\xff";
//    // hdr_id, hdr_len, next_state, trans_fd_num; (hdr_id, internal_offset, length)
//    uint8_t value[] = "\x00\x00\x70\x01\x01\x00\x00\x60\x00\x10";
////    uint8_t value[] = "\x00\x00\x70\x01\x01\x00\x00\x5e\x00\x11";
//    parser->modify_parser_entry(0, key, value);
//
//    uint8_t key1[] = "\x01\x00\x00\x08\x00";
//    uint8_t value1[] = "\x01\x00\xa0\x02\x01\x01\x00\x48\x00\x08";
//    parser->modify_parser_entry(1, key1, value1);
//
//    uint8_t key2[] = "\x02\x00\x00\x00\x06";
//    uint8_t value2[] = "\x02\x00\xa0\x03\x00";
//    parser->modify_parser_entry(2, key2, value2);

    parser->parse(phv);

    return 0;
}

