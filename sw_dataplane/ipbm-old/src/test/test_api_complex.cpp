#include "api/api_complex.h"

using namespace api;

int main() {
    ApiComplex::build();
    // TEST MAC FORWARDING
    {
        uint8_t packet[] = "\x00\x00\x00\x00\x01\x02\x00\x00\x00\x00\x01\x03\x90\x00";
        int len = sizeof(packet) - 1;
        Global::process_packet(packet, len, 1);
        assert(glb.meta_data.out_port == 0x01);
    }
    {
        uint8_t packet[] = "\x00\x00\x00\x00\x01\x04\x00\x00\x00\x00\x01\x02\x90\x00";
        int len = sizeof(packet) - 1;
        Global::process_packet(packet, len, 0);
        assert(glb.meta_data.out_port == 0x04);
    }
    {
        uint8_t packet[] = "\x00\x00\x00\x00\x99\x99\x00\x00\x00\x00\x01\x02\x90\x00";
        int len = sizeof(packet) - 1;
        Global::process_packet(packet, len, 0);
        assert(glb.meta_data.out_port == 0x0f);
    }
    // TEST IP FORWARDING
    {
        in_addr_t ip1, ip2;
        inet_aton("10.0.1.2", (in_addr *) &ip1);
        inet_aton("10.0.2.2", (in_addr *) &ip2);

        uint8_t packet[BUFSIZ];
        auto eth_hdr = (ether_header *) packet;
        *eth_hdr = {
                .ether_dhost = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01},
                .ether_shost = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02},
                .ether_type = htons(ETHERTYPE_IP)
        };
        auto ip_hdr = (iphdr *) (eth_hdr + 1);
        *ip_hdr = {.ihl = 5, .version = 4, .tos = 0, .tot_len = 0, .id = 0, .frag_off = 0,
                .ttl = 64, .protocol = 0, .check = 0, .saddr = ip1, .daddr = ip2};
        int len = sizeof(ether_header) + sizeof(iphdr);
        Global::process_packet(packet, len, 0);
        assert(glb.meta_data.out_port == 0x10);
        assert(ip_hdr->ttl == 63);
    }
    {
        in_addr_t ip1, ip2;
        inet_aton("10.0.2.2", (in_addr *) &ip1);
        inet_aton("10.0.1.2", (in_addr *) &ip2);

        uint8_t packet[BUFSIZ];
        auto eth_hdr = (ether_header *) packet;
        *eth_hdr = {
                .ether_dhost = {0x00, 0x00, 0x00, 0x00, 0x02, 0x01},
                .ether_shost = {0x00, 0x00, 0x00, 0x00, 0x02, 0x02},
                .ether_type = htons(ETHERTYPE_IP)
        };
        auto ip_hdr = (iphdr *) (eth_hdr + 1);
        *ip_hdr = {.ihl = 5, .version = 4, .tos = 0, .tot_len = 0, .id = 0, .frag_off = 0,
                .ttl = 64, .protocol = 0, .check = 0, .saddr = ip1, .daddr = ip2};
        int len = sizeof(ether_header) + sizeof(iphdr);
        Global::process_packet(packet, len, 4);
        assert(glb.meta_data.out_port == 0x01);
        assert(ip_hdr->ttl == 63);
    }
    ApiComplex::add_vlan();
    {
        uint8_t packet[] = "\x00\x00\x00\x00\x01\x03\x00\x00\x00\x00\x01\x02\x90\x00";
        int len = sizeof(packet) - 1;
        Global::process_packet(packet, len, 0);
        assert(glb.meta_data.out_port == 0x02);
    }
    {
        uint8_t packet[] = "\x00\x00\x00\x00\x01\x04\x00\x00\x00\x00\x01\x02\x90\x00";
        int len = sizeof(packet) - 1;
        Global::process_packet(packet, len, 0);
        assert(glb.meta_data.out_port == 0x03);
    }
    {
        uint8_t packet[] = "\x00\x00\x00\x00\x01\x03\x00\x00\x00\x00\x01\x05\x90\x00";
        int len = sizeof(packet) - 1;
        Global::process_packet(packet, len, 3);
        assert(glb.meta_data.out_port == (0x04 | 0x08));
    }
    return 0;
}
