#pragma once

#include "api/api_client.h"
#include <glog/logging.h>

#define ETHERTYPE_TUNNEL 0x1212

struct TunnelHdr {
    uint16_t proto;
    uint16_t dst_id;
};

class CtrlFwd {
public:
    api::ModClient api_client;

    static const int PROC_ETHER_ID = 0;
    static const int PROC_TUNNEL_ID = 1;
    static const int PROC_IP_ID = 2;
    static const int PROC_COUNTER_ID = 3;

    static const int HDR_ETHER_ID = 0;
    static const int HDR_TUNNEL_ID = 1;
    static const int HDR_IP_ID = 2;

    static const int ACT_NONE_ID = 0;
    static const int ACT_MAC_BCAST = 1;
    static const int ACT_MAC_FWD_ID = 2;
    static const int ACT_IP_FWD_ID = 3;
    static const int ACT_TUNNEL_FWD_ID = 4;

    static const int TAB_MAC_ID = 0;
    static const int TAB_ARP_ID = 1;
    static const int TAB_TUNNEL_ID = 2;
    static const int TAB_COUNTER_ID = 3;

    CtrlFwd(std::shared_ptr<grpc::Channel> channel) : api_client(std::move(channel)) {}

    void basic_forward() {
        RC rc;
        // ----- BUILD VLAN TABLE -----
        std::vector<std::pair<int, uint32_t>> vlan_table = {
                {0, 0x0f},
                {1, 0x0f},
                {2, 0x0f},
                {3, 0x0f},
                {4, 0x10},
        };
        for (auto &entry : vlan_table) {
            int iface = entry.first;
            uint32_t dst_port_mask = entry.second;

            api::FlowEntry src_flow;
            uint32_t src_port = 0xffffffff;
            src_flow.key = std::string((char *) &iface, sizeof(int));
            src_flow.action_id = -1;
            src_flow.action_data = {std::string((char *) &src_port, sizeof(uint32_t))};

            api::FlowEntry dst_flow;
            dst_flow.key = std::string((char *) &iface, sizeof(int));
            dst_flow.action_id = -1;
            dst_flow.action_data = {std::string((char *) &dst_port_mask, sizeof(uint32_t))};

            rc = api_client.mod_flow_entry(TAB_ID_VLAN_GROUP, src_flow, dst_flow);
            if (rc) { LOG(ERROR) << "Error code " << rc; }
        }

        // ----- BUILD PARSER -----
        ParserHeader hdr_ether(
                HDR_ETHER_ID, sizeof(ether_header), offsetof(ether_header, ether_type), sizeof(uint16_t), {});
        rc = api_client.add_parser_header({}, "", hdr_ether);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        ParserHeader hdr_ip(HDR_IP_ID, sizeof(iphdr), offsetof(iphdr, protocol), sizeof(uint8_t), {});
        uint16_t ip_tag = htons(ETHERTYPE_IP);
        std::string ip_tag_s((char *) &ip_tag, sizeof(ip_tag));
        rc = api_client.add_parser_header({HDR_ETHER_ID}, ip_tag_s, hdr_ip);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- BUILD FLOW TABLE -----
        {
            // MAC TABLE: (VLAN_ID, DST_MAC) -> (OUT_PORT)
            rc = api_client.set_flow_table(TAB_MAC_ID, 1, MEM_SRAM, MATCH_EXACT,
                                           {
                                                   FieldSpec(HDR_ID_META_DATA, offsetof(MetaData, vlan_id),
                                                             sizeof(uint32_t)),
                                                   FieldSpec(HDR_ETHER_ID, offsetof(ether_header, ether_dhost),
                                                             sizeof(ether_addr))
                                           },
                                           sizeof(uint32_t), false);
            if (rc) { LOG(ERROR) << "Error code " << rc; }
        }
        {
            // ARP TABLE: (DST_IP) -> (NEXT_HOP_MAC, OUT_PORT)
            rc = api_client.set_flow_table(TAB_ARP_ID, 0, MEM_SRAM, MATCH_EXACT,
                                           {FieldSpec(HDR_IP_ID, offsetof(iphdr, daddr), sizeof(in_addr_t))},
                                           sizeof(ether_addr) + sizeof(uint32_t), false);
            if (rc) { LOG(ERROR) << "Error code " << rc; }
        }

        // ----- BUILD ACTION -----
        Action act_none;
        rc = api_client.set_action(ACT_NONE_ID, act_none);
        if (rc) { LOG(ERROR) << "Error code " << rc; }
        Action act_mac_bcast({Primitive::set_multicast()});
        rc = api_client.set_action(ACT_MAC_BCAST, act_mac_bcast);
        if (rc) { LOG(ERROR) << "Error code " << rc; }
        Action act_ip_fwd(
                {
                        Primitive::add(FieldSpec(HDR_IP_ID, offsetof(iphdr, ttl), sizeof(uint8_t)), -1),
                        Primitive::cksum(FieldSpec(HDR_IP_ID, 0, sizeof(iphdr)),
                                         FieldSpec(HDR_IP_ID, offsetof(iphdr, check), sizeof(uint16_t)),
                                         HASH_CSUM16),
                        Primitive::copy_field(
                                FieldSpec(HDR_ETHER_ID, offsetof(ether_header, ether_shost), sizeof(ether_addr)),
                                FieldSpec(HDR_ETHER_ID, offsetof(ether_header, ether_dhost), sizeof(ether_addr))),
                        Primitive::set_field(
                                FieldSpec(HDR_ETHER_ID, offsetof(ether_header, ether_dhost), sizeof(ether_addr)), 0),
                        Primitive::set_port_from_arg(1),
                }
        );
        rc = api_client.set_action(ACT_IP_FWD_ID, act_ip_fwd);
        if (rc) { LOG(ERROR) << "Error code " << rc; }
        Action act_mac_fwd({Primitive::set_port_from_arg(0)});
        rc = api_client.set_action(ACT_MAC_FWD_ID, act_mac_fwd);
        if (rc) { LOG(ERROR) << "Error code " << rc; }
        // ----- BUILD EXECUTOR -----
        std::vector<RuntimeArgSpec> mac_fwd_args{
                RuntimeArgSpec(0, sizeof(uint32_t))
        };
        rc = api_client.set_executor(PROC_ETHER_ID, ACT_MAC_FWD_ID, Processor::PROC_ID_END, ACT_MAC_BCAST, PROC_IP_ID,
                                     mac_fwd_args);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        std::vector<RuntimeArgSpec> ip_fwd_args{
                RuntimeArgSpec(0, sizeof(ether_addr)),
                RuntimeArgSpec(sizeof(ether_addr), sizeof(uint32_t)),
        };
        rc = api_client.set_executor(PROC_IP_ID, ACT_IP_FWD_ID, Processor::PROC_ID_END, ACT_NONE_ID,
                                     Processor::PROC_ID_END,
                                     ip_fwd_args);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- BUILD PROCESSOR -----
        rc = api_client.set_processor(PROC_ETHER_ID, Condition::is_valid(HDR_ETHER_ID),
                                      NextStage::table(TAB_MAC_ID), NextStage::proc(PROC_IP_ID));
        if (rc) { LOG(ERROR) << "Error code " << rc; }
        rc = api_client.set_processor(PROC_IP_ID, Condition::is_valid(HDR_IP_ID),
                                      NextStage::table(TAB_ARP_ID), NextStage::proc(Processor::PROC_ID_END));
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- INSERT FLOW ENTRY -----
        {
            // ARP TABLE
            uint32_t port = 1;
            for (int i = 2; i < 6; i++) {
                in_addr_t ip;
                char ip_s[32];
                sprintf(ip_s, "10.0.1.%d", i);
                inet_aton(ip_s, (in_addr *) &ip);
                uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x01, (uint8_t) i};

                api::FlowEntry flow;
                flow.key = std::string((char *) &ip, sizeof(in_addr_t));
                flow.action_id = ACT_IP_FWD_ID;
                flow.action_data = {
                        std::string((char *) mac, sizeof(ether_addr)),
                        std::string((char *) &port, sizeof(uint32_t))
                };
                rc = api_client.add_flow_entry(TAB_ARP_ID, flow);
                if (rc) { LOG(ERROR) << "Error code " << rc; }
                port <<= 1;
            }
        }
        {
            in_addr_t ip;
            inet_aton("10.0.2.2", (in_addr *) &ip);
            uint32_t port = 0x10;
            uint8_t mac2[] = "\x00\x00\x00\x00\x02\x02";

            api::FlowEntry flow;
            flow.key = std::string((char *) &ip, sizeof(in_addr_t));
            flow.action_id = ACT_IP_FWD_ID;
            flow.action_data = {
                    std::string((char *) mac2, sizeof(ether_addr)),
                    std::string((char *) &port, sizeof(uint32_t))
            };
            rc = api_client.add_flow_entry(TAB_ARP_ID, flow);
            if (rc) { LOG(ERROR) << "Error code " << rc; }
        }

        {
            // MAC TABLE
            uint32_t port_mask = 1;
            uint32_t vlan_id = 0x0f;
            for (uint32_t i = 2; i < 6; i++) {
                // vlan_id,   dst_mac      -> out_port
                // 0x0f, 00:00:00:00:01:02 -> 0x01
                // 0x0f, 00:00:00:00:01:03 -> 0x02
                // 0x0f, 00:00:00:00:01:04 -> 0x04
                // 0x0f, 00:00:00:00:01:05 -> 0x08
                uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x01, (uint8_t) i};
                api::FlowEntry flow;
                flow.key = std::string((char *) &vlan_id, sizeof(uint32_t)) +
                           std::string((char *) mac, sizeof(ether_addr));
                flow.action_id = ACT_MAC_FWD_ID;    // no effect
                flow.action_data = {
                        std::string((char *) &port_mask, sizeof(uint32_t))
                };
                rc = api_client.add_flow_entry(TAB_MAC_ID, flow);
                if (rc) { LOG(ERROR) << "Error code " << rc; }
                port_mask <<= 1;
            }
        }
    }

    void add_vlan() {
        RC rc;
        // UPDATE VLAN GROUP
        std::vector<std::pair<int, uint32_t>> vlan_table = {
                {0, 0x01 | 0x02},
                {1, 0x01 | 0x02},
                {2, 0x04 | 0x08},
                {3, 0x04 | 0x08},
        };
        for (auto &entry : vlan_table) {
            int iface = entry.first;
            uint32_t dst_port_mask = entry.second;

            api::FlowEntry src_flow;
            uint32_t src_port = 0x0f;
            src_flow.key = std::string((char *) &iface, sizeof(int));
            src_flow.action_id = -1;
            src_flow.action_data = {std::string((char *) &src_port, sizeof(uint32_t))};

            api::FlowEntry dst_flow;
            dst_flow.key = std::string((char *) &iface, sizeof(int));
            dst_flow.action_id = -1;
            dst_flow.action_data = {std::string((char *) &dst_port_mask, sizeof(uint32_t))};

            rc = api_client.mod_flow_entry(TAB_ID_VLAN_GROUP, src_flow, dst_flow);
            if (rc) { LOG(ERROR) << "Error code " << rc; }
        }

        // UPDATE MAC TABLE
        uint32_t port_mask = 1;
        uint32_t vlan_id = 0x0f;
        for (uint32_t i = 2; i < 6; i++) {
            uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x01, (uint8_t) i};
            api::FlowEntry flow;
            flow.key = std::string((char *) &vlan_id, sizeof(uint32_t)) +
                       std::string((char *) mac, sizeof(ether_addr));
            flow.action_id = ACT_MAC_FWD_ID;    // no effect
            flow.action_data = {
                    std::string((char *) &port_mask, sizeof(uint32_t))
            };
            api::FlowEntry dst_flow;
            uint32_t dst_vlan_id = (i < 4) ? (0x01 | 0x02) : (0x04 | 0x08);
            dst_flow.key = std::string((char *) &dst_vlan_id, sizeof(uint32_t)) +
                           std::string((char *) mac, sizeof(ether_addr));
            dst_flow.action_id = ACT_MAC_FWD_ID;
            dst_flow.action_data = {
                    std::string((char *) &port_mask, sizeof(uint32_t))
            };
            rc = api_client.mod_flow_entry(TAB_MAC_ID, flow, dst_flow);
            if (rc) { LOG(ERROR) << "Error code " << rc; }
            port_mask <<= 1;
        }
    }

    void add_tunnel() {
        RC rc;
        // ----- BUILD PARSER -----
        uint16_t ip_tag = htons(ETHERTYPE_IP);
        std::string ip_tag_s = std::string((char *) &ip_tag, sizeof(ip_tag));

        uint16_t tunnel_tag = htons(ETHERTYPE_TUNNEL);
        std::string tunnel_tag_s = std::string((char *) &tunnel_tag, sizeof(uint16_t));
        ParserHeader hdr_tunnel(HDR_TUNNEL_ID, sizeof(TunnelHdr), offsetof(TunnelHdr, proto), sizeof(uint16_t), {
                NextParserHeader(ip_tag_s, HDR_IP_ID)
        });
        rc = api_client.add_parser_header({HDR_ETHER_ID}, tunnel_tag_s, hdr_tunnel);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- BUILD FLOW TABLE -----
        rc = api_client.set_flow_table(TAB_TUNNEL_ID, 2, MEM_SRAM, MATCH_EXACT,
                                       {FieldSpec(HDR_TUNNEL_ID, offsetof(TunnelHdr, dst_id), sizeof(uint16_t))},
                                       sizeof(uint32_t), false);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- BUILD ACTION -----
        Action act_tun{{Primitive::set_port_from_arg(0)}};
        rc = api_client.set_action(ACT_TUNNEL_FWD_ID, act_tun);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- BUILD EXECUTOR -----
        std::vector<RuntimeArgSpec> args_tunnel{
                RuntimeArgSpec(0, sizeof(uint16_t))
        };
        rc = api_client.set_executor(PROC_TUNNEL_ID, ACT_TUNNEL_FWD_ID, Processor::PROC_ID_END, ACT_NONE_ID,
                                     Processor::PROC_ID_END,
                                     args_tunnel);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- BUILD PROCESSOR -----
        rc = api_client.set_processor(PROC_ETHER_ID, Condition::is_valid(HDR_ETHER_ID),
                                      NextStage::table(TAB_MAC_ID), NextStage::proc(PROC_TUNNEL_ID));
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        rc = api_client.set_processor(PROC_TUNNEL_ID, Condition::is_valid(HDR_TUNNEL_ID),
                                      NextStage::table(TAB_TUNNEL_ID), NextStage::proc(PROC_IP_ID));
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- INSERT FLOW ENTRY -----
        {
            // TUNNEL TABLE
            for (uint16_t dst_id = 0; dst_id < 5; dst_id++) {
                uint32_t port = 1 << dst_id;
                api::FlowEntry flow;
                flow.key = std::string((char *) &dst_id, sizeof(dst_id));
                flow.action_id = ACT_TUNNEL_FWD_ID;
                flow.action_data = {
                        std::string((char *) &port, sizeof(port)),
                };
                rc = api_client.add_flow_entry(TAB_TUNNEL_ID, flow);
                if (rc) { LOG(ERROR) << "Error code " << rc; }
            }
        }
    }

    void add_counter() {
        RC rc;
        // ----- BUILD FLOW TABLE -----
        rc = api_client.set_flow_table(TAB_COUNTER_ID, 3, MEM_SRAM, MATCH_EXACT,
                                       {FieldSpec(HDR_IP_ID, offsetof(iphdr, saddr), sizeof(in_addr_t))},
                                       sizeof(uint32_t), true);
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- BUILD EXECUTOR -----
        std::vector<RuntimeArgSpec> args_ip_fwd{
                RuntimeArgSpec(0, sizeof(ether_addr)),
                RuntimeArgSpec(sizeof(ether_addr), sizeof(uint32_t)),
        };
        rc = api_client.set_executor(PROC_IP_ID, ACT_IP_FWD_ID, PROC_COUNTER_ID, ACT_NONE_ID, PROC_COUNTER_ID,
                                     args_ip_fwd);
        if (rc) { LOG(ERROR) << "Error code " << rc; }
        rc = api_client.set_executor(PROC_COUNTER_ID, ACT_NONE_ID, Processor::PROC_ID_END, ACT_NONE_ID,
                                     Processor::PROC_ID_END, {});
        if (rc) { LOG(ERROR) << "Error code " << rc; }

        // ----- BUILD PROCESSOR -----
        rc = api_client.set_processor(PROC_COUNTER_ID, Condition::is_valid(HDR_IP_ID),
                                      NextStage::table(TAB_COUNTER_ID), NextStage::proc(Processor::PROC_ID_END));
        if (rc) { LOG(ERROR) << "Error code " << rc; }
    }
};
