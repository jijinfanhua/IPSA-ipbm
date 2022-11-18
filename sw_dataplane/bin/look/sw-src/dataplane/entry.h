//
// Created by xilinx_0 on 1/10/22.
//

#ifndef RECONF_SWITCH_IPSA_ENTRY_H
#define RECONF_SWITCH_IPSA_ENTRY_H

#include "defs.h"

#include <regex>
#include <unordered_map>

enum class KeyType {
    IPv4, IPv6, MAC
};

struct FlowEntry {
    int flow_table_id;
    std::vector<int> field_ids;
    std::vector<int> field_lens;
//    std::unordered_map<int, int> field_id_length; // bit
    std::unordered_map<int, std::string> field_id_value;
    int action_id;
    std::unordered_map<int, int> para_id_length;
    std::unordered_map<int, std::string> para_id_value;
};

std::string trim(std::string & str) {
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
    return str;
}

std::vector<std::string> split(std::string entry, std::string reg) {
    std::regex ws(reg);
    std::vector<std::string> res(std::sregex_token_iterator(entry.begin(),
                                                            entry.end(), ws, -1),
                                 std::sregex_token_iterator());
    return res;
}

uint8_t * convert_from_ipv4_to_binary(std::string key) {
    std::vector<std::string> ip_segs = split(key, ".");
    static uint8_t ips[4];
    for(int i = 0; i < 4; i++) {
        ips[i] = (uint8_t) atoi(ip_segs[i].c_str());
    }
    return ips;
}

uint8_t * convert_from_ipv6_to_binary(std::string) {
    std::vector<std::string> ip_segs;
    return nullptr;
}

uint8_t * convert_from_hex_to_binary(std::string) {

    return nullptr;
}

uint8_t * convert_from_sint_to_binary(std::string) {

    return nullptr;
}

uint8_t * convert_from_bin_to_binary(std::string) {

    return nullptr;
}

// 127.0.0.1 > 0 12 0
void convert_to_flow_entry(std::string s_flow_entry) {
    std::vector<std::string> res = split(s_flow_entry, ">");

    if(res.capacity() != 2) {
        return;
    }

    std::string flow_key = trim(res[0]), flow_value = trim(res[1]);

    std::vector<std::string> keys = split(flow_key, " ");
    std::vector<std::string> values = split(flow_value, " ");

    for(auto key : keys) {
        if(key.find(".", 0)) {
            convert_from_ipv4_to_binary(key);
        } else if(key.find(":", 0)) {
            convert_from_ipv6_to_binary(key);
        } else if(key[0] == '0' and (key[1] == 'x' or key[1] == 'X')) {
            convert_from_hex_to_binary(key);
        } else if(key[0] == '0' and (key[1] == 'b' or key[1] == 'B')) {
            convert_from_bin_to_binary(key);
        } else {
            convert_from_sint_to_binary(key);
        }
    }
}

#endif //RECONF_SWITCH_IPSA_ENTRY_H
