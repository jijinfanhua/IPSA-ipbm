#include "parser.h"
#include "global.h"

void Parser::parse(Buffer packet, int len) {
    for (int i = 0; i < NUM_HDRS; i++) {
        glb.hdrs[i].clear();
    }
    int node_idx = PARSE_START_ID;
    int parse_offset = 0;
    while (node_idx != PARSE_NODE_ACCEPT) {
        if (parse_offset >= len) {
            throw std::overflow_error("Parser packet offset overflow");
        }
        auto &node = parse_hdrs[node_idx];
        auto hdr_buf = packet + parse_offset;
        glb.hdrs[node.hdr_id] = BufView(hdr_buf, node.hdr_len);
        parse_offset += node.hdr_len;
        auto next_hdr_type = hdr_buf + node.next_hdr_type_start;
        int next_node_idx = PARSE_NODE_ACCEPT;  // TODO: use default action
        for (auto &trans : node.next_table) {
            if (memcmp(next_hdr_type, trans.tag.c_str(), node.next_hdr_type_len) == 0) {
                next_node_idx = trans.hdr_id;
                break;
            }
        }
        node_idx = next_node_idx;
    }
}
