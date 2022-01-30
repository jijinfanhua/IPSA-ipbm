#pragma once

#include "processor.h"
#include "table.h"
#include "action.h"
#include "field.h"
#include "defs.h"
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <vector>
#include <string>
#include <memory>
#include <map>

static const int TAB_ID_VLAN_GROUP = 1002;

static const int HDR_ID_META_DATA = 1000;
static const int HDR_ID_ACTION_PARAM = 1001;

struct MetaData {
    uint32_t out_port;
    uint32_t vlan_id;
};

struct VLanGroup {
    uint32_t port_masks[MAX_IF_NUM];

    VLanGroup() {
        for (int i = 0; i < MAX_IF_NUM; i++) {
            port_masks[i] = 0xffffffff;
        }
    }
};

struct Global {
    // interface info
    std::vector<std::string> if_names;
    std::vector<in_addr_t> if_ips;
    std::vector<ether_addr> if_macs;
    int num_if;
    // logical & flow table
    std::vector<std::shared_ptr<LogicTable>> logic_tables;
    FlowTable flow_tables[NUM_FLOW_TABLES];
    // global headers
    BufView hdrs[NUM_HDRS];
    // processors
    Processor processors[NUM_PROC];
    // global parser
    Parser parser;
    // Actions
    Action actions[NUM_ACTIONS];
    // packet meta data
    MetaData meta_data;  // port mask
    // VLan group
    VLanGroup vlan_group;

    Global();

    static void process_packet(uint8_t *packet, int recv_len, int if_idx);
};

extern Global glb;

void config_init(const std::string &config_path);
