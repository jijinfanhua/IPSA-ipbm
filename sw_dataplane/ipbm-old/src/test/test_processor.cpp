#include "test_common.h"
#include "dataplane/processor.h"
#include "dataplane/Global/global.h"

int main() {
    int ACT_NONE_ID = 0;
    int ACT_FWD_ID = 1;

    int HDR_ETHER_ID = 0;
    int HDR_IP_ID = 1;

    // Build tables
    int TAB_ARP_ID = 0;
    TestRouter::build_arp_table(TAB_ARP_ID, HDR_IP_ID, ACT_NONE_ID, ACT_FWD_ID);

    // Build global parser
    glb.parser = TestRouter::build_parser_ipv4(0, 1);

    // Build processors
    int PROC_FWD_ID = 0;
    auto &proc_fwd = glb.processors[PROC_FWD_ID];
    proc_fwd.executor = TestRouter::build_executor_forward(ACT_NONE_ID, ACT_FWD_ID, HDR_IP_ID, HDR_ETHER_ID);
    proc_fwd.true_next = NextStage::table(TAB_ARP_ID);
    proc_fwd.false_next = NextStage::proc(Processor::PROC_ID_END);
    proc_fwd.cond = Condition::is_valid(HDR_IP_ID);

    // Mock packet
    uint8_t packet[] = "\x00\x00\x00\x00\x01\x01\x00\x00\x00\x00\x01\x50\x08\x00"
                       "\x45\x00\x00\x14\x00\x01\x00\x00\x40\x00\x63\x86\x0a\x00\x01\x32\x0a\x00\x02\x32";
    int len = sizeof(packet);

    std::cout << "Input\n";
    print_packet(packet, len);

    // Start processing
    Global::process_packet(packet, len, 0);

    std::cout << "Output\n";
    print_packet(packet, len);
    std::cout << "Emit via port mask " << glb.meta_data.out_port << std::endl;
    assert(glb.meta_data.out_port == 0x02);
    return 0;
}
