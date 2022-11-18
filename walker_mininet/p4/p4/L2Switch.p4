/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>
/***
        This is an L2 Switch.
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

};

struct metadata {
    /* empty */
}

struct headers {
    ethernet_t   ethernet;

}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser L2SwitchParser(
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


control L2SwitchVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}

/*************************************************************************
**************             L 2 S W I T C H             *******************
*************************************************************************/

control L2SwitchIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {
    action flooding() {
        standard_metadata.egress_spec = 0x1ff;
    }

    action switching(egress_spec_t port) {
        standard_metadata.egress_spec = port;
    }

    table ethernet_switch {
        key = {
            hdr.ethernet.dst_address: exact;
        }
        actions = {
            switching;
            flooding;
        }
        size = 1024;
        default_action = flooding();
    }

    apply {
        ethernet_switch.apply();
    }
}

control L2SwitchEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}


control L2SwitchComputeChecksum(inout headers  hdr, inout metadata meta) {
    apply {  }
}

control L2SwitchDeparser (packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
L2SwitchParser(),
L2SwitchVerifyChecksum(),
L2SwitchIngress(),
L2SwitchEgress(),
L2SwitchComputeChecksum(),
L2SwitchDeparser()
) main;
