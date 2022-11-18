/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

const bit<16> TYPE_IPV4 = 0x800;
typedef bit<48> mac_addr_t;
typedef bit<32> ipv4_addr_t;
typedef bit<128> ipv6_addr_t;
typedef bit<12> vlan_id_t;

typedef bit<16> ether_type_t;
const ether_type_t ETHERTYPE_IPV4 = 16w0x0800;
const ether_type_t ETHERTYPE_ARP = 16w0x0806;
const ether_type_t ETHERTYPE_VLAN = 16w0x8100;
const ether_type_t ETHERTYPE_IPV6 = 16w0x86dd;

typedef bit<8> ip_protocol_t;
const ip_protocol_t IP_PROTOCOLS_ICMP = 1;
const ip_protocol_t IP_PROTOCOLS_IPV4 = 4;
const ip_protocol_t IP_PROTOCOLS_TCP = 6;
const ip_protocol_t IP_PROTOCOLS_UDP = 17;
const ip_protocol_t IP_PROTOCOLS_IPV6 = 41;
// const ip_protocol_t IP_PROTOCOLS_SRV6 = 43;
// const ip_protocol_t IP_PROTOCOLS_NONXT = 59;

typedef bit<16> bd_t;
typedef bit<16> vrf_t;
typedef bit<16> nexthop_t;
typedef bit<16> ifindex_t;

typedef bit<8> bypass_t;
const bypass_t BYPASS_L2 = 8w0x01;
const bypass_t BYPASS_L3 = 8w0x02;
const bypass_t BYPASS_ACL = 8w0x04;
// Add more bypass flags here.
const bypass_t BYPASS_ALL = 8w0xff;
#define BYPASS(t) (ig_md.bypass & BYPASS_##t != 0)

/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

typedef bit<9>  egressSpec_t;
typedef bit<48> macAddr_t;
typedef bit<32> ip4Addr_t;

header ethernet_t {
    mac_addr_t dst_addr;
    mac_addr_t src_addr;
    bit<16> ether_type;
}

header vlan_tag_t {
    bit<3> pcp;
    bit<1> cfi;
    vlan_id_t vid;
    bit<16> ether_type;
}

header ipv4_t {
    bit<4> version;
    bit<4> ihl;
    bit<8> diffserv;
    bit<16> total_len;
    bit<16> identification;
    bit<3> flags;
    bit<13> frag_offset;
    bit<8> ttl;
    bit<8> protocol;
    bit<16> hdr_checksum;
    ipv4_addr_t src_addr;
    ipv4_addr_t dst_addr;
}

header ipv6_t {
    bit<4> version;
    bit<8> traffic_class;
    bit<20> flow_label;
    bit<16> payload_len;
    bit<8> next_hdr;
    bit<8> hop_limit;
    ipv6_addr_t src_addr;
    ipv6_addr_t dst_addr;
}

header tcp_t {
    bit<16> src_port;
    bit<16> dst_port;
    bit<32> seq_no;
    bit<32> ack_no;
    bit<4> data_offset;
    bit<4> res;
    bit<8> flags;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgent_ptr;
}

header udp_t {
    bit<16> src_port;
    bit<16> dst_port;
    bit<16> hdr_length;
    bit<16> checksum;
}

header icmp_t {
    bit<8> type_;
    bit<8> code;
    bit<16> hdr_checksum;
}

struct metadata_t {
    // bool checksum_err;
    bd_t bd;
    vrf_t vrf;
    nexthop_t nexthop;
    ifindex_t ifindex;
    ifindex_t egress_ifindex;
    bypass_t bypass;
}

struct header_t {
    ethernet_t      ethernet;
    vlan_tag_t      vlan_tag;
    ipv4_t          ipv4;
    ipv6_t          ipv6;
    tcp_t           tcp;
    udp_t           udp;
}

struct lookup_fields_t {
    mac_addr_t mac_src_addr;
    mac_addr_t mac_dst_addr;
    bit<16> mac_type;

    bit<4> ip_version;
    bit<8> ip_proto;
    bit<8> ip_ttl;
    bit<8> ip_dscp;

    bit<20> ipv6_flow_label;
    ipv4_addr_t ipv4_src_addr;
    ipv4_addr_t ipv4_dst_addr;
    ipv6_addr_t ipv6_src_addr;
    ipv6_addr_t ipv6_dst_addr;
}

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser MyParser(packet_in pkt,
                out header_t hdr,
                inout metadata_t meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        pkt.extract(hdr.ethernet);
        transition select(hdr.ethernet.ether_type) {
            ETHERTYPE_IPV4 : parse_ipv4;
            ETHERTYPE_IPV6 : parse_ipv6;
            ETHERTYPE_VLAN : parse_vlan;
            default : accept;
        }
    }

    state parse_vlan {
        pkt.extract(hdr.vlan_tag);
        transition select(hdr.vlan_tag.ether_type) {
            ETHERTYPE_IPV4 : parse_ipv4;
            ETHERTYPE_IPV6 : parse_ipv6;
            default : accept;
        }
    }

    state parse_ipv4 {
        pkt.extract(hdr.ipv4);
        transition select(hdr.ipv4.protocol) {
            IP_PROTOCOLS_TCP : parse_tcp;
            IP_PROTOCOLS_UDP : parse_udp;
            default : accept;
        }
    }

    state parse_ipv6 {
        pkt.extract(hdr.ipv6);
        transition select(hdr.ipv6.next_hdr) {
            IP_PROTOCOLS_TCP : parse_tcp;
            IP_PROTOCOLS_UDP : parse_udp;
            default : accept;
        }
    }

    state parse_tcp {
        pkt.extract(hdr.tcp);
        transition accept;
    }

    state parse_udp {
        pkt.extract(hdr.udp);
        transition accept;
    }

}

/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout header_t hdr, inout metadata_t meta) {   
    apply {  }
}

control PktValidation(in header_t hdr, out lookup_fields_t lkp) {

    const bit<32> table_size = 512;

    action malformed_pkt() {
        // drop.
    }

    action valid_pkt_untagged() {
        lkp.mac_src_addr = hdr.ethernet.src_addr;
        lkp.mac_dst_addr = hdr.ethernet.dst_addr;
        lkp.mac_type = hdr.ethernet.ether_type;
    }

    action valid_pkt_tagged() {
        lkp.mac_src_addr = hdr.ethernet.src_addr;
        lkp.mac_dst_addr = hdr.ethernet.dst_addr;
        lkp.mac_type = hdr.vlan_tag.ether_type;
    }

    table validate_ethernet {
        key = {
            hdr.ethernet.src_addr : ternary;
            hdr.ethernet.dst_addr : ternary;
            hdr.vlan_tag.isValid() : ternary;
        }

        actions = {
            malformed_pkt;
            valid_pkt_untagged;
            valid_pkt_tagged;
        }

        size = table_size;
    }

    action valid_ipv4_pkt() {
        // Set common lookup fields
        lkp.ip_version = 4w4;
        lkp.ip_dscp = hdr.ipv4.diffserv;
        lkp.ip_proto = hdr.ipv4.protocol;
        lkp.ip_ttl = hdr.ipv4.ttl;
        lkp.ipv4_src_addr = hdr.ipv4.src_addr;
        lkp.ipv4_dst_addr = hdr.ipv4.dst_addr;
    }

    table validate_ipv4 {
        key = {
            //ig_md.checksum_err : ternary;
            hdr.ipv4.version : ternary;
            hdr.ipv4.ihl : ternary;
            hdr.ipv4.ttl : ternary;
        }

        actions = {
            valid_ipv4_pkt;
            malformed_pkt;
        }

        size = table_size;
    }

    action valid_ipv6_pkt() {
        // Set common lookup fields
        lkp.ip_version = 4w6;
        lkp.ip_dscp = hdr.ipv6.traffic_class;
        lkp.ip_proto = hdr.ipv6.next_hdr;
        lkp.ip_ttl = hdr.ipv6.hop_limit;
        lkp.ipv6_src_addr = hdr.ipv6.src_addr;
        lkp.ipv6_dst_addr = hdr.ipv6.dst_addr;
    }

    table validate_ipv6 {
        key = {
            hdr.ipv6.version : ternary;
            hdr.ipv6.hop_limit : ternary;
        }

        actions = {
            valid_ipv6_pkt;
            malformed_pkt;
        }

        size = table_size;
    }

    apply {
        validate_ethernet.apply();
        if (hdr.ipv4.isValid()) {
            validate_ipv4.apply();
        } else if (hdr.ipv6.isValid()) {
            validate_ipv6.apply();
        }
    }
}

control PortMapping(
        in bit<9> port,
        in vlan_tag_t vlan_tag,
        inout metadata_t ig_md)(
        bit<32> port_vlan_table_size,
        bit<32> bd_table_size) {

    // ActionProfile(bd_table_size) bd_action_profile;

    action set_port_attributes(ifindex_t ifindex) {
        ig_md.ifindex = ifindex;

        // Add more port attributes here.
    }

    table port_mapping {
        key = { port : exact; }
        actions = { set_port_attributes; }
    }

    action set_bd_attributes(bd_t bd, vrf_t vrf) {
        ig_md.bd = bd;
        ig_md.vrf = vrf;
    }

    table port_vlan_to_bd_mapping {
        key = {
            ig_md.ifindex : exact;
            vlan_tag.isValid() : ternary;
            vlan_tag.vid : ternary;
        }

        actions = {
            NoAction;
            set_bd_attributes;
        }

        const default_action = NoAction;
        // implementation = bd_action_profile;
        size = port_vlan_table_size;
    }

    apply {
        port_mapping.apply();
        port_vlan_to_bd_mapping.apply();
    }
}


/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MAC(
    in mac_addr_t dst_addr,
    in bd_t bd,
    out ifindex_t egress_ifindex)(
    bit<32> mac_table_size) {

    action dmac_miss() {
        egress_ifindex = 16w0xffff;
    }

    action dmac_hit(ifindex_t ifindex) {
        egress_ifindex = ifindex;
    }

    table dmac {
        key = {
            bd : exact;
            dst_addr : exact;
        }

        actions = {
            dmac_miss;
            dmac_hit;
        }

        const default_action = dmac_miss;
        size = mac_table_size;
    }

    apply {
        dmac.apply();
    }
}

control FIB(in ipv4_addr_t dst_addr,
            in vrf_t vrf,
            out nexthop_t nexthop)(
            bit<32> host_table_size,
            bit<32> lpm_table_size) {

    action fib_hit(nexthop_t nexthop_index) {
        nexthop = nexthop_index;
    }

    action fib_miss() { }

    table fib {
        key = {
            vrf : exact;
            dst_addr : exact;
        }

        actions = {
            fib_miss;
            fib_hit;
        }

        const default_action = fib_miss;
        size = host_table_size;
    }

    table fib_lpm {
        key = {
            vrf : exact;
            dst_addr : lpm;
        }

        actions = {
            fib_miss;
            fib_hit;
        }

        const default_action = fib_miss;
        size = lpm_table_size;
    }

    apply {
        nexthop = 0;
        if (!fib.apply().hit) {
            fib_lpm.apply();
        }
    }
}

control FIBv6(in ipv6_addr_t dst_addr,
              in vrf_t vrf,
              out nexthop_t nexthop)(
              bit<32> host_table_size,
              bit<32> lpm_table_size) {

    action fib_hit(nexthop_t nexthop_index) {
        nexthop = nexthop_index;
    }

    action fib_miss() { }

    table fib {
        key = {
            vrf : exact;
            dst_addr : exact;
        }

        actions = {
            fib_miss;
            fib_hit;
        }

        const default_action = fib_miss;
        size = host_table_size;
    }

    table fib_lpm {
        key = {
            vrf : exact;
            dst_addr : lpm;
        }

        actions = {
            fib_miss;
            fib_hit;
        }

        const default_action = fib_miss;
        size = lpm_table_size;
    }

    apply {
        nexthop = 0;
        if (!fib.apply().hit) {
            fib_lpm.apply();
        }
    }
}

control Nexthop(in lookup_fields_t lkp,
                inout header_t hdr,
                inout metadata_t ig_md)(
                bit<32> table_size) {
    bool routed;
    // Hash<bit<32>>(HashAlgorithm_t.CRC32) sel_hash;
    // ActionSelector(
    //     1024, sel_hash, SelectorMode_t.FAIR) ecmp_selector;

    action set_nexthop_attributes(bd_t bd, mac_addr_t dmac) {
        routed = true;
        ig_md.bd = bd;
        hdr.ethernet.dst_addr = dmac;
    }

    table ecmp {
        key = {
            ig_md.nexthop : exact;
            lkp.ipv6_src_addr : exact;
            lkp.ipv6_dst_addr : exact;
            lkp.ipv6_flow_label : exact;
            lkp.ipv4_src_addr : exact;
            lkp.ipv4_dst_addr : exact;
        }

        actions = {
            NoAction;
            set_nexthop_attributes;
        }

        const default_action = NoAction;
        size = table_size;
        // implementation = ecmp_selector;
    }

    // table nexthop {
    //     key = {
    //         ig_md.nexthop : exact;
    //     }

    //     actions = {
    //         NoAction;
    //         set_nexthop_attributes;
    //     }

    //     const default_action = NoAction;
    //     size = table_size;
    // }

    action rewrite_ipv4() {
        hdr.ipv4.ttl = hdr.ipv4.ttl - 1;
    }

    action rewrite_ipv6() {
        hdr.ipv6.hop_limit = hdr.ipv6.hop_limit - 1;
    }

    table ip_rewrite {
        key = {
            hdr.ipv4.isValid() : exact;
            hdr.ipv6.isValid() : exact;
        }

        actions = {
            rewrite_ipv4;
            rewrite_ipv6;
        }

        const entries = {
            (true, false) : rewrite_ipv4();
            (false, true) : rewrite_ipv6();
        }
    }


    action rewrite_smac(mac_addr_t smac) {
        hdr.ethernet.src_addr = smac;
    }

    table smac_rewrite {
        key = { ig_md.bd : exact; }
        actions = {
            NoAction;
            rewrite_smac;
        }

        const default_action = NoAction;
    }

    apply {
        routed = false;
        ecmp.apply();

        // Decrement TTL and rewrite ethernet src addr if the packet is routed.
        if (routed) {
            ip_rewrite.apply();
            smac_rewrite.apply();
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyIngress(inout header_t hdr,
                  inout metadata_t ig_md,
                  inout standard_metadata_t standard_metadata) {
    PortMapping(1024, 1024) port_mapping;
    PktValidation() pkt_validation;
    MAC(1024) mac;
    FIB(1024, 1024) fib;
    FIBv6(1024, 1024) fibv6;
    Nexthop(1024) nexthop;
    lookup_fields_t lkp;
    
    action rmac_hit() { }
    table rmac {
        key = {
            lkp.mac_dst_addr : exact;
        }

        actions = {
            NoAction;
            rmac_hit;
        }

        const default_action = NoAction;
        size = 1024;
    }

    apply {
        port_mapping.apply(standard_metadata.ingress_port, hdr.vlan_tag, ig_md);
        pkt_validation.apply(hdr, lkp);
        switch (rmac.apply().action_run) {
            rmac_hit : {
                ig_md.bypass = 0;
                if (lkp.ip_version == 4w4) {
                    fib.apply(lkp.ipv4_dst_addr, ig_md.vrf, ig_md.nexthop);
                } else {
                    fibv6.apply(lkp.ipv6_dst_addr, ig_md.vrf, ig_md.nexthop);
                }
                nexthop.apply(lkp, hdr, ig_md);
            }
        }

        mac.apply(hdr.ethernet.dst_addr, ig_md.bd, ig_md.egress_ifindex);
    }
}

control MyEgress(inout header_t hdr,
                 inout metadata_t meta,
                 inout standard_metadata_t standard_metadata) {
    apply {  }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout header_t hdr, inout metadata_t meta) {
     apply {
	update_checksum(
	    hdr.ipv4.isValid(),
            { hdr.ipv4.version,
	      hdr.ipv4.ihl,
              hdr.ipv4.diffserv,
              hdr.ipv4.total_len,
              hdr.ipv4.identification,
              hdr.ipv4.flags,
              hdr.ipv4.frag_offset,
              hdr.ipv4.ttl,
              hdr.ipv4.protocol,
              hdr.ipv4.src_addr,
              hdr.ipv4.dst_addr },
            hdr.ipv4.hdr_checksum,
            HashAlgorithm.csum16);
    }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in header_t hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.vlan_tag);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.ipv6);
        packet.emit(hdr.udp);
        packet.emit(hdr.tcp);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;
