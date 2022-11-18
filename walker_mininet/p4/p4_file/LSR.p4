/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

/***
        This is an LSR based on Simple Router.
*/


const bit<16> TYPE_MPLS = 0x8847;
/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

typedef bit<9>      ingress_port_t;
typedef bit<9>       egress_spec_t;
typedef bit<48>      mac_address_t;

header ethernet_t {
    mac_address_t      dst_address;
    mac_address_t      src_address;
    bit<16>             ether_type;
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
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser LSRParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.ether_type) {
            TYPE_MPLS: parse_mpls;
            default: accept;
        }
    }

    state parse_mpls {
        packet.extract(hdr.mpls);
        transition accept;
    }

}


control LSRVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}
/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/


control LSRIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action drop() {
        mark_to_drop(standard_metadata);
    }

    action forward(egress_spec_t port, bit<20> next_label) {
        standard_metadata.egress_spec = port;
        hdr.mpls.label = next_label;
    }
    table mpls_forward {
        key = {
            hdr.mpls.label : exact;
        }
        actions = {
            forward;
            drop;
        }
        size = 1024;
        default_action = drop();
    }

    apply {
        if (hdr.mpls.isValid()) {
            mpls_forward.apply();
        } else {
            drop();
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control LSREgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control LSRComputeChecksum(inout headers  hdr, inout metadata meta) {
    apply {  }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control LSRDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
    	packet.emit(hdr.mpls);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
LSRParser(),
LSRVerifyChecksum(),
LSRIngress(),
LSREgress(),
LSRComputeChecksum(),
LSRDeparser()
) main;
