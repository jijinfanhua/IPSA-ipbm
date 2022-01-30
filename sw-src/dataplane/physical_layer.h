//
// Created by xilinx_0 on 1/7/22.
//

#ifndef RECONF_SWITCH_IPSA_PHYSICAL_LAYER_H
#define RECONF_SWITCH_IPSA_PHYSICAL_LAYER_H

#include <pcap/pcap.h>

#include "global.h"
#include "defs.h"

class PhysicalLayer{
public:
    pcap_t * pcap_handle[8]{};

    PhysicalLayer() {
        if(glb.if_names.empty()) {
            throw std::runtime_error("No interface specified.");
        }

        char error_buffer[256];
        for(int i = 0; i < glb.num_if; i++) {
            pcap_handle[i] = pcap_open_live(glb.if_names[i].c_str(), BUFSIZ, 1, 1, error_buffer);
            if(pcap_handle[i] == nullptr) {
                throw std::runtime_error("interface " + glb.if_names[i] + " not found or permission denied.");
            }
            pcap_setnonblock(pcap_handle[i], 1, error_buffer);
        }
    }

    static uint64_t get_ticks() {
        struct timespec tp = {0};
        clock_gettime(CLOCK_MONOTONIC, &tp);
        return (uint64_t) tp.tv_sec * 1000 + (uint64_t) tp.tv_nsec / 1000000;
    }

    uint64_t recv_packet(Buffer buffer, int64_t timeout, int &if_index) {
        int64_t begin = get_ticks();
        if_index = 0;

        // round robin
        do {
            struct pcap_pkthdr hdr;
            InputBuffer packet = pcap_next(pcap_handle[if_index], &hdr);
            if (packet) {
                memcpy(buffer, packet, hdr.caplen);
                return hdr.caplen;
            }
            if_index = (if_index + 1) % glb.num_if;
        } while(get_ticks() < (uint64_t) (begin + timeout) || timeout < 0);
        return 0;
    }

    int send_packet(int if_index, Buffer buffer, uint32_t len) {
        std::cout<< ">>>>>> in here" <<std::endl;
        return pcap_inject(pcap_handle[if_index], buffer, len);
    }
};

#endif //RECONF_SWITCH_IPSA_PHYSICAL_LAYER_H
