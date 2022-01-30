#pragma once

#include "global.h"
#include <pcap/pcap.h>
#include <glog/logging.h>
#include <vector>
#include <string>
#include <iomanip>


std::string pkt2str(Buffer packet, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; i++) {
        oss << std::setw(2) << std::setfill('0') << std::hex << (uint32_t) packet[i] << ' ';
    }
    return oss.str();
}

class PhysicalLayer {
public:
    pcap_t *pcap_handle[MAX_IF_NUM];

    PhysicalLayer() {
        if (glb.if_names.empty()) {
            throw std::runtime_error("No interface specified");
        }

        // init pcap handles
        char error_buffer[PCAP_ERRBUF_SIZE];
        for (int i = 0; i < glb.num_if; i++) {
            pcap_handle[i] = pcap_open_live(glb.if_names[i].c_str(), BUFSIZ, 1, 1, error_buffer);
            if (pcap_handle[i] == nullptr) {
                throw std::runtime_error("interface " + glb.if_names[i] + " not found or permission denied");
            }
            pcap_setnonblock(pcap_handle[i], 1, error_buffer);
        }
    }

    static uint64_t get_ticks() {
        struct timespec tp = {0};
        clock_gettime(CLOCK_MONOTONIC, &tp);
        // millisecond
        return (uint64_t) tp.tv_sec * 1000 + (uint64_t) tp.tv_nsec / 1000000;
    }

    uint32_t recv_packet(Buffer buffer, int64_t timeout, int &if_index) {
        int64_t begin = get_ticks();
        // Round robin
        if_index = 0;
        do {
            struct pcap_pkthdr hdr;
            InputBuffer packet = pcap_next(pcap_handle[if_index], &hdr);
            if (packet) {
                memcpy(buffer, packet, hdr.caplen);
                return hdr.caplen;
            }
            if_index = (if_index + 1) % glb.num_if;
        } while (get_ticks() < (uint64_t) (begin + timeout) || timeout < 0);
        return 0;
    }

    int send_packet(int if_index, Buffer buffer, uint32_t len) {
        return pcap_inject(pcap_handle[if_index], buffer, len);
    }
};
