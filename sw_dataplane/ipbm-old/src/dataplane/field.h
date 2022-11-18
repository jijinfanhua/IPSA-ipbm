#pragma once

struct FieldSpec {
    int hdr_id;
    int start;
    int len;

    FieldSpec() = default;

    FieldSpec(int hdr_id_, int start_, int len_) : hdr_id(hdr_id_), start(start_), len(len_) {}
};
