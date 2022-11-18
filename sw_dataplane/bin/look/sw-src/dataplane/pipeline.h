//
// Created by xilinx_0 on 1/6/22.
//

#ifndef RECONF_SWITCH_IPSA_PIPELINE_H
#define RECONF_SWITCH_IPSA_PIPELINE_H

#include <utility>

#include "processor.h"

class Pipeline {
    std::vector<Processor*> procs;
    int proc_num{};

    std::vector<HeaderInfo*> meta_infos;

public:
    Pipeline() = default;

    Pipeline(int _proc_num) {
        proc_num = _proc_num;
        for(int i = 0; i < proc_num; i++) {
            auto proc = new Processor();
            proc->set_proc_id(i);
            procs.push_back(proc);
        }
    }

    void execute(PHV * phv) {
        while(true) {
            int cur_proc_id = phv->next_proc_id;
            if(cur_proc_id == -1) {
                break;
            } else {
                LOG(INFO) << "Packet processed by processor " << cur_proc_id;
                procs[cur_proc_id]->process_packet(phv);
            }
        }
    }

    void set_metadata_header(std::vector<HeaderInfo*> _meta_infos) {
        meta_infos.clear();
        LOG(INFO) << "Print Metadata Init: " ;
        for(auto it : _meta_infos) {
            meta_infos.push_back(it);
            LOG(INFO) << " header_id: " << it->hdr_id;
            LOG(INFO) << " header_len: " << it->hdr_len;
            LOG(INFO) << " header_offset: " << it->hdr_offset;
        }
    }

    std::vector<HeaderInfo*> get_metadata_header() {
        return meta_infos;
    }

    /*********** start parser configuration  ************/
    void init_parser_level(int proc_id, int level) {
        procs[proc_id]->init_parser_level(level);
    }

    void clear_parser (int proc_id) {
        procs[proc_id]->clear_parser();
    }

    void modify_parser_entry(int proc_id, int level, uint8_t key[], uint8_t value[]) {
        procs[proc_id]->modify_parser_entry(level, key, value);
    }

    void modify_parser_entry_direct(int proc_id, int level, uint8_t state, uint32_t entry, uint32_t mask, uint8_t hdr_id, uint16_t hdr_len,
                                    uint8_t next_state, uint8_t trans_fd_num, std::vector<FieldInfo*> trans_fds, uint8_t miss_act) {
        procs[proc_id]->modify_parser_entry_direct(level, state, entry, mask, hdr_id, hdr_len,
                                           next_state, trans_fd_num, std::move(trans_fds), miss_act);
    }

//    void modify_parser_entry_string(int level, std::string key, std::string value) {
//
//    }

    /*********** end parser configuration  ************/


    /*********** start gateway configuration  ************/
    void insert_exp(int proc_id, RelationExp * exp) {
        procs[proc_id]->insert_exp(exp);
    }

    void clear_exp(int proc_id) {
        procs[proc_id]->clear_exp();
    }

    void clear_res_map(int proc_id) {
        procs[proc_id]->clear_res_map();
    }

    void set_default_entry(int proc_id, GateEntryType ge, int val){
        procs[proc_id]->set_default_entry(ge, val);
    }

    // e.g., two assertion: a==b, c==d; bitmap is [11]000000
    void modify_res_map(int proc_id, uint8_t key, GateEntryType type, int val) {
        procs[proc_id]->modify_res_map(key, type, val);
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
    void clear_old_config(int proc_id, int idx) const {
        procs[proc_id]->clear_old_config(idx);
    }

    void set_action_proc_map(int proc_id, int matcher_id,
                             const std::unordered_map<int, int>& _action_proc_map) const {
        procs[proc_id]->set_action_proc_map(matcher_id, _action_proc_map);
    }

    void init_match_type(int proc_id, int idx, MatchType mt) const {
        procs[proc_id]->init_match_type(idx, mt);
    }

    void set_mem_config(int proc_id, int idx, int key_width, int value_width, int depth,
                         uint8_t * key_config, uint8_t * value_config) {
        procs[proc_id]->matcher->mts[idx]
                        ->set_mem_config(key_width, value_width, depth, key_config, value_config);
    }

    void set_no_table(int proc_id, int matcher_id, bool _no_table) {
        procs[proc_id]->set_no_table(matcher_id, _no_table);
    }

    void insert_sram_entry(int proc_id, int idx, uint8_t * key, uint8_t * value, int key_byte_len, int value_byte_len) {
        procs[proc_id]->matcher->mts[idx]->insert_sram_entry(key, value, key_byte_len, value_byte_len);
    }

    void insert_tcam_entry(int proc_id, int idx, uint8_t * key, uint8_t * mask, uint8_t * value, int key_byte_len, int value_byte_len) {
        procs[proc_id]->matcher->mts[idx]->insert_tcam_entry(key, mask, value, key_byte_len, value_byte_len);
    }

    void set_field_infos(int proc_id, int idx, std::vector<FieldInfo*> _fdInfos) {
        procs[proc_id]->matcher->mts[idx]->set_field_info(std::move(_fdInfos));
    }

//    void set_hit_miss_bitmap(int proc_id, int idx, int value) {
//        procs[proc_id]->matcher->set_hit_miss_bitmap(idx, value);
//    }

    void set_miss_act_id (int proc_id, int idx, int act_id) {
        procs[proc_id]->matcher->mts[idx]->set_miss_act_id(act_id);
    }

//    void set_hit_act_id (int proc_id, int idx, int act_id) {
//        procs[proc_id]->matcher->mts[idx]->set_hit_act_id(act_id);
//    }

//    void get_match_res(int idx, uint8_t * match_key, LEN key_len, uint8_t * out_key, uint8_t * out_value) {
//        matchers[idx]->get_match_res(match_key, key_len, out_key, out_value);
//    }
    /************** end matcher configuration *****************/


    /************** start executor configuration *****************/
    void insert_action(int proc_id, Action *ac, int action_id) {
        procs[proc_id]->exe->insert_action(ac, action_id);
    }

    void del_action(int proc_id, int action_id) {
        procs[proc_id]->del_action(action_id);
    }

    void clear_action(int proc_id) {
        procs[proc_id]->clear_action();
    }
};

#endif //RECONF_SWITCH_IPSA_PIPELINE_H
