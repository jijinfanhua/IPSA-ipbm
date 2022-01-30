#pragma once

#include "api.h"

#define ETHERTYPE_TUNNEL 0x1212

using namespace api;

struct TunnelHdr {
    uint16_t proto;
    uint16_t dst_id;
};

class ApiTunnel {
public:
    static const int PROC_TUNNEL_ID = 0;
    static const int PROC_IPV4_ID = 1;

    static const int HDR_ETHER_ID = 0;
    static const int HDR_IP_ID = 1;
    static const int HDR_TUNNEL_ID = 2;

    static const int ACT_NONE_ID = 0;
    static const int ACT_FWD_ID = 1;
    static const int ACT_TUNNEL_ID = 2;

    static const int TAB_TUNNEL_ID = 0;
    static const int TAB_ARP_ID = 1;

    static void build() {
        // ----- BUILD PARSER -----
        ParserHeader hdr_ether(
                HDR_ETHER_ID, sizeof(ether_header), offsetof(ether_header, ether_type), sizeof(uint16_t), {});
        assert(add_parser_header({}, {}, hdr_ether) == 0);

        uint16_t tunnel_tag = htons(ETHERTYPE_TUNNEL);
        std::string tunnel_tag_s = std::string((char *) &tunnel_tag, sizeof(uint16_t));
        ParserHeader hdr_tunnel(HDR_TUNNEL_ID, sizeof(TunnelHdr), offsetof(TunnelHdr, proto), sizeof(uint16_t), {});
        assert(add_parser_header({HDR_ETHER_ID}, tunnel_tag_s, hdr_tunnel) == 0);

        ParserHeader hdr_ip(HDR_IP_ID, sizeof(iphdr), offsetof(iphdr, protocol), sizeof(uint8_t), {});
        uint16_t ip_tag = htons(ETHERTYPE_IP);
        std::string ip_tag_s = std::string((char *) &ip_tag, sizeof(ip_tag));
        assert(add_parser_header({HDR_ETHER_ID, HDR_TUNNEL_ID}, ip_tag_s, hdr_ip) == 0);

        // ----- BUILD FLOW TABLE -----
        set_flow_table(TAB_ARP_ID, 0, MEM_SRAM, MATCH_EXACT,
                       {FieldSpec(HDR_IP_ID, offsetof(iphdr, daddr), sizeof(in_addr_t))},
                       sizeof(ether_addr) + sizeof(uint32_t), false);

        set_flow_table(TAB_TUNNEL_ID, 1, MEM_SRAM, MATCH_EXACT,
                       {FieldSpec(HDR_TUNNEL_ID, offsetof(TunnelHdr, dst_id), sizeof(uint16_t))},
                       sizeof(uint16_t), false);

        // ----- BUILD EXECUTOR -----
        Action act_none;
        set_action(ACT_NONE_ID, act_none);

        Action act_fwd(
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
        set_action(ACT_FWD_ID, act_fwd);

        Action act_tun{
                {
                        Primitive::set_port_from_arg(0)
                }
        };
        set_action(ACT_TUNNEL_ID, act_tun);

        std::vector<RuntimeArgSpec> args_fwd{
                RuntimeArgSpec(0, sizeof(ether_addr)),
                RuntimeArgSpec(sizeof(ether_addr), sizeof(uint32_t)),
        };
        set_executor(PROC_IPV4_ID, ACT_FWD_ID, Processor::PROC_ID_END, ACT_NONE_ID, Processor::PROC_ID_END,
                     args_fwd);

        std::vector<RuntimeArgSpec> args_tunnel{
                RuntimeArgSpec(0, sizeof(uint16_t))
        };
        set_executor(PROC_TUNNEL_ID, ACT_TUNNEL_ID, Processor::PROC_ID_END, ACT_NONE_ID, Processor::PROC_ID_END,
                     args_tunnel);

        // ----- BUILD PROCESSOR -----
        set_processor(PROC_TUNNEL_ID, Condition::is_valid(HDR_TUNNEL_ID),
                      NextStage::table(TAB_TUNNEL_ID), NextStage::proc(PROC_IPV4_ID));
        set_processor(PROC_IPV4_ID, Condition::is_valid(HDR_IP_ID),
                      NextStage::table(TAB_ARP_ID), NextStage::proc(Processor::PROC_ID_END));

        // ----- INSERT FLOW ENTRY -----
        {
            // ARP TABLE
            FlowEntry flow1;
            in_addr_t ip1;
            uint32_t port1 = 0x01;
            inet_aton("10.0.1.50", (in_addr *) &ip1);
            uint8_t mac1[] = "\x00\x00\x00\x00\x01\x50";
            flow1.key = std::string((char *) &ip1, sizeof(in_addr_t));
            flow1.action_id = ACT_FWD_ID;
            flow1.action_data = {
                    std::string((char *) mac1, sizeof(ether_addr)),
                    std::string((char *) &port1, sizeof(uint32_t))
            };

            FlowEntry flow2;
            in_addr_t ip2;
            uint32_t port2 = 0x02;
            inet_aton("10.0.2.50", (in_addr *) &ip2);
            uint8_t mac2[] = "\x00\x00\x00\x00\x02\x50";
            flow2.key = std::string((char *) &ip2, sizeof(in_addr_t));
            flow2.action_id = ACT_FWD_ID;
            flow2.action_data = {
                    std::string((char *) mac2, sizeof(ether_addr)),
                    std::string((char *) &port2, sizeof(uint32_t))
            };
            {
                assert(add_flow_entry(MEM_SRAM, TAB_ARP_ID, flow1) == 0);
                assert(add_flow_entry(MEM_SRAM, TAB_ARP_ID, flow2) == 0);
                assert(del_flow_entry(MEM_SRAM, TAB_ARP_ID, flow1) == 0);
                assert(mod_flow_entry(MEM_SRAM, TAB_ARP_ID, flow2, flow1) == 0);
                assert(del_flow_entry(MEM_SRAM, TAB_ARP_ID, flow1) == 0);
            }
            assert(add_flow_entry(MEM_SRAM, TAB_ARP_ID, flow1) == 0);
            assert(add_flow_entry(MEM_SRAM, TAB_ARP_ID, flow2) == 0);

            auto flow1_buf = get_flow_entry_val(TAB_ARP_ID, (Buffer) flow1.key.c_str());
            assert(memcmp(flow1_buf, mac1, sizeof(ether_addr)) == 0);
            assert(*(uint32_t *) (flow1_buf + sizeof(ether_addr)) == port1);
        }
        {
            // TUNNEL TABLE
            {
                FlowEntry flow;
                uint16_t dst_id = 0;
                uint16_t port = 0x01;
                flow.key = std::string((char *) &dst_id, sizeof(dst_id));
                flow.action_id = ACT_TUNNEL_ID;
                flow.action_data = {
                        std::string((char *) &port, sizeof(port)),
                };
                assert(add_flow_entry(MEM_SRAM, TAB_TUNNEL_ID, flow) == 0);
            }
            {
                FlowEntry flow;
                uint16_t dst_id = htons(1);
                uint16_t port = 0x02;
                flow.key = std::string((char *) &dst_id, sizeof(dst_id));
                flow.action_id = ACT_TUNNEL_ID;
                flow.action_data = {
                        std::string((char *) &port, sizeof(port)),
                };
                assert(add_flow_entry(MEM_SRAM, TAB_TUNNEL_ID, flow) == 0);
            }
        }
    }
};
