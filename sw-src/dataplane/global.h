//
// Created by xilinx_0 on 12/27/21.
//
#pragma once
#ifndef RECONF_SWITCH_IPSA_GLOBAL_H
#define RECONF_SWITCH_IPSA_GLOBAL_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

#include <arpa/inet.h>
#include <net/ethernet.h>

#include "defs.h"

//#define PHYSICAL_SRAM_CAPACITY (128 * 1024)
//#define PHYSICAL_SRAM_CAPACITY_BYTE PHYSICAL_SRAM_CAPACITY/8
//#define SRAM_DEPTH 10
//#define SRAM_WIDTH 128
//
//#define MATCHER_THREAD_NUM 16
//
//typedef uint16_t LEN;
//
//const static int CLUSTER_N = 20;
//const static int REG_WIDTH = 4; // log2(CLUSTER_N) up
//
//const static int SRAM_NUM_ALL = 80;//0;
//const static int CLUSTER_NUM = 4;
//const static int PROC_NUM = 12;//12;
//const static int PROC_NUM_PER_CLUSTER = PROC_NUM / CLUSTER_NUM;
//const static int SRAM_NUM_PER_CLUSTER = SRAM_NUM_ALL / CLUSTER_NUM;
//const static int SRAM_IDX_BASE = SRAM_NUM_ALL / CLUSTER_NUM;
//
//static const int FRONT_HEADER_LEN = 1500; // bytes
//static const int META_LEN = 100;
//
//enum class OpCode {
//    ADD, SUB, MUL, DIV, MOD,
//    SHIFT_LEFT, SHIFT_RIGHT, BIT_AND, BIT_OR, BIT_XOR, BIT_NEG,
//    SET_FIELD, COPY_FIELD,
//    SET_TTL, DECREMENT_TTL, COPY_TTL_OUTWARDS, COPY_TTL_INWARDS,
//    PUSH, POP, DECREMENT
//};
//
//typedef enum class ParameterType {
//    CONSTANT, HEADER, FIELD, PARAM
//} PT;
//
////struct Parameter {
////    ParameterType type;
////};
//
//struct HeaderInfo { // all of the offset and length are in bit
//    uint8_t hdr_id;
//    uint16_t hdr_offset; // should be larger than 32 bytes, e.g., uint16
//    uint16_t hdr_len;
//};
//
//struct FieldInfo { // 24
//    uint8_t hdr_id;
//    uint16_t internal_offset; // 2^8 = 256 bits = 32 bytes, enough for current version
//    uint16_t fd_len;
//};
//
//struct Constant {
//    int value;
//};
//
//struct Sram {
//    int width = 128;
//    int depth = 1024;
//
//    uint8_t tbl[PHYSICAL_SRAM_CAPACITY / 8];
//    Sram () {
//        memset(tbl,0, sizeof(uint8_t) * (PHYSICAL_SRAM_CAPACITY / 8));
//    }
//};

//Sram * srams[SRAM_NUM_ALL];
//Tcam * tcams[TCAM_NUM_ALL];

//void init_mem() {
//    for(int i = 0; i < SRAM_NUM_ALL; i++) {
//        srams[i] = new Sram();
//    }
//    for(int i = 0; i < TCAM_NUM_ALL; i++) {
//        tcams[i] = new Tcam();
//    }
//}


//struct PHV {
//    // the offset of metadata is (front_header_length), and the id can be set as 15
//    uint8_t packet[FRONT_HEADER_LEN + META_LEN]{};
//    // header_info
//    HeaderInfo* parsed_headers[32]{};
//    uint32_t bitmap = 0;
//
//    int cur_state = 0;
//    uint16_t cur_offset = 0;
//    int cur_trans_fd_num = 0;
//    uint32_t cur_trans_key; // derived from cur_trans_fds or initialized
//    bool tcam_miss;
//
//    uint8_t * match_value;
//    int match_value_len;
//
//    bool hit = false;
//
//    int next_proc_id = 0;
//    int next_matcher_id = 0;
//
//    int ig_if = 0;
//    int eg_if = 0;
//
//    PHV() {
//        for(auto & parsed_header : parsed_headers) {
//            parsed_header = new HeaderInfo();
//        }
//        tcam_miss = true;
//        cur_trans_key = 0xffff; // the first stage is to extract "Ethernet"
//        bitmap = 0;
//    }
//};

struct Global {
    std::vector<std::string> if_names;
    std::vector<in_addr_t> if_ips;
    std::vector<ether_addr> if_macs;
    int num_if;
    Sram * srams[SRAM_NUM_ALL];
    Tcam * tcams[TCAM_NUM_ALL];

    Global(){};
};

extern Global glb;

//struct ActionParam {
//    int action_para_len;
////    bool if_sign;
//    // 64 bit may not accommodate the value, so we should use array
//    uint8_t * val;
//};
//
//struct Data {
//    int data_len;
////    bool if_sign; // true: signed;  false: unsigned
//    uint8_t * val; // store positive integer
//};
//
//struct Parameter {
//    ParameterType type;
//    union {
//        int header_id; // header,
//        struct {
//            uint8_t hdr_id;
//            uint16_t internal_offset;
//            uint16_t fd_len;
//        } field; // field
//        Data constant_data;
//        int data; // constant
//        int action_para_id; // action parameter
//    } val;
//};
//
//struct GateParam { // field or constant data
//    ParameterType type;
//    union {
//        struct {
//            uint8_t hdr_id;
//            uint16_t internal_offset;
//            uint16_t fd_len;
//        } field;
//        Data constant_data;
//    };
//};

void net_config_init(const std::string &config_path);

void modify_phv_with_data(FieldInfo field, Data data, PHV * phv);

uint8_t * get_uints(uint32_t data, int length);

uint32_t get_field_value(FieldInfo fdInfo, PHV * phv);

void init_mem();

void drop_packet(PHV * phv);

#endif //RECONF_SWITCH_IPSA_GLOBAL_H
