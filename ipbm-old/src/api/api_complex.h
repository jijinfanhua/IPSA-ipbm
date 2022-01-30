#pragma once

#include "api.h"

#define ETHERTYPE_TUNNEL 0x1212

using namespace api;

struct TunnelHdr {
    uint16_t proto;
    uint16_t dst_id;
};

class ApiComplex {
public:
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

    static void build() {
        // ----- BUILD VLAN TABLE -----
        for (int i = 0; i < 4; i++) {
            glb.vlan_group.port_masks[i] = 0x0f;
        }
        glb.vlan_group.port_masks[4] = 0x10;

        // ----- BUILD PARSER -----
        ParserHeader hdr_ether(
                HDR_ETHER_ID, sizeof(ether_header), offsetof(ether_header, ether_type), sizeof(uint16_t), {});
        assert(add_parser_header({}, "", hdr_ether) == 0);

        ParserHeader hdr_ip(HDR_IP_ID, sizeof(iphdr), offsetof(iphdr, protocol), sizeof(uint8_t), {});
        uint16_t ip_tag = htons(ETHERTYPE_IP);
        std::string ip_tag_s((char *) &ip_tag, sizeof(ip_tag));
        assert(add_parser_header({HDR_ETHER_ID}, ip_tag_s, hdr_ip) == 0);

        // ----- BUILD FLOW TABLE -----
        {
            // MAC TABLE: (VLAN_ID, DST_MAC) -> (OUT_PORT)
            set_flow_table(TAB_MAC_ID, 1, MEM_SRAM, MATCH_EXACT,
                           {
                                   FieldSpec(HDR_ID_META_DATA, offsetof(MetaData, vlan_id), sizeof(uint32_t)),
                                   FieldSpec(HDR_ETHER_ID, offsetof(ether_header, ether_dhost), sizeof(ether_addr))
                           },
                           sizeof(uint32_t), false);
        }
        {
            // ARP TABLE: (DST_IP) -> (NEXT_HOP_MAC, OUT_PORT)
            set_flow_table(TAB_ARP_ID, 0, MEM_SRAM, MATCH_EXACT,
                           {FieldSpec(HDR_IP_ID, offsetof(iphdr, daddr), sizeof(in_addr_t))},
                           sizeof(ether_addr) + sizeof(uint32_t), false);
        }

        // ----- BUILD ACTION -----
        Action act_none;
        set_action(ACT_NONE_ID, act_none);

        Action act_mac_bcast({Primitive::set_multicast()});
        set_action(ACT_MAC_BCAST, act_mac_bcast);

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
        set_action(ACT_IP_FWD_ID, act_ip_fwd);

        Action act_mac_fwd({Primitive::set_port_from_arg(0)});
        set_action(ACT_MAC_FWD_ID, act_mac_fwd);

        // ----- BUILD EXECUTOR -----
        std::vector<RuntimeArgSpec> mac_fwd_args{
                RuntimeArgSpec(0, sizeof(uint32_t))
        };
        set_executor(PROC_ETHER_ID, ACT_MAC_FWD_ID, Processor::PROC_ID_END, ACT_MAC_BCAST, PROC_IP_ID, mac_fwd_args);

        std::vector<RuntimeArgSpec> ip_fwd_args{
                RuntimeArgSpec(0, sizeof(ether_addr)),
                RuntimeArgSpec(sizeof(ether_addr), sizeof(uint32_t)),
        };
        set_executor(PROC_IP_ID, ACT_IP_FWD_ID, Processor::PROC_ID_END, ACT_NONE_ID, Processor::PROC_ID_END,
                     ip_fwd_args);

        // ----- BUILD PROCESSOR -----
        set_processor(PROC_ETHER_ID, Condition::is_valid(HDR_ETHER_ID),
                      NextStage::table(TAB_MAC_ID), NextStage::proc(PROC_IP_ID));
        set_processor(PROC_IP_ID, Condition::is_valid(HDR_IP_ID),
                      NextStage::table(TAB_ARP_ID), NextStage::proc(Processor::PROC_ID_END));

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

                FlowEntry flow;
                flow.key = std::string((char *) &ip, sizeof(in_addr_t));
                flow.action_id = ACT_IP_FWD_ID;
                flow.action_data = {
                        std::string((char *) mac, sizeof(ether_addr)),
                        std::string((char *) &port, sizeof(uint32_t))
                };
                assert(add_flow_entry(MEM_SRAM, TAB_ARP_ID, flow) == 0);
                port <<= 1;
            }
        }
        {
            in_addr_t ip;
            inet_aton("10.0.2.2", (in_addr *) &ip);
            uint32_t port = 0x10;
            uint8_t mac2[] = "\x00\x00\x00\x00\x02\x02";

            FlowEntry flow;
            flow.key = std::string((char *) &ip, sizeof(in_addr_t));
            flow.action_id = ACT_IP_FWD_ID;
            flow.action_data = {
                    std::string((char *) mac2, sizeof(ether_addr)),
                    std::string((char *) &port, sizeof(uint32_t))
            };
            assert(add_flow_entry(MEM_SRAM, TAB_ARP_ID, flow) == 0);
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
                FlowEntry flow;
                flow.key = std::string((char *) &vlan_id, sizeof(uint32_t)) +
                           std::string((char *) mac, sizeof(ether_addr));
                flow.action_id = ACT_MAC_FWD_ID;    // no effect
                flow.action_data = {
                        std::string((char *) &port_mask, sizeof(uint32_t))
                };
                assert(add_flow_entry(MEM_SRAM, TAB_MAC_ID, flow) == 0);
                port_mask <<= 1;
            }
        }
    }

    static void add_vlan() {
        // UPDATE VLAN GROUP
        glb.vlan_group.port_masks[0] = 0x01 | 0x02;
        glb.vlan_group.port_masks[1] = 0x01 | 0x02;
        glb.vlan_group.port_masks[2] = 0x04 | 0x08;
        glb.vlan_group.port_masks[3] = 0x04 | 0x08;

        // UPDATE MAC TABLE
        uint32_t port_mask = 1;
        uint32_t vlan_id = 0x0f;
        for (uint32_t i = 2; i < 6; i++) {
            uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x01, (uint8_t) i};
            FlowEntry flow;
            flow.key = std::string((char *) &vlan_id, sizeof(uint32_t)) +
                       std::string((char *) mac, sizeof(ether_addr));
            flow.action_id = ACT_MAC_FWD_ID;    // no effect
            flow.action_data = {
                    std::string((char *) &port_mask, sizeof(uint32_t))
            };
            FlowEntry dst_flow;
            uint32_t dst_vlan_id = (i < 4) ? (0x01 | 0x02) : (0x04 | 0x08);
            dst_flow.key = std::string((char *) &dst_vlan_id, sizeof(uint32_t)) +
                           std::string((char *) mac, sizeof(ether_addr));
            dst_flow.action_id = ACT_MAC_FWD_ID;
            dst_flow.action_data = {
                    std::string((char *) &port_mask, sizeof(uint32_t))
            };
            assert(mod_flow_entry(MEM_SRAM, TAB_MAC_ID, flow, dst_flow) == 0);
            port_mask <<= 1;
        }
    }

    static void add_tunnel() {
        // ----- BUILD PARSER -----
        uint16_t ip_tag = htons(ETHERTYPE_IP);
        std::string ip_tag_s = std::string((char *) &ip_tag, sizeof(ip_tag));

        uint16_t tunnel_tag = htons(ETHERTYPE_TUNNEL);
        std::string tunnel_tag_s = std::string((char *) &tunnel_tag, sizeof(uint16_t));
        ParserHeader hdr_tunnel(HDR_TUNNEL_ID, sizeof(TunnelHdr), offsetof(TunnelHdr, proto), sizeof(uint16_t), {
                NextParserHeader(ip_tag_s, HDR_IP_ID)
        });
        assert(add_parser_header({HDR_ETHER_ID}, tunnel_tag_s, hdr_tunnel) == 0);

        // ----- BUILD FLOW TABLE -----
        set_flow_table(TAB_TUNNEL_ID, 2, MEM_SRAM, MATCH_EXACT,
                       {FieldSpec(HDR_TUNNEL_ID, offsetof(TunnelHdr, dst_id), sizeof(uint16_t))},
                       sizeof(uint32_t), false);

        // ----- BUILD ACTION -----
        Action act_tun{{Primitive::set_port_from_arg(0)}};
        set_action(ACT_TUNNEL_FWD_ID, act_tun);

        // ----- BUILD EXECUTOR -----
        std::vector<RuntimeArgSpec> args_tunnel{
                RuntimeArgSpec(0, sizeof(uint16_t))
        };
        set_executor(PROC_TUNNEL_ID, ACT_TUNNEL_FWD_ID, Processor::PROC_ID_END, ACT_NONE_ID, Processor::PROC_ID_END,
                     args_tunnel);

        // ----- BUILD PROCESSOR -----
        set_processor(PROC_ETHER_ID, Condition::is_valid(HDR_ETHER_ID),
                      NextStage::table(TAB_MAC_ID), NextStage::proc(PROC_TUNNEL_ID));
        set_processor(PROC_TUNNEL_ID, Condition::is_valid(HDR_TUNNEL_ID),
                      NextStage::table(TAB_TUNNEL_ID), NextStage::proc(PROC_IP_ID));

        // ----- INSERT FLOW ENTRY -----
        {
            // TUNNEL TABLE
            for (uint16_t dst_id = 0; dst_id < 5; dst_id++) {
                uint32_t port = 1 << dst_id;
                FlowEntry flow;
                flow.key = std::string((char *) &dst_id, sizeof(dst_id));
                flow.action_id = ACT_TUNNEL_FWD_ID;
                flow.action_data = {
                        std::string((char *) &port, sizeof(port)),
                };
                assert(add_flow_entry(MEM_SRAM, TAB_TUNNEL_ID, flow) == 0);
            }
        }
    }

    static void add_counter() {
        // ----- BUILD FLOW TABLE -----
        set_flow_table(TAB_COUNTER_ID, 3, MEM_SRAM, MATCH_EXACT,
                       {FieldSpec(HDR_IP_ID, offsetof(iphdr, saddr), sizeof(in_addr_t))},
                       sizeof(uint32_t), true);

        // ----- BUILD EXECUTOR -----
        std::vector<RuntimeArgSpec> args_ip_fwd{
                RuntimeArgSpec(0, sizeof(ether_addr)),
                RuntimeArgSpec(sizeof(ether_addr), sizeof(uint32_t)),
        };
        set_executor(PROC_IP_ID, ACT_IP_FWD_ID, PROC_COUNTER_ID, ACT_NONE_ID, PROC_COUNTER_ID, args_ip_fwd);
        set_executor(PROC_COUNTER_ID, ACT_NONE_ID, Processor::PROC_ID_END, ACT_NONE_ID, Processor::PROC_ID_END, {});

        // ----- BUILD PROCESSOR -----
        set_processor(PROC_COUNTER_ID, Condition::is_valid(HDR_IP_ID),
                      NextStage::table(TAB_COUNTER_ID), NextStage::proc(Processor::PROC_ID_END));
    }
};
