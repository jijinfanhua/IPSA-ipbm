#include "test_common.h"
#include <net/ethernet.h>
#include <linux/ip.h>

int main() {
    auto parser = TestRouter::build_parser_ipv4(0, 1);
    {
        // ARP
        uint8_t packet[] = "\xff\xff\xff\xff\xff\xff\xc0\xb4\x7d\x99\xa0\xa0\x08\x06\x00\x01"
                           "\x08\x00\x06\x04\x00\x01\xc0\xb4\x7d\x99\xa0\xa0\xc0\xa8\x03\x01"
                           "\x00\x00\x00\x00\x00\x00\xc0\xa8\x03\x17";
        parser.parse(packet, sizeof(packet));
        assert(glb.hdrs[0].data == packet);
        assert(glb.hdrs[0].size == sizeof(ether_header));
        assert(glb.hdrs[1].empty());
    }
    {
        // DNS
        uint8_t packet[] = "\xc0\xb4\x7d\x99\xa0\xa0\xc8\x58\xc0\xb5\xfe\x1e\x08\x00\x45\x00"
                           "\x00\x3b\x74\x93\x40\x00\x40\x11\x3e\xb9\xc0\xa8\x03\x14\xc0\xa8"
                           "\x03\x01\xd8\x47\x00\x35\x00\x27\x87\x9e\x67\x74\x01\x00\x00\x01"
                           "\x00\x00\x00\x00\x00\x00\x03\x77\x77\x77\x05\x62\x61\x69\x64\x75"
                           "\x03\x63\x6f\x6d\x00\x00\x01\x00\x01";
        parser.parse(packet, sizeof(packet));
        assert(glb.hdrs[0].data == packet);
        assert(glb.hdrs[0].size == sizeof(ether_header));
        assert(glb.hdrs[1].data == packet + sizeof(ether_header));
        assert(glb.hdrs[1].size == sizeof(iphdr));
    }
    {
        // TCP
        uint8_t packet[] = "\xc0\xb4\x7d\x99\xa0\xa0\xc8\x58\xc0\xb5\xfe\x1e\x08\x00\x45\x00"
                           "\x00\x34\x04\x48\x40\x00\x40\x06\x59\x37\xc0\xa8\x03\x14\x67\x63"
                           "\xb2\x25\xd5\x42\x60\xf1\xc9\x11\xe5\xc9\x5e\x5e\x33\xb9\x80\x10"
                           "\x01\xf6\xdd\x6b\x00\x00\x01\x01\x08\x0a\x9e\xb4\x3f\xdd\x02\x1d"
                           "\x07\x53";
        parser.parse(packet, sizeof(packet));
        assert(glb.hdrs[0].data == packet);
        assert(glb.hdrs[0].size == sizeof(ether_header));
        assert(glb.hdrs[1].data == packet + sizeof(ether_header));
        assert(glb.hdrs[1].size == sizeof(iphdr));
    }
    return 0;
}
