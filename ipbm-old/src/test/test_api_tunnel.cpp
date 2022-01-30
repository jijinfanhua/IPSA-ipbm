#include "api/api_tunnel.h"
#include <net/ethernet.h>
#include <linux/ip.h>

using namespace api;

int main() {
    ApiTunnel::build();
    uint8_t packet[128];
    char ip1_s[] = "10.0.1.50";
    char ip2_s[] = "10.0.2.50";
    in_addr_t ip1, ip2;
    inet_aton(ip1_s, (in_addr *) &ip1);
    inet_aton(ip2_s, (in_addr *) &ip2);
    {
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
        Global::process_packet(packet, len ,0);
        assert(!glb.hdrs[ApiTunnel::HDR_ETHER_ID].empty());
        assert(!glb.hdrs[ApiTunnel::HDR_IP_ID].empty());
        assert(glb.hdrs[ApiTunnel::HDR_TUNNEL_ID].empty());
        assert(glb.meta_data.out_port == 0x02);
    }
    {
        auto eth_hdr = (ether_header *) packet;
        *eth_hdr = {
                .ether_dhost = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01},
                .ether_shost = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02},
                .ether_type = htons(ETHERTYPE_TUNNEL),
        };
        auto tun_hdr = (TunnelHdr *) (eth_hdr + 1);
        *tun_hdr = {
                .proto = htons(ETHERTYPE_IP),
                .dst_id = 0,
        };
        auto ip_hdr = (iphdr *) (tun_hdr + 1);
        *ip_hdr = {.ihl = 5, .version = 4, .tos = 0, .tot_len = 0, .id = 0, .frag_off = 0,
                .ttl = 0, .protocol = 0, .check = 0, .saddr = ip1, .daddr = ip2,};
        int len = sizeof(ether_header) + sizeof(TunnelHdr) + sizeof(iphdr);
        Global::process_packet(packet, len, 0);
        assert(!glb.hdrs[ApiTunnel::HDR_ETHER_ID].empty());
        assert(!glb.hdrs[ApiTunnel::HDR_TUNNEL_ID].empty());
        assert(!glb.hdrs[ApiTunnel::HDR_IP_ID].empty());
        assert(glb.meta_data.out_port == 0x01);
    }
    {
        uint8_t packet[] = "\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\x01\x50\x12\x12\x08\x00\x00\x01"
                           "\x45\x00\x00\x1f\x00\x01\x00\x00\x40\x00\x63\x7b\x0a\x00\x01\x32\x0a\x00\x02\x32\x68\x65\x6c\x6c\x6f\x20\x77\x6f\x72\x6c\x64";
        int len = sizeof(packet) - 1;
        Global::process_packet(packet, len, 0);
        assert(!glb.hdrs[ApiTunnel::HDR_ETHER_ID].empty());
        assert(!glb.hdrs[ApiTunnel::HDR_TUNNEL_ID].empty());
        assert(!glb.hdrs[ApiTunnel::HDR_IP_ID].empty());
        assert(glb.meta_data.out_port == 0x02);
    }

    return 0;
}