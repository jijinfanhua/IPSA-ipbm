#pragma once

#include "p4_header.h"
#include "p4_parser.h"
#include "p4_action.h"
#include "p4_pipeline.h"

class P4Json {
public:
    P4Headers headers;
    P4Parsers parsers;
    P4Actions actions;
    P4Pipelines pipelines;

    friend std::ostream& operator<<(std::ostream& out, P4Json const & p4);
};

std::ostream& operator<<(std::ostream& out, P4Json const & p4) {
    out << p4.headers << std::endl;
    // standard_metadata
    out << "struct standard_metadata_t {\n\
    bit<9> ingress_port;\n\
    bit<9> egress_spec;\n\
    bit<9> egress_port;\n\
    bit<32> clone_spec;\n\
    bit<32> instance_type;\n\
    bit<1> drop;\n\
    bit<16> recirculate_port;\n\
    bit<32> packet_length;\n\
} standard_metadata;\n" << std::endl;
    out << p4.parsers << std::endl;
    out << p4.actions << std::endl;
    out << p4.pipelines << std::endl;
    return out;
}