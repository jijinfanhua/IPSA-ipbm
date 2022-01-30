//
// Created by xilinx_0 on 1/12/22.
//

#ifndef RECONF_SWITCH_IPSA_MATCHER_B_H
#define RECONF_SWITCH_IPSA_MATCHER_B_H

#include "global.h"

#include <utility>
#include <vector>
#include <functional>
#include <unordered_map>

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

enum class MatchType {
    EXACT, TERNARY, LPM
};

class MatcherThread {
public:
    Sram * sram[SRAM_NUM_PER_CLUSTER];
    Tcam * tcam[TCAM_NUM_PER_CLUSTER];

    std::unordered_map<int, int> action_proc_map;
    bool no_table = false;

    union KeyConfig {
        uint8_t sram_key_config[SRAM_NUM_PER_CLUSTER];
        uint8_t tcam_key_config[TCAM_NUM_PER_CLUSTER];

    } key_config;

    uint8_t sram_value_config[SRAM_NUM_PER_CLUSTER];

    union KeyWidth {
        int sram_slice_key_width;
        int tcam_slice_key_width;
    } key_width;

    int sram_slice_value_width = 0;

    union Depth {
        int sram_slice_depth;
        int tcam_slice_depth;
    } depth;

    int proc_id;
    MatchType match_type = MatchType::EXACT;
    std::vector<FieldInfo*> fdInfos;

    int tcam_idx = 0;

    // no condition satisfied: default processor
    // action: goto
//    int miss_action_id = -1;

//    int hit_act_id = -1;
    int miss_act_id = -1;

    MatcherThread(int _proc_id) {
        this->proc_id = _proc_id;
        this->no_table = true;
        for(int i = 0; i < SRAM_NUM_PER_CLUSTER; i++) {
            sram[i] = glb.srams[proc_id / PROC_NUM_PER_CLUSTER * SRAM_IDX_BASE + i];
        }

        for(int i = 0; i < TCAM_NUM_PER_CLUSTER; i++) {
            tcam[i] = glb.tcams[proc_id / PROC_NUM_PER_CLUSTER * TCAM_IDX_BASE + i];
        }

        depth.sram_slice_depth = 0;
        key_width.sram_slice_key_width = 0;
    }

    void set_action_proc_map(const std::unordered_map<int, int>& _action_proc_map) {
//        action_proc_map = _action_proc_map;
        LOG(INFO) << "  action2proc: ";
        for(auto it : _action_proc_map) {
            action_proc_map[it.first] = it.second;
            LOG(INFO) << "      " << it.first << " : " << it.second;
        }
    }

    void clear_old_config () {
        no_table = true;
//        std::cout << "set_mem_config in 7" << std::endl;
//        for(auto it : fdInfos) {
//            delete it;
//        }
//        std::cout << "set_mem_config in 8" << std::endl;
        LOG(INFO) << "  clear fd_infos";
        fdInfos.clear();
        LOG(INFO) << "  clear action_proc_map";
        action_proc_map.clear();
        miss_act_id = -1;
//        hit_act_id = -1;

        if(match_type == MatchType::EXACT) {
            // clear key sram
//            std::cout << "sram_slice_key_width: " << key_width.sram_slice_key_width << std::endl;
//            std::cout << "depth.sram_slice_depth: " << depth.sram_slice_depth << std::endl;
            for(int i = 0; i < key_width.sram_slice_key_width * depth.sram_slice_depth; i++) {
                memset(sram[key_config.sram_key_config[i]]->tbl, 0, PHYSICAL_SRAM_CAPACITY / 8 * sizeof(uint8_t));
            }
            LOG(INFO) << "  key sram cleared";
            // clear value sram
//            std::cout << "sram_slice_value_width: " << sram_slice_value_width << std::endl;
//            std::cout << "depth.sram_slice_depth: " << depth.sram_slice_depth << std::endl;

            for(int i = 0; i < sram_slice_value_width * depth.sram_slice_depth; i++) {
                memset(sram[sram_value_config[i]]->tbl, 0, PHYSICAL_SRAM_CAPACITY / 8 * sizeof(uint8_t));
            }
            LOG(INFO) << "  value sram cleared";
            memset(key_config.sram_key_config, 0, SRAM_NUM_PER_CLUSTER * sizeof(uint8_t));
            LOG(INFO) << "  key config cleared";
            memset(sram_value_config, 0, SRAM_NUM_PER_CLUSTER * sizeof(uint8_t));
            LOG(INFO) << "  value config cleared";
            key_width.sram_slice_key_width = 0;
            sram_slice_value_width = 0;
            depth.sram_slice_depth = 0;
        } else {
            // clear key tcam
            for(int i = 0; i < key_width.tcam_slice_key_width * depth.tcam_slice_depth; i++) {
                memset(tcam[key_config.tcam_key_config[i]]->tbl, 0, PHYSICAL_TCAM_CAPACITY / 8 * sizeof(uint8_t));
            }
            // clear key mask
            for(int i = 0; i < key_width.tcam_slice_key_width * depth.tcam_slice_depth; i++) {
                memset(tcam[key_config.tcam_key_config[i]]->mask, 0, PHYSICAL_TCAM_CAPACITY / 8 * sizeof(uint8_t));
            }
            // clear value sram
            for(int i = 0; i < sram_slice_value_width * depth.tcam_slice_depth; i++) {
                memset(sram[sram_value_config[i]]->tbl, 0, PHYSICAL_TCAM_CAPACITY / 8 * sizeof(uint8_t));
            }
            memset(key_config.tcam_key_config, 0, TCAM_NUM_PER_CLUSTER * sizeof(uint8_t));
            memset(sram_value_config, 0, TCAM_NUM_PER_CLUSTER * sizeof(uint8_t));

            key_width.tcam_slice_key_width = 0;
            sram_slice_value_width = 0;
            depth.tcam_slice_depth = 0;
            tcam_idx = 0;
        }
    }

    void init_match_type(MatchType _match_type) {
        LOG(INFO) << "  match type(exact, ternary, lpm): " << (int)_match_type;
        match_type = _match_type;
    }

    void set_no_table(bool _no_table) {
        no_table = _no_table;
    }

    // should be called after init_match_type
    void set_mem_config(int _key_width, int _value_width, int _depth, const uint8_t * _key_config, const uint8_t * _value_config) {
        no_table = false;
        if(match_type == MatchType::EXACT) {
            key_width.sram_slice_key_width = _key_width;
            sram_slice_value_width = _value_width;
            depth.sram_slice_depth = _depth;
            LOG(INFO) << "  key_width: " << _key_width;
            LOG(INFO) << "  value_width: " << _value_width;
            LOG(INFO) << "  depth: " << _depth;
            LOG(INFO) << "  new key_config: ";
            for (int i = 0; i < key_width.sram_slice_key_width * _depth; i++) {
                key_config.sram_key_config[i] = _key_config[i];
                LOG(INFO) << "      " << (int)_key_config[i];
            }

            LOG(INFO) << "  new value_config: ";
            for (int i = 0; i < sram_slice_value_width * _depth; i++) {
                sram_value_config[i] = _value_config[i];
                LOG(INFO) << "      " << (int)_value_config[i];
            }

            // pointer to the global memory every time
            for(int i = 0; i < SRAM_NUM_PER_CLUSTER; i++) {
                sram[i] = glb.srams[proc_id / PROC_NUM_PER_CLUSTER * SRAM_IDX_BASE + i];
            }
        } else {
            key_width.tcam_slice_key_width = _key_width;
            sram_slice_value_width = _value_width;
            depth.tcam_slice_depth = _depth;
            for (int i = 0; i < key_width.tcam_slice_key_width * _depth; i++) {
                key_config.tcam_key_config[i] = _key_config[i];
            }

            for (int i = 0; i < sram_slice_value_width * _depth; i++) {
                sram_value_config[i] = _value_config[i];
            }

            for(int i = 0; i < TCAM_NUM_PER_CLUSTER; i++) {
                tcam[i] = glb.tcams[proc_id / PROC_NUM_PER_CLUSTER * TCAM_IDX_BASE + i];
            }

            for(int i = 0; i < SRAM_NUM_PER_CLUSTER; i++) {
                sram[i] = glb.srams[proc_id / PROC_NUM_PER_CLUSTER * SRAM_IDX_BASE + i];
            }
        }
    }

    void set_miss_act_id(int _miss_act_id) {
        miss_act_id = _miss_act_id;
    }

//    void set_hit_act_id(int _hit_act_id) {
//        hit_act_id = _hit_act_id;
//    }

    void set_field_info(std::vector<FieldInfo*> _fdInfos) {
        fdInfos = std::move(_fdInfos);
        LOG(INFO) << "      in matcher field: ";
        for(auto it : fdInfos){
            LOG(INFO) << "          header id: " << (int)it->hdr_id;
            LOG(INFO) << "          internal offset: " << (int)it->internal_offset;
            LOG(INFO) << "          field length: " << (int)it->fd_len;
            LOG(INFO) << "          field type: " << (int)it->fd_type;
        }
//        std::cout << "set field info ok!" << std::endl;
    }

    // key should be aligned to SRAM width
    void insert_sram_entry(uint8_t * key, uint8_t * value, int key_byte_len, int value_byte_len) {
        LOG(INFO) << "Insert SRAM entry: ";
        LOG(INFO) << "  key_byte_len: " << key_byte_len;
        LOG(INFO) << "  key: ";
        uint16_t key_sum = 0;
        for(int i = 0; i < key_byte_len; i++) {
            key_sum += key[i];
            LOG(INFO) << "      " << (int)key[i];
        }
        int slice_idx = key_sum % depth.sram_slice_depth;
        int idx = key_sum % (1 << SRAM_DEPTH);
        LOG(INFO) << "  key index is: " << idx;

        LOG(INFO) << "  value_byte_len: " << value_byte_len;
        LOG(INFO) << "  value: ";
        for(int i = 0; i < value_byte_len; i++) {
            LOG(INFO) << "      " << (int)value[i];
        }

        // determine the sram key sequence
        auto sram_key_seq = new uint8_t[key_width.sram_slice_key_width];
        auto sram_value_seq = new uint8_t[sram_slice_value_width];
        for(int i = 0; i < key_width.sram_slice_key_width; i++) {
            sram_key_seq[i] = key_config.sram_key_config[slice_idx + depth.sram_slice_depth * i];
        }
        for(int i = 0; i < sram_slice_value_width; i++) {
            sram_value_seq[i] = sram_value_config[slice_idx + depth.sram_slice_depth * i];
        }

        // key should be SRAM_WIDTH-aligned
        // should be right-aligned?
        for(int i = 0; i < key_width.sram_slice_key_width; i++) {
            int offset = (key_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (key_byte_len - i * SRAM_WIDTH / 8);
            // assign the entry at the left of the SRAM
            memcpy(sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), key + i * SRAM_WIDTH / 8 , offset);
            // assign the entry at the right of the SRAM
//            memcpy(sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8) + (SRAM_WIDTH / 8 - offset), key + i * SRAM_WIDTH / 8 , offset);
        }

        for(int i = 0; i < sram_slice_value_width; i++) {
            int offset = (value_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (value_byte_len - i * SRAM_WIDTH / 8);
            memcpy(sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), value + i * SRAM_WIDTH / 8 , offset);
            LOG(INFO) << "  sram value: ";
            for(int j = 0; j < sram_slice_value_width * SRAM_WIDTH / 8; j++) {
                LOG(INFO) << "      " << (int)sram[sram_value_seq[i]]->tbl[(idx * SRAM_WIDTH / 8) + j];
            }
//            memcpy(sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8) + (SRAM_WIDTH / 8 - offset), value + i * SRAM_WIDTH / 8 , offset);
        }
    }

    void insert_tcam_entry(uint8_t * key, uint8_t * mask, uint8_t * value, int key_byte_len, int value_byte_len) {
        uint16_t key_sum = 0;
        for(int i = 0; i < key_byte_len; i++) {
            key_sum += key[i];
        }
        int slice_idx = key_sum % depth.tcam_slice_depth;
        int idx = tcam_idx;

        // determine the sram key sequence
        auto tcam_key_seq = new uint8_t[key_width.tcam_slice_key_width];
        auto sram_value_seq = new uint8_t[sram_slice_value_width];
        for(int i = 0; i < key_width.tcam_slice_key_width; i++) {
            tcam_key_seq[i] = key_config.tcam_key_config[slice_idx + depth.tcam_slice_depth * i];
        }
        for(int i = 0; i < sram_slice_value_width; i++) {
            sram_value_seq[i] = sram_value_config[slice_idx + depth.tcam_slice_depth * i];
        }

        // key should be SRAM_WIDTH-aligned
        // should be right-aligned?
        for(int i = 0; i < key_width.tcam_slice_key_width; i++) {
            int offset = (key_byte_len - i * TCAM_WIDTH / 8) >= TCAM_WIDTH / 8 ? TCAM_WIDTH / 8 : (key_byte_len - i * TCAM_WIDTH / 8);
            // assign the entry at the left of the SRAM
            memcpy(tcam[tcam_key_seq[i]]->tbl + (idx * TCAM_WIDTH / 8), key + i * TCAM_WIDTH / 8 , offset);
            memcpy(tcam[tcam_key_seq[i]]->mask + (idx * TCAM_WIDTH / 8), mask + i * TCAM_WIDTH / 8 , offset);
        }

        for(int i = 0; i < sram_slice_value_width; i++) {
            int offset = (value_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (value_byte_len - i * SRAM_WIDTH / 8);
            memcpy(sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), value + i * SRAM_WIDTH / 8 , offset);
        }
        tcam_idx++;
    }

//    void print_entry(int sram_idx) {
//        uint8_t* tbl = sram[sram_idx]->tbl;
//        for(int i = 0; i < (1 << SRAM_DEPTH); i++) {
//            for(int j = 0; j < 128 / 8; j++) {
//                std:: cout << (int)tbl[i * 128 / 8 + j] << " ";
//            }
//            std::cout <<  std::endl;
//        }
//    }


    // key should be aligned to SRAM width
//    void insert_entry(uint8_t * key, uint8_t * value, int key_byte_len, int value_byte_len) {
//        uint16_t key_sum = 0;
//        for(int i = 0; i < key_byte_len; i++) {
//            key_sum += key[i];
//        }
//        int slice_idx = key_sum % sram_slice_depth;
//        int idx = key_sum % (1 << SRAM_DEPTH);
//
//        // determine the sram key sequence
//        auto sram_key_seq = new uint8_t[sram_slice_key_width];
//        auto sram_value_seq = new uint8_t[sram_slice_value_width];
//        for(int i = 0; i < sram_slice_key_width; i++) {
//            sram_key_seq[i] = sram_key_config[slice_idx + sram_slice_depth * i];
//        }
//        for(int i = 0; i < sram_slice_value_width; i++) {
//            sram_value_seq[i] = sram_value_config[slice_idx + sram_slice_depth * i];
//        }
//
//        // key should be SRAM_WIDTH-aligned
//        // should be right-aligned?
//        for(int i = 0; i < sram_slice_key_width; i++) {
//            int offset = (key_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (key_byte_len - i * SRAM_WIDTH / 8);
//            // assign the entry at the left of the SRAM
//            memcpy(sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), key + i * SRAM_WIDTH / 8 , offset);
//            // assign the entry at the right of the SRAM
////            memcpy(sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8) + (SRAM_WIDTH / 8 - offset), key + i * SRAM_WIDTH / 8 , offset);
//        }
//
//        for(int i = 0; i < sram_slice_value_width; i++) {
//            int offset = (value_byte_len - i * SRAM_WIDTH / 8) >= SRAM_WIDTH / 8 ? SRAM_WIDTH / 8 : (value_byte_len - i * SRAM_WIDTH / 8);
//            memcpy(sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8), value + i * SRAM_WIDTH / 8 , offset);
////            memcpy(sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8) + (SRAM_WIDTH / 8 - offset), value + i * SRAM_WIDTH / 8 , offset);
//        }
//    }

    static KeyInfo * get_field(FieldInfo * fd, PHV * phv) {
        if(fd->fd_type == FieldType::VALID) {
            auto field = new uint8_t[1];
            field[0] = (phv->valid_bitmap[fd->hdr_id] << 7);
            LOG(INFO) << "          value: " << (int)field[0];
            auto keyInfo = new KeyInfo(7, field, 1, 1);
            return keyInfo;
        }

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

        LOG(INFO) << "          value: ";
        for(int i = 0; i < byte_len; i++) {
            LOG(INFO) << "              " << (int)field[i];
        }

        int empty_len = fd->fd_len % 8 == 0 ? 0 : (8 - fd->fd_len % 8);

        field[byte_len - 1] = field[byte_len - 1] >> empty_len << empty_len;

        auto keyInfo = new KeyInfo(empty_len, field, byte_len, fd->fd_len);
        return keyInfo;
    }

    static uint8_t *combine_to_single_key(const std::vector<KeyInfo*>& keyInfos, LEN fd_len_sum) {

        LOG(INFO) << "      combine to single key:";
        int len = fd_len_sum / 8 + ((fd_len_sum % 8) > 0);
        LOG(INFO) << "      all key bit length is: " << fd_len_sum;
        LOG(INFO) << "      all key byte length is: " << len;
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

        for(int i = 0; i < len; i++){
            LOG(INFO) << "         " << (int)match_key[i];
        }

        // used to assign the entry at the right
//        int right_shift = fd_len_sum % 8 == 0 ? 0 : (8 - fd_len_sum % 8);
//        for(int i = len - 1; i > 0; i--) {
//            match_key[i] = (match_key[i] >> right_shift) + (match_key[i-1] << (8 - right_shift));
//        }
//        match_key[0] = match_key[0] >> right_shift;

        return match_key;
    }

    void get_match_res(const uint8_t * match_key, LEN key_len, uint8_t * out_key, uint8_t * out_value, int &hit) { //, uint8_t * out_key, uint8_t * out_value
        if(match_type == MatchType::EXACT) {
            int byte_len = key_len / 8 + ((key_len % 8) > 0);
            uint16_t key_sum = 0;
            for (int i = 0; i < byte_len; i++) {
                key_sum += match_key[i];
            }
            auto sram_key_seq = new uint8_t[key_width.sram_slice_key_width];
            auto sram_value_seq = new uint8_t[sram_slice_value_width];
            int base = key_sum % depth.sram_slice_depth;

            for (int i = 0; i < key_width.sram_slice_key_width; i++) {
                sram_key_seq[i] = key_config.sram_key_config[base + depth.sram_slice_depth * i];
            }

            for (int i = 0; i < sram_slice_value_width; i++) {
                sram_value_seq[i] = sram_value_config[base + depth.sram_slice_depth * i];
            }

            int idx = key_sum % (1 << SRAM_DEPTH);
            LOG(INFO) << "          locate the sram entry at " << idx;

            for (int i = 0; i < key_width.sram_slice_key_width; i++) {
                memcpy(out_key + i * SRAM_WIDTH / 8, sram[sram_key_seq[i]]->tbl + (idx * SRAM_WIDTH / 8),
                       SRAM_WIDTH / 8);
            }

            LOG(INFO) << "         out key:";
            for(int i = 0; i < key_width.sram_slice_key_width * SRAM_WIDTH / 8; i ++) {
                LOG(INFO) << "              " << (int) out_key[i];
            }

            for (int i = 0; i < sram_slice_value_width; i++) {
                memcpy(out_value + i * SRAM_WIDTH / 8, sram[sram_value_seq[i]]->tbl + (idx * SRAM_WIDTH / 8),
                       SRAM_WIDTH / 8);
            }

            LOG(INFO) << "         out value:";
            for(int i = 0; i < sram_slice_value_width * SRAM_WIDTH / 8; i ++) {
                LOG(INFO) << "           " << (int) out_value[i];
            }

            for(int i = 0; i < byte_len; i++) {
                if(match_key[i] != out_key[i]) {
                    hit = 0;
                    LOG(INFO) << "         miss! " << hit;
                    return;
                }
            }
            hit = 1;
            LOG(INFO) << "         hit! " << hit;
        } else {
            int byte_len = key_len / 8 + ((key_len % 8) > 0);
            uint16_t key_sum = 0;
            for (int i = 0; i < byte_len; i++) {
                key_sum += match_key[i];
            }
            int base = key_sum % depth.sram_slice_depth;
            auto tcam_key_seq = new uint8_t[key_width.tcam_slice_key_width];
            auto sram_value_seq = new uint8_t[sram_slice_value_width];
            for (int i = 0; i < key_width.tcam_slice_key_width; i++) {
                tcam_key_seq[i] = key_config.tcam_key_config[base + depth.tcam_slice_depth * i];
            }

            for (int i = 0; i < sram_slice_value_width; i++) {
                sram_value_seq[i] = sram_value_config[base + depth.tcam_slice_depth * i];
            }

            auto tmp_key = new uint8_t[key_width.tcam_slice_key_width * TCAM_WIDTH / 8];
            auto tmp_mask = new uint8_t[key_width.tcam_slice_key_width * TCAM_WIDTH / 8];
            for(int i = 0; i < (1 << TCAM_DEPTH); i++) {
                for(int j = 0; j < key_width.tcam_slice_key_width; j++){
                    memcpy(tmp_key + j * TCAM_WIDTH / 8, tcam[tcam_key_seq[j]]->tbl + (i * TCAM_WIDTH / 8), TCAM_WIDTH / 8);
                    memcpy(tmp_key + j * TCAM_WIDTH / 8, tcam[tcam_key_seq[j]]->mask + (i * TCAM_WIDTH / 8), TCAM_WIDTH / 8);
                }
                int flag = 0;
                for(int k = 0; k < byte_len; k++) {
                    if((match_key[k] & tmp_mask[k]) != tmp_key[k]) {
                        flag = 1;
                        break;
                    }
                }
                if(flag == 0) {
                    for (int l = 0; l < key_width.tcam_slice_key_width; l++) {
                        memcpy(out_key + l * TCAM_WIDTH / 8, tcam[tcam_key_seq[l]]->tbl + (i * TCAM_WIDTH / 8),
                               TCAM_WIDTH / 8);
                    }

                    for (int l = 0; l < sram_slice_value_width; l++) {
                        memcpy(out_value + l * SRAM_WIDTH / 8, sram[sram_value_seq[l]]->tbl + (i * SRAM_WIDTH / 8),
                               SRAM_WIDTH / 8);
                    }
                    hit = 1;
                    break;
                } else {
                    continue;
                }
            }
        }
    }

    void generate_match_key_and_match(PHV * phv) { // std::vector<FieldInfo*> fdInfos,
        std::vector<KeyInfo*> keyInfos;
        LEN fd_len_sum = 0;
        LOG(INFO) << "      matching fields: ";
        LOG(INFO) << "      matching field num: " << fdInfos.size();
        for (auto fi : fdInfos) {
            LOG(INFO) << "          field: ";
            LOG(INFO) << "              header id: " << (int)fi->hdr_id;
            LOG(INFO) << "              field internal offset: " << (int)fi->internal_offset;
            LOG(INFO) << "              field length: " << (int)fi->fd_len;
            LOG(INFO) << "              field type(field, valid, hit, miss): " << (int)fi->fd_type;
            keyInfos.push_back(get_field(fi, phv));
            fd_len_sum += fi->fd_len;
        }
        int byte_len = fd_len_sum / 8 + ((fd_len_sum % 8) > 0);
        uint8_t * match_key = combine_to_single_key(keyInfos, fd_len_sum);

        int key_byte_len = ((match_type == MatchType::EXACT) ? key_width.sram_slice_key_width * SRAM_WIDTH / 8
                                                                : key_width.tcam_slice_key_width * TCAM_WIDTH / 8);
        int value_byte_len = sram_slice_value_width * SRAM_WIDTH / 8;

        auto out_key = new uint8_t[key_byte_len];
        auto out_value = new uint8_t[value_byte_len]; // here to optimize

        int hit = 0;

        get_match_res(match_key, fd_len_sum, out_key, out_value, hit); // here
        if(hit == 1) {
            phv->hit = true;
            phv->match_value = out_value;
            phv->match_value_len = value_byte_len;

            phv->next_action_id = (uint32_t(out_value[1])) + ((uint32_t)out_value[0] << 8);
            LOG(INFO) << "      phv->match_value_len: " << phv->match_value_len;
            LOG(INFO) << "      phv->next_action_id: " << phv->next_action_id;
        } else {
            phv->hit = false;
            phv->next_action_id = miss_act_id;
            LOG(INFO) << "      phv->next_action_id: " << phv->next_action_id;
        }
    }
};

class Matcher {
public:
    MatcherThread * mts[MATCHER_THREAD_NUM];
    int proc_id;
//    uint8_t hit_miss_bitmap[MATCHER_THREAD_NUM];

    // hash on key to determine which srams should be used

    Matcher(int _proc_id) {
        this->proc_id = _proc_id;
        for(int i = 0; i < MATCHER_THREAD_NUM; i++) {
            mts[i] = new MatcherThread(_proc_id);
        }
    }

//    void set_hit_miss_bitmap(int idx, int value) {
//        this->hit_miss_bitmap[idx] = value;
//    }

    void insert_sram_entry(int matcher_id, uint8_t * key, uint8_t * value, int key_byte_len, int value_byte_len) {
        mts[matcher_id]->insert_sram_entry(key, value, key_byte_len, value_byte_len);
    }

    void insert_tcam_entry(int matcher_id, uint8_t * key, uint8_t * mask, uint8_t * value, int key_byte_len, int value_byte_len) {
        mts[matcher_id]->insert_tcam_entry(key, mask, value, key_byte_len, value_byte_len);
    }

    void set_fd_info(int matcher_id, std::vector<FieldInfo*> _fdInfos) {
        mts[matcher_id]->set_field_info(std::move(_fdInfos));
    }

    void set_mem_config(int matcher_id, MatchType _match_type, int key_width, int value_width, int depth,
                         uint8_t * key_config, uint8_t * value_config) {
        mts[matcher_id]->clear_old_config();
        mts[matcher_id]->init_match_type(_match_type);
        mts[matcher_id]->set_mem_config(key_width, value_width, depth, key_config, value_config);
    }

    void execute(int matcher_id, PHV* phv) {
        LOG(INFO) << "  Matcher executing... id: " << matcher_id;
        if(mts[matcher_id]->no_table){
            phv->hit = false;
            phv->next_action_id = mts[matcher_id]->miss_act_id;
            LOG(INFO) << "      no_table: true";
            LOG(INFO) << "      next_action_id is: " << phv->next_action_id;
            return;
        }
        LOG(INFO) << "      no_table: false";
        mts[matcher_id]->generate_match_key_and_match(phv);
    }
};


#endif //RECONF_SWITCH_IPSA_MATCHER_B_H
