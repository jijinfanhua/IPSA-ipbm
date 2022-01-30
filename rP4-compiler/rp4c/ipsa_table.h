#pragma once

#include "ipsa_module.h"
#include "rp4_ast.h"

class IpsaActionProcPair : public IpsaModule {
public:
    int action_id;
    int proc_id;
    IpsaActionProcPair(int _action_id, int _proc_id):
        action_id(_action_id), proc_id(_proc_id) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"action_id", makeValue(action_id)},
            {"proc_id", makeValue(proc_id)}
        };
        return makeValue(dst);
    }
};

enum IpsaMemoryType {
    MEM_TCAM, MEM_SRAM
};

static inline std::string to_string(IpsaMemoryType mt) {
    static const std::map<IpsaMemoryType, std::string> m = {
            {MEM_TCAM, "TCAM"},
            {MEM_SRAM, "SRAM"}
    };
    return m.at(mt);
}

class IpsaValueMemory : public IpsaModule {
public:
    IpsaMemoryType type = MEM_SRAM;
    std::vector<int> config;
    int width;
    int depth;
    IpsaValueMemory() {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"config", makeValue(config)},
            {"width", makeValue(width)}
        };
        return makeValue(dst);
    }
};

class IpsaKeyMemory : public IpsaValueMemory {
public:
    IpsaKeyMemory() {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"type", makeValue(to_string(type))},
            {"config", makeValue(config)},
            {"width", makeValue(width)},
            {"depth", makeValue(depth)}
        };
        return makeValue(dst);
    }
};

class IpsaTable : public IpsaModule {
public:
    int proc_id = 0; // in which processor
    std::vector<int> key_width_vec;

    int id = 0; // in matcher
    int table_id; // global
    // int hit_miss;
    // int hit_action_id;
    int miss_action_id;
    int no_table = 0;
    std::vector<IpsaActionProcPair> action_to_proc;
    Rp4MatchType match_type = MT_EXACT;
    IpsaKeyMemory key_memory;
    IpsaValueMemory value_memory;
    std::vector<std::shared_ptr<IpsaValue>> field_infos;
    IpsaTable(int _table_id): table_id(_table_id) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"id", makeValue(id)},
            // {"hit_miss", makeValue(hit_miss)},
            // {"hit_action_id", makeValue(hit_action_id)},
            {"miss_action_id", makeValue(miss_action_id)},
            {"no_table", makeValue(no_table)},
            {"action_to_proc", makeValue(action_to_proc)},
            {"match_type", makeValue(to_string(match_type))},
            {"key_memory", key_memory.toIpsaValue()},
            {"value_memory", value_memory.toIpsaValue()},
            {"field_infos", makeValue(field_infos)}
        };
        return makeValue(dst);
    }
};
