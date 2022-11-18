#include "api/api_router.h"


int main() {
    ApiRouter::build();
    {
        // test parser with TCP packet
        uint8_t packet[] = "\xc0\xb4\x7d\x99\xa0\xa0\xc8\x58\xc0\xb5\xfe\x1e\x08\x00\x45\x00"
                           "\x00\x34\x04\x48\x40\x00\x40\x06\x59\x37\xc0\xa8\x03\x14\x67\x63"
                           "\xb2\x25\xd5\x42\x60\xf1\xc9\x11\xe5\xc9\x5e\x5e\x33\xb9\x80\x10"
                           "\x01\xf6\xdd\x6b\x00\x00\x01\x01\x08\x0a\x9e\xb4\x3f\xdd\x02\x1d"
                           "\x07\x53";
        int len = sizeof(packet) - 1;
        glb.parser.parse(packet, len);
        assert(!glb.hdrs[ApiRouter::HDR_ETHER_ID].empty());
        assert(!glb.hdrs[ApiRouter::HDR_IP_ID].empty());
    }
    {
        // test parser with ARP packet
        uint8_t packet[] = "\xff\xff\xff\xff\xff\xff\xc0\xb4\x7d\x99\xa0\xa0\x08\x06\x00\x01"
                           "\x08\x00\x06\x04\x00\x01\xc0\xb4\x7d\x99\xa0\xa0\xc0\xa8\x03\x01"
                           "\x00\x00\x00\x00\x00\x00\xc0\xa8\x03\x17";
        int len = sizeof(packet) - 1;
        glb.parser.parse(packet, len);
        assert(!glb.hdrs[ApiRouter::HDR_ETHER_ID].empty());
        assert(glb.hdrs[ApiRouter::HDR_IP_ID].empty());
    }

    {
        // test hit
        uint8_t packet[] = "\x00\x00\x00\x00\x01\x01\x00\x00\x00\x00\x01\x50\x08\x00"
                           "\x45\x00\x00\x14\x00\x01\x00\x00\x40\x00\x63\x86\x0a\x00\x01\x32\x0a\x00\x02\x32";
        int len = sizeof(packet) - 1;

        std::cout << "Input\n";
        print_packet(packet, len);

        // Start processing
        Global::process_packet(packet, len, 0);

        assert(!glb.hdrs[ApiRouter::HDR_ETHER_ID].empty());
        assert(!glb.hdrs[ApiRouter::HDR_IP_ID].empty());

        std::cout << "Output\n";
        print_packet(packet, len);

        assert(glb.meta_data.out_port == 0x02);
        std::cout << "Emit via port " << glb.meta_data.out_port << "\n\n";
    }
    {
        // test miss
        uint8_t packet[] = "\xc0\xb4\x7d\x99\xa0\xa0\xc8\x58\xc0\xb5\xfe\x1e\x08\x00\x45\x00"
                           "\x00\x34\x04\x48\x40\x00\x40\x06\x59\x37\xc0\xa8\x03\x14\x67\x63"
                           "\xb2\x25\xd5\x42\x60\xf1\xc9\x11\xe5\xc9\x5e\x5e\x33\xb9\x80\x10"
                           "\x01\xf6\xdd\x6b\x00\x00\x01\x01\x08\x0a\x9e\xb4\x3f\xdd\x02\x1d"
                           "\x07\x53";
        int len = sizeof(packet) - 1;

        std::cout << "Input\n";
        print_packet(packet, len);

        // Start processing
        Global::process_packet(packet, len, 0);

        assert(!glb.hdrs[ApiRouter::HDR_ETHER_ID].empty());
        assert(!glb.hdrs[ApiRouter::HDR_IP_ID].empty());

        std::cout << "Output\n";
        print_packet(packet, len);

        assert(glb.meta_data.out_port == 0);
        std::cout << "Drop packet\n\n";
    }
    return 0;
}
