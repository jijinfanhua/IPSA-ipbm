#include "processor.h"
#include "global.h"

const int Processor::PROC_ID_END;

bool Processor::eval_condition() const {
    if (cond.type == COND_NONE) {
        return true;
    } else if (cond.type == COND_IS_VALID) {
        return glb.hdrs[cond.args.is_valid.hdr_id].data != nullptr;
    } else {
        assert(0);
    }
}

int Processor::process(Buffer packet, size_t len) {
    glb.parser.parse(packet, len);
    NextStage next = eval_condition() ? true_next : false_next;
    int next_proc_id;
    if (next.is_table) {
        // Match flow table
        Buffer match_val = matcher.match(next.table_id);
        // Execute corresponding action
        next_proc_id = executor.execute(match_val);
    } else {
        next_proc_id = next.proc_id;
    }
    return next_proc_id;
}