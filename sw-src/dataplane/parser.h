//
// Created by xilinx_0 on 12/23/21.
//

#ifndef RECONF_SWITCH_IPSA_PARSER_H
#define RECONF_SWITCH_IPSA_PARSER_H

#include <cstdint>
#include <utility>
#include <vector>
#include <string>
#include <iostream>

#include "global.h"

//#define TEST

struct ParserTcamEntry {
    uint8_t state;
    uint32_t entry;
    uint32_t mask;
};

struct ParserSramEntry { // 24 + 8 + 24 * (1,2,3,4) = 51, 75, 99, 123
    uint8_t hdr_id;
    uint16_t hdr_len; //
    uint8_t next_state;
    uint8_t trans_fd_num; //
    FieldInfo* trans_fds[4]; // 24 * 4
    uint8_t miss_act; // 0 is accept, 1 is drop , go to next state

    ParserSramEntry() {
        for(auto & trans_fd : trans_fds) {
            trans_fd = new FieldInfo();
        }
    }
};


struct ParserStage {
    int stage_level = 0;

    ParserTcamEntry* ps_trans_tbl[256]{};
    ParserSramEntry* ps_act_tbl[256]{};

    ParserStage() {
        for(int i = 0; i < 256; i++) {
            ps_trans_tbl[i] = new ParserTcamEntry();
            ps_trans_tbl[i]->entry = 0xffffffff;
            ps_trans_tbl[i]->mask = 0xffffffff;
            ps_act_tbl[i] = new ParserSramEntry();
        }
    }

    void clear() {
        for(int i = 0; i < 256; i++) {
            memset(ps_trans_tbl[i], 0, sizeof(ParserTcamEntry));
            ps_trans_tbl[i]->entry = 0xffffffff;
            ps_trans_tbl[i]->mask = 0xffffffff;
            for(int j = 0; j < 4; j++) {
                memset(ps_act_tbl[i]->trans_fds[j], 0, sizeof(FieldInfo));
            }
            memset(ps_act_tbl[i], 0, 5);
        }
    }

    void modify_trans_act(const uint8_t key[], const uint8_t value[]) {
        uint8_t idx = 0;
        int flag = 0;
        for(int i = 0; i < 5; i++) {
            idx = (uint8_t)(idx + key[i]);
            if(key[i] != 0 || value[i] != 0) {
                flag = 1;
            }
        }

        if(flag == 0) {
            idx = 255;
        }

        ps_trans_tbl[idx]->state = key[0]; // first 8-bit
        ps_trans_tbl[idx]->entry = uint32_t(key[1] << 24) + uint32_t(key[2] << 16) + uint32_t(key[3] << 8) + key[4]; // last 32 bits for match key.
        ps_trans_tbl[idx]->mask = (key[5] << 24) + (key[6] << 16) + (key[7] << 8) + key[8];


        // hdr_id(8), hdr_len(16), next_state(8), transition field num(8)
        ps_act_tbl[idx]->hdr_id = value[0];
        ps_act_tbl[idx]->hdr_len = (value[1] << 8) + value[2];
        ps_act_tbl[idx]->next_state = value[3];
        ps_act_tbl[idx]->trans_fd_num = value[4];

#ifdef TEST
        std::cout << "idx: " << (int) idx << std::endl;
        std::cout << (int) ps_act_tbl[idx]->hdr_id << "~" << ps_act_tbl[idx]->hdr_len << std::endl;
#endif

        for(int i = 0; i < value[4]; i++) {
            // hdr_id, internal_offset, field_length
            ps_act_tbl[idx]->trans_fds[i]->hdr_id = value[5 + i * 6]; // 8-bit
            ps_act_tbl[idx]->trans_fds[i]->internal_offset = (value[6 + i * 6] << 8) + value[7 + i * 6]; // 16-bit
            ps_act_tbl[idx]->trans_fds[i]->fd_len = (value[8 + i * 6] << 8) + value[9 + i * 6]; // 16-bit
            ps_act_tbl[idx]->trans_fds[i]->fd_type = ((value[10 + i * 6] == 0) ? FieldType::FIELD : FieldType::VALID);
        }
    }

    void modify_trans_entry(uint8_t state, uint32_t entry, uint32_t mask, uint8_t hdr_id, uint16_t hdr_len,
                                uint8_t next_state, uint8_t trans_fd_num, std::vector<FieldInfo*> trans_fds,
                                uint8_t miss_act) {
        uint8_t idx = state;
        int flag = 0;
        for(int i = 0; i < 4; i++) {
            idx += (uint8_t)(entry >> (8 * i));
            if((entry >> (8 * i)) != 0 || (mask >> (8 * i)) != 0) {
                flag = 1;
            }
        }

        if(flag == 0) {
            idx = 255 - state;
        }

        LOG(INFO) << "Mod parser entry at: " << (int)idx;
        LOG(INFO) << "  state: " << (int)state;
        LOG(INFO) << "  key: " << entry;
        LOG(INFO) << "  mask: " << mask;
        LOG(INFO) << "  extracted header id: " << (int)hdr_id;
        LOG(INFO) << "  extracted header length: " << (int)hdr_len;
        LOG(INFO) << "  next state: " << (int)next_state;
        LOG(INFO) << "  number of transition fields: " << (int)trans_fd_num;
        LOG(INFO) << "  accept, drop, or go to next state: " << (int)miss_act;
//        LOG(INFO) << "  transition fields: ";


        ps_trans_tbl[idx]->state = state; // first 8-bit
        ps_trans_tbl[idx]->entry = entry;
        ps_trans_tbl[idx]->mask = mask;

        ps_act_tbl[idx]->hdr_id = hdr_id;
        ps_act_tbl[idx]->hdr_len = hdr_len;
        ps_act_tbl[idx]->next_state = next_state;
        ps_act_tbl[idx]->trans_fd_num = trans_fd_num;
        ps_act_tbl[idx]->miss_act = miss_act;

        for(int i = 0; i < trans_fd_num; i++) {
            // hdr_id, internal_offset, field_length
            ps_act_tbl[idx]->trans_fds[i]->hdr_id = trans_fds[i]->hdr_id; // 8-bit
            ps_act_tbl[idx]->trans_fds[i]->internal_offset = trans_fds[i]->internal_offset; // 16-bit
            ps_act_tbl[idx]->trans_fds[i]->fd_len = trans_fds[i]->fd_len; // 16-bit
            ps_act_tbl[idx]->trans_fds[i]->fd_type = trans_fds[i]->fd_type;
//            LOG(INFO) << "      idx: " << (int)i;
//            LOG(INFO) << "      header_id: " << (int)trans_fds[i]->hdr_id;
//            LOG(INFO) << "      field_internal_offset: " << (int)trans_fds[i]->internal_offset;
//            LOG(INFO) << "      field_length: " << (int)trans_fds[i]->fd_len;
//            LOG(INFO) << "      field_type: " << (int)trans_fds[i]->fd_type;
        }
    }

    static uint32_t get_field_int(PHV * phv, FieldInfo * trans_fd/*uint8_t hdr_id, LEN internal_offset, LEN len*/) {
//        uint8_t hdr_id = sram_entry->trans_fds[i]->hdr_id;
        uint8_t hdr_id = trans_fd->hdr_id;
        if (trans_fd->fd_type == FieldType::VALID) {
            return phv->valid_bitmap[hdr_id];
        }

        uint16_t internal_offset = trans_fd->internal_offset;
        uint16_t len = trans_fd->fd_len;
        auto header = phv->parsed_headers[hdr_id];

        int begin_idx = header->hdr_offset + internal_offset;
        int end_idx = begin_idx + len - 1;

        int left_empty = begin_idx % 8;
        int right_empy = (end_idx + 1) % 8 == 0 ? 0 : (8 - (end_idx + 1) % 8);

        int begin_byte = begin_idx / 8, end_byte = end_idx / 8;

        uint8_t left_mask = 0, right_mask = 0;
        for(int j = 0; j < 8 - left_empty; j++) {
            left_mask += (1 << j);
        }

        for(int j = 0; j < 8 - right_empy; j++) {
            right_mask += (1 << (7 - j));
        }

        uint32_t res = 0;
        int cur_off = 0;
        for(int j = end_byte; j >= begin_byte; j--) {
            if(j == end_byte) {
                res += (phv->packet[j] >> right_empy);
                cur_off += (8 - right_empy);
            } else if(j == begin_byte) {
                res += ( (uint32_t)(phv->packet[j] & left_mask) << cur_off );
            } else {
                res += (phv->packet[j] << cur_off);
                cur_off += 8;
            }
        }
        return res;
    }

    void parse(PHV * phv) {
        int sram_index = -1;
        for(int i = 0; i < 256; i++) {
            if((ps_trans_tbl[i]->state == phv->cur_state) &&
                            (ps_trans_tbl[i]->entry == (phv->cur_trans_key & ps_trans_tbl[i]->mask))) {
                sram_index = i;
//                phv->tcam_miss = false;
                break;
            }
        }
        LOG(INFO) << "          parser entry index: " << sram_index;

        if(sram_index == -1) {
            if(phv->miss_act == 0) {
                LOG(INFO) << "          accept directly";
                return;
            } else if (phv->miss_act == 1){
                LOG(INFO) << "          drop directly";
                phv->next_proc_id = -1;
//                phv->drop = 1;
                // modify egress port to 0;
                drop_packet(phv);
                return;
            } else {
                LOG(INFO) << "          accept directly";
                return;
//                sram_index = 255 - phv->cur_state; // point to default entry
            }
        }

        auto sram_entry = ps_act_tbl[sram_index];
        auto hdr = phv->parsed_headers[sram_entry->hdr_id];

        phv->valid_bitmap[sram_entry->hdr_id] = 1;

        hdr->hdr_id = sram_entry->hdr_id;
        hdr->hdr_len = sram_entry->hdr_len;
        hdr->hdr_offset = phv->cur_offset;

        LOG(INFO) << "          parsed header id: " << (int)hdr->hdr_id;
        LOG(INFO) << "          parsed header length: " << (int)hdr->hdr_len;
        LOG(INFO) << "          parsed header offset: " << (int)hdr->hdr_offset;

        phv->bitmap += 1 << (31 - hdr->hdr_id);
        phv->cur_offset += sram_entry->hdr_len;
        phv->cur_state = sram_entry->next_state;
        phv->miss_act = sram_entry->miss_act;

        if(phv->miss_act == 0) {
            LOG(INFO) << "          accept directly";
            return;
        } else if (phv->miss_act == 1){
            LOG(INFO) << "          drop directly";
            phv->next_proc_id = -1;
            drop_packet(phv);
            return;
        }

        int _trans_fd_num = sram_entry->trans_fd_num;
        uint32_t _trans_key = 0;

        for(int i = 0; i < _trans_fd_num; i++) {
            uint32_t res = get_field_int(phv, sram_entry->trans_fds[i] /*, sram_entry->trans_fds[i]->hdr_id,
                            sram_entry->trans_fds[i]->internal_offset, sram_entry->trans_fds[i]->fd_len*/);
            _trans_key += (_trans_key << sram_entry->trans_fds[i]->fd_len) + res;
        }
        phv->cur_trans_key = _trans_key;

#ifdef TEST
        std::cout << "----------------------------" << std::endl;
        std::cout << "hdr_id: " << (int) hdr->hdr_id << std::endl;
        std::cout << "hdr_len: " << (int) hdr->hdr_len << std::endl;
        std::cout << "hdr_offset: " << (int) hdr->hdr_offset << std::endl;
        std::cout << "cur_state: " << (int) phv->cur_state << std::endl;
        std::cout << "cur_trans_key: " << (int) phv->cur_trans_key << std::endl;
#endif
    }
};

class Parser {
    int needed_parser_level = 0;
    ParserStage * parser_levels;
    int if_work = 0;

public:
    Parser() {
        parser_levels = new ParserStage[4];
    }

    void init_parser_level(int parser_level) {
        LOG(INFO) << "Parser level initiated: " << parser_level;
        needed_parser_level = parser_level;
        if_work = 1;
    }

    void clear() {
        for(int i = 0; i < needed_parser_level; i++) {
            parser_levels[i].clear();
        }
        needed_parser_level = 0;
        if_work = 0;
    }

    void modify_parser_entry(int level, uint8_t key[], uint8_t value[]) const {
        parser_levels[level].modify_trans_act(key, value);
    }

    void modify_parser_entry_direct(int level, uint8_t state, uint32_t entry, uint32_t mask, uint8_t hdr_id, uint8_t hdr_len,
                            uint8_t next_state, uint8_t trans_fd_num, std::vector<FieldInfo*> trans_fds, uint8_t miss_act) {
        parser_levels[level].modify_trans_entry(state, entry, mask, hdr_id, hdr_len,
                                next_state, trans_fd_num, std::move(trans_fds), miss_act);
    }

    void parse(PHV * phv) const {
        if(if_work == 0) {
            phv->next_proc_id = -1;
            return;
        }
        LOG(INFO) << "  Parsing:";
        if(needed_parser_level == 0) return;
        for(int i = 0; i < needed_parser_level; i++) {
            LOG(INFO) << "  parsed by level " << i;
            parser_levels[i].parse(phv);
            if(phv->next_proc_id == -1) {
                return;
            }
        }
//        std::cout << phv->bitmap << std::endl;
    }
};


#endif //RECONF_SWITCH_IPSA_PARSER_H
