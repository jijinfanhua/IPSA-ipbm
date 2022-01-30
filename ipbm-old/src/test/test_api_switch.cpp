#include "api/api_switch.h"

using namespace api;

int main() {
    ApiSwitch::build();
    {
        uint8_t packet[] = "\x00\x00\x00\x00\x01\x01\x00\x00\x00\x00\x01\x02\x90\x00";
        int len = sizeof(packet) - 1;
        Global::process_packet(packet, len, 0);
        assert(glb.meta_data.out_port == 0x01);
    }
    {
        uint8_t packet[] = "\x00\x00\x00\x00\x01\x03\x00\x00\x00\x00\x01\x02\x90\x00";
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
    return 0;
}
