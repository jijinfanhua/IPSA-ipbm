#pragma once

#include "ipsa_processor_manager.h"
#include "ipsa_distribution.h"
#include "ipsa_configuration.h"

class IpsaMemory {
public:
    // proc_id -> (sram#, tcam#)
    IpsaProcessorManager* processor_manager;
    IpsaDistribution* distribution;
    std::map<int, IpsaTable*> tables;
    std::map<int, std::pair<int, int>> table_space;
    std::map<int, std::pair<int, int>> proc_space;
    std::map<int, int> physical_proc_id;
    IpsaMemory(IpsaProcessorManager* _processor_manager, IpsaDistribution* _distribution): 
        processor_manager(_processor_manager), distribution(_distribution) {}
    void allocateMemory();
    void calculateTableSpace();
    void removeAllEntries();
};

void IpsaMemory::removeAllEntries() {
    tables.clear();
    table_space.clear();
    proc_space.clear();
    physical_proc_id.clear();
}

void IpsaMemory::calculateTableSpace() {
    // table_id -> space
    for (auto& [name, table] : processor_manager->table_manager->tables) {
        int sram_number = 0;
        int tcam_number = 0;
        if (table.key_memory.type == MEM_SRAM) {
            sram_number += table.key_memory.width * table.key_memory.depth;
        } else {
            tcam_number += table.key_memory.width * table.key_memory.depth;
        }
        sram_number += table.value_memory.width * table.value_memory.depth;
        table_space.insert({{table.table_id, {sram_number, tcam_number}}});
        tables.insert({{table.table_id, &table}});
    }
    // proc -> space
    for (auto& stage : processor_manager->stage_manager->logical_stages) {
        if (stage.removed) { continue; }
        int sram_number = 0;
        int tcam_number = 0;
        for (int table_id : stage.table_id) {
            sram_number += table_space[table_id].first;
            tcam_number += table_space[table_id].second;
        }
        proc_space.insert({{stage.stage_id, {sram_number, tcam_number}}});
    }  
}

void IpsaMemory::allocateMemory() {
    calculateTableSpace();
    // allocate memory
    std::vector<std::pair<int, int>> cluster_space; // space in clusters
    std::vector<std::vector<int>> cluster_proc; // proc id in clusters
    for (int i = 0; i < ipsa_configuration::CLUSTER_COUNT; i++) {
        cluster_space.push_back({
            ipsa_configuration::CLUSTER_SRAM_COUNT,
            ipsa_configuration::CLUSTER_TCAM_COUNT
        });
        cluster_proc.push_back({});
    }
    for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++) {
        physical_proc_id.insert({{i, -1}});
    }
    for (int stage_id : distribution->topo_sequence) {
        int target = -1;
        for (int i = 0; i < ipsa_configuration::CLUSTER_COUNT; i++) {
            if (cluster_proc[i].size() < ipsa_configuration::CLUSTER_PROC_COUNT &&
                cluster_space[i].first >= proc_space[stage_id].first &&
                cluster_space[i].second >= proc_space[stage_id].second) {
                cluster_space[i] = {
                    cluster_space[i].first - proc_space[stage_id].first,
                    cluster_space[i].second - proc_space[stage_id].second
                };
                target = i * ipsa_configuration::CLUSTER_PROC_COUNT + cluster_proc[i].size();
                cluster_proc[i].push_back(stage_id);
                break;
            }
        }
        if (target >= 0) {
            physical_proc_id[target] = stage_id;
        } else {
            std::cout << "error cannot allocate memory for stage " << stage_id << std::endl;
        }
    }
    // set configs
    std::vector<std::pair<int, int>> cluster_number;
    for (int i = 0; i < ipsa_configuration::CLUSTER_COUNT; i++) {
        cluster_number.push_back({0, 0});
    }
    std::map<int, int> proc_cluster;
    for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++) {
        if (physical_proc_id[i] >= 0) {
            proc_cluster.insert({{physical_proc_id[i], i / ipsa_configuration::CLUSTER_PROC_COUNT}});
        }
    }
    for (auto& stage : processor_manager->stage_manager->logical_stages) {
        if (stage.removed) { continue; }
        auto [x, y] = cluster_number[proc_cluster[stage.stage_id]];
        for (int table_id : stage.table_id) {
            auto table = tables[table_id];
            auto& key_memory = table->key_memory;
            if (key_memory.type == MEM_TCAM) {
                for (int i = 0; i < key_memory.depth * key_memory.width; i++) {
                    key_memory.config.push_back(x++);
                }
            } else {
                for (int i = 0; i < key_memory.depth * key_memory.width; i++) {
                    key_memory.config.push_back(y++);
                }
            }
            auto& value_memory = table->value_memory;
            for (int i = 0; i < value_memory.depth * value_memory.width; i++) {
                value_memory.config.push_back(y++);
            }
        }
        cluster_number[proc_cluster[stage.stage_id]] = {x, y};
    }
}