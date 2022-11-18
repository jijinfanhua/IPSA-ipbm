#pragma once

#include "ipsa_module.h"
#include "rp4_ast.h"

class IpsaStage {
public:
    int stage_id;
    int stage_belongs = -1;
    bool in_ingress;
    std::vector<std::pair<int, int>> action_proc;
    int gateway_id = 0;
    std::vector<int> table_id;
    Rp4StageDef def;
    IpsaStage(const Rp4StageDef *_def, int _stage_id, bool _in_ingress) : stage_id(_stage_id), in_ingress(_in_ingress) {
        def = *_def;
    }
};

class IpsaStageManager {
public:
    int global_stage_id = 0;
    std::map<std::string, IpsaStage> logical_stages;

    IpsaStageManager() {}
    void load(const std::unique_ptr<Rp4Ast>& ast);
    void addStage(const Rp4StageDef *stage_def, bool in_ingress);
    IpsaStage *lookup(std::string stage_name);
    IpsaStage *lookup(int stage_id);
};