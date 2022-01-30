#pragma once

#include "ipsa_processor_manager.h"
#include "ipsa_configuration.h"
#include "ipsa_output.h"

class IpsaDistribution {
public:
    IpsaProcessorManager* processor_manager;
    std::map<int, std::vector<int>> distributed_levels;
    std::vector<int> topo_sequence;
    IpsaDistribution(IpsaProcessorManager* _processor_manager):
        processor_manager(_processor_manager) {}
    void distributeParsers();
private:
    void topologicalSort(
        const std::map<int, std::vector<int>>& last_table,
        const std::map<int, std::vector<int>>& next_table,
        std::vector<int>& topo_sequence
    );
};

void IpsaDistribution::topologicalSort(
        const std::map<int, std::vector<int>>& last_table,
        const std::map<int, std::vector<int>>& next_table,
        std::vector<int>& topo_sequence
    ) {
        std::map<int, int> in_degree;
        int remain = 0;
        for (auto& [proc_id, last_proc_ids] : last_table) {
            in_degree.insert({{proc_id, last_proc_ids.size()}});
            remain++;
        }
        for (; remain > 0; remain--) {
            int next = -1;
            for (auto [proc_id, deg] : in_degree) {
                if (deg == 0) {
                    next = proc_id;
                    break;
                }
            }
            if (next < 0) {
                std::cout << "error stage dag" << std::endl;
            }
            topo_sequence.push_back(next);
            in_degree[next] = -1;
            for (int next_proc_id : next_table.at(next)) {
                in_degree[next_proc_id]--;
            }
        }
    }

void IpsaDistribution::distributeParsers() {
    // header_id -> listof(level_id)
    std::map<int, std::vector<int>> header_level;
    for (auto& [name, header] : processor_manager->header_manager->headers) {
        // std::cout << "header " << header.header_id << " = " << name << std::endl;
        header_level.insert({{ header.header_id, {}}});
    }
    for (int level_id = 0; auto& level : processor_manager->level_manager->levels) {
        for (auto & entry : level.entries) {
            if (entry.parser_sram_entry.accept_drop < 2) { continue; }
            int header_id = entry.parser_sram_entry.header_id;
            auto& temp = header_level[header_id];
            if (std::find(std::begin(temp), std::end(temp), level_id) == std::end(temp)) {
                temp.push_back(level_id);
            }
        }
        level_id++;
    }
    // std::cout << "headers = " << std::endl;
    // for (auto& [name, header] : processor_manager->header_manager->headers) {
    //     std::cout << header.header_id << " = " << name << std::endl;
    // }
    // std::cout << "header_level = ";
    // for (auto& [a, b] : header_level) {
    //     std::cout << a << " : ";
    //     for (int c : b) {
    //         std::cout << c << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // topological sort of levels
    std::vector<int> level_topological;
    std::map<int, std::vector<int>> level_last_table;
    std::map<int, std::vector<int>> level_next_table;
    for (int i = 0; i < processor_manager->level_manager->levels.size(); i++) {
        std::vector<int> temp;
        for (auto& entry : processor_manager->level_manager->levels[i].entries) {
            if (entry.parser_sram_entry.accept_drop < 2) { continue; }
            int next_level_id = entry.parser_sram_entry.next_state;
            if (std::find(std::begin(temp), std::end(temp), next_level_id) == std::end(temp)) {
                temp.push_back(next_level_id);
            }
        }
        level_next_table.insert({{i, std::move(temp)}});
        level_last_table.insert({{i, {}}});
    }
    for (auto& [level_id, next_level_ids] : level_next_table) {
        for (int next_level_id : next_level_ids) {
            level_last_table[next_level_id].push_back(level_id);
        } 
    }
    topologicalSort(level_last_table, level_next_table, level_topological);
    // std::cout << "level topo = ";
    // for (int x : level_topological) {
    //     std::cout << x << " ";
    // }
    // std::cout << std::endl;
    // level_id -> listof(level_id)
    std::map<int, std::vector<int>> level_level;
    for (int i = 0; i < processor_manager->level_manager->levels.size(); i++) {
        level_level.insert({{i, {i}}});
    }
    for (int level_id : level_topological) {
        auto& pred_levels = level_level[level_id];
        for (int pred_id : level_last_table.at(level_id)) {
            for (int level : level_level.at(pred_id)) {
                if (std::find(std::begin(pred_levels), std::end(pred_levels), level) == std::end(pred_levels)) {
                    pred_levels.push_back(level);
                }
            } 
        }
    }
    // std::cout << "level_level = ";
    // for (auto& [a, b] : level_level) {
    //     std::cout << a << " : ";
    //     for (int c : b) {
    //         std::cout << c << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // proc_id -> listof(level_id)
    std::map<int, std::vector<int>> proc_level;
    for (auto& stage : processor_manager->stage_manager->logical_stages) {
        if (stage.removed) { continue; }
        std::vector<int> temp;
        for (auto& header : stage.def->parser.headers) {
            int header_id = processor_manager->header_manager->get_header(header.name)->header_id;
            for (int level_id : header_level[header_id]) {
                for (int level : level_level[level_id]) {
                    if (std::find(std::begin(temp), std::end(temp), level) == std::end(temp)) {
                        temp.push_back(level);
                    }
                }
            }
        }
        proc_level.insert({{
            stage.stage_id, std::move(temp)
        }});
    }
    // std::cout << "proc_level = ";
    // for (auto& [a, b] : proc_level) {
    //     std::cout << a << " : ";
    //     for (int c : b) {
    //         std::cout << c << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // topological sort of processors
    std::map<int, std::vector<int>> last_table;
    std::map<int, std::vector<int>> next_table;
    for (auto& stage : processor_manager->stage_manager->logical_stages) {
        if (stage.removed) { continue; }
        std::vector<int> temp;
        for (auto [act, proc] : stage.action_proc) {
            if (proc < 0) { continue; }
            if (std::find(std::begin(temp), std::end(temp), proc) == std::end(temp)) {
                // output.emit(processor_manager->action_manager->lookup(act)->toIpsaValue());
                temp.push_back(proc);
            }
        }
        next_table.insert({{stage.stage_id, std::move(temp)}});
        last_table.insert({{stage.stage_id, {}}});
    }
    for (auto& [proc_id, next_proc_ids] : next_table) {
        for (int next_proc_id : next_proc_ids) {
            last_table[next_proc_id].push_back(proc_id);
        } 
    }
    topologicalSort(last_table, next_table, topo_sequence);
    // distribute levels
    std::map<int, std::vector<int>> total_levels;
    for (int proc_id : topo_sequence) {
        std::vector<int> temp;
        std::vector<int> total;
        auto& last_proc_ids = last_table[proc_id];
        auto& level_ids = proc_level[proc_id];
        if (last_proc_ids.size() == 0) {
            // first processor
            std::copy(std::begin(level_ids), std::end(level_ids), std::back_inserter(temp));
            std::copy(std::begin(temp), std::end(temp), std::back_inserter(total));
        } else {
            auto& one_last_total = total_levels[last_proc_ids[0]];
            for (int x : one_last_total) {
                if (last_proc_ids.size() == 1 || std::all_of(
                    std::next(std::begin(last_proc_ids), 1), std::end(last_proc_ids), [&](auto& last_proc_id) {
                    auto& total_level = total_levels[last_proc_id];
                    return std::find(std::begin(total_level), std::end(total_level), x) != std::end(total_level);
                })) {
                    total.push_back(x);
                }
            }
            for (int level_id : level_ids) {
                if (std::find(std::begin(total), std::end(total), level_id) == std::end(total)) {
                    temp.push_back(level_id);
                    total.push_back(level_id);
                }
            }
        }
        distributed_levels.insert({{proc_id, std::move(temp)}});
        total_levels.insert({{proc_id, std::move(total)}});
    }
    // for (int x : topo_sequence) {
    //     std::cout << x << " = " << processor_manager->stage_manager->logical_stages[x].def->name << " ";
    //     for (int y : distributed_levels.at(x)) {
    //         std::cout << y << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // final topological sort
    auto cmp = [&](int x, int y) {
        for (int z : level_topological) {
            if (z == x) { return true; }
            else if (z == y) { return false; }
        }
        return true;
    };
    for (auto& [proc_id, seq] : distributed_levels) {
        std::sort(std::begin(seq), std::end(seq), cmp);
    }
}



