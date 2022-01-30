#pragma once

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/ip.h>
#include <cinttypes>
#include <iostream>

typedef uint8_t *Buffer;
typedef const uint8_t *InputBuffer;

struct BufView {
    Buffer data;
    int size;

    BufView(Buffer data_ = nullptr, int size_ = 0) : data(data_), size(size_) {}

    void clear() {
        data = nullptr;
        size = 0;
    }

    bool empty() const { return size == 0; }
};

static inline std::string mac2str(uint8_t mac[6]) {
    char buf[32];
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return buf;
}

static inline void print_packet(uint8_t *packet, int len) {
    if (len < (int) sizeof(ethhdr)) {
        std::cout << "Broken ethernet packet\n";
        return;
    }
    auto eth_hdr = (ether_header *) packet;
    std::cout << "Ether: " << mac2str(eth_hdr->ether_shost) << " -> " << mac2str(eth_hdr->ether_dhost) << std::endl;
    if (eth_hdr->ether_type == htons(ETHERTYPE_IP)) {
        auto ip_packet = packet + sizeof(ether_header);
        int ip_len = len - (int) sizeof(ether_header);
        if (ip_len < (int) sizeof(iphdr)) {
            std::cout << "Broken IP packet\n";
            return;
        }
        auto ip_hdr = (iphdr *) ip_packet;
        std::cout << "IP: " << inet_ntoa({ip_hdr->saddr}) << " -> " << inet_ntoa({ip_hdr->daddr})
                  << " ttl=" << (int) ip_hdr->ttl << " cksum=" << ip_hdr->check << std::endl;
    } else {
        std::cout << "Unknown ether type " << (int) eth_hdr->ether_type << std::endl;
    }
}

static const int NUM_FLOW_TABLES = 32;
static const int NUM_HDRS = 32;
static const int NUM_PROC = 32;
static const int NUM_ACTIONS = 32;
static const int MAX_IF_NUM = 8;
