#pragma once

#include "ipsa_table.h"
#include "ipsa_header_manager.h"
#include "ipsa_action_manager.h"
#include "ipsa_configuration.h"

class IpsaTableManager {
public:
    int global_table_id;
    IpsaHeaderManager* header_manager;
    IpsaActionManager* action_manager;
    std::map<std::string, IpsaTable> tables;
    IpsaTableManager(IpsaHeaderManager* _header_manager, IpsaActionManager* _action_manager): 
        header_manager(_header_manager), action_manager(_action_manager) {}
    void load(const Rp4Ast* ast);
    const IpsaTable* lookup(int table_id) const;
    const IpsaTable* lookup(std::string name) const;
    void setMatcherId(int table_id, int proc_id, int matcher_id, const std::vector<std::pair<int, int>>& action_proc);
    void reorderStages(std::map<int, int> proc_proc);
};

// also set action_to_proc list
void IpsaTableManager::setMatcherId(int table_id, int proc_id, int matcher_id, const std::vector<std::pair<int, int>>& action_proc) {
    for (auto& [name, table] : tables) {
        if (table.table_id == table_id) {
            table.proc_id = proc_id;
            table.id = matcher_id;
            for (auto [action, proc] : action_proc) {
                table.action_to_proc.push_back(IpsaActionProcPair(action, proc));
            }
            break;
        }
    }
}

void IpsaTableManager::reorderStages(std::map<int, int> proc_proc) {
    for (auto& [name, table] : tables) {
        table.proc_id = proc_proc[table.proc_id];
        for (auto& [action, proc] : table.action_to_proc) {
            proc = proc_proc[proc];
        }
    }
}

const IpsaTable* IpsaTableManager::lookup(int table_id) const {
    for (auto& [name, table] : tables) {
        if (table.table_id == table_id) {
            return &table;
        }
    }
    return nullptr;
}

const IpsaTable* IpsaTableManager::lookup(std::string name) const {
    if (auto x = tables.find(name); x != std::end(tables)) {
        return &(x->second);
    } else {
        return nullptr;
    }
}

// leave id, action to proc and memory config undefined
void IpsaTableManager::load(const Rp4Ast* ast) {
    global_table_id = 0;
    for (auto& table_def : ast->tables_def.tables) {
        IpsaTable table(global_table_id++);
        auto& key_def_entries = table_def.key_def.entries;
        int default_action_id = 0;
        for (auto x : table_def.optional_stmts) {
            if (x->isDefault()) {
                auto& y = std::static_pointer_cast<Rp4DefaultActionStmt>(x)->action_name;
                auto default_action = action_manager->lookup(y);
                if (default_action != nullptr) {
                    default_action_id = default_action->id;
                }
            }
        }
        table.miss_action_id = default_action_id;
        if (key_def_entries.size() == 0) {
            table.no_table = 1; // no entries, no table
        } else {
            table.no_table = 0;
            if (key_def_entries.size() == 1) {
                auto& entry = key_def_entries[0];
                table.match_type = entry.match_type;
            } else {
                if (std::all_of(std::begin(key_def_entries), std::end(key_def_entries), [](auto& entry) {
                    return entry.match_type == MT_EXACT;
                })) {
                    table.match_type = MT_EXACT;
                } else {
                    table.match_type = MT_TERNARY;
                }
            }
            if (table.match_type == MT_EXACT) {
                table.key_memory.type = MEM_SRAM;
            } else {
                table.key_memory.type = MEM_TCAM;
            }
            int table_key_width = 0;
            for (auto& entry : key_def_entries) {
                auto x = header_manager->lookup(entry.lvalue);
                table.key_width_vec.push_back(x->field_length);
                table_key_width += x->field_length;
                table.field_infos.push_back(x->toIpsaValue());
            }
            int mem_width = table.key_memory.type == MEM_SRAM ? ipsa_configuration::SRAM_WIDTH : ipsa_configuration::TCAM_WIDTH;
            int mem_depth = table.key_memory.type == MEM_SRAM ? ipsa_configuration::SRAM_DEPTH : ipsa_configuration::TCAM_DEPTH;
            table.key_memory.width = (table_key_width + (mem_width - 1)) / mem_width;
            int depth = 1;
            for (auto x : table_def.optional_stmts) {
                if (x->isSize()) {
                    depth = std::static_pointer_cast<Rp4TableSizeStmt>(x)->size;
                    depth = (depth + (mem_depth - 1)) / mem_depth;
                    break;
                }
            }
            table.key_memory.depth = table.value_memory.depth = depth;
            table.value_memory.type = MEM_SRAM;
            mem_width = ipsa_configuration::SRAM_WIDTH;
            int width = 1;
            for (auto x : table_def.optional_stmts) {
                if (x->isWidth()) {
                    width = std::static_pointer_cast<Rp4TableValueWidthStmt>(x)->width;
                    width = (width + (mem_width - 1)) / mem_width;
                    break;
                }
            }
            table.value_memory.width = width;
        }
        tables.insert({{table_def.name, std::move(table)}});
    }
}