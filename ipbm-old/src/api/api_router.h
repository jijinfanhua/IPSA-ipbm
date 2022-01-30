#pragma once

#include "api.h"

using namespace api;

class ApiRouter {
public:
    static const int PROC_FWD_ID = 0;
    static const int HDR_ETHER_ID = 0;
    static const int HDR_IP_ID = 1;
    static const int ACT_NONE_ID = 0;
    static const int ACT_FWD_ID = 1;
    static const int TAB_ARP_ID = 0;

    static void build() {
        // ----- BUILD PARSER -----
        ParserHeader hdr_ether(
                HDR_ETHER_ID, sizeof(ether_header), offsetof(ether_header, ether_type), sizeof(uint16_t), {});
        assert(add_parser_header({}, {}, hdr_ether) == 0);

        ParserHeader hdr_ip(HDR_IP_ID, sizeof(iphdr), offsetof(iphdr, protocol), sizeof(uint8_t), {});

        uint16_t ip_tag = htons(ETHERTYPE_IP);
        assert(add_parser_header({HDR_ETHER_ID}, {std::string((char *) &ip_tag, sizeof(ip_tag))}, hdr_ip) == 0);

        // ----- BUILD FLOW TABLE -----
        set_flow_table(TAB_ARP_ID, 0, MEM_SRAM, MATCH_EXACT,
                       {FieldSpec(HDR_IP_ID, offsetof(iphdr, daddr), sizeof(in_addr_t))},
                       sizeof(ether_addr) + sizeof(uint32_t), false);

        // ----- BUILD EXECUTOR -----
        std::vector<RuntimeArgSpec> runtime_args_spec{
                RuntimeArgSpec(0, sizeof(ether_addr)),
                RuntimeArgSpec(sizeof(ether_addr), sizeof(uint32_t)),
        };
        Action act_none;
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
        set_executor(PROC_FWD_ID, ACT_FWD_ID, Processor::PROC_ID_END, ACT_NONE_ID, Processor::PROC_ID_END,
                     runtime_args_spec);
        set_action(ACT_NONE_ID, act_none);
        set_action(ACT_FWD_ID, act_fwd);

        // ----- BUILD PROCESSOR -----
        set_processor(PROC_FWD_ID, Condition::is_valid(HDR_IP_ID),
                      NextStage::table(TAB_ARP_ID), NextStage::proc(Processor::PROC_ID_END));

        // ----- INSERT FLOW ENTRY -----
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
};
