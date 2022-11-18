/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

/***
        This is a MPLS Edge Router that based on the Simple Router,
        which is push / pop the label in MPLS to make sure that the packet can be forward correctly.
*/


const bit<16> TYPE_IPV4 = 0x0800;
const bit<16> TYPE_MPLS = 0x8847;

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


header mpls_t {
    bit<20> label;
    bit<3>  exp;
    bit<1>  bos;
    bit<8>  ttl;
}

struct metadata {
    /* empty */
}

struct headers {
    ethernet_t   ethernet;
    mpls_t           mpls;
    ipv4_t           ipv4;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser LERParser(packet_in packet,
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
            TYPE_MPLS: parse_mpls;
            default: accept;
        }
    }

    state parse_mpls {
        packet.extract(hdr.mpls);
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


control LERVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}
/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/


control LERIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action drop() {
        mark_to_drop(standard_metadata);
    }

    action push_label(bit<20> label) {
        // hdr.mpls.setValid();
        hdr.mpls.label = label;
        hdr.mpls.bos = 1;
        hdr.mpls.exp = 0;
        hdr.mpls.ttl = 64;
        hdr.ether_type = TYPE_MPLS;
    }

    table mpls_push {
        key = {
            hdr.ipv4.dst_address : exact;
        }
        actions = {
            push_label;
            drop;
        }
        size = 1024;
        default_action = drop();
    }

    action mpls_pop() {
        //hdr.mpls.setValid();
        hdr.ethernet.ether_type = TYPE_IPV4;
    }

    apply {
        if (standard_metadata.ingress_port == 0) {
            if (hdr.ipv4.isValid()) {
                mpls_push.apply();
            }
        } else if (hdr.mpls.isValid()) {
                mpls_pop();
            }
        } else {
            drop();
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control LEREgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control LERComputeChecksum(inout headers  hdr, inout metadata meta) {
    apply {  }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control LERDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.ipv4);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
LERParser(),
LERVerifyChecksum(),
LERIngress(),
LEREgress(),
LERComputeChecksum(),
LERDeparser()
) main;
