#include "matcher.h"
#include "global.h"

static Buffer load_field_buf(const FieldSpec &field) {
    if (field.hdr_id == HDR_ID_META_DATA) {
        return (Buffer) (&glb.meta_data) + field.start;
    } else {
        return glb.hdrs[field.hdr_id].data + field.start;
    }
}

Buffer Matcher::match(int flow_table_id) {
    auto &tab = glb.flow_tables[flow_table_id];
    ByteArray key(tab.key_len);
    int offset = 0;
    for (auto &key_field: tab.key_fields) {
        Buffer field_buf = load_field_buf(key_field);
        memcpy(key.data() + offset, field_buf, key_field.len);
        offset += key_field.len;
    }
    Buffer mask = nullptr;  // TODO: get mask
    int pos;
    if (tab.match_type == MATCH_EXACT) {
        pos = tab.match_exact(key.data());
    } else if (tab.match_type == MATCH_TERNARY) {
        pos = tab.match_ternary(key.data(), mask);
    } else if (tab.match_type == MATCH_LPM) {
        pos = tab.match_lpm(key.data());
    } else {
        assert(0);
    }
    Buffer match_val = nullptr;
    if (pos != tab.end_pos()) {
        // get entry value
        match_val = tab.get_val(pos);
    }
    if (tab.is_counter) {
        if (match_val) {
            // Find counter metric
            auto count_val = (uint32_t *) match_val;
            (*count_val)++;
            std::cout << "COUNT = " << *count_val << std::endl;
        } else {
            // Init count as 1
            uint32_t count = 1;
            tab.insert(key.data(), (Buffer) &count);
            std::cout << "INIT COUNT = 1" << std::endl;
        }
    }
    return match_val;
}
