#pragma once

#include "ipsa_stage_manager.h"
#include "ipsa_header_manager.h"
#include "ipsa_level_manager.h"
#include "ipsa_action_manager.h"
#include "ipsa_table_manager.h"
#include "ipsa_gateway_manager.h"

/*----------------------------------------------------------------
    does not aggregate the stages
    each stage occupy a different processor

    first, if a stage is empty -> only 1 (action -> next_stage)
        1. add its action to all the actions points to it
        2. remove the stage and the action / table
*/

// generate the processors
class IpsaProcessorManager {
public:
    IpsaStageManager* stage_manager;
    IpsaHeaderManager* header_manager;
    IpsaLevelManager* level_manager;
    IpsaActionManager* action_manager;
    IpsaTableManager* table_manager;
    
    IpsaProcessorManager(
        IpsaStageManager* _stage_manager,
        IpsaHeaderManager* _header_manager,
        IpsaLevelManager* _level_manager,
        IpsaActionManager* _action_manager,
        IpsaTableManager* _table_manager
    ):
        stage_manager(_stage_manager),
        header_manager(_header_manager),
        level_manager(_level_manager),
        action_manager(_action_manager),
        table_manager(_table_manager) 
        {}
    void initializeStages();
    void reorderStages(std::map<int, int> proc_proc);
    void setupStages(IpsaGatewayManager* gateway_manager);
};

void IpsaProcessorManager::initializeStages() {
    // action to proc table
    for (auto& stage : stage_manager->logical_stages) {
        auto& exec_entries = stage.def->executor.entries;
        for (auto& entry : exec_entries) {
            auto action = action_manager->lookup(entry.action_name);
            int action_id = action == nullptr ? 0 : action->id;
            auto processor = stage_manager->lookup(entry.stage_name);
            int processor_id = processor == nullptr ? -1 : processor->stage_id;
            stage.action_proc.push_back({action_id, processor_id});
        }
    }
    // remove empty stages
    std::map<int, int> goto_maps;
    for (auto& stage : stage_manager->logical_stages) {
        if (auto virtual_action = stage.def->get_virtual_action(); virtual_action != nullptr) {
            // replace stage with virtual_action
            auto action = action_manager->lookup(virtual_action->action_name);
            if (action->parameter_num > 0) {
                continue; // cannot be removed
            }
            auto next_stage = stage_manager->lookup(virtual_action->stage_name);
            int next_stage_id = next_stage == nullptr ? -1 : next_stage->stage_id;
            bool conflict_flag = false;
            for (auto& prev_stage : stage_manager->logical_stages) {
                if (!prev_stage.removed) {
                    for (int i = 0; i < prev_stage.action_proc.size(); i++) {
                        if (prev_stage.action_proc[i].second == stage.stage_id) {
                            int prev_action_id = prev_stage.action_proc[i].first;
                            if (action_manager->concatAction(prev_action_id, action)) {
                                prev_stage.action_proc[i] = {
                                    prev_action_id,
                                    next_stage_id
                                };
                            } else {
                                conflict_flag = true;
                            }
                        }
                    }
                }
            }
            if (!conflict_flag) {
                stage.removed = true;
                goto_maps.insert({{stage.stage_id, next_stage_id}});
            }
        }
    }
    // reorder the stages
    std::map<int, int> reorder_map = {{-1, -1}};
    int global_stage_id = 0;
    for (auto& stage : stage_manager->logical_stages) {
        if (!stage.removed) {
            reorder_map.insert({{stage.stage_id, global_stage_id++}});
        }
    }
    for (auto& stage : stage_manager->logical_stages) {
        for (IpsaStage& x = stage; x.removed; ) {
            IpsaStage& y = stage_manager->logical_stages[goto_maps[x.stage_id]];
            if (y.removed) {
                x = y;
            } else {
                reorder_map.insert({{stage.stage_id, reorder_map[y.stage_id]}});
                break;
            }
        }
    }
    reorderStages(std::move(reorder_map));
    // generate tables
    for (auto& stage : stage_manager->logical_stages) {
        for (auto& switch_entry : stage.def->matcher.switch_entries) {
            if (switch_entry.value->isTableStmt()) {
                auto& name = std::static_pointer_cast<Rp4SwitchTableStmt>(switch_entry.value)->name;
                stage.table_id.push_back(table_manager->lookup(name)->table_id);
            }
        }
    }
}

void IpsaProcessorManager::reorderStages(std::map<int, int> proc_proc) {
    for (auto& stage : stage_manager->logical_stages) {
        stage.stage_id = proc_proc[stage.stage_id];
        for (int i = 0; i < stage.action_proc.size(); i++) {
            stage.action_proc[i] = {
                stage.action_proc[i].first,
                proc_proc[stage.action_proc[i].second]
            };
        }
    }
}

void IpsaProcessorManager::setupStages(IpsaGatewayManager* gateway_manager) {
    // set id in tables and the corresponding gateway entries (with action2proc map)
    for (auto& stage : stage_manager->logical_stages) {
        for (int matcher_id = 0; int table_id : stage.table_id) {
            auto& gateway = gateway_manager->gateways[stage.gateway_id];
            table_manager->setMatcherId(table_id, stage.stage_id, matcher_id, stage.action_proc);
            gateway.next_table.setMatcherId(table_id, matcher_id++);
        }
    }
}
