/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

/***
        This is a Simple Router Without L2 functions.
*/


const bit<16> TYPE_IPV4 = 0x0800;

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
    ipv4_t           ipv4;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser RouterParser(packet_in packet,
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
            default: accept;
        }
    }

    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition accept;
    }

}


control RouterVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}
/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/


control RouterIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action drop() {
        mark_to_drop(standard_metadata);
    }

    action forward(mac_address_t src_address, mac_address_t dst_address, egress_spec_t port) {
        standard_metadata.egress_spec = port;
        hdr.ethernet.src_address = src_address;
        hdr.ethernet.dst_address = dst_address;
        // hdr.ipv4.ttl = hdr.ipv4.ttl - 1;
    }

    table ip_forward {
        key = {
            hdr.ipv4.dst_address: lpm;
        }
        actions = {
            forward;
            drop;
        }
        size = 1024;
        default_action = drop();
    }

    table dest_check {
        key = {
            hdr.ethernet.dst_address: exact;
        }
        actions = {
            NoAction;
            drop;
        }
        size = 1024;
        default_action = drop();
    }

    apply {
        if (hdr.ipv4.isValid()) {
            if (dest_check.apply().hit) {
                ip_forward.apply();
            }
        } else {
            drop();
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control RouterEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control RouterComputeChecksum(inout headers  hdr, inout metadata meta) {
    apply {  }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control RouterDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.ipv4);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
RouterParser(),
RouterVerifyChecksum(),
RouterIngress(),
RouterEgress(),
RouterComputeChecksum(),
RouterDeparser()
) main;
