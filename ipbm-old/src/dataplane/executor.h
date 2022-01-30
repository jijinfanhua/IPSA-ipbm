#pragma once

#include "action.h"

struct RuntimeArgSpec {
    int val_offset;     // offset from start of the entry value
    int len;            // arg len

    RuntimeArgSpec() = default;

    RuntimeArgSpec(int offset_, int len_) : val_offset(offset_), len(len_) {}
};

class Executor {
public:
    int hit_action;
    int hit_next_proc;
    int miss_action;
    int miss_next_proc;
    std::vector<RuntimeArgSpec> runtime_arg_specs;

    Executor() = default;

    Executor(int hitAction, int hitNextProc, int missAction, int missNextProc,
             std::vector<RuntimeArgSpec> runtimeArgSpecs)
            : hit_action(hitAction), hit_next_proc(hitNextProc), miss_action(missAction),
              miss_next_proc(missNextProc), runtime_arg_specs(std::move(runtimeArgSpecs)) {
    }

    int execute(Buffer match_val);

    static uint16_t calc_cksum(InputBuffer field, int len, HashType hash_type);
};
