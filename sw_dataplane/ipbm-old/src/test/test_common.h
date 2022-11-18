#pragma once

#include "dataplane/Global/global.h"
#include <cassert>

class TestRouter {
public:
    static Executor build_executor_forward(int act_none_id, int act_fwd_id, int hdr_ip_id, int hdr_ether_id) {
        Executor executor(act_fwd_id, Processor::PROC_ID_END, act_none_id, Processor::PROC_ID_END,
                          std::vector<RuntimeArgSpec>{
                                  RuntimeArgSpec(0, sizeof(ether_addr)),
                                  RuntimeArgSpec(sizeof(ether_addr), sizeof(uint32_t))
                          });
        glb.actions[act_none_id].ops = {};
        glb.actions[act_fwd_id].ops = {
                Primitive::add(FieldSpec(hdr_ip_id, offsetof(iphdr, ttl), sizeof(uint8_t)), -1),
                Primitive::cksum(FieldSpec(hdr_ip_id, 0, sizeof(iphdr)),
                                 FieldSpec(hdr_ip_id, offsetof(iphdr, check), sizeof(uint16_t)),
                                 HASH_CSUM16),
                Primitive::copy_field(
                        FieldSpec(hdr_ether_id, offsetof(ether_header, ether_shost), sizeof(ether_addr)),
                        FieldSpec(hdr_ether_id, offsetof(ether_header, ether_dhost), sizeof(ether_addr))),
                Primitive::set_field(
                        FieldSpec(hdr_ether_id, offsetof(ether_header, ether_dhost), sizeof(ether_addr)), 0),
                Primitive::set_port_from_arg(1),
        };
        return executor;
    }

    static Parser build_parser_ipv4(int hdr_ether_id, int hdr_ip_id) {
        assert(hdr_ether_id == 0);
        assert(hdr_ip_id == 1);
        uint16_t TYPE_IP = htons(ETHERTYPE_IP);
        ParserHeader hdr_ether(hdr_ether_id, sizeof(ether_header), offsetof(ether_header, ether_type), sizeof(uint16_t),
                               {
                                       {std::string((char *) &TYPE_IP, sizeof(TYPE_IP)), hdr_ip_id}
                               });
        ParserHeader hdr_ip(hdr_ip_id, sizeof(iphdr), 0, 0, {});
        Parser parser;
        parser.parse_hdrs[hdr_ether_id] = hdr_ether;
        parser.parse_hdrs[hdr_ip_id] = hdr_ip;
        return parser;
    }

    static void build_arp_table(int tab_arp_id, int hdr_ip_id, int act_none_id, int act_fwd_id) {
        int arp_val_len = sizeof(ether_addr) + sizeof(uint32_t);
        Processor proc;
        uint8_t tag = 0;
        FlowTable tab_arp(
                tag, arp_val_len, glb.logic_tables[0],
                MEM_SRAM, MATCH_EXACT, {FieldSpec(hdr_ip_id, offsetof(iphdr, daddr), sizeof(in_addr_t))}, false
        );
        {
            in_addr arp_ip;
            inet_aton("10.0.1.50", &arp_ip);
            char arp_val[10] = "\x00\x00\x00\x00\x01\x50";  // mac addr
            *(int *) (arp_val + sizeof(ether_addr)) = 0x01; // port mask
            tab_arp.insert((Buffer) &arp_ip, (Buffer) arp_val);
        }
        {
            in_addr arp_ip;
            inet_aton("10.0.2.50", &arp_ip);
            char arp_val[10] = "\x00\x00\x00\x00\x02\x50";  // mac addr
            *(int *) (arp_val + sizeof(ether_addr)) = 0x02; // port mask
            tab_arp.insert((Buffer) &arp_ip, (Buffer) arp_val);
        }
        glb.flow_tables[tab_arp_id] = tab_arp;
    }
};
