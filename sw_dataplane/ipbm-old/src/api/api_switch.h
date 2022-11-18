#pragma once

#include "api/api.h"

using namespace api;

class ApiSwitch {
public:
    static const int HDR_ETHER_ID = 0;
    static const int PROC_FWD_ID = 0;

    static void build() {
        // ----- BUILD VLAN TABLE -----
        for (int i = 0; i < 4; i++) {
            glb.vlan_group.port_masks[i] = 0x0f;
        }
        // ----- BUILD PARSER -----
        ParserHeader hdr_ether(
                HDR_ETHER_ID, sizeof(ether_header), offsetof(ether_header, ether_type), sizeof(uint16_t), {});
        assert(add_parser_header({}, {}, hdr_ether) == 0);

        // ----- BUILD FLOW TABLE -----
        int ACT_L2_BCAST_ID = 0;
        int ACT_L2_FWD_ID = 1;
        int TAB_MAC_ID = 0;
        set_flow_table(TAB_MAC_ID, 0, MEM_SRAM, MATCH_EXACT,
                       {FieldSpec(HDR_ETHER_ID, offsetof(ether_header, ether_dhost), sizeof(ether_addr))},
                       sizeof(uint32_t), false);

        // ----- BUILD EXECUTOR -----
        std::vector<RuntimeArgSpec> runtime_args_spec{
                RuntimeArgSpec(0, sizeof(uint32_t))
        };

        Action act_bcast({Primitive::set_multicast()});
        Action act_fwd({Primitive::set_port_from_arg(0)});
        set_executor(PROC_FWD_ID, ACT_L2_FWD_ID, Processor::PROC_ID_END, ACT_L2_BCAST_ID, Processor::PROC_ID_END,
                     runtime_args_spec);
        set_action(ACT_L2_BCAST_ID, act_bcast);
        set_action(ACT_L2_FWD_ID, act_fwd);

        // ----- BUILD PROCESSOR -----
        set_processor(PROC_FWD_ID, Condition::is_valid(HDR_ETHER_ID),
                      NextStage::table(TAB_MAC_ID), NextStage::proc(Processor::PROC_ID_END));

        // ----- INSERT FLOW ENTRY -----
        {
            FlowEntry flow1;
            uint32_t port1 = 0x01;
            uint8_t mac1[] = "\x00\x00\x00\x00\x01\x01";
            flow1.key = std::string((char *) mac1, sizeof(ether_addr));
            flow1.action_id = ACT_L2_FWD_ID;    // no effect
            flow1.action_data = {
                    std::string((char *) &port1, sizeof(uint32_t))
            };
            assert(add_flow_entry(MEM_SRAM, TAB_MAC_ID, flow1) == 0);
            auto flow1_buf = get_flow_entry_val(TAB_MAC_ID, (Buffer) flow1.key.c_str());
            assert(*(uint32_t *) flow1_buf == port1);
        }
        {
            FlowEntry flow2;
            uint32_t port2 = 0x02;
            uint8_t mac2[] = "\x00\x00\x00\x00\x01\x02";
            flow2.key = std::string((char *) mac2, sizeof(ether_addr));
            flow2.action_id = ACT_L2_FWD_ID;    // no effect
            flow2.action_data = {
                    std::string((char *) &port2, sizeof(uint32_t))
            };
            assert(add_flow_entry(MEM_SRAM, TAB_MAC_ID, flow2) == 0);
        }
        {
            FlowEntry flow3;
            uint32_t port3 = 0x04;
            uint8_t mac3[] = "\x00\x00\x00\x00\x01\x03";
            flow3.key = std::string((char *) mac3, sizeof(ether_addr));
            flow3.action_id = ACT_L2_FWD_ID;    // no effect
            flow3.action_data = {
                    std::string((char *) &port3, sizeof(uint32_t))
            };
            assert(add_flow_entry(MEM_SRAM, TAB_MAC_ID, flow3) == 0);
        }
        {
            FlowEntry flow4;
            uint32_t port4 = 0x08;
            uint8_t mac4[] = "\x00\x00\x00\x00\x01\x04";
            flow4.key = std::string((char *) mac4, sizeof(ether_addr));
            flow4.action_id = ACT_L2_FWD_ID;    // no effect
            flow4.action_data = {
                    std::string((char *) &port4, sizeof(uint32_t))
            };
            assert(add_flow_entry(MEM_SRAM, TAB_MAC_ID, flow4) == 0);
        }
    }
};
