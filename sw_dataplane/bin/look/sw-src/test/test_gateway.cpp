//
// Created by xilinx_0 on 1/5/22.
//

#include "../dataplane/gateway.h"
#include "../dataplane/parser.h"

using namespace std;

#include <iostream>

void packet_parse(PHV * phv) {
    auto parser = new Parser();

    parser->init_parser_level(3);

    uint8_t key[] = "\x00\x00\x00\xff\xff";
    // hdr_id, hdr_len, next_state, trans_fd_num; (hdr_id, internal_offset, length)
    uint8_t value[] = "\x00\x00\x70\x01\x01\x00\x00\x60\x00\x10";
    parser->modify_parser_entry(0, key, value);

    uint8_t key1[] = "\x01\x00\x00\x08\x00";
    uint8_t value1[] = "\x01\x00\xa0\x02\x01\x01\x00\x48\x00\x08";
    parser->modify_parser_entry(1, key1, value1);

    uint8_t key2[] = "\x02\x00\x00\x00\x06";
    uint8_t value2[] = "\x02\x00\xa0\x03\x00";
    parser->modify_parser_entry(2, key2, value2);

    parser->parse(phv);
}

int main() {
    uint8_t packet[] = "\x11\x11\x11\x11\x11\x11\xAC\x1F\x6B\xF8\x7F\x12\x08\x00"
                       "\x45\x00\x00\x2D\x00\x01\x00\x00\x40\x06\xF9\x76\xC0\xA8\x00\x01\xC0\xA8\x00\x02"
                       "\x30\x39\xD4\x31\x00\x00\x00\x00\x00\x00\x00\x00\x50\x02\x20\x00\xC6\x4C\x00\x00";
    PHV *phv = new PHV();
    memcpy(phv->packet, packet, 54 * sizeof(uint8_t));
    phv->cur_trans_fd_num = 1;
    phv->cur_state = 0;

    packet_parse(phv);

    Gateway * gw = new Gateway();

    /******** exp0 start *********/
    RelationExp * exp0 = new RelationExp();
    exp0->param1.type = ParameterType::CONSTANT;
    exp0->param1.constant_data.data_len = 16;
    uint8_t data0[] = "\x01\x11";
    exp0->param1.constant_data.val = data0;

    exp0->param2.type = ParameterType::CONSTANT;
    exp0->param2.constant_data.data_len = 16;
    uint8_t data1[] = "\x01\x11";
    exp0->param2.constant_data.val = data1;

    exp0->relation = RelationCode::EQ;
    /********* exp0 end *************/

    /******** exp1 start *********/

    RelationExp * exp1 = new RelationExp();
    exp1->param1.type = ParameterType::FIELD;
    exp1->param1.field = {0, 0, 8};

    exp1->param2.type = ParameterType::CONSTANT;
    exp1->param2.constant_data.data_len = 8;
    uint8_t data2[] = "\x01";
    exp1->param2.constant_data.val = data2;

    exp1->relation = RelationCode::GT;
    /******** exp1 end *********/

    gw->insert_exp(exp0);
    gw->insert_exp(exp1);

    gw->modify_res_map(128, 1, 3, -1);
    gw->modify_res_map(192, 5, 6, -1);

    gw->execute(phv);


//    std::cout << gw->get_value(param1, phv) << std::endl;
//
//    GateParam param2;
//    param2.type = ParameterType::FIELD;
//    param2.field.hdr_id = 0;
//    param2.field.internal_offset = 5;
//    param2.field.fd_len = 30;
//
//    std::cout << gw->get_value(param2, phv) << std::endl;

    return 0;
}