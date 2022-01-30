//
// Created by xilinx_0 on 1/12/22.
//

#ifndef RECONF_SWITCH_IPSA_MATCHER_B_H
#define RECONF_SWITCH_IPSA_MATCHER_B_H

#include "global.h"

#include <utility>
#include <vector>
#include <functional>

#define NUM_PHYSICAL_SRAM 12
#define NUM_PHYSICAL_TCAM 8

struct KeyInfo {
    int tail_offset; // e.g., 14-bit field, using 2 bytes, and last 2 bit is empty, so tail offset is 2
    uint8_t * field;
    int byte_len;
    LEN field_len;

    KeyInfo(int _tail_mod, uint8_t * fd, int _byte_length, LEN _field_len)
            : tail_offset(_tail_mod), field(fd), byte_len(_byte_length), field_len(_field_len){ }
};

class MatcherThread {
public:
//    union mems {
//        Sram * sram[SRAM_NUM_PER_CLUSTER];
//        Tcam * tcam[TCAM_NUM_PER_CLUSTER];
//    };
//
//    union key_config {
//        uint8_t sram_key_config[SRAM_NUM_PER_CLUSTER];
//        uint8_t tcam_key_config[TCAM_NUM_PER_CLUSTER];
//
//    };
//
//    union value_config {
//        uint8_t sram_value_config[SRAM_NUM_PER_CLUSTER];
//        uint8_t tcam_value_config[TCAM_NUM_PER_CLUSTER];
//    };
//
//    union key_width {
//        int sram_slice_key_width;
//        int tcam_slice_key_width;
//    };
//
//    union value_width {
//        int sram_slice_value_width;
//        int tcam_slice_value_width;
//    };
//
//    union depth {
//        int sram_slice_depth;
//        int tcam_slice_depth;
//    };

    Sram * sram[SRAM_NUM_PER_CLUSTER];
    Tcam * tcam[TCAM_NUM_PER_CLUSTER];
    int proc_id;

    uint8_t sram_key_config[SRAM_NUM_PER_CLUSTER]; // log2(N)
    uint8_t sram_value_config[SRAM_NUM_PER_CLUSTER];

    uint8_t tcam_key_config[TCAM_NUM_PER_CLUSTER];
    uint8_t tcam_value_config[TCAM_NUM_PER_CLUSTER];

    int sram_slice_key_width = 0;
    int sram_slice_value_width = 0;
    int sram_slice_depth = 0;

    int tcam_slice_key_width = 0;
    int tcam_slice_value_width = 0;
    int tcam_slice_depth = 0;

    std::vector<FieldInfo*> fdInfos;

    // no condition satisfied: default processor
    // action: goto
    int miss_action_id;

    MatcherThread(int _proc_id) {
        this->proc_id = _proc_id;
        for(int i = 0; i < SRAM_NUM_PER_CLUSTER; i++) {
            sram[i] = srams[proc_id / PROC_NUM_PER_CLUSTER * SRAM_IDX_BASE + i];
        }
    }

    void set_sram_config(int key_width, int value_width, int depth, const uint8_t * key_config, const uint8_t * value_config) {
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

    void set_miss_action_id(int _miss_action_id) {
        miss_action_id = _miss_action_id;
    }

    void set_field_info(std::vector<FieldInfo*> _fdInfos) {
        fdInfos = std::move(_fdInfos);
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
        auto sram_key_seq = new uint8_t[sram_slice_key_width];
        auto sram_value_seq = new uint8_t[sram_slice_value_width];
        for(int i = 0; i < sram_slice_key_width; i++) {
            sram_key_seq[i] = sram_key_config[slice_idx + sram_slice_depth * i];
        }
        for(int i = 0; i < sram_slice_value_width; i++) {
            sram_value_seq[i] = sram_value_config[slice_idx + sram_slice_depth * i];
        }

        // key should be SRAM_WIDTH-aligned
        // should be right-aligned?
        for(int i = 0; i < sram_slice_key_width; i++) {
            int offset = (key_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (key_byte_len - i * SRAM_WIDTH / 8);
            // assign the entry at the left of the SRAM
            memcpy(sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), key + i * SRAM_WIDTH / 8 , offset);
            // assign the entry at the right of the SRAM
//            memcpy(sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8) + (SRAM_WIDTH / 8 - offset), key + i * SRAM_WIDTH / 8 , offset);
        }

        for(int i = 0; i < sram_slice_value_width; i++) {
            int offset = (value_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (value_byte_len - i * SRAM_WIDTH / 8);
            memcpy(sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), value + i * SRAM_WIDTH / 8 , offset);
//            memcpy(sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8) + (SRAM_WIDTH / 8 - offset), value + i * SRAM_WIDTH / 8 , offset);
        }
    }

    static KeyInfo * get_field(FieldInfo * fd, PHV * phv) {
        LEN hdr_offset = phv->parsed_headers[fd->hdr_id]->hdr_offset;

        int begin_idx = hdr_offset + fd->internal_offset;
        int end_idx = begin_idx + fd->fd_len - 1;

        int begin_byte = begin_idx / 8, end_byte = end_idx / 8;
        int left_empty = begin_idx % 8, right_empty = (end_idx + 1) % 8 == 0 ? 0 : (8 - (end_idx + 1) % 8);

        int byte_len = ceil(fd->fd_len * 1.0 / 8);

        auto field = new uint8_t[byte_len];

        if(left_empty + right_empty >= 8) {
            field[byte_len - 1] = (phv->packet[begin_byte + byte_len - 1] << left_empty) + (phv->packet[begin_byte + byte_len] >> (8 - left_empty));
        } else {
            field[byte_len - 1] = (phv->packet[begin_byte + byte_len - 1] << left_empty);
        }

        for(int i = 0; i < byte_len - 1; i++) {
            field[i] = (phv->packet[begin_byte + i] << left_empty) + (phv->packet[begin_byte + i + 1] >> (8 - left_empty));
        }

        int empty_len = fd->fd_len % 8 == 0 ? 0 : (8 - fd->fd_len % 8);

        field[byte_len - 1] = field[byte_len - 1] >> empty_len << empty_len;

        auto keyInfo = new KeyInfo(empty_len, field, byte_len, fd->fd_len);
        return keyInfo;
    }

    static uint8_t *combine_to_single_key(const std::vector<KeyInfo*>& keyInfos, LEN fd_len_sum) {

        int len = fd_len_sum / 8 + ((fd_len_sum % 8) > 0);
        auto match_key = (uint8_t*)malloc(len * sizeof(uint8_t));
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

        // used to assign the entry at the right
//        int right_shift = fd_len_sum % 8 == 0 ? 0 : (8 - fd_len_sum % 8);
//        for(int i = len - 1; i > 0; i--) {
//            match_key[i] = (match_key[i] >> right_shift) + (match_key[i-1] << (8 - right_shift));
//        }
//        match_key[0] = match_key[0] >> right_shift;

        return match_key;
    }

    void get_match_res(const uint8_t * match_key, LEN key_len, uint8_t * out_key, uint8_t * out_value) { //, uint8_t * out_key, uint8_t * out_value
        int byte_len = key_len / 8 + ((key_len % 8) > 0);
        uint16_t key_sum = 0;
        for(int i = 0; i < byte_len; i++) {
            key_sum += match_key[i];
        }

        // sram index
        auto sram_key_seq = new uint8_t[sram_slice_key_width];
        auto sram_value_seq = new uint8_t[sram_slice_value_width];
        int base = key_sum % sram_slice_depth;

        for(int i = 0; i < sram_slice_key_width; i++) {
            sram_key_seq[i] = sram_key_config[base + sram_slice_depth * i];
        }

        for(int i = 0; i < sram_slice_value_width; i++) {
            sram_value_seq[i] = sram_value_config[base + sram_slice_depth * i];
        }

        int idx = key_sum % (1 << SRAM_DEPTH);

//        int s_key_len = SRAM_WIDTH * sram_slice_key_width;
//        int s_value_len = SRAM_WIDTH * sram_slice_value_width;

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

        auto out_key = new uint8_t[key_byte_len];
        auto out_value = new uint8_t[value_byte_len]; // here to optimize

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
    }
};

class Matcher {
public:
    MatcherThread * mts[MATCHER_THREAD_NUM];
    int proc_id;
    uint8_t miss_act_bitmap[MATCHER_THREAD_NUM];

    // hash on key to determine which srams should be used

    Matcher(int _proc_id) {
        this->proc_id = _proc_id;
        for(int i = 0; i < MATCHER_THREAD_NUM; i++) {
            mts[i] = new MatcherThread(_proc_id);
        }
    }

    void set_miss_act_bitmap(int idx, int value) {
        this->miss_act_bitmap[idx] = value;
    }

    void insert_entry(int matcher_id, uint8_t * key, uint8_t * value, int key_byte_len, int value_byte_len) {
        mts[matcher_id]->insert_entry(key, value, key_byte_len, value_byte_len);
    }

    void set_fd_info(int matcher_id, std::vector<FieldInfo*> _fdInfos) {
        mts[matcher_id]->set_field_info(std::move(_fdInfos));
    }

    void set_sram_config(int matcher_id, int key_width, int value_width, int depth,
                         uint8_t * key_config, uint8_t * value_config) {
        mts[matcher_id]->set_sram_config(key_width, value_width, depth, key_config, value_config);
    }

    void execute(int matcher_id, PHV* phv) {
        mts[matcher_id]->generate_match_key_and_match(phv);
    }

};


#endif //RECONF_SWITCH_IPSA_MATCHER_B_H
