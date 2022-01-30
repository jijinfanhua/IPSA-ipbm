//
// Created by xilinx_0 on 1/5/22.
//

#ifndef RECONF_SWITCH_IPSA_GATEWAY_H
#define RECONF_SWITCH_IPSA_GATEWAY_H

#include "defs.h"
#include <unordered_map>

// determine matching which table, if none, bypass this stage and goto next stage;
// determine the next processor_id

// only support assertion below 32 bits

enum class RelationCode {
    GT, GTE,
    LT, LTE,
    EQ, NEQ
};

struct RelationExp {
    GateParam param1;
    GateParam param2;
    RelationCode relation;
};

//enum class LogicCode {
//    AND, OR, NOT
//};

class Gateway {
    // relation
    // logic
    uint8_t bitmap;
    GateEntry default_entry;
//    RelationExp exps[8];
    std::vector<RelationExp*> exps;
    // bitmap->matcher_id, if id == 100, bypass and goto next
    std::unordered_map<uint8_t, GateEntry> res_next;

//    std::unordered_map<uint8_t, int> res_next_proc;
//    std::unordered_map<uint8_t, int> res_next_matcher;
//    std::unordered_map<uint8_t, int> res_next_action;

public:
    Gateway() {
        bitmap = 0;
    }

    void set_default_entry (GateEntry ge) {
        LOG(INFO) << "  default entry: ";
        LOG(INFO) << "      type (stage, matcher): " << (int)ge.type;
        LOG(INFO) << "      value: " << ge.val;
        this->default_entry = ge;
    }

    void clear_default_entry () {
        this->default_entry = {};
    }

    void insert_exp(RelationExp * exp) {
        exps.push_back(exp);
    }

    // clear all
    void clear() {
        this->default_entry = {};
        exps.clear();
        res_next.clear();
        clear_default_entry();
//        res_next_proc.clear();
//        res_next_matcher.clear();
//        res_next_action.clear();
    }

    void modify_res_map(uint8_t key, GateEntryType type, int val) {
        LOG(INFO) << "     entry: ";
        LOG(INFO) << "      action_id: " << (int)key;
        LOG(INFO) << "      type(table, stage): " << (int)type;
        LOG(INFO) << "      value: " << (int)val;
        res_next[key] = GateEntry{type, val};
//        res_next_proc[key] = proc_value;
//        res_next_matcher[key] = matcher_value;
//        res_next_action[key] = action_value;
    }

    void clear_res_next() {
        res_next.clear();
//        res_next_proc.clear();
//        res_next_matcher.clear();
//        res_next_action.clear();
    }

    static uint32_t get_value(GateParam param, PHV * phv) {
        uint32_t res = 0;
        LOG(INFO) << "              param type(const, header, field):" << (int)param.type;
        switch(param.type) {
            case ParameterType::FIELD : {
                LOG(INFO) << "                  field type(field, valid, hit, miss): " << (int)param.field.fd_type;
                switch(param.field.fd_type) {
                    case FieldType::VALID : {
                        res = phv->valid_bitmap[param.field.hdr_id];
                        LOG(INFO) << "                  value: " << res;
                        return res;
                    }
                    case FieldType::HIT : {
                        res = phv->hit;
                        return res;
                    }
                    case FieldType::MISS : {
                        res = ~(phv->hit);
                        return res;
                    }
                    default:
                        break;
                }
                auto hi = phv->parsed_headers[param.field.hdr_id];
                int begin = hi->hdr_offset + param.field.internal_offset;
                int end = begin + param.field.fd_len;

                int begin_offset = begin % 8;
                int end_offset = end % 8;

                int byte_start = begin / 8;
                int byte_end = end / 8 - (end_offset == 0 ? 1 : 0);

                int temp = end_offset == 0 ? 8 : end_offset;

                int offset = 0;
                if(byte_start == byte_end) {
                    uint8_t mid = uint8_t(phv->packet[byte_start] << begin_offset) >> begin_offset;
                    mid = mid >> (8 - temp);
                    res += mid;
                } else {
                    for(int i = byte_end; i >= byte_start; i--) {
                        if(i == byte_end){
                            res += (phv->packet[i] >> (8 - temp));
                            offset += temp;
                            continue;
                        }
                        if(i == byte_start) {
                            res += uint32_t(uint8_t(phv->packet[i] << begin_offset) >> begin_offset) << offset;
                            continue;
                        }
                        if(i != byte_end && i != byte_start){
                            res += (uint32_t)phv->packet[i] << offset;
                            offset += 8;
                        }
                    }
                }

                break;
            }
            case ParameterType::CONSTANT : {
                int len = param.constant_data.data_len;
                int byte_len = len / 8 + (len % 8 == 0 ? 0 : 1);
                uint8_t * data = param.constant_data.val;
                for(int i = 0; i < byte_len; i++) {
                    LOG(INFO) << "                  val[" << i << "]: " << (int)data[i];
                }
                int offset = 0;
                for(int i = byte_len - 1; i >= 0; i--) {
                    res += ((uint32_t)data[i] << offset);
                    offset += 8;
                }
                LOG(INFO) << "                  value: " << res;
            }
            default:
                break;
        }
        return res;
    }

    void execute(PHV * phv) {
        LOG(INFO) << "  Gateway:";
        for(int i = 0; i < exps.capacity(); i++) {
            LOG(INFO) << "      expr " << i;
            LOG(INFO) << "          left: ";
            uint32_t res1 = get_value(exps[i]->param1, phv);
            LOG(INFO) << "          right: ";
            uint32_t res2 = get_value(exps[i]->param2, phv);
            LOG(INFO) << "          relation(> >= < <= == !=): " << (int)exps[i]->relation;
            switch(exps[i]->relation) {
                case RelationCode::GT : {
                    bitmap += uint8_t(uint8_t(res1 > res2) << (7 - i));
                    break;
                }
                case RelationCode::GTE : {
                    bitmap += uint8_t(uint8_t(res1 >= res2) << (7 - i));
                    break;
                }
                case RelationCode::LT : {
                    bitmap += uint8_t(uint8_t(res1 < res2) << (7 - i));
                    break;
                }
                case RelationCode::LTE : {
                    bitmap += uint8_t(uint8_t(res1 <= res2) << (7 - i));
                    break;
                }
                case RelationCode::EQ : {
                    bitmap += uint8_t(uint8_t(res1 == res2) << (7 - i));
                    break;
                }
                case RelationCode::NEQ : {
                    bitmap += uint8_t(uint8_t(res1 != res2) << (7 - i));
                    break;
                }
                default:
                    break;
            }
        }
        LOG(INFO) << "      bitmap is: " << (int)bitmap;
        // determine the matcher_id or next_stage_id
//        std::cout << (int)bitmap << std::endl;
        if(res_next.find(bitmap)== res_next.end()) {
            phv->next_op = this->default_entry;
        } else {
            phv->next_op = res_next[bitmap];
        }
        LOG(INFO) << "      next_op: ";
        LOG(INFO) << "          type(table, stage): " << (int)phv->next_op.type;
        LOG(INFO) << "          value: " << (int)phv->next_op.val;
//        phv->next_proc_id = res_next_proc[bitmap];
//        phv->next_matcher_id = res_next_matcher[bitmap];
//        phv->next_action_id = res_next_action[bitmap];
//        std::cout << phv->next_proc_id << std::endl;
//        std::cout << phv->next_matcher_id << std::endl;
        bitmap = 0;
    }

};

#endif //RECONF_SWITCH_IPSA_GATEWAY_H
