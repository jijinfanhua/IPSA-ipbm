#pragma once

#include "parser.h"
#include "matcher.h"
#include "executor.h"
#include <net/ethernet.h>
#include <linux/ip.h>
#include <cstring>
#include <vector>
#include <map>

enum ConditionType {
    COND_NONE, COND_IS_VALID
};

struct Condition {
    ConditionType type;

    union {
        struct {
            int hdr_id;
        } is_valid;
    } args;

    static Condition is_valid(int hdr_id) {
        Condition cond;
        cond.type = COND_IS_VALID;
        cond.args.is_valid.hdr_id = hdr_id;
        return cond;
    }

    static Condition no_cond() {
        Condition cond;
        cond.type = COND_NONE;
        return cond;
    }
};

struct NextStage {
    bool is_table;
    union {
        // Table info
        int table_id;
        // Processor info
        int proc_id;
    };

    static NextStage table(int table_id_) {
        NextStage next_stage;
        next_stage.is_table = true;
        next_stage.table_id = table_id_;
        return next_stage;
    }

    static NextStage proc(int proc_id_) {
        NextStage next_stage;
        next_stage.is_table = false;
        next_stage.proc_id = proc_id_;
        return next_stage;
    }
};

class Processor {
public:
    static const int PROC_START = 0;
    static const int PROC_ID_END = -1;

    Matcher matcher;
    Executor executor;
    Condition cond;
    NextStage true_next;      // Next table/processor id if condition is true
    NextStage false_next;     // Next table/processor id if condition is false

    bool eval_condition() const;

    int process(Buffer packet, size_t len);
};
