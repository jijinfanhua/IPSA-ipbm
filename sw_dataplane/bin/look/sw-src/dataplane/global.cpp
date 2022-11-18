//
// Created by xilinx_0 on 1/7/22.
//

#include "global.h"

#include <yaml-cpp/yaml.h>
#include <linux/if_packet.h>
#include <ifaddrs.h>

Global glb;

void net_config_init(const std::string & config_path) {
    YAML::Node yml_if_config = YAML::LoadFile(config_path);
    glb.num_if = yml_if_config["interfaces"].size();

    for(auto &&yml_if : yml_if_config["interfaces"]) {
        std::string if_name = yml_if["name"].as<std::string>();
        std::string if_ip = yml_if["ip"].as<std::string>();

        in_addr_t ip;
        inet_aton(if_ip.c_str(), (in_addr *) &ip);
        glb.if_names.push_back(if_name);
        glb.if_ips.push_back(ip);
    }

//    assert((int) glb.if_names.size() == glb.num_if);
//    assert((int) glb.if_ips.size() == glb.num_if);

    struct ifaddrs * ifaddr;
    if(getifaddrs(&ifaddr) < 0) {
        throw std::runtime_error("Cannot get mac address from interface");
    }

    for(struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if(ifa->ifa_addr == nullptr) continue;
        for(int i = 0; i < glb.num_if; i++) {
            if(ifa->ifa_addr->sa_family == AF_PACKET
                    && ifa->ifa_name == glb.if_names[i]) {
                ether_addr mac;
                memcpy(&mac, ((struct sockaddr_ll *)ifa->ifa_addr)->sll_addr, sizeof(ether_addr));

                glb.if_macs.push_back(mac);
            }
        }
    }
}

//uint8_t * get_uints(uint32_t data, int length) {
//    int byte_len = ceil(length * 1.0 / 8);
//
//    uint32_t bitmap = 0;
//    for(int i = 0; i < length; i++) {
//        bitmap += (1 << i);
//    }
//    int num = data & bitmap;
//
//    auto res = new uint8_t[byte_len];
//
//    for(int i = byte_len - 1; i >= 0; i--) {
//        res[i] = (data >> ((byte_len - 1 - i) * 8));
//    }
//
//    return res;
//}

//void drop_packet(PHV * phv) {
//    FieldInfo eg_if_fd{31, 9, 9};
//    Data data0 = {9, get_uints(0, 9)};
//    modify_phv_with_data(eg_if_fd, data0, phv);
//}
//
//void modify_phv_with_data(FieldInfo field, Data data, PHV * phv) {
//    int internal_offset = field.internal_offset;
//    int fd_len = field.fd_len;
//    auto hi = phv->parsed_headers[field.hdr_id];
//
//    int begin = hi->hdr_offset + internal_offset;
//    int end = begin + fd_len;
//
//    int begin_offset = begin % 8;
//    int end_offset = end % 8;
//
//    int byte_begin = begin / 8;
//    int byte_end = end / 8 - (end_offset == 0 ? 1 : 0);//ceil(end * 1.0 / 8);
//    int byte_len = byte_end - byte_begin + 1;
//
//    int origin_len =  data.data_len / 8 + (data.data_len % 8 ? 1 : 0);
//    int len = 0;
//    if(end_offset == 0) {
//        len = data.data_len / 8 + data.data_len % 8 ? 1 : 0;
//    } else {
////        std::cout << ":" << (data.data_len + (8 - end_offset)) / 8 << std::endl;
//        len = (data.data_len + (8 - end_offset)) / 8 + ((data.data_len + (8 - end_offset)) % 8 ? 1 : 0);
//    }
////        int len = (data.data_len + (8 - end_offset)) / 8 + (data.data_len + (8 - end_offset)) % 8 ? 1 : 0;
//
//    auto * res = new uint8_t[len];
//    memset(res, 0, len * sizeof(uint8_t));
//
//    int idx = len - 1;
//    for(int i = origin_len - 1; i >= 0; i--) {
//        res[idx] += (data.val[i] << (8 - (end_offset == 0 ? 8 : end_offset)));
//        if(idx > 0){
//            res[idx - 1] += (data.val[i] >> end_offset);
//            idx--;
//        }
//    }
//
//    idx = len - 1;
//    for(int i = byte_end; i >= byte_begin; i--) {
//        int save_bit_num_right = 0;
//        int save_bit_num_left = 0;
//        if (i == byte_end && end_offset != 0){
//            save_bit_num_right = 8 - end_offset;
//        }
//        if (i == byte_begin && begin_offset != 0){
//            save_bit_num_left = begin_offset;
//        }
//        uint8_t sd = 0;
//        for(int j = 0; j < save_bit_num_left; j++) {
//            sd += (1 << (7 - j));
//        }
//        for(int j = 0; j < save_bit_num_right; j++) {
//            sd += (1 << j);
//        }
//        phv->packet[i] = (phv->packet[i] & sd) + res[idx--]; // here
//    }
//}

uint32_t get_field_value(FieldInfo fdInfo, PHV * phv) {
    uint32_t res = 0;
    int fd_len = fdInfo.fd_len;
    int internal_offset = fdInfo.internal_offset;
    HeaderInfo * hi = phv->parsed_headers[fdInfo.hdr_id];

    int start = hi->hdr_offset + internal_offset;
    int end = start + fd_len;

    int byte_start = start / 8;
    int byte_end = end / 8 + (end % 8 == 0 ? 0 : 1);

    int left_shift = start % 8;
    int right_shift = 8 - (end % 8 == 0 ? 8 : (end % 8));

    uint32_t value = 0;

    for(int i = byte_end; i >= byte_start; i--) {
        value += (uint32_t)phv->packet[i] << ((byte_end - 1 - i) * 8);
    }
    res = value >> right_shift << (32 - fd_len) >> (32 - fd_len);

    return res;
}

void init_mem() {
    for(int i = 0; i < SRAM_NUM_ALL; i++) {
        glb.srams[i] = new Sram();
    }
    for(int i = 0; i < TCAM_NUM_ALL; i++) {
        glb.tcams[i] = new Tcam();
    }
}

void modify_phv_with_data(FieldInfo field, Data data, PHV * phv) {
    int internal_offset = field.internal_offset;
    int fd_len = field.fd_len;
    auto hi = phv->parsed_headers[field.hdr_id];

    int begin = hi->hdr_offset + internal_offset;
    int end = begin + fd_len;

    int begin_offset = begin % 8;
    int end_offset = end % 8;

    int byte_begin = begin / 8;
    int byte_end = end / 8 - (end_offset == 0 ? 1 : 0);//ceil(end * 1.0 / 8);
    int byte_len = byte_end - byte_begin + 1;

    int origin_len =  data.data_len / 8 + (data.data_len % 8 ? 1 : 0);
    int len = 0;
    if(end_offset == 0) {
        len = data.data_len / 8 + data.data_len % 8 ? 1 : 0;
    } else {
//        std::cout << ":" << (data.data_len + (8 - end_offset)) / 8 << std::endl;
        len = (data.data_len + (8 - end_offset)) / 8 + ((data.data_len + (8 - end_offset)) % 8 ? 1 : 0);
    }
//        int len = (data.data_len + (8 - end_offset)) / 8 + (data.data_len + (8 - end_offset)) % 8 ? 1 : 0;

    auto * res = new uint8_t[len];
    memset(res, 0, len * sizeof(uint8_t));

    int idx = len - 1;
    for(int i = origin_len - 1; i >= 0; i--) {
        res[idx] += (data.val[i] << (8 - (end_offset == 0 ? 8 : end_offset)));
        if(idx > 0){
            res[idx - 1] += (data.val[i] >> end_offset);
            idx--;
        }
    }

    idx = len - 1;
    for(int i = byte_end; i >= byte_begin; i--) {
        int save_bit_num_right = 0;
        int save_bit_num_left = 0;
        if (i == byte_end && end_offset != 0){
            save_bit_num_right = 8 - end_offset;
        }
        if (i == byte_begin && begin_offset != 0){
            save_bit_num_left = begin_offset;
        }
        uint8_t sd = 0;
        for(int j = 0; j < save_bit_num_left; j++) {
            sd += (1 << (7 - j));
        }
        for(int j = 0; j < save_bit_num_right; j++) {
            sd += (1 << j);
        }
        phv->packet[i] = (phv->packet[i] & sd) + res[idx--]; // here
    }
}

uint8_t * get_uints(uint32_t data, int length) {
    int byte_len = ceil(length * 1.0 / 8);

    uint32_t bitmap = 0;
    for(int i = 0; i < length; i++) {
        bitmap += (1 << i);
    }
    int num = (data & bitmap);

    auto res = new uint8_t[byte_len];

    for(int i = byte_len - 1; i >= 0; i--) {
        res[i] = (data >> ((byte_len - 1 - i) * 8));
    }

    return res;
}

void drop_packet(PHV * phv) {
    FieldInfo eg_if_fd{31, 9, 9};
    Data data0 = {9, get_uints(0, 9)};
    modify_phv_with_data(eg_if_fd, data0, phv);
}