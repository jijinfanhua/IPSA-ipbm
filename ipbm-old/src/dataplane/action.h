#pragma once

#include "field.h"
#include "defs.h"
#include <vector>

static const int MAX_COUNTER_KEYS = 8;

enum OpType {
    OP_CKSUM = 0,
    OP_COPY_FIELD = 1,
    OP_ADD = 2,
    OP_SET_PORT = 3,
    OP_SET_FIELD = 4,
    OP_RECIRCULATE = 5,
    OP_SET_MULTICAST = 6,
};

enum HashType {
    HASH_CSUM16
};

struct Primitive {
    OpType type;

    union {
        struct {
            FieldSpec cksum_field;
            FieldSpec dst_field;
            HashType hash_type;
        } args_cksum;

        struct {
            FieldSpec field;
            int delta;
        } args_add;

        struct {
            FieldSpec dst;
            FieldSpec src;
        } args_copy_field;

        struct {
            FieldSpec dst;
            int arg_src;
        } args_set_field;

        struct {
            int arg_idx;
        } args_set_port;

        struct {
            int counter_id;
            int num_keys;
            FieldSpec keys[MAX_COUNTER_KEYS];
        } args_counter;
    };

    Primitive() = default;

    Primitive(OpType type_) : type(type_) {}

    static Primitive cksum(const FieldSpec &cksum_field, const FieldSpec &dst_field, HashType hash_type) {
        Primitive op(OP_CKSUM);
        op.args_cksum = {
                .cksum_field = cksum_field,
                .dst_field = dst_field,
                .hash_type = hash_type,
        };
        return op;
    }

    static Primitive add(const FieldSpec &field, int delta) {
        Primitive op(OP_ADD);
        op.args_add = {
                .field = field,
                .delta = delta,
        };
        return op;
    }

    static Primitive copy_field(const FieldSpec &dst, const FieldSpec &src) {
        Primitive op(OP_COPY_FIELD);
        op.args_copy_field = {.dst = dst, .src = src};
        return op;
    }

    static Primitive set_field(const FieldSpec &dst, int arg_src) {
        Primitive op(OP_SET_FIELD);
        op.args_set_field = {.dst = dst, .arg_src = arg_src};
        return op;
    }

    static Primitive set_port_from_arg(int arg_idx) {
        Primitive op(OP_SET_PORT);
        op.args_set_port.arg_idx = arg_idx;
        return op;
    }

    static Primitive set_multicast() {
        Primitive op(OP_SET_MULTICAST);
        return op;
    }

    static Primitive recirculate() {
        Primitive op;
        op.type = OP_RECIRCULATE;
        return op;
    }
};

struct Action {
    std::vector<Primitive> ops;

    Action() = default;

    Action(std::vector<Primitive> ops_) : ops(std::move(ops_)) {}
};
