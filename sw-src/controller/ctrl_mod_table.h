//
// Created by xilinx_0 on 1/26/22.
//

#ifndef RECONF_SWITCH_IPSA_CTRL_MOD_TABLE_H
#define RECONF_SWITCH_IPSA_CTRL_MOD_TABLE_H

#include "../api/controller_client.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include "json.hpp"

using json = nlohmann::json;
using namespace std;

//#define NO_CFG

/**
 *  0 0 10.0.2.50 => 00:00:00:00:02:50 2 (1->3)
 *  0 0 10.0.1.50 => 00:00:00:00:01:50 1 (3->1)
 */

void add_table_entry (api::CfgClient & cfg) {

    uint8_t key1[] = "\x0a\x00\x02\x32";
    uint8_t value1[] = "\x00\x00\x00\x00\x00\x00\x02\x50\x01\x00";
    cfg.insert_sram_entry(0, 0, key1, value1, 4, 10);

    uint8_t key2[] = "\x0a\x00\x01\x32";
    uint8_t value2[] = "\x00\x00\x00\x00\x00\x00\x01\x50\x00\x80";
    cfg.insert_sram_entry(0, 0, key2, value2, 4, 10);
}

MatchType get_match_type(std::string mt) {
    std::unordered_map<std::string, MatchType> mt_map = {
            {"EXACT", MatchType::EXACT},
            {"TERNARY", MatchType::TERNARY},
            {"LPM", MatchType::LPM}
    };
    return mt_map[mt];
}

uint8_t *combine_to_single_key(const std::vector<KeyInfo*>& keyInfos, LEN fd_len_sum) {

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
        for(int i = 0 ; i < ki->byte_len; i++) { // 2
            if(i < ki->byte_len - 1) { // 0
                match_key[cur_idx] = match_key[cur_idx] + (fd[i] >> (8 - cur_bit_left));

                if(cur_bit_left < 8) {
                    match_key[cur_idx + 1] = (fd[i] << cur_bit_left);
                }
                cur_idx += 1;
            } else {
                match_key[cur_idx] = match_key[cur_idx] + (fd[i] >> (8 - cur_bit_left));
                if(cur_bit_left > ki->tail_offset) {
                    cur_bit_left = cur_bit_left - ki->tail_offset;
                    cur_idx += 1;
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

uint8_t* extract_action_para(json j, int & value_width) {
    int width_sum = 0;
    std::vector<KeyInfo*> key_infos;
    for(auto & para : j) {
        LOG(INFO) << "Param data: ";
        int width = para["width"].get<int>();
        LOG(INFO) << "  width: " << width;
        width_sum += width;
        int width_byte = ceil(width * 1.0 / 8);
        uint8_t * para_byte = new uint8_t[width_byte];
        LOG(INFO) << "  width_byte: " << width_byte;
        int idx = 0;
        LOG(INFO) << "  value: ";
        for(auto & it : para["value"]) {
            para_byte[idx] = it.get<int>();
            LOG(INFO) << "      [" << idx << "]: " << (int)para_byte[idx];
            idx++;
        }
        int empty_len = width_byte * 8 - width;
        LOG(INFO) << "  after shift: ";
        for(int i = 0; i < width_byte - 1; i++) {
            para_byte[i] = (para_byte[i] << empty_len) + (para_byte[i+1] >> (8 - empty_len));
            LOG(INFO) << "      [" << i << "]: " << (int)para_byte[i];
        }
        para_byte[width_byte-1] = para_byte[width_byte-1] << empty_len;
        LOG(INFO) << "      [" << width_byte-1 << "]: " << (int)para_byte[width_byte - 1];

        auto keyInfo = new KeyInfo(empty_len, para_byte, width_byte, width);
        key_infos.push_back(keyInfo);
        LOG(INFO) << "  key_info: ";
        LOG(INFO) << "      empty_len: " << empty_len;
        LOG(INFO) << "      byte_width: " << width_byte;
        LOG(INFO) << "      width: " << width;
        LOG(INFO) << "      value: ";
        for(int i = 0; i < width_byte; i++) {
            LOG(INFO) << "        [" << i << "]: " << (int)keyInfo->field[i];
        }
    }
    value_width = ceil(width_sum * 1.0 / 8);

    uint8_t *single_key = combine_to_single_key(key_infos, width_sum);
    return single_key;
}

uint8_t* extract_exact(json j, int & key_width) {
    int width_sum = 0;
    std::vector<KeyInfo*> key_infos;
    for(auto & key : j) {
        int width = key["width"].get<int>();
        width_sum += width;
        int width_byte = ceil(width * 1.0 / 8);
        uint8_t * key_byte = new uint8_t[width_byte];
        int idx = 0;
        for(auto & it : key["value"]) {
            key_byte[idx] = it.get<int>();
            idx++;
        }
        int empty_len = width_byte * 8 - width;
        for(int i = 0; i < width_byte - 1; i++) {
            key_byte[i] = (key_byte[i] << empty_len) + (key_byte[i+1] >> (8 - empty_len));
        }
        key_byte[width_byte-1] = key_byte[width_byte-1] << empty_len;

        auto keyInfo = new KeyInfo(empty_len, key_byte, width_byte, width);
        key_infos.push_back(keyInfo);
    }
    key_width = ceil(width_sum * 1.0 / 8);

    uint8_t * single_key = combine_to_single_key(key_infos, width_sum);
    return single_key;
}

std::pair<uint8_t*, uint8_t*> extract_ternary(json j, int &key_width) {
    int width_sum = 0;
    std::vector<KeyInfo*> key_infos;
    std::vector<KeyInfo*> mask_infos;
    for(auto & key : j) {
        int width = key["width"].get<int>();
        width_sum += width;
        int width_byte = ceil(width * 1.0 / 8);

        uint8_t * key_byte = new uint8_t[width_byte];
        uint8_t * mask_byte = new uint8_t[width_byte];

        int idx = 0;
        for(auto & it : key["value"]) {
            key_byte[idx] = it.get<int>();
            idx++;
        }
        idx = 0;
        for(auto & it : key["mask"]) {
            mask_byte[idx] = it.get<int>();
            idx++;
        }

        int empty_len = width_byte * 8 - width;
        for(int i = 0; i < width_byte - 1; i++) {
            key_byte[i] = (key_byte[i] << empty_len) + (key_byte[i+1] >> (8 - empty_len));
            mask_byte[i] = (mask_byte[i] << empty_len) + (mask_byte[i+1] >> (8 - empty_len));
        }
        key_byte[width_byte-1] = key_byte[width_byte-1] << empty_len;
        mask_byte[width_byte-1] = mask_byte[width_byte-1] << empty_len;

        auto keyInfo = new KeyInfo(empty_len, key_byte, width_byte, width);
        key_infos.push_back(keyInfo);
        auto maskInfo = new KeyInfo(empty_len, mask_byte, width_byte, width);
        mask_infos.push_back(maskInfo);
    }
    key_width = ceil(width_sum * 1.0 / 8);

    uint8_t *key = combine_to_single_key(key_infos, width_sum);
    uint8_t *mask = combine_to_single_key(mask_infos, width_sum);
    return std::make_pair(key, mask);
}

#ifdef NO_CFG
void extract(const std::string & json_path) {
#else
void extract(const std::string & json_path, api::CfgClient & cfg) {
#endif
    json j;
    std::ifstream(json_path) >> j;

    std::string update_type = j.at("type").get<std::string>();
    int proc_id = j["proc_id"].get<int>();
    int matcher_id = j["matcher_id"].get<int>();
    int action_id = j["action_id"].get<int>();
    MatchType match_type = get_match_type(j["match_type"].get<std::string>());

    uint8_t * key;
    uint8_t * mask;
    int key_width = 0, value_width = 0;
    uint8_t * value;

    switch(match_type) {
        case MatchType::EXACT : {
            key = extract_exact(j["keys"], key_width);
            break;
        } case MatchType::TERNARY : {

        } case MatchType::LPM : {
            auto it =  extract_ternary(j["keys"], key_width);
            key = it.first;
            mask = it.second;
            break;
        } default:
                break;
    }

    value = extract_action_para(j["action_para"], value_width);

    uint8_t * new_value = new uint8_t[2 + value_width];
    new_value[0] = action_id >> 8;
    new_value[1] = action_id;
    memcpy(new_value+2, value, value_width * sizeof(uint8_t));

    LOG(INFO) << "Key: ";
    for(int i = 0 ; i < key_width; i++) {
        LOG(INFO) << "  key[" << i << "]: " << (int)key[i];
    }

    LOG(INFO) << "Mask: ";
    if(match_type != MatchType::EXACT) {
        for (int i = 0; i < key_width; i++) {
            LOG(INFO) << "  mask[" << i << "]: " << (int) mask[i];
        }
    }

    LOG(INFO) << "Result value: ";
    for(int i = 0 ; i < value_width+2; i++) {
        LOG(INFO) << "  [" << i << "]: " << (int)new_value[i];
    }

#ifndef NO_CFG
    int rc;
    if(match_type == MatchType::EXACT) {
        rc = cfg.insert_sram_entry(proc_id, matcher_id, key, value, key_width, value_width);
    } else {
        rc = cfg.insert_tcam_entry(proc_id, matcher_id, key, mask, value, key_width, value_width);
    }
    std::cout << "insert table entry: " << rc << std::endl;
#endif

    return;
}


#endif //RECONF_SWITCH_IPSA_CTRL_MOD_TABLE_H
