//
// Created by xilinx_0 on 1/13/22.
//

#ifndef RECONF_SWITCH_IPSA_DEFS_H
#define RECONF_SWITCH_IPSA_DEFS_H

#include <iostream>
#include <cstring>
#include <linux/ip.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <cinttypes>
#include <cmath>

#include <gflags/gflags.h>
#include <glog/logging.h>

typedef uint8_t *Buffer;
typedef const uint8_t *InputBuffer;

#define PHYSICAL_SRAM_CAPACITY (128 * 1024)
#define SRAM_DEPTH 10
#define SRAM_WIDTH 128

#define PHYSICAL_TCAM_CAPACITY (64 * 1024)
#define TCAM_DEPTH 10
#define TCAM_WIDTH 64

#define MATCHER_THREAD_NUM 16

#define EXECUTOR_ACTION_NUM 32

typedef uint16_t LEN;

const static int CLUSTER_N = 20;
const static int REG_WIDTH = 4; // log2(CLUSTER_N) up

const static int SRAM_NUM_ALL = 80;//0;
const static int CLUSTER_NUM = 4;
const static int PROC_NUM = 12;//12;
const static int PROC_NUM_PER_CLUSTER = PROC_NUM / CLUSTER_NUM;
const static int SRAM_NUM_PER_CLUSTER = SRAM_NUM_ALL / CLUSTER_NUM;
const static int SRAM_IDX_BASE = SRAM_NUM_ALL / CLUSTER_NUM;

const static int TCAM_NUM_ALL = 64;
const static int TCAM_NUM_PER_CLUSTER = TCAM_NUM_ALL / CLUSTER_NUM;
const static int TCAM_IDX_BASE = TCAM_NUM_ALL / CLUSTER_NUM;

static const int FRONT_HEADER_LEN = 1500; // bytes
static const int META_LEN = 100;

enum class OpCode {
    ADD, SUB, MUL, DIV, MOD,
    SHIFT_LEFT, SHIFT_RIGHT, BIT_AND, BIT_OR, BIT_XOR, BIT_NEG,
    SET_FIELD, COPY_FIELD,
    SET_TTL, DECREMENT_TTL, COPY_TTL_OUTWARDS, COPY_TTL_INWARDS,
    PUSH, POP, DECREMENT
};

typedef enum class ParameterType {
    CONSTANT, HEADER, FIELD, PARAM, OPERATOR
} PT;

//struct Parameter {
//    ParameterType type;
//};

struct HeaderInfo { // all of the offset and length are in bit
    uint8_t hdr_id;
    uint16_t hdr_offset; // should be larger than 32 bytes, e.g., uint16
    uint16_t hdr_len;
};

enum class FieldType {
    FIELD, VALID, HIT, MISS
};

struct FieldInfo { // 24
    uint8_t hdr_id;
    uint16_t internal_offset; // 2^8 = 256 bits = 32 bytes, enough for current version
    uint16_t fd_len;
    FieldType fd_type;
};

struct Constant {
    int value;
};

struct Sram {
    int width = SRAM_WIDTH;
    int depth = (1 << SRAM_DEPTH);

    uint8_t tbl[PHYSICAL_SRAM_CAPACITY / 8];
    Sram () {
        memset(tbl,0, sizeof(uint8_t) * (PHYSICAL_SRAM_CAPACITY / 8));
    }
};

struct Tcam {
    int width = TCAM_WIDTH;
    int depth = (1 << TCAM_DEPTH);

    uint8_t tbl[PHYSICAL_TCAM_CAPACITY / 8];
    uint8_t mask[PHYSICAL_TCAM_CAPACITY / 8];
    Tcam () {
       memset(tbl, 0, sizeof(uint8_t) * (PHYSICAL_TCAM_CAPACITY / 8));
       memset(mask, 0, sizeof(uint8_t) * (PHYSICAL_TCAM_CAPACITY / 8));
    }

};

struct ActionParam {
    int action_para_len;
//    bool if_sign;
    // 64 bit may not accommodate the value, so we should use array
    uint8_t * val;
};

struct Data {
    int data_len;
//    bool if_sign; // true: signed;  false: unsigned
    uint8_t * val; // store positive integer
};

struct Parameter {
    ParameterType type;
    union {
        int header_id; // header,
        struct {
            uint8_t hdr_id;
            uint16_t internal_offset;
            uint16_t fd_len;
            FieldType fd_type;
        } field; // field
        OpCode op;
        Data constant_data;
//        int data; // constant
        int action_para_id; // action parameter
    } val;
};

struct GateParam { // field or constant data
    ParameterType type;
    union {
        struct {
            uint8_t hdr_id;
            uint16_t internal_offset;
            uint16_t fd_len;
            FieldType fd_type;
        } field;
        Data constant_data;
    };
};

enum struct GateEntryType{
    TABLE, STAGE
//    , ACTION
};

struct GateEntry {
    GateEntryType type;
    int val;
};

struct PHV {
    // the offset of metadata is (front_header_length), and the id can be set as 15
    uint8_t packet[FRONT_HEADER_LEN + META_LEN]{};
    // header_info
    HeaderInfo* parsed_headers[32]{};
    uint8_t valid_bitmap[32]{0};
    uint32_t bitmap = 0;

    int cur_state = 0;
    uint16_t cur_offset = 0;
    int cur_trans_fd_num = 0;
    uint32_t cur_trans_key; // derived from cur_trans_fds or initialized
    bool tcam_miss;
    uint8_t miss_act = 2;

    uint8_t * match_value;
    int match_value_len;

    bool hit = false;

    int drop = 0;

    int next_proc_id = 0;
    int next_matcher_id = 0;
    int next_action_id = 0;

    GateEntry next_op;

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

static inline std::string mac2str(uint8_t mac[6]) {
    char buf[32];
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return buf;
}

static inline void print_packet(uint8_t *packet, int len) {
    if (len < (int) sizeof(ethhdr)) {
        std::cout << "Broken ethernet packet\n";
        return;
    }
    auto eth_hdr = (ether_header *) packet;
    std::cout << "Ether: " << mac2str(eth_hdr->ether_shost) << " -> " << mac2str(eth_hdr->ether_dhost) << std::endl;
    if (eth_hdr->ether_type == htons(ETHERTYPE_IP)) {
        auto ip_packet = packet + sizeof(ether_header);
        int ip_len = len - (int) sizeof(ether_header);
        if (ip_len < (int) sizeof(iphdr)) {
            std::cout << "Broken IP packet\n";
            return;
        }
        auto ip_hdr = (iphdr *) ip_packet;
        std::cout << "IP: " << inet_ntoa({ip_hdr->saddr}) << " -> " << inet_ntoa({ip_hdr->daddr})
                  << " ttl=" << (int) ip_hdr->ttl << " cksum=" << ip_hdr->check << std::endl;
    } else {
        std::cout << "Unknown ether type " << (int) eth_hdr->ether_type << std::endl;
    }
}

//void modify_phv_with_data(FieldInfo field, Data data, PHV * phv) {
//    int internal_offset = field.internal_offset;
//    int fd_len = field.fd_len;
//    auto hi = phv->parsed_headers[field.hdr_id];
//
//    int begin = hi->hdr_offset + internal_offset;
//    int end = begin + fd_len;
//
//    int begin_offset = begin % 8;
//    int end_offset = end % 8;
//
//    int byte_begin = begin / 8;
//    int byte_end = end / 8 - (end_offset == 0 ? 1 : 0);//ceil(end * 1.0 / 8);
//    int byte_len = byte_end - byte_begin + 1;
//
//    int origin_len =  data.data_len / 8 + (data.data_len % 8 ? 1 : 0);
//    int len = 0;
//    if(end_offset == 0) {
//        len = data.data_len / 8 + data.data_len % 8 ? 1 : 0;
//    } else {
////        std::cout << ":" << (data.data_len + (8 - end_offset)) / 8 << std::endl;
//        len = (data.data_len + (8 - end_offset)) / 8 + ((data.data_len + (8 - end_offset)) % 8 ? 1 : 0);
//    }
////        int len = (data.data_len + (8 - end_offset)) / 8 + (data.data_len + (8 - end_offset)) % 8 ? 1 : 0;
//
//    auto * res = new uint8_t[len];
//    memset(res, 0, len * sizeof(uint8_t));
//
//    int idx = len - 1;
//    for(int i = origin_len - 1; i >= 0; i--) {
//        res[idx] += (data.val[i] << (8 - (end_offset == 0 ? 8 : end_offset)));
//        if(idx > 0){
//            res[idx - 1] += (data.val[i] >> end_offset);
//            idx--;
//        }
//    }
//
//    idx = len - 1;
//    for(int i = byte_end; i >= byte_begin; i--) {
//        int save_bit_num_right = 0;
//        int save_bit_num_left = 0;
//        if (i == byte_end && end_offset != 0){
//            save_bit_num_right = 8 - end_offset;
//        }
//        if (i == byte_begin && begin_offset != 0){
//            save_bit_num_left = begin_offset;
//        }
//        uint8_t sd = 0;
//        for(int j = 0; j < save_bit_num_left; j++) {
//            sd += (1 << (7 - j));
//        }
//        for(int j = 0; j < save_bit_num_right; j++) {
//            sd += (1 << j);
//        }
//        phv->packet[i] = (phv->packet[i] & sd) + res[idx--]; // here
//    }
//}
//
//uint8_t * get_uints(uint32_t data, int length) {
//    int byte_len = ceil(length * 1.0 / 8);
//
//    uint32_t bitmap = 0;
//    for(int i = 0; i < length; i++) {
//        bitmap += (1 << i);
//    }
//    int num = (data & bitmap);
//
//    auto res = new uint8_t[byte_len];
//
//    for(int i = byte_len - 1; i >= 0; i--) {
//        res[i] = (data >> ((byte_len - 1 - i) * 8));
//    }
//
//    return res;
//}
//
//void drop_packet(PHV * phv) {
//    FieldInfo eg_if_fd{31, 9, 9};
//    Data data0 = {9, get_uints(0, 9)};
//    modify_phv_with_data(eg_if_fd, data0, phv);
//}

#endif //RECONF_SWITCH_IPSA_DEFS_H
