#include "global.h"
#include "processor.h"
#include <glog/logging.h>
#include <yaml-cpp/yaml.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>

struct TableSpec {
    int physical_table_id;
    int key_len;
    int val_len;
    int num_entry;
};

Global::Global() {
    // Init logical SRAM
    std::vector<TableSpec> sram_spec = {
            {0, 10, 10, 257},
            {1, 20, 20, 257},
            {2, 24, 24, 257},
            {3, 32, 32, 257},
            {0, 48, 48, 257},
            {1, 64, 64, 257},
            {2, 80, 80, 257},
            {3, 96, 96, 257},
    };
    for (auto &spec: sram_spec) {
        glb.logic_tables.emplace_back(std::make_shared<LogicSram>(
                spec.physical_table_id, spec.key_len, spec.val_len, spec.num_entry));
    }
    // Init logical TCAM
    std::vector<TableSpec> tcam_spec = {
            {0, 10, 10, 257},
            {1, 20, 20, 257},
            {2, 24, 24, 257},
            {3, 32, 32, 257},
            {0, 48, 48, 257},
            {1, 64, 64, 257},
            {2, 80, 80, 257},
            {3, 96, 96, 257},
    };
    for (auto &spec: tcam_spec) {
        glb.logic_tables.emplace_back(std::make_shared<LogicTcam>(
                spec.physical_table_id, spec.key_len, spec.val_len, spec.num_entry));
    }
}

void Global::process_packet(uint8_t *packet, int recv_len, int if_idx) {
    glb.meta_data.out_port = 0;
    glb.meta_data.vlan_id = glb.vlan_group.port_masks[if_idx];
    int curr_proc_id = Processor::PROC_START;
    while (curr_proc_id != Processor::PROC_ID_END) {
        auto &proc = glb.processors[curr_proc_id];
        int next_proc_id = proc.process(packet, recv_len);
        curr_proc_id = next_proc_id;
    }
}

Global glb;

void config_init(const std::string &config_path) {
    // Parse interface config
    YAML::Node yml_if_config = YAML::LoadFile(config_path);
    glb.num_if = yml_if_config["interfaces"].size();
    for (auto &&yml_if : yml_if_config["interfaces"]) {
        std::string if_name = yml_if["name"].as<std::string>();
        std::string if_ip = yml_if["ip"].as<std::string>();
        in_addr_t ip;
        inet_aton(if_ip.c_str(), (in_addr *) &ip);
        glb.if_names.push_back(if_name);
        glb.if_ips.push_back(ip);
    }
    assert((int) glb.if_names.size() == glb.num_if);
    assert((int) glb.if_ips.size() == glb.num_if);

    // Find mac address of interfaces
    struct ifaddrs *ifaddr;
    if (getifaddrs(&ifaddr) < 0) {
        throw std::runtime_error("Cannot get mac address from interface");
    }
    for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) { continue; }
        for (int i = 0; i < glb.num_if; i++) {
            if (ifa->ifa_addr->sa_family == AF_PACKET && ifa->ifa_name == glb.if_names[i]) {
                // found
                ether_addr mac;
                memcpy(&mac, ((struct sockaddr_ll *) ifa->ifa_addr)->sll_addr, sizeof(ether_addr));
                LOG(INFO) << "MAC address of interface " + glb.if_names[i] + " is " +
                             mac2str(mac.ether_addr_octet);
                glb.if_macs.push_back(mac);
                break;
            }
        }
    }
    freeifaddrs(ifaddr);
//    assert((int) glb.if_macs.size() == glb.num_if);
}
