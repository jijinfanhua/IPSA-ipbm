#pragma once

#include "defs.h"
#include <utility>
#include <vector>
#include <stdexcept>

struct NextParserHeader {
    std::string tag;
    int hdr_id;

    NextParserHeader() = default;

    NextParserHeader(std::string tag, int hdr_id) : tag(std::move(tag)), hdr_id(hdr_id) {}
};

struct ParserHeader {
    static const int NO_HEADER = -1;

    int hdr_id;     // global header id
    int hdr_len;    // header length

    int next_hdr_type_start;  // next header type offset
    int next_hdr_type_len;    // next header type length
    std::vector<NextParserHeader> next_table;  // type id -> parse node idx, accept by -1

    ParserHeader() = default;

    ParserHeader(int hdrId, int hdrLen, int nextHdrTypeStart, int nextHdrTypeLen,
                 std::vector<NextParserHeader> nextTable)
            : hdr_id(hdrId), hdr_len(hdrLen), next_hdr_type_start(nextHdrTypeStart),
              next_hdr_type_len(nextHdrTypeLen), next_table(std::move(nextTable)) {}
};

class Parser {
public:
    ParserHeader parse_hdrs[NUM_HDRS];

    static const int PARSE_START_ID = 0;
    static const int PARSE_NODE_ACCEPT = -1;

    Parser() = default;

    void parse(Buffer packet, int len);
};
