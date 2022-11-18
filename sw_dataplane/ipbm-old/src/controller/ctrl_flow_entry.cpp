#include "api/api_client.h"
#include "api/api_router.h"
#include <chrono>
#include <thread>

int main(int argc, char **argv) {
    ModClient rp4_client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    FlowEntry flow1;
    in_addr_t ip1;
    uint32_t port1 = 0x01;
    inet_aton("10.0.1.50", (in_addr *) &ip1);
    uint8_t mac1[] = "\x00\x00\x00\x00\x01\x50";
    flow1.key = std::string((char *) &ip1, sizeof(in_addr_t));
    flow1.action_id = ApiRouter::ACT_FWD_ID;
    flow1.action_data = {
            std::string((char *) mac1, sizeof(ether_addr)),
            std::string((char *) &port1, sizeof(uint32_t))
    };
    rp4_client.del_flow_entry(ApiRouter::TAB_ARP_ID, flow1);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    rp4_client.add_flow_entry(ApiRouter::TAB_ARP_ID, flow1);

    return 0;
}
