//
// Created by xilinx_0 on 1/5/22.
//

#ifndef RECONF_SWITCH_IPSA_PROCESSOR_H
#define RECONF_SWITCH_IPSA_PROCESSOR_H

#include <utility>

#include "parser.h"
#include "gateway.h"
#include "matcher_c.h"
#include "executor.h"

class Processor{
public:
    Parser * parser;
    Gateway * gateway;
    Matcher * matcher;
    Executor* exe;

    int proc_id;

    Processor() {
        proc_id = 0;
        parser = new Parser();
        gateway = new Gateway();
        exe = new Executor();
    }

    void set_proc_id(int _proc_id){
        proc_id = _proc_id;
        matcher = new Matcher(_proc_id);
    }

    void process_packet(PHV * phv) const{
        parser->parse(phv);
        if(phv->next_proc_id == -1) {
            return;
        }
        gateway->execute(phv);
        switch (phv->next_op.type) {
            case GateEntryType::TABLE : {
                matcher->execute(phv->next_op.val, phv);
                break;
            }
            case GateEntryType::STAGE : {
                phv->next_proc_id = phv->next_op.val;
                return;
            }
//            case GateEntryType::ACTION : {
//                exe->execute_miss_action(phv->next_op.val, phv);
//            }
            default:
                break;
        }
//        if(phv->next_proc_id == proc_id) {
//            matcher->execute(phv->next_matcher_id, phv);
//        } else {
//            return;
//        }
        LOG(INFO) << "      phv->next_op.type: " << (int)phv->next_op.type;
        LOG(INFO) << "      phv->next_op.val: " << (int)phv->next_op.val;
        for(auto it : matcher->mts[phv->next_op.val]->action_proc_map) {
            LOG(INFO) << "          " << it.first << " : " << it.second;
        }
        phv->next_proc_id = matcher->mts[phv->next_op.val]->action_proc_map[phv->next_action_id];
        LOG(INFO) << "      phv->next_proc_id: " << phv->next_proc_id;
        exe->execute(phv);

//        switch(matcher->hit_miss_bitmap[phv->next_op.val]) {
//            case 0 : {
//                if(phv->hit) {
//                    phv->next_action_id = matcher->mts[phv->next_op.val]->hit_act_id;
//                    exe->execute(phv);
////                    exe->execute_action(matcher->mts[phv->next_op.val]->hit_act_id, phv);
//                } else {
//                    exe->execute_action(matcher->mts[phv->next_op.val]->miss_act_id, phv);
//                }
//                break;
//            }
//            case 1 : {
//                exe->execute(phv);
//            }
//            default : break;
//        }

//        if(phv->hit) {
//            exe->execute(phv);
////            phv->next_proc_id = matcher->mts[phv->next_matcher_id]->action_proc_map[phv->next_action_id];
//        } else {
//            // = 0, goto hit miss; = 1, real action
//            switch(matcher->miss_act_bitmap[phv->next_matcher_id]) {
//                case 0 : {
//                    return;
//                    break;
//                }
//                case 1 : {
//                    exe->execute_miss_action(phv->next_action_id, phv);
//                    break;
//                }
//                default:
//                    break;
//            }
////            phv->next_proc_id = matcher->mts[phv->next_matcher_id]->action_proc_map[phv->next_action_id];
//        }
//
//        exe->execute(phv);
    }

    /*********** start parser configuration  ************/
    void init_parser_level(int level) const {
        parser->init_parser_level(level);
    }

    void clear_parser () const {
        parser->clear();
    }

    void modify_parser_entry(int level, uint8_t key[], uint8_t value[]) const {
        parser->modify_parser_entry(level, key, value);
    }

    void modify_parser_entry_direct(int level, uint8_t state, uint32_t entry, uint32_t mask, uint8_t hdr_id, uint16_t hdr_len,
                                    uint8_t next_state, uint8_t trans_fd_num, std::vector<FieldInfo*> trans_fds, uint8_t miss_act) const {
        parser->modify_parser_entry_direct(level, state, entry, mask, hdr_id, hdr_len,
                                                next_state, trans_fd_num, std::move(trans_fds), miss_act);
    }

//    void modify_parser_entry_string(int level, std::string key, std::string value) {
//
//    }

    void parse(PHV * phv) const {
        parser->parse(phv);
    }
    /*********** end parser configuration  ************/


    /*********** start gateway configuration  ************/
    void insert_exp(RelationExp * exp) const {
        gateway->insert_exp(exp);
    }

    // clear all
    void clear_exp() const {
        gateway->clear();
    }

    void clear_res_map() const {
        gateway->clear_res_next();
    }

    void set_default_entry(GateEntryType ge, int val) const {
        gateway->set_default_entry(GateEntry{ge, val});
    }

    // e.g., two assertion: a==b, c==d; bitmap is [11]000000
    void modify_res_map(uint8_t key, GateEntryType type, int val) const {
        gateway->modify_res_map(key, type, val);
    }

    void execute(PHV * phv) const {
        gateway->execute(phv);
    }
    /*********** end gateway configuration  ************/


    /************** start matcher configuration *****************/
//    void add_matcher() {
//        Matcher * m = new Matcher();
//        matchers.push_back(m);
//    }

    /**
     * note that key_width, value_width and depth is in SRAM slice
     * @param idx
     * @param key_width
     * @param value_width
     * @param depth
     * @param key_config
     * @param value_config
     */
    void clear_old_config(int idx) const {
        matcher->mts[idx]->clear_old_config();
    }

    void set_action_proc_map(int matcher_id,
                             const std::unordered_map<int, int>& _action_proc_map) const {
        matcher->mts[matcher_id]->set_action_proc_map(_action_proc_map);
    }

    void init_match_type(int idx, MatchType mt) const {
        matcher->mts[idx]->init_match_type(mt);
    }

    void set_mem_config(int idx, int key_width, int value_width, int depth,
                         uint8_t * key_config, uint8_t * value_config) const {
        matcher->mts[idx]->set_mem_config(key_width, value_width, depth, key_config, value_config);
    }

    void insert_sram_entry(int idx, uint8_t * key, uint8_t * value, int key_byte_len, int value_byte_len) const {
        matcher->mts[idx]->insert_sram_entry(key, value, key_byte_len, value_byte_len);
    }

    void insert_tcam_entry(int idx, uint8_t * key, uint8_t * mask, uint8_t * value, int key_byte_len, int value_byte_len) const {
        matcher->mts[idx]->insert_tcam_entry(key, mask, value, key_byte_len, value_byte_len);
    }

    void set_field_infos(int idx, std::vector<FieldInfo*> _fdInfos) const {
        matcher->mts[idx]->set_field_info(std::move(_fdInfos));
    }

//    void set_hit_miss_bitmap(int idx, int value) const {
//        matcher->set_hit_miss_bitmap(idx, value);
//    }

    void set_no_table(int matcher_id, bool _no_table) const {
        matcher->mts[matcher_id]->set_no_table(_no_table);
    }

//    void set_hit_act_id (int matcher_id, int _hit_act_id) const {
//        matcher->mts[matcher_id]->set_hit_act_id(_hit_act_id);
//    }

    void set_miss_act_id (int matcher_id, int _miss_act_id) const {
        matcher->mts[matcher_id]->set_miss_act_id(_miss_act_id);
    }

//    void get_match_res(int idx, uint8_t * match_key, LEN key_len, uint8_t * out_key, uint8_t * out_value) {
//        matchers[idx]->get_match_res(match_key, key_len, out_key, out_value);
//    }
    /************** end matcher configuration *****************/


    /************** start executor configuration *****************/
    void insert_action(Action *ac, int action_id) const {
        exe->insert_action(ac, action_id);
    }

    void del_action(int action_id) const {
        exe->del_action(action_id);
    }

    void clear_action() const {
        exe->clear_action();
    }

//    void execute_action(int action_id, PHV * phv){
//        exe->execute_action(action_id, phv);
//    }

//    void set_action_para_value(uint8_t * value) {
//        exe->set_action_para_value(value);
//    }
    /************** end matcher configuration *****************/

};



#endif //RECONF_SWITCH_IPSA_PROCESSOR_H
