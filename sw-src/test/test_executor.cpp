//
// Created by xilinx_0 on 1/2/22.
//

#include <iostream>
#include <cstring>

#include "../dataplane/executor.h"
#include "../dataplane/parser.h"

using namespace std;

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

void test_add(PHV * phv) { // field and const
    int para_num0 = 3;
    Primitive *p0 = new Primitive();
    p0->parameter_num = 3;
    p0->op = OpCode::ADD;
    Parameter _para0[3];
    _para0[0].type = ParameterType::FIELD;
    _para0[0].val.field = {0, 0, 8};

//    _para0[1].type = ParameterType::CONSTANT;
//    uint8_t _data_0[] = "\x01";
//    _para0[1].val.constant_data = {8, _data_0};

    _para0[1].type = ParameterType::FIELD;
    uint8_t _data_0[] = "\x01";
    _para0[1].val.field = {0, 8, 8};

//    _para0[2].type = ParameterType::CONSTANT;
//    uint8_t _data_1[] = "\x02";
//    _para0[2].val.constant_data = {8, _data_1};

    _para0[2].type = ParameterType::PARAM;
    _para0[2].val.action_para_id = 0;

    p0->set_para(_para0);

    std::vector<Primitive*> _prims;
    _prims.push_back(p0);

    Action * ac0 = new Action(para_num0);
    ac0->set_primitive(_prims);

    std::vector<int> para_lens = {5, 12, 19};
    ac0->set_action_para_lens(para_lens);

    Executor * exe = new Executor();
    exe->insert_action(ac0, 0);

    uint8_t data[] = "\x00\x00\x55\x44\x33\x22\x11\x66";

    exe->set_action_para_value(data);

    exe->execute_action(0, phv);

//    ac->set_action_para_lens(para_lens);
//
//    ac->set_action_paras(data);

//    ac0->execute(phv);
}

void test_set_copy(PHV * phv) {
    int para_num0 = 0;
    Primitive *p0 = new Primitive();
    p0->parameter_num = 2;
    p0->op = OpCode::SET_FIELD;

    Parameter _para0[2];
    _para0[0].type = ParameterType::FIELD;
    _para0[0].val.field = {0, 6, 7};

    _para0[1].type = ParameterType::CONSTANT;
    uint8_t _data_1[] = "\x87";
    _para0[1].val.constant_data = {8, _data_1};

    p0->set_para(_para0);
    /**** primitive 0 done ****/

//    Primitive *p1 = new Primitive();
//    p1->parameter_num = 2;
//    p1->op = OpCode::COPY_FIELD;
//
//    Parameter _para1[2];
//    _para1[0].type = ParameterType::FIELD;
//    _para1[0].val.field = {0, 6, 7};
//
//    _para1[0].type = ParameterType::FIELD;
//    _para1[0].val.field = {0, 16, 8};
//    p1->set_para(_para1);
    /***** primitive 1 done *****/

    std::vector<Primitive*> _prims;
    _prims.push_back(p0);
//    _prims.push_back(p1);

    Action * ac0 = new Action(0);
    ac0->set_primitive(_prims);

    ac0->execute(phv);
}

void test_para_set_add(PHV * phv) {

}


void test_set_action_para() {
    Action * ac = new Action(3);

    uint8_t data[] = "\x00\x00\x55\x44\x33\x22\x11\x66";
    std::vector<int> para_lens = {5, 12, 19};

    ac->set_action_para_lens(para_lens);

    ac->set_action_paras(data);
}

int main() {
    uint8_t packet[] = "\x11\x11\x11\x11\x11\x11\xAC\x1F\x6B\xF8\x7F\x12\x08\x00"
                       "\x45\x00\x00\x2D\x00\x01\x00\x00\x40\x06\xF9\x76\xC0\xA8\x00\x01\xC0\xA8\x00\x02"
                       "\x30\x39\xD4\x31\x00\x00\x00\x00\x00\x00\x00\x00\x50\x02\x20\x00\xC6\x4C\x00\x00";
    PHV *phv = new PHV();
    memcpy(phv->packet, packet, 54 * sizeof(uint8_t));
    phv->cur_trans_fd_num = 1;
    phv->cur_state = 0;

//    uint8_t a = 17, b = 0, c = 3;
//    std::cout << (int)((a & b) + c) << std::endl;
    packet_parse(phv); // get the header info: Ethernet, IPv4, TCP
//
//    test_set_copy(phv);
////    test_add(phv);
//
//    std::cout << (int)phv->packet[0] << std::endl;
//    std::cout << (int)phv->packet[1] << std::endl;
//    std::cout << (int)phv->packet[2] << std::endl;

//    test_set_action_para();

    test_add(phv);

    std::cout << (int)phv->packet[0] << std::endl;
    std::cout << (int)phv->packet[1] << std::endl;

//    uint8_t a = 85;
//    std::cout << (int)((uint8_t)(a << 5) >> 5) << std::endl;

    return 0;
}