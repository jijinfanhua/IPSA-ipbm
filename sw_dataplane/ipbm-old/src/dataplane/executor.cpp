#include "executor.h"
#include "global.h"

int Executor::execute(Buffer match_val) {
    int action_id;
    int next_proc_id;
    std::vector<BufView> runtime_args;
    if (match_val) {
        // hit
        action_id = hit_action;
        next_proc_id = hit_next_proc;
        // Get runtime args
        for (auto &spec: runtime_arg_specs) {
            BufView runtime_arg(match_val + spec.val_offset, spec.len);
            runtime_args.push_back(runtime_arg);
        }
    } else {
        // miss
        action_id = miss_action;
        next_proc_id = miss_next_proc;
    }
    for (auto &op: glb.actions[action_id].ops) {
        switch (op.type) {
            case OP_CKSUM: {
                auto args = op.args_cksum;
                Buffer cksum_field = glb.hdrs[args.cksum_field.hdr_id].data + args.cksum_field.start;
                Buffer dst_field = glb.hdrs[args.dst_field.hdr_id].data + args.dst_field.start;
                *(uint16_t *) dst_field = 0;
                *(uint16_t *) dst_field = calc_cksum(cksum_field, args.cksum_field.len, args.hash_type);
                break;
            }
            case OP_COPY_FIELD: {
                auto args = op.args_copy_field;
                if (args.dst.len != args.src.len) {
                    throw std::runtime_error("dst len != src len");
                }
                int len = args.src.len;
                Buffer src = glb.hdrs[args.src.hdr_id].data + args.src.start;
                Buffer dst = glb.hdrs[args.dst.hdr_id].data + args.dst.start;
                memmove(dst, src, len);
                break;
            }
            case OP_ADD: {
                auto args = op.args_add;
                Buffer val_buf = glb.hdrs[args.field.hdr_id].data + args.field.start;
                if (args.field.len == 1) {
                    *(Buffer) val_buf += args.delta;
                } else if (args.field.len == 2) {
                    *(uint16_t *) val_buf += args.delta;
                } else if (args.field.len == 4) {
                    *(uint32_t *) val_buf += args.delta;
                } else if (args.field.len == 8) {
                    *(uint64_t *) val_buf += args.delta;
                }
                break;
            }
            case OP_SET_PORT: {
                auto args = op.args_set_port;
                BufView port = runtime_args[args.arg_idx];
                if (port.size == sizeof(uint16_t)) {
                    glb.meta_data.out_port = *(uint16_t *) port.data;
                } else if (port.size == sizeof(uint32_t)) {
                    glb.meta_data.out_port = *(uint32_t *) port.data;
                } else {
                    assert(0);
                }
                break;
            }
            case OP_SET_FIELD: {
                auto args = op.args_set_field;
                BufView val = runtime_args[args.arg_src];
                Buffer dst_buf = glb.hdrs[args.dst.hdr_id].data + args.dst.start;
                assert(val.size == args.dst.len);
                memcpy(dst_buf, val.data, val.size);
                break;
            }
            case OP_RECIRCULATE: {
                next_proc_id = Processor::PROC_START;
                break;
            }
            case OP_SET_MULTICAST: {
                glb.meta_data.out_port = glb.meta_data.vlan_id;
                break;
            }
            default:
                assert(0);
        }
    }
    return next_proc_id;
}

uint16_t Executor::calc_cksum(InputBuffer field, int len, HashType hash_type) {
    if (hash_type == HASH_CSUM16) {
        uint32_t cksum = 0;
        int i;
        for (i = 0; i < len; i += 2) {
            cksum += *(uint16_t *) &field[i];
        }
        if (i < len) {
            cksum += field[i];
        }
        cksum = (cksum >> 16) + (cksum & 0xffff);
        cksum += (cksum >> 16);
        return (uint16_t) ~cksum;
    } else {
        throw std::runtime_error("This hash type is not implemented");
    }
}
