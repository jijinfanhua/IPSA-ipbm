/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>
/***
        This is a Vlan Edge Switch.
*/

/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

typedef bit<9>       egress_spec_t;
typedef bit<48>      mac_address_t;

header ethernet_t {
    mac_address_t      dst_address;
    mac_address_t      src_address;
    bit<16>             ether_type;
}

header vlan_t {
    bit<3> pri;
    bit<1> cfi;
    bit<12> vid;
    bit<16> vlan_type;
};

struct metadata {
    /* empty */
}

struct headers {
    ethernet_t   ethernet;
    vlan_t           vlan;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser VlanEdgeParser(
                packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition accept;
    }

}


control VlanEdgeVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}

/*************************************************************************
**************             L 2 S W I T C H             *******************
*************************************************************************/

control VlanEdgeIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {



    action add_tag() {

    }

    action remove_tag() {

    }

    apply {
        if (standard_metadata.ingress_port == 0) {
            add_tag.apply();
        } else {
            del_tag.apply();
        }
    }
}

control VlanEdgeEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}


control VlanEdgeComputeChecksum(inout headers  hdr, inout metadata meta) {
    apply {  }
}

control VlanEdgeDeparser (packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
VlanEdgeParser(),
VlanEdgeVerifyChecksum(),
VlanEdgeIngress(),
VlanEdgeEgress(),
VlanEdgeComputeChecksum(),
VlanEdgeDeparser()
) main;
