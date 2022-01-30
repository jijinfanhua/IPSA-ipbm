#include <iostream>

//#include "sw-src/api_pb/api.pb.h"
//#include "sw-src/api_pb/api.grpc.pb.h"

#include "sw-src/dataplane/pipeline.h"
#include "sw-src/dataplane/physical_layer.h"

#include "sw-src/api/dataplane_server.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <unistd.h>
#include <thread>

DEFINE_string(ifconf, "../../sw-src/conf/switch.yml", "Interfaces");

#define PIPE

PHV * init_phv(uint8_t packet[], uint32_t recv_len, int if_index, Pipeline * pipe) {
    PHV *phv = new PHV();
    memcpy(phv->packet, packet, recv_len * sizeof(uint8_t));
    memset(phv->packet + FRONT_HEADER_LEN, 0, sizeof(uint8_t) * META_LEN);
    phv->cur_trans_fd_num = 1;
    phv->cur_state = 0;
    phv->cur_trans_key = 0xffff;

    phv->ig_if = if_index;

    int base_offset = FRONT_HEADER_LEN * 8;

    for(auto meta : pipe->get_metadata_header()) {
        phv->parsed_headers[meta->hdr_id]->hdr_id = meta->hdr_id;
        phv->parsed_headers[meta->hdr_id]->hdr_len = meta->hdr_len * 8;
        phv->parsed_headers[meta->hdr_id]->hdr_offset = meta->hdr_offset * 8 + FRONT_HEADER_LEN * 8;
        LOG(INFO) << "MataData: ";
        LOG(INFO) << "  header_id: " << phv->parsed_headers[meta->hdr_id]->hdr_id;
        LOG(INFO) << "  header_len: " << phv->parsed_headers[meta->hdr_id]->hdr_len;
        LOG(INFO) << "  header_offset: " << phv->parsed_headers[meta->hdr_id]->hdr_offset;
    }

//    phv->parsed_headers[31]->hdr_id = 31;
//    phv->parsed_headers[31]->hdr_offset = FRONT_HEADER_LEN * 8;

    // add the standard_metadata.ingress_port to phv
    FieldInfo ig_if_fd{31, 0, 9};
    Data data = {9, get_uints(if_index, 9)};
    modify_phv_with_data(ig_if_fd, data, phv);

//    int eg_if = 0;
//    if(if_index == 0) eg_if = 0b10;
//    else if(if_index == 1) eg_if = 0b01;
//    FieldInfo eg_if_fd{31, 9, 9};
//    Data data1 = {9, get_uints(eg_if, 9)};
//    modify_phv_with_data(eg_if_fd, data1, phv);

    return phv;
}

int main(int argc, char *argv[]) {

    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    net_config_init(FLAGS_ifconf);

    auto pipe = new Pipeline(PROC_NUM);

    init_mem();

    std::thread server_thread(api::RunServer, pipe);
    server_thread.detach();

    static const uint8_t MAC1[] = "\x00\x00\x00\x00\x01\x50";
    static const uint8_t MAC3[] = "\x00\x00\x00\x00\x02\x50";

    PhysicalLayer phy;
    FieldInfo fd {31, 9, 9};
    while(true) {
        uint8_t packet[BUFSIZ];
        int if_index;
        std::cout << "nonononono" << std::endl;
        uint32_t recv_len = phy.recv_packet(packet, -1, if_index);
        std::cout << "===== IN PACKET ======\n";
        std::cout << "Ingress interface index: " << if_index << "\n";
        std::cout << "Rx length: " << recv_len << "\n";
        print_packet(packet, recv_len);

        for(int i = 0; i < recv_len; i ++) {
            if(i % 12 == 0 && i != 0) {
                std::cout << std::endl;
            }
            std::cout << (int)packet[i] << "  ";
        }
        std::cout << std::endl;

#ifdef PIPE
        auto phv = init_phv(packet, recv_len, if_index, pipe);

        pipe->execute(phv);

        uint32_t out_port = get_field_value(fd, phv);
        std::cout << "egress interface port: " << out_port << "\n";
#endif

#ifndef PIPE
        uint32_t out_port = 0;
        if(if_index == 0) out_port = 2;
        if(if_index == 1) out_port = 1;
#endif
        try {

            if (out_port) {
                std::cout << "===== OUT PACKET ======\n";
                for (int if_idx = 0; if_idx < glb.num_if; if_idx++) {
                    if ((out_port >> if_idx) & 1) {
                        LOG(INFO) << "Emit via " << glb.if_names[if_idx];
                        static const uint8_t ZERO_MAC[] = "\x00\x00\x00\x00\x00\x00";
                        if (memcmp(&glb.if_macs[if_idx], ZERO_MAC, sizeof(ether_addr)) != 0) {
#ifdef PIPE
                            memcpy(packet, phv->packet, recv_len * sizeof(uint8_t));
                            print_packet(packet, recv_len);
#endif
                            auto eth_hdr = (ether_header *) packet;
                            memcpy(eth_hdr->ether_shost, &glb.if_macs[if_idx], sizeof(ether_addr));
//                        print_packet(packet, recv_len);
#ifndef PIPE
                            if(if_idx == 0) {
                                memcpy(eth_hdr->ether_dhost, MAC1, sizeof(ether_addr));
                            } else if(if_idx == 1) {
                                memcpy(eth_hdr->ether_dhost, MAC3, sizeof(ether_addr));
                            }
#endif
                        }

                    for(int i = 0; i < recv_len; i ++) {
                        if(i % 12 == 0 && i != 0) {
                            std::cout << std::endl;
                        }
                        std::cout << (int)packet[i] << "  ";
                    }
                    std::cout << std::endl;
                        std::cout << "Egress interface index: " << if_idx << "\n";
                        std::cout << "Tx length: " << recv_len << "\n";
                        print_packet(packet, recv_len);
                        phy.send_packet(if_idx, packet, recv_len);
                        std::cout << "send ok!" << std::endl;
                    }
                }
            } else {
                // standard_metadata.egress_port = 0; is the tag to drop packet
                LOG(INFO) << "Dropping packet";
//                break;
            }
        }
        catch (const std::runtime_error &e) {
                LOG(ERROR) << e.what();
        }

//        for(auto & parsed_header : phv->parsed_headers) {
//            delete parsed_header;
//        }
//        delete phv->match_value;
//        delete phv;  // auto delete
    }

    return 0;
}
