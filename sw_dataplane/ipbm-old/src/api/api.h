#pragma once

#include "dataplane/Global/global.h"
#include <algorithm>
#include <utility>

typedef int RC;
#define TABLE_NOT_FOUND 101
#define INVALID_KEY_LEN 102
#define INVALID_VAL_LEN 103
#define INSERT_FAIL 104
#define ERASE_FAIL 105
#define PROC_NOT_FOUND 106
#define PREV_HDR_NOT_FOUND 107
#define INVALID_HDR_ID 108
#define OVERFLOW_ERROR 109
#define OUT_OF_RANGE_ERROR 110
#define ARGUMENT_ERROR 111

#define RPC_FAIL 201

#define INTERNAL_ERROR 301

namespace api {

enum ModType {
    MOD_INSERT, MOD_DELETE, MOD_UPDATE
};

struct FlowEntry {
    std::string key;
    int action_id;
    std::vector<std::string> action_data;
};

RC add_flow_entry(MemType mem_type, int table_id, const FlowEntry &flow) {
    if (table_id >= NUM_FLOW_TABLES) {
        return TABLE_NOT_FOUND;
    }
    auto &tab = glb.flow_tables[table_id];
    // Check key length
    if (tab.key_len != (int) flow.key.size()) {
        return INVALID_KEY_LEN;
    }
    // check value length
    int flow_val_len = 0;
    for (auto &val: flow.action_data) {
        flow_val_len += val.size();
    }
    if (tab.val_len != flow_val_len) {
        return INVALID_VAL_LEN;
    }
    // Make entry
    ByteArray entry(tab.key_len + tab.val_len);
    // Copy key field
    Buffer key_buf = entry.data();
    memcpy(key_buf, flow.key.c_str(), tab.key_len);
    // Copy value field
    Buffer val_buf = entry.data() + tab.key_len;
    int val_offset = 0;
    for (auto &val: flow.action_data) {
        memcpy(val_buf + val_offset, val.c_str(), val.size());
        val_offset += val.size();
    }
    // Insert into table
    try {
        tab.insert(key_buf, val_buf);
    } catch (std::exception &) {
        return INSERT_FAIL;
    }
    return 0;
}

RC del_flow_entry(MemType mem_type, int table_id, const FlowEntry &flow) {
    if (table_id >= NUM_FLOW_TABLES) {
        return TABLE_NOT_FOUND;
    }
    auto &tab = glb.flow_tables[table_id];
    if (tab.key_len != (int) flow.key.size()) {
        return INVALID_KEY_LEN;
    }
    try {
        tab.erase((Buffer) flow.key.c_str());
    } catch (std::exception &) {
        return ERASE_FAIL;
    }
    return 0;
}

RC mod_flow_entry(MemType mem_type, int table_id, const FlowEntry &src_flow, const FlowEntry &dst_flow) {
    if (table_id == TAB_ID_VLAN_GROUP) {
        // special case
        assert(src_flow.key.size() == sizeof(int));
        assert(dst_flow.action_data.size() == 1 && dst_flow.action_data[0].size() == sizeof(uint32_t));
        int if_idx = *(int *) src_flow.key.c_str();
        uint32_t port_mask = *(uint32_t *) dst_flow.action_data[0].c_str();
        glb.vlan_group.port_masks[if_idx] = port_mask;
        return 0;
    } else {
        RC rc = del_flow_entry(mem_type, table_id, src_flow);
        if (rc) { return rc; }
        return add_flow_entry(mem_type, table_id, dst_flow);
    }
}

Buffer get_flow_entry_val(int table_id, Buffer key) {
    auto &tab = glb.flow_tables[table_id];
    int pos = tab.match_exact(key);
    if (pos == tab.end_pos()) {
        return nullptr;
    } else {
        return tab.get_val(pos);
    }
}

std::string get_flow_entry_action_data(int table_id, const std::string &key) {
    auto &tab = glb.flow_tables[table_id];
    int pos = tab.match_exact((Buffer) key.c_str());
    if (pos == tab.end_pos()) {
        return {};
    } else {
        Buffer buf = tab.get_val(pos);
        return std::string((char *) buf, tab.val_len);
    }
}

RC add_parser_header(const std::vector<int> &prev_hdr_ids, const std::string &hdr_tag,
                     const ParserHeader &hdr) {
    auto &parser = glb.parser;
    // Find previous header
    for (int prev_hdr_id : prev_hdr_ids) {
        auto &prev_hdr = parser.parse_hdrs[prev_hdr_id];
        // Add new header tag in previous header
        NextParserHeader next_state(hdr_tag, hdr.hdr_id);
        prev_hdr.next_table.push_back(next_state);
    }
    parser.parse_hdrs[hdr.hdr_id] = hdr;
    return 0;
}

RC set_flow_table(int flow_table_id, int logic_table_id, MemType mem_type, MatchType match_type,
                  const std::vector<FieldSpec> &key_fields, int val_len, bool is_counter) {
    if (flow_table_id >= NUM_FLOW_TABLES) {
        return TABLE_NOT_FOUND;
    }
    glb.flow_tables[flow_table_id] = FlowTable(
            flow_table_id, val_len, glb.logic_tables[logic_table_id],
            mem_type, match_type, key_fields, is_counter);
    return 0;
}

RC set_executor(int proc_id, int hit_action, int hit_next_proc,
                int miss_action, int miss_next_proc, std::vector<RuntimeArgSpec> runtime_args_spec) {
    auto &proc = glb.processors[proc_id];
    proc.executor = Executor(
            hit_action, hit_next_proc, miss_action, miss_next_proc, std::move(runtime_args_spec));
    return 0;
}

RC set_action(int action_id, const Action &action) {
    if (action_id >= NUM_ACTIONS) {
        return OUT_OF_RANGE_ERROR;
    }
    glb.actions[action_id] = action;
    return 0;
}

RC set_processor(int proc_id, const Condition &cond, const NextStage &true_next, const NextStage &false_next) {
    auto &proc = glb.processors[proc_id];
    proc.cond = cond;
    proc.true_next = true_next;
    proc.false_next = false_next;
    return 0;
}

}
