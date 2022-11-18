#include "test_common.h"
#include "dataplane/action.h"
#include "dataplane/Parser/Parser.h"
#include <linux/ip.h>

static const uint16_t TYPE_IP = htons(ETHERTYPE_IP);

int main() {
    int HDR_ETHER_ID = 0;
    int HDR_IP_ID = 1;
    Parser parser = TestRouter::build_parser_ipv4(HDR_ETHER_ID, HDR_IP_ID);

    // TCP packet
    uint8_t packet[] = "\xc0\xb4\x7d\x99\xa0\xa0\xc8\x58\xc0\xb5\xfe\x1e\x08\x00\x45\x00"
                       "\x00\x34\x04\x48\x40\x00\x40\x06\x59\x37\xc0\xa8\x03\x14\x67\x63"
                       "\xb2\x25\xd5\x42\x60\xf1\xc9\x11\xe5\xc9\x5e\x5e\x33\xb9\x80\x10"
                       "\x01\xf6\xdd\x6b\x00\x00\x01\x01\x08\x0a\x9e\xb4\x3f\xdd\x02\x1d"
                       "\x07\x53";
    parser.parse(packet, sizeof(packet));

    auto ip_packet = (iphdr *) glb.hdrs[1].data;
    Action action;
    action.ops = {
            Primitive::add(FieldSpec(1, offsetof(iphdr, ttl), sizeof(uint8_t)), -1),
            Primitive::cksum(FieldSpec(1, 0, sizeof(iphdr)),
                             FieldSpec(1, offsetof(iphdr, check), sizeof(uint16_t)),
                             HASH_CSUM16)
    };
    std::cout << std::hex;
    std::cout << "Prev: ttl=" << (int) ip_packet->ttl << " cksum=" << (int) ip_packet->check << std::endl;
    uint8_t prev_ttl = ip_packet->ttl;
    uint16_t prev_cksum = ip_packet->check;

    // Execute
    int ACT_ID = 0;
    Executor executor = Executor(ACT_ID, Processor::PROC_ID_END, ACT_ID, Processor::PROC_ID_END, {});
    glb.actions[ACT_ID] = action;
    executor.execute(nullptr);

    uint8_t curr_ttl = ip_packet->ttl;
    uint16_t curr_cksum = ip_packet->check;
    std::cout << "Curr: ttl=" << (int) ip_packet->ttl << " cksum=" << (int) ip_packet->check << std::endl;
    assert(prev_ttl - 1 == curr_ttl);
    assert(prev_cksum + 1 == curr_cksum);
    return 0;
}
