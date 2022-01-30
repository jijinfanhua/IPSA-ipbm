#pragma once

#include "ipsa_level_manager.h"
#include "ipsa_action_manager.h"
#include "ipsa_table_manager.h"
#include "ipsa_processor_manager.h"
#include "ipsa_gateway_manager.h"
#include "ipsa_distribution.h"
#include "ipsa_memory.h"

class IpsaParser : public IpsaModule {
public:
    int parser_level = 0;
    std::vector<std::shared_ptr<IpsaValue>> levels;
    IpsaParser() {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"parser_level", makeValue(parser_level)},
            {"levels", makeValue(levels)}
        };
        return makeValue(dst);
    }
};

class IpsaProcessor : public IpsaModule {
public:
    int id;
    IpsaParser parser;
    IpsaGateway* gateway = nullptr;
    std::vector<const IpsaTable*> matcher;
    std::vector<const IpsaAction*> executor;

    // if the gateway is defined by "new" (user-defined processor),
    // use this variable to mark it and delete when the processor is destroyed
    bool user_gateway = false;
    IpsaProcessor(int _id): id(_id) {}
    ~IpsaProcessor() {
        if (user_gateway) {
            delete gateway;
        }
    }
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"id", makeValue(id)},
            {"parser", parser.toIpsaValue()},
            {"gateway", gateway->toIpsaValue()}
        };
        std::vector<std::shared_ptr<IpsaValue>> matcher_list;
        for (auto table : matcher) {
            matcher_list.push_back(table->toIpsaValue());
        }
        dst.insert({{"matcher", makeValue(matcher_list)}});
        std::vector<std::shared_ptr<IpsaValue>> executor_list;
        for (auto action : executor) {
            executor_list.push_back(action->toIpsaValue());
        }
        dst.insert({{"executor", makeValue(executor_list)}});
        return makeValue(dst);
    }
};

class Ipsa : public IpsaModule {
public:
    int is_incremental = 0;
    const IpsaMetadata* metadata;
    std::vector<std::shared_ptr<IpsaProcessor>> processors;
    Ipsa(const IpsaMetadata* _metadata): metadata(_metadata) {
        processors.resize(ipsa_configuration::PROC_COUNT);
    }
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"is_incremental", makeValue(is_incremental)},
            {"metadata", metadata->toIpsaValue()},
        };
        for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++) {
            dst.insert({{
                "processor_" + std::to_string(i),
                processors[i].get() == nullptr ? nullptr : processors[i]->toIpsaValue()
            }});
        }
        return makeValue(dst);
    }
};

class IpsaBuilder {
public:
    IpsaHeaderManager header_manager;
    IpsaLevelManager level_manager;
    IpsaActionManager action_manager;
    IpsaTableManager table_manager;
    IpsaStageManager stage_manager;
    IpsaProcessorManager processor_manager;
    IpsaGatewayManager gateway_manager;
    IpsaDistribution distribution;
    IpsaMemory memory;
    Ipsa ipsa;
    IpsaBuilder():
        header_manager(), 
        level_manager(&header_manager),
        action_manager(&header_manager),
        table_manager(&header_manager, &action_manager),
        stage_manager(),
        gateway_manager(&header_manager, &stage_manager, &table_manager),
        processor_manager(
            &stage_manager,
            &header_manager,
            &level_manager, 
            &action_manager,
            &table_manager
        ),
        distribution(&processor_manager),
        memory(&processor_manager, &distribution),
        ipsa(&(header_manager.metadata)) {}
    void allocateParsers() { distribution.distributeParsers(); }
    void allocateMemory() { memory.allocateMemory(); }
    bool allocateMemory(const IpsaBuilder& prev);
    void allocateProcessors();
    void load(const Rp4Ast* ast) {
        header_manager.load(ast);
        level_manager.load(ast);
        action_manager.load(ast);
        table_manager.load(ast);
        stage_manager.load(ast);
        processor_manager.initializeStages();
        gateway_manager.load();
        processor_manager.setupStages(&gateway_manager);
    }
};

void IpsaBuilder::allocateProcessors() {
    for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++) {
        ipsa.processors[i] = nullptr;
    }
    int extra_processor_id = 0;
    for (int stage_id : distribution.topo_sequence) {
        extra_processor_id = std::max(extra_processor_id, stage_id);
    }
    extra_processor_id++; // unused stage id to store parser-only stages
    for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++) {
        auto& processor = ipsa.processors[i];
        if (int x = memory.physical_proc_id[i]; x >= 0) {
            processor = std::make_shared<IpsaProcessor>(x);
            auto& stage = *stage_manager.lookup(x);
            // parser
            auto& parser_levels = distribution.distributed_levels[x];
            for (int j = 0; j < parser_levels.size(); j += ipsa_configuration::MAX_LEVEL) {
                int target_stage = i;
                if (j + ipsa_configuration::MAX_LEVEL < parser_levels.size()) { // not the last
                    for (int k = 0; k < ipsa_configuration::PROC_COUNT; k++) { // allocate a new processor
                        if (ipsa.processors[k].get() == nullptr && memory.physical_proc_id[k] < 0) {
                            ipsa.processors[k] = std::make_shared<IpsaProcessor>(extra_processor_id++);
                            target_stage = k;
                            break;
                        }
                    }
                    if (target_stage == i) {
                        std::cout << "error no processors can be allcoated" << std::endl;
                    }
                }
                auto& parser = ipsa.processors[target_stage]->parser;
                for (int k = 0; k < ipsa_configuration::MAX_LEVEL && j + k < parser_levels.size(); k++) {
                    int level_id = parser_levels[j + k];
                    parser.parser_level++;
                    parser.levels.push_back(
                        level_manager.levels[level_id].toIpsaValue()
                    );
                }
                if (target_stage != i) {
                    // build parser-only stage
                    int next_stage_id = x;
                    if (j + ipsa_configuration::MAX_LEVEL*2 < parser_levels.size()) {
                        next_stage_id = extra_processor_id;
                    }
                    // may cause memory leak, set the mark "user_gateway" to avoid it
                    ipsa.processors[target_stage]->user_gateway = true;
                    auto gateway = ipsa.processors[target_stage]->gateway = new IpsaGateway();
                    gateway->next_table.default_entry = std::make_shared<IpsaGatewayStageEntry>(next_stage_id);
                    // no matcher and executor
                }
            }
            // gateway
            processor->gateway = &(gateway_manager.gateways[stage.gateway_id]);
            // matcher
            for (int table_id : stage.table_id) {
                processor->matcher.push_back(table_manager.lookup(table_id));
            }
            // executor 
            for (auto [action_id, _] : stage.action_proc) {
                processor->executor.push_back(action_manager.lookup(action_id));
            }
        }
    }
    // reorder the processors to match the physical allocation
    // may cause chaos (cannot identify processors in differenct snapshots)
    std::map<int, int> proc_proc = {{-1, -1}};
    for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++) {
        if (ipsa.processors[i].get() != nullptr) {
            proc_proc.insert({{ipsa.processors[i]->id, i}});
        }
    }
    processor_manager.reorderStages(proc_proc);
    table_manager.reorderStages(proc_proc);
    for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++) {
        if (ipsa.processors[i].get() != nullptr) {
            auto gateway = ipsa.processors[i]->gateway;
            auto default_entry = gateway->next_table.default_entry;
            if (default_entry.get() != nullptr) {
                default_entry->setNextId(proc_proc);
            }
        }
    }
}

bool IpsaBuilder::allocateMemory(const IpsaBuilder& prev) {
    memory.calculateTableSpace();
    std::map<int, const IpsaTable*> table_map;
    // diff the table list
    for (auto& [name, table] : table_manager.tables) {
        bool exist = false;
        for (auto& [prev_name, prev_table] : prev.table_manager.tables) {
            if (prev_name == name) {
                table_map.insert({{table.table_id, &prev_table}});
                exist = true;
                break;
            }
        }
        if (!exist) {
            table_map.insert({{table.table_id, nullptr}});
        }
    }
    // existed table make the processors in clusters
    std::map<int, int> proc_cluster;
    for (int stage_id : distribution.topo_sequence) {
        proc_cluster.insert({{stage_id, -1}});
    }
    for (auto& [name, table] : table_manager.tables) {
        auto proc_id = table.proc_id;
        auto prev_table = table_map[table.table_id];
        if (prev_table != nullptr) {
            int prev_proc_id = prev_table->proc_id;
            int cluster_id = prev_proc_id / ipsa_configuration::CLUSTER_PROC_COUNT;
            if (int x = proc_cluster[proc_id]; x >= 0) {
                if (x != cluster_id) {
                    return false; // tables in different clusters must be allocated to the same processor
                }
            } else {
                proc_cluster[proc_id] = cluster_id;
            }
        }
    }
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
    // locate fixed processors
    auto& proc_space = memory.proc_space;
    auto& physical_proc_id = memory.physical_proc_id;
    for (int stage_id : distribution.topo_sequence) {
        if (int i = proc_cluster[stage_id]; i >= 0) {
            if (cluster_proc[i].size() < ipsa_configuration::CLUSTER_PROC_COUNT &&
                cluster_space[i].first >= proc_space[stage_id].first &&
                cluster_space[i].second >= proc_space[stage_id].second) {
                cluster_space[i] = {
                    cluster_space[i].first - proc_space[stage_id].first,
                    cluster_space[i].second - proc_space[stage_id].second
                };
                int target = i * ipsa_configuration::CLUSTER_PROC_COUNT + cluster_proc[i].size();
                cluster_proc[i].push_back(stage_id);
                physical_proc_id[target] = stage_id;
            } else {
                return false; 
                //    a cluster must contain exceeded processors
                // or a cluster cannot include existing tables (actually this should not happen)
            }
        }
    }
    // locate dynamic processors
    for (int stage_id : distribution.topo_sequence) {
        if (proc_cluster[stage_id] < 0) {
            int target = -1;
            for (int i = 0; i < ipsa_configuration::CLUSTER_COUNT; i++) {
                if (cluster_proc[i].size() < ipsa_configuration::CLUSTER_PROC_COUNT &&
                    cluster_space[i].first >= proc_space[stage_id].first &&
                    cluster_space[i].second >= proc_space[stage_id].second) {
                    cluster_space[i] = {
                        cluster_space[i].first - proc_space[stage_id].first,
                        cluster_space[i].second - proc_space[stage_id].second
                    };
                    int target = i * ipsa_configuration::CLUSTER_PROC_COUNT + cluster_proc[i].size();
                    cluster_proc[i].push_back(stage_id);
                    break;
                }
            }
            if (target >= 0) {
                physical_proc_id[target] = stage_id;
            } else {
                return false; // cannot arrange memory
            }
        }
    }
    // if arrive this point, incremental update is possible
    // now, it is time to set configs
    std::vector<std::pair<std::vector<bool>, std::vector<bool> > > cluster_bitmap;
    for (int i = 0; i < ipsa_configuration::CLUSTER_COUNT; i++) {
        cluster_bitmap.push_back({
            std::vector<bool>(ipsa_configuration::CLUSTER_TCAM_COUNT, false),
            std::vector<bool>(ipsa_configuration::CLUSTER_SRAM_COUNT, false)
        });
    }
    // existing tables
    for (auto& [name, table] : table_manager.tables) {
        auto prev_table = table_map[table.table_id];
        auto& now_cluster = cluster_bitmap[proc_cluster[table.proc_id]];
        if (prev_table != nullptr) {
            // copy table
            for (int x : prev_table->key_memory.config) {
                if (prev_table->key_memory.type == MEM_TCAM) {
                    now_cluster.first[x] = true;
                } else {
                    now_cluster.second[x] = true;
                }
                table.key_memory.config.push_back(x);
            }
            for (int y : prev_table->value_memory.config) {
                now_cluster.second[y] = true;
                table.value_memory.config.push_back(y);
            }
        }
    }
    // other tables
    for (auto& [name, table] : table_manager.tables) {
        auto prev_table = table_map[table.table_id];
        auto& now_cluster = cluster_bitmap[proc_cluster[table.proc_id]];
        if (prev_table == nullptr) {
            // key
            for (int i = 0; i < table.key_memory.depth * table.key_memory.width; i++) {
                int target = -1;
                if (table.key_memory.type == MEM_TCAM) {
                    for (int j = 0; j < ipsa_configuration::CLUSTER_TCAM_COUNT; j++) {
                        if (!now_cluster.first[j]) {
                            now_cluster.first[target = j] = true;
                            break;
                        }
                    }
                } else {
                    for (int j = 0; j < ipsa_configuration::CLUSTER_SRAM_COUNT; j++) {
                        if (!now_cluster.second[j]) {
                            now_cluster.first[target = j] = true;
                            break;
                        }
                    }
                }
                if (target >= 0) {
                    table.key_memory.config.push_back(target);
                }
            }
            // value
            for (int i = 0; i < table.value_memory.depth * table.value_memory.width; i++) {
                int target = -1;
                for (int j = 0; j < ipsa_configuration::CLUSTER_SRAM_COUNT; j++) {
                    if (!now_cluster.second[j]) {
                        now_cluster.first[target = j] = true;
                        break;
                    }
                }
                if (target >= 0) {
                    table.value_memory.config.push_back(target);
                }
            }
        }
    }
    // ok
    return true;
}