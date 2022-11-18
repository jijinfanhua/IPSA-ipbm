#include "api/api_server.h"
#include "physical_layer.h"
#include "global.h"
#include "api/api_complex.h"
//#include "api/api_router.h"
//#include "api/api_switch.h"
//#include "api/api_tunnel.h"
//#include "api/api_counter.h"
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <thread>

DEFINE_string(ifconf, "../conf/router.yml", "Interface config file");

int main(int argc, char *argv[]) {
    // Start API server
    std::thread server_thread(RunServer);
    try {
        // Parse command line args
        google::InitGoogleLogging(argv[0]);
        gflags::ParseCommandLineFlags(&argc, &argv, true);
        LOG(INFO) << "Starting Reconfigurable Switch";

        config_init(FLAGS_ifconf);

        // Init as empty
        api::set_processor(0, Condition::is_valid(0),
                      NextStage::proc(Processor::PROC_ID_END), NextStage::proc(Processor::PROC_ID_END));

        // Build switch
//        ApiComplex::build();
//        ApiComplex::add_vlan();
//        ApiComplex::add_tunnel();
//        ApiComplex::add_counter();

//        ApiRouter::build();
//        ApiSwitch::build();
//        ApiTunnel::build();
//        ApiCounter::build();

        // Listen to all ports
        PhysicalLayer phy;
        while (true) {
            // Init packet meta data
            uint8_t packet[BUFSIZ];
            int if_index;
            uint32_t recv_len = phy.recv_packet(packet, -1, if_index);

            std::cout << "===== IN PACKET ======\n";
            print_packet(packet, recv_len);

            Global::process_packet(packet, recv_len, if_index);

            if (glb.meta_data.out_port) {
                std::cout << "===== OUT PACKET ======\n";
                print_packet(packet, recv_len);
                for (int if_idx = 0; if_idx < glb.num_if; if_idx++) {
                    if ((glb.meta_data.out_port >> if_idx) & 1) {
                        LOG(INFO) << "Emit via " << glb.if_names[if_idx];
                        static const uint8_t ZERO_MAC[] = "\x00\x00\x00\x00\x00\x00";
                        if (memcmp(&glb.if_macs[if_idx], ZERO_MAC, sizeof(ether_addr)) != 0) {
                            auto eth_hdr = (ether_header *) packet;
                            memcpy(eth_hdr->ether_shost, &glb.if_macs[if_idx], sizeof(ether_addr));
                        }
                        phy.send_packet(if_idx, packet, recv_len);
                    }
                }
            } else {
                LOG(INFO) << "Dropping packet";
            }
        }
    } catch (const std::runtime_error &e) {
        LOG(ERROR) << e.what();
    }
    return 0;
}
