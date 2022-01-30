#include "api/api_counter.h"
#include <net/ethernet.h>
#include <linux/ip.h>


int main() {
    ApiCounter::build();
    uint8_t packet[128];
    char ip1_s[] = "10.0.1.50";
    char ip2_s[] = "10.0.2.50";
    in_addr_t ip1, ip2;
    inet_aton(ip1_s, (in_addr *) &ip1);
    inet_aton(ip2_s, (in_addr *) &ip2);

    auto eth_hdr = (ether_header *) packet;
    *eth_hdr = {
            .ether_dhost = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01},
            .ether_shost = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02},
            .ether_type = htons(ETHERTYPE_IP)
    };
    auto ip_hdr = (iphdr *) (eth_hdr + 1);
    *ip_hdr = {.ihl = 5, .version = 4, .tos = 0, .tot_len = 0, .id = 0, .frag_off = 0,
            .ttl = 0, .protocol = 0, .check = 0, .saddr = ip1, .daddr = ip2,};
    int len = sizeof(ether_header) + sizeof(iphdr);
    {
        Global::process_packet(packet, len, 0);
        assert(!glb.hdrs[ApiCounter::HDR_ETHER_ID].empty());
        assert(!glb.hdrs[ApiCounter::HDR_IP_ID].empty());
        assert(glb.meta_data.out_port == 0x02);
        // Check counter value
        auto &tab = glb.flow_tables[ApiCounter::TAB_COUNTER_ID];
        int pos = tab.match_exact((Buffer) &ip1);
        assert(pos != tab.end_pos());
        assert(*(uint32_t *) tab.get_val(pos) == 1);
    }
    {
        Global::process_packet(packet, len, 0);
        assert(!glb.hdrs[ApiCounter::HDR_ETHER_ID].empty());
        assert(!glb.hdrs[ApiCounter::HDR_IP_ID].empty());
        assert(glb.meta_data.out_port == 0x02);
        // Check counter value
        auto &tab = glb.flow_tables[ApiCounter::TAB_COUNTER_ID];
        int pos = tab.match_exact((Buffer) &ip1);
        assert(pos != tab.end_pos());
        assert(*(uint32_t *) tab.get_val(pos) == 2);
    }
    return 0;
}