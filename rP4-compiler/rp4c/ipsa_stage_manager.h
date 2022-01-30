#pragma once

#include "ipsa_module.h"
#include "rp4_ast.h"

class IpsaStage {
public:
    int stage_id;
    bool in_ingress;
    bool removed = false;
    std::vector<std::pair<int, int>> action_proc;
    int gateway_id;
    std::vector<int> table_id;
    const Rp4StageDef* def;
    IpsaStage(const Rp4StageDef* _def, int _stage_id, bool _in_ingress):
        def(_def), stage_id(_stage_id), in_ingress(_in_ingress) {}
};

class IpsaStageManager {
public:
    int global_stage_id;
    std::vector<IpsaStage> logical_stages;
    std::map<std::string, int> logical_stages_map;
    IpsaStageManager() {} 
    void load(const Rp4Ast* ast);
    void add_stage(const Rp4StageDef* stage_def, bool in_ingress);
    const IpsaStage* lookup(std::string stage_name) const;
    const IpsaStage* lookup(int stage_id) const;
};

void IpsaStageManager::load(const Rp4Ast* ast) {
    global_stage_id = 0;
    for (auto& stage_def : ast->ingress_def.stage_defs) {
        add_stage(&stage_def, true);
    }
    for (auto& stage_def : ast->egress_def.stage_defs) {
        add_stage(&stage_def, false);
    }
}

void IpsaStageManager::add_stage(const Rp4StageDef* stage_def, bool in_ingress) {
    if (logical_stages_map.find(stage_def->name) == std::end(logical_stages_map)) {
        int x = logical_stages.size();
        logical_stages.push_back( IpsaStage(stage_def, global_stage_id++, in_ingress));
        logical_stages_map.insert({{
            stage_def->name, x
        }});
    }
}

const IpsaStage* IpsaStageManager::lookup(std::string stage_name) const {
    if (auto x = logical_stages_map.find(stage_name); x != std::end(logical_stages_map)) {
        return &(logical_stages[x->second]);
    } else {
        return nullptr;
    }
}

const IpsaStage* IpsaStageManager::lookup(int stage_id) const {
    for (auto& stage : logical_stages) {
        if (!stage.removed && stage.stage_id == stage_id) {
            return &stage;
        }
    }
    return nullptr;
}