//
// Created by xilinx_0 on 12/27/21.
//

#ifndef RECONF_SWITCH_IPSA_MATCHER_H
#define RECONF_SWITCH_IPSA_MATCHER_H

#include "global.h"

#include <vector>
#include <functional>

#define NUM_PHYSICAL_SRAM 12
#define NUM_PHYSICAL_TCAM 8

// 1. get match_keys, (hdr_id, internal_offset, fd_len);
//      for a match key field, we can get the right field by shift
// 2. combine these match keys;
// 3. get the sram or tcam indexes, calculate the sram or tcam blocks that need to be looked up.
// 4. hash on the combined match_keys
// 5. get action id and parameters

//struct

//struct Sram {
//    int width = 128; // 128
//    int depth = 1024; // 1024
//
//    uint8_t tbl[PHYSICAL_SRAM_CAPACITY / 8]; // 8-bit sequence
//
//    Sram () {
////        tbl = (uint8_t *)malloc(sizeof(uint8_t)* (PHYSICAL_SRAM_CAPACITY / 8));
//        memset(tbl,0, sizeof(uint8_t)* (PHYSICAL_SRAM_CAPACITY / 8));
//    }
//
//    uint8_t* search(int index) {
//        uint8_t * value = tbl + index * width / 8;
//        return value;
//    }
//
//    void insert(int index, uint8_t * key) {
//        uint8_t * pos = tbl + index * width / 8;
//        memcpy(pos, key, width / 8);
//    }
//};

struct ClusterConfig {
    // srams
    // processors
    // should be bound at runtime
    // All srams, How many clusters, How many processors.
    // and at runtime, we can assign
    int SRAM_NUM_ALL;
    int PROCESSOR_NUM;
    int CLUSTER_NUM;
};

struct KeyInfo {
    int tail_offset; // e.g., 14-bit field, using 2 bytes, and last 2 bit is empty, so tail offset is 2
    uint8_t * field;
    int byte_len;
    LEN field_len;

    KeyInfo(int _tail_mod, uint8_t * fd, int _byte_length, LEN _field_len)
            : tail_offset(_tail_mod), field(fd), byte_len(_byte_length), field_len(_field_len){ }
};

class Matcher {
public:
    Sram * sram[NUM_PHYSICAL_SRAM];

    uint8_t sram_key_config[CLUSTER_N]; // log2(N)
    uint8_t sram_value_config[CLUSTER_N];

    int sram_slice_key_width = 0;
    int sram_slice_value_width = 0;
    int sram_slice_depth = 0;

    std::vector<FieldInfo*> fdInfos; // the match fields info should be configured or bypassed

    int miss_act; // 0: none; 1: execute_default_action; 2: set_proc_id; 3. execute_default_action and set_proc_id
    int miss_proc_id;
    int hit_proc_id;
    int default_action_id;

    // hash on key to determine which srams should be used

    Matcher() {
        for (int i =0; i < NUM_PHYSICAL_SRAM; i++) {
            sram[i] = new Sram();
        }
    }

    void set_miss_act(int _miss_act) {
        miss_act = _miss_act;
    }

    void set_hit_proc_id(int _hit_proc_id) {
        hit_proc_id = _hit_proc_id;
    }

    void set_miss_proc_id(int _miss_proc_id) {
        miss_proc_id = _miss_proc_id;
    }

    void set_default_action_id(int _default_action_id) {
        default_action_id = _default_action_id;
    }

    void set_field_info(std::vector<FieldInfo*> _fdInfos) {
        fdInfos = _fdInfos;
    }

    void print_entry(int sram_idx) {
        uint8_t* tbl = sram[sram_idx]->tbl;
        for(int i = 0; i < (1 << SRAM_DEPTH); i++) {
            for(int j = 0; j < 128 / 8; j++) {
                std:: cout << (int)tbl[i * 128 / 8 + j] << " ";
            }
            std::cout <<  std::endl;
        }
    }

    // key should be aligned to SRAM width
    void insert_entry(uint8_t * key, uint8_t * value, int key_byte_len, int value_byte_len) {
        uint16_t key_sum = 0;
        for(int i = 0; i < key_byte_len; i++) {
            key_sum += key[i];
        }
        int slice_idx = key_sum % sram_slice_depth;
        int idx = key_sum % (1 << SRAM_DEPTH);

        // determine the sram key sequence
        uint8_t * sram_key_seq = new uint8_t[sram_slice_key_width];
        uint8_t * sram_value_seq = new uint8_t[sram_slice_value_width];
        for(int i = 0; i < sram_slice_key_width; i++) {
            sram_key_seq[i] = sram_key_config[slice_idx + sram_slice_depth * i];
        }
        for(int i = 0; i < sram_slice_value_width; i++) {
            sram_value_seq[i] = sram_value_config[slice_idx + sram_slice_depth * i];
        }

        // key should be SRAM_WIDTH-aligned
        for(int i = 0; i < sram_slice_key_width; i++) {
            int offset = (key_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (key_byte_len - i * SRAM_WIDTH / 8);
            memcpy(sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), key + i * SRAM_WIDTH / 8 , offset);
        }

        for(int i = 0; i < sram_slice_value_width; i++) {
            int offset = (value_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (value_byte_len - i * SRAM_WIDTH / 8);
            memcpy(sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), value + i * SRAM_WIDTH / 8 , offset);
        }
    }

    void set_sram_config(int key_width, int value_width, int depth, uint8_t * key_config, uint8_t * value_config) {
        sram_slice_key_width = key_width;
        sram_slice_value_width = value_width;
        sram_slice_depth = depth;
        for(int i = 0; i < sram_slice_key_width * depth; i++) {
            sram_key_config[i] = key_config[i];
        }

        for(int i = 0; i < sram_slice_value_width * depth; i++) {
            sram_value_config[i] = value_config[i];
        }
    }

    KeyInfo* get_field(FieldInfo * fd, PHV * phv) {
        // All of the keys should be aligned forward, otherwise it is resource-consuming and error-prone, though it is complex work
        LEN hdr_offset = phv->parsed_headers[fd->hdr_id]->hdr_offset;

        int begin = (hdr_offset + fd->internal_offset) / 8;
        int end = (hdr_offset + fd->internal_offset + fd->fd_len) / 8; // + ((fd->internal_offset + fd->fd_len) % 8 == 0 ? 0 : 1);

        uint8_t field[end - begin + 1];
        uint8_t * field_c;
        int real_len = end - begin + 1;

        memcpy(field, phv->packet + begin, end - begin + 1);

        // handle head and tail that not in byte
        int head_mod = fd->internal_offset % 8;
        int tail_mod = (fd->internal_offset + fd->fd_len) % 8;

        if(tail_mod != 0) {
            field[end - begin] = field[end - begin] >> (8 - tail_mod) << (8 - tail_mod);
        }

        if(head_mod != 0) {
            for(int i = 0; i < real_len - 1; i++) {
                field[i] = (field[i] << head_mod) + (field[i+1] >> (8 - head_mod));
            }

            if(head_mod >= tail_mod) {
                real_len -= 1;
            }
        }
        field_c = (uint8_t*) malloc(sizeof(uint8_t) * real_len);
        memcpy(field_c, field, real_len);
        tail_mod = fd->fd_len % 8;

        KeyInfo *keyInfo = new KeyInfo(tail_mod, field_c, real_len, fd->fd_len);
        return keyInfo;
    }

    uint8_t *combine_to_single_key(std::vector<KeyInfo*> keyInfos, LEN fd_len_sum) {

        int len = fd_len_sum / 8 + ((fd_len_sum % 8) > 0);
        uint8_t * match_key = (uint8_t*)malloc(len * sizeof(uint8_t));
        memset(match_key, 0, sizeof(uint8_t) * len);

        int cur_idx = 0;
        int cur_bit_left = 8;
        for(auto ki : keyInfos) {

            auto fd = ki->field;
            for(int i = 0 ; i < ki->byte_len; i++) {
                if(i < ki->byte_len - 1) {
                    match_key[cur_idx] = match_key[cur_idx] + (fd[i] >> (8 - cur_bit_left));

                    if(cur_bit_left < 8) {
                        match_key[cur_idx + 1] = (fd[i] << cur_bit_left);
                    }
                    cur_idx += 1;
                } else {
                    match_key[cur_idx] = match_key[cur_idx] + (fd[i] >> (8 - cur_bit_left));
                    if(cur_bit_left > ki->tail_offset) {
                        cur_bit_left = cur_bit_left - ki->tail_offset;
                    } else if(cur_bit_left == ki->tail_offset) {
                        cur_bit_left = 8;
                        cur_idx += 1;
                    } else {
                        match_key[cur_idx + 1] = (fd[i] << cur_bit_left);
                        cur_bit_left = 8 - ki->tail_offset + cur_bit_left;
                        cur_idx += 1;
                    }
                }
            }
        }

        return match_key;
    }

    void get_match_res(uint8_t * match_key, LEN key_len, uint8_t * out_key, uint8_t * out_value) { //, uint8_t * out_key, uint8_t * out_value
        int byte_len = key_len / 8 + ((key_len % 8) > 0);
        uint16_t key_sum = 0;
        for(int i = 0; i < byte_len; i++) {
            key_sum += match_key[i];
        }

        // sram index
        uint8_t * sram_key_seq = new uint8_t[sram_slice_key_width];
        uint8_t * sram_value_seq = new uint8_t[sram_slice_value_width];
        int base = key_sum % sram_slice_depth;

        for(int i = 0; i < sram_slice_key_width; i++) {
            sram_key_seq[i] = sram_key_config[base + sram_slice_depth * i];
        }

        for(int i = 0; i < sram_slice_value_width; i++) {
            sram_value_seq[i] = sram_value_config[base + sram_slice_depth * i];
        }

        int idx = key_sum % (1 << SRAM_DEPTH);

        int s_key_len = SRAM_WIDTH * sram_slice_key_width;
        int s_value_len = SRAM_WIDTH * sram_slice_value_width;

//        uint8_t * out_key = new uint8_t[s_key_len / 8];
//        out_value = new uint8_t[s_value_len / 8];

        for(int i = 0; i < sram_slice_key_width; i++) {
            memcpy(out_key + i * SRAM_WIDTH / 8, sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), SRAM_WIDTH / 8);
        }

        for(int i = 0; i < sram_slice_value_width; i++) {
            memcpy(out_value + i * SRAM_WIDTH / 8, sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), SRAM_WIDTH / 8);
        }
    }

    void get_sram_key_value(uint8_t * sram_sequence, int width) {

    }

    void generate_match_key_and_match(PHV * phv) { // std::vector<FieldInfo*> fdInfos,
        std::vector<KeyInfo*> keyInfos;
        LEN fd_len_sum = 0;
        for (auto fi : fdInfos) {
            keyInfos.push_back(get_field(fi, phv));
            fd_len_sum += fi->fd_len;
        }
        int byte_len = fd_len_sum / 8 + ((fd_len_sum % 8) > 0);
        uint8_t * match_key = combine_to_single_key(keyInfos, fd_len_sum);

        int key_byte_len = sram_slice_key_width * SRAM_WIDTH / 8;
        int value_byte_len = sram_slice_key_width * SRAM_WIDTH / 8;

        uint8_t * out_key = new uint8_t[key_byte_len];
        uint8_t * out_value = new uint8_t[value_byte_len]; // here to optimize

        get_match_res(match_key, fd_len_sum, out_key, out_value); // here

        for(int i = 0; i < byte_len; i++) {
            if(match_key[i] != out_key[i]) {
                phv->hit = false;
                std::cout << "No entry!" << std::endl;
                return;
            }
        }
        phv->hit = true;
        phv->match_value = out_value;
        phv->match_value_len = value_byte_len;

//        out_value; // action id,
        // front 16-bit action id;
        // parameter list:

    }
};

#endif //RECONF_SWITCH_IPSA_MATCHER_H
