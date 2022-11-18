/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

/***
        This is an L3Switch.
*/


const bit<16> TYPE_IPV4 = 0x0800;
const bit<16> TYPE_ARP  = 0x0806;

/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

typedef bit<9>      ingress_port_t;
typedef bit<9>       egress_spec_t;
typedef bit<48>      mac_address_t;
typedef bit<32>      ip4_address_t;

header ethernet_t {
    mac_address_t      dst_address;
    mac_address_t      src_address;
    bit<16>             ether_type;
}

header arp_t {
    bit<16>              hard_type;
    bit<16>          protocol_type;
    bit<8>      mac_address_length;
    bit<8>       ip_address_length;
    bit<16>                     op;
    mac_address_t  src_mac_address;
    ip4_address_t   src_ip_address;
    mac_address_t  dst_mac_address;
    ip4_address_t   dst_ip_address;
}
header ipv4_t {
    bit<4>                 version;
    bit<4>                     ihl;
    bit<8>            type_service;
    bit<16>           total_length;
    bit<16>         identification;
    bit<3>                   flags;
    bit<13>        fragment_offset;
    bit<8>                     ttl;
    bit<8>                protocol;
    bit<16>        header_checksum;
    ip4_address_t      src_address;
    ip4_address_t      dst_address;
}

struct metadata {
    /* empty */
}

struct headers {
    ethernet_t   ethernet;
    arp_t             arp;
    ipv4_t           ipv4;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser L3SwitchParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.ether_type) {
            TYPE_IPV4: parse_ipv4;
            TYPE_ARP : parse_arp;
            default: accept;
        }
    }

    state parse_arp {
        packet.extract(hdr.arp);
        transition accept;
    }

    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition accept;
    }

}


control L3SwitchVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}
/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/


control L3SwitchIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action drop() {
        mark_to_drop(standard_metadata);
    }

    table dest_check {
        key = {
            hdr.ethernet.dst_address: exact;
        }
        actions = {
            NoAction;
        }
        size = 1024;
        default_action = NoAction();
    }

    action forward(mac_address_t src_address, mac_address_t dst_address, egress_spec_t port) {
        standard_metadata.egress_spec = port;
        hdr.ethernet.dst_address = dst_address;
        hdr.ethernet.src_address = src_address;
    }

    table ip_forward {
        key = {
            hdr.ipv4.dst_address : lpm;
        }
        actions = {
            forward;
            drop;
        }
        size = 1024;
        default_action = drop();
    }

    action arp_reply(mac_address_t self_mac_address, ip4_address_t self_ip_address) {
        hdr.ethernet.dst_address = hdr.ethernet.src_address;
        hdr.ethernet.src_address = self_mac_address;
        hdr.arp.op = 2;
        hdr.arp.dst_mac_address = hdr.arp.src_mac_address;
        hdr.arp.dst_ip_address  = hdr.arp.src_ip_address;
        hdr.arp.src_mac_address = self_mac_address;
        hdr.arp.src_ip_address  = self_ip_address;
    }

    table dest_ip_check {
        key = {
            hdr.arp.dst_ip_address : exact;
        }
        actions = {
            arp_reply;
            NoAction;
        }
        size = 1024;
        default_action = NoAction();
    }


    action L2_switch(egress_spec_t port) {
        standard_metadata.egress_spec = port;
    }

    table ethernet_switch {
        key = {
            hdr.ipv4.src_address : exact;
            hdr.ipv4.dst_address : exact;
            hdr.ethernet.dst_address : exact;
        }
        actions = {
            L2_switch;
            drop;
        }
        size = 1024;
        default_action = drop;
    }

    apply {
        if (dest_check.apply().hit) {
            if (hdr.ipv4.isValid()) {
                ip_forward.apply();
            }
        } else {
            if (hdr.arp.isValid() && hdr.arp.op == 1) {
                dest_ip_check.apply();
            }
            ethernet_switch.apply();
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control L3SwitchEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control L3SwitchComputeChecksum(inout headers  hdr, inout metadata meta) {
    apply {  }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control L3SwitchDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.arp);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
L3SwitchParser(),
L3SwitchVerifyChecksum(),
L3SwitchIngress(),
L3SwitchEgress(),
L3SwitchComputeChecksum(),
L3SwitchDeparser()
) main;
