#include "ctrl_complex.h"
#include <chrono>
#include <thread>

using namespace api;

int main(int argc, char **argv) {
    ModClient rp4_client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    for (int i = 0; i < 10000; i++) {
        FlowEntry flow1;
        in_addr_t ip1;
        char ip1_s[] = "10.0.1.2";
        inet_aton(ip1_s, (in_addr *) &ip1);

        std::string key((char *) &ip1, sizeof(in_addr_t));
        auto count_s = rp4_client.get_flow_entry_value(CtrlFwd::TAB_COUNTER_ID, key);
        uint32_t count_val = *(uint32_t *) count_s.c_str();
        std::cout << "SRC " << ip1_s << " COUNT " << count_val << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
