/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

/***
        This is a VlanSwitch.
*/


const bit<16> TYPE_IPV4 = 0x0800;
const bit<16> TYPE_VLAN = 0x8100;
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

header vlan_t {
    bit<3> pri;
    bit<1> cfi;
    bit<12> vid;
    bit<16> vlan_type;
}

struct metadata {
    /* empty */
}

struct headers {
    ethernet_t   ethernet;
    vlan_t           vlan;
    ipv4_t           ipv4;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser VlanSwitchParser(packet_in packet,
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
            TYPE_VLAN: parse_vlan;
        }
    }
    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition accept;
    }

    state parse_vlan {
        packet.extract(hdr.vlan);
        transition select(hdr.vlan.vlan_type) {
            TYPE_IPV4: parse_ipv4;
            default  : accept;
        }
    }

}


control VlanSwitchVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}
/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/


control VlanSwitchIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action drop() {
        mark_to_drop(standard_metadata);
    }

    action add_tag(bit<12> vid) {
        // hdr.vlan.setValid();
        hdr.vlan.vid = vid;
        hdr.vlan.pri = 0;
        hdr.vlan.cfi = 0;
        hdr.vlan.vlan_type = hdr.ethernet.ether_type;
        hdr.ethernet.ether_type = TYPE_VLAN;
    }

    table access_in_check {
        key = {
            standard_metadata.ingress_port : exact;
        }
        actions = {
            add_tag;
            NoAction;
        }
        size = 1024;
        default_action = NoAction();
    }


    table dest_mac_check {
        key = {
            hdr.ethernet.dst_address : exact;
        }
        actions = {
            NoAction;
        }
        size = 1024;
        default_action = NoAction();
    }


    action forward(mac_address_t src_address, mac_address_t dst_address, egress_spec_t port, bit<12> vid) {
        standard_metadata.egress_spec = port;
        hdr.ethernet.dst_address = dst_address;
        hdr.ethernet.src_address = src_address;
        hdr.vlan.vid = vid;
    }

    table ipv4_forward {
        key = {
            hdr.ipv4.dst_address : exact;
        }
        actions = {
            forward;
            drop;
        }
        size = 1024;
        default_action = drop();
    }


    action vlan_switch(egress_spec_t port) {
        standard_metadata.egress_spec = port;
    }



    table vlan_forward {
        key = {
            hdr.vlan.vid : exact;
            hdr.ethernet.dst_address : exact;
        }
        actions = {
            vlan_switch;
            drop;
        }
        size = 1024;
        default_action = drop();
    }


    action del_tag() {
        // del_vlan();
    }

    table access_out_check {
        key = {
                standard_metadata.egress_spec : exact;
        }
        actions = {
                del_tag;
                NoAction;
        }
        size = 1024;
        default_action = NoAction();
    }


    apply {
        if (!hdr.ipv4.isValid()) {
            drop();
        } else {
            access_in_check.apply();
            if (!hdr.vlan.isValid()) {
                drop();
            } else {
                if (dest_mac_check.apply().hit) {
                    ipv4_forward.apply();
                } else {
                    vlan_forward.apply();
                }
            }
            access_out_check.apply();
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control VlanSwitchEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control VlanSwitchComputeChecksum(inout headers  hdr, inout metadata meta) {
    apply {  }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control VlanSwitchDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.vlan);
        packet.emit(hdr.ipv4);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
VlanSwitchParser(),
VlanSwitchVerifyChecksum(),
VlanSwitchIngress(),
VlanSwitchEgress(),
VlanSwitchComputeChecksum(),
VlanSwitchDeparser()
) main;
