#pragma once

#include "ipsa_builder.h"
#include "ipsa_configuration.h"
#include "ipsa_stage_manager.h"

#include <algorithm>
#include <bitset>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

extern int num;
extern int begin_with[ipsa_configuration::MAX_PORT];

using namespace std;

class IpsaAllocate {
public:
    IpsaStageManager &stage_manager;
    vector<shared_ptr<IpsaProcessor>> &processors;
    map<int, std::pair<int, int>> &proc_space;
    vector<std::shared_ptr<IpsaRealProcessor>> &real_processors;
    map<int, int> &proc_cluster;
    int &initial_processor;
    int &initial_stage;
    std::map<int, int> &processor_maps;
    IpsaAllocate(vector<std::shared_ptr<IpsaRealProcessor>> &_real_processors,
                 vector<shared_ptr<IpsaProcessor>> &_processors,
                 map<int, pair<int, int>> &_proc_space,
                 map<int, int> &_proc_cluster,
                 IpsaStageManager &_stageManager,
                 int &_initial_processor,
                 int &_initial_stage,
                 std::map<int, int> &_processor_maps) : real_processors(_real_processors),
                                                        processors(_processors),
                                                        proc_space(_proc_space),
                                                        proc_cluster(_proc_cluster),
                                                        stage_manager(_stageManager),
                                                        initial_processor(_initial_processor),
                                                        initial_stage(_initial_stage),
                                                        processor_maps(_processor_maps) {}
    void allocatePhysicalProcessors() {
        allocateProcessors();
    }

    map<int, int> &getProcessorCluster() {
        return proc_cluster;
    }

private:
    struct Stage {
        int sram, tcam;
        bitset<ipsa_configuration::MAX_STAGE> last_stage, next_stage, stage_needed;
    };

    struct Processor {
        vector<int> stage;
        int next = -1;

        void toString() {
            for (int i = 0; i < stage.size(); i++) {
                printf("STAGE %d = %d\n", i, stage[i]);
            }
            cout << endl;
        }
    };

    struct Cluster {
        vector<struct Processor> processor;
        int sram = ipsa_configuration::CLUSTER_SRAM_COUNT;
        int tcam = ipsa_configuration::CLUSTER_TCAM_COUNT;

        void toString() {
            printf("SRAM: %d\tTCAM: %d\n", sram, tcam);
            for (int i = 0; i < processor.size(); i++) {
                cout << "PROCESSOR: " << i << endl;
                processor[i].toString();
            }
        }
    };

    struct State {
        vector<struct Cluster> cluster;
        int value;
        int cal() {
            int sum = 0;
            sum += (ipsa_configuration::CLUSTER_COUNT - cluster.size()) * ipsa_configuration::CLUSTER_PROC_COUNT * (ipsa_configuration::CLUSTER_SRAM_COUNT + 1) * (ipsa_configuration::CLUSTER_TCAM_COUNT + 1);
            for (auto &clus: cluster) {
                sum += (ipsa_configuration::CLUSTER_PROC_COUNT - clus.processor.size()) * (clus.tcam + 1) * (clus.sram + 1);
            }
            value = sum;
            return sum;
        }
        void toString() {
            for (int i = 0; i < cluster.size(); i++) {
                cout << "CLUSTER: " << i << endl;
                cluster[i].toString();
            }
        }
    };

    Stage stage[ipsa_configuration::CLUSTER_COUNT * ipsa_configuration::CLUSTER_PROC_COUNT * ipsa_configuration::PROC_STAGE_COUNT];

    void push(bitset<ipsa_configuration::MAX_STAGE> &a, int b) {
        if (b == -1)
            return;
        a[b] = 1;
    }

    bitset<ipsa_configuration::MAX_STAGE> &set_stage_needed(Stage stage[], int stage_id, int total_state) {
        if (stage[stage_id].stage_needed.any())
            return stage[stage_id].stage_needed;
        stage[stage_id].stage_needed = stage[stage_id].last_stage;
        for (int i = 0; i < total_state; i++) {
            if (stage[stage_id].last_stage[i]) {
                stage[stage_id].stage_needed |= set_stage_needed(stage, i, total_state);
            }
        }
        return stage[stage_id].stage_needed;
    }

    void topologicalSort(int total_stage, Stage stage[], vector<int> &topo_sequence) {
        vector<int> in_degree;
        for (int i = 0; i < total_stage; i++) {
            in_degree.push_back(0);
            for (int j = 0; j < total_stage; j++)
                if (stage[i].last_stage[j])
                    in_degree[i]++;
        }
        while (topo_sequence.size() < total_stage) {
            for (int i = 0; i < total_stage; i++)
                if (in_degree[i] == 0) {
                    topo_sequence.push_back(i);
                    in_degree[i] = -1;
                    for (int j = 0; j < total_stage; j++)
                        if (stage[i].next_stage[j])
                            in_degree[j]--;
                    break;
                }
        }
    }

    bool check_sequence(State &state) {
        unordered_map<bitset<ipsa_configuration::MAX_STAGE>, bitset<ipsa_configuration::MAX_STAGE>> pro_sequence;
        unordered_map<bitset<ipsa_configuration::MAX_STAGE>, int> stage_sum;
        for (int clus = 0; clus < state.cluster.size(); clus++) {
            for (int pro_num = 0; pro_num < state.cluster[clus].processor.size(); pro_num++) {
                const auto &pro = state.cluster[clus].processor[pro_num];
                bitset<ipsa_configuration::MAX_STAGE> sum_stage(0), sum_stage_needed(0);
                for (int i = 0; i < pro.stage.size(); i++) {
                    push(sum_stage, pro.stage[i]);
                    sum_stage_needed |= stage[pro.stage[i]].stage_needed;
                }
                pro_sequence[sum_stage] = sum_stage_needed;
                stage_sum[sum_stage] = clus * ipsa_configuration::CLUSTER_PROC_COUNT + pro_num;
            }
        }

        int pre = -1;
        while (pro_sequence.size()) {
            bool del = false;
            for (auto &[sum_stage, sum_stage_needed]: pro_sequence) {
                if (sum_stage_needed == 0) {
                    for (auto &[temp, sum_stage_needed]: pro_sequence) {
                        sum_stage_needed &= (~sum_stage);
                    }
                    if (pre != -1) {
                        state.cluster[pre / ipsa_configuration::CLUSTER_PROC_COUNT].processor[pre % ipsa_configuration::CLUSTER_PROC_COUNT].next = stage_sum[sum_stage];
                    }
                    pre = stage_sum[sum_stage];
                    stage_sum.erase(sum_stage);
                    pro_sequence.erase(sum_stage);
                    del = true;
                    break;
                }
            }
            if (!del)
                return false;
        }
        return true;
    }

    void check(int stage_id, State state, int clus, int pro, vector<State> &bfs) {
        while (clus >= state.cluster.size())
            state.cluster.push_back(Cluster());
        if (pro == state.cluster[clus].processor.size())
            state.cluster[clus].processor.push_back(Processor());
        if (state.cluster[clus].sram < stage[stage_id].sram || state.cluster[clus].tcam < stage[stage_id].tcam)
            return;
        state.cluster[clus].sram -= stage[stage_id].sram;
        state.cluster[clus].tcam -= stage[stage_id].tcam;

        if (state.cluster[clus].processor[pro].stage.size() == ipsa_configuration::PROC_STAGE_COUNT)
            return;
        for (auto state_stage: state.cluster[clus].processor[pro].stage) {
            if (stage[state_stage].stage_needed[stage_id] || stage[stage_id].stage_needed[state_stage])
                return;
        }
        state.cluster[clus].processor[pro].stage.push_back(stage_id);
        if (check_sequence(state)) {
            state.cal();
            bfs.push_back(state);
        }
    }

    struct cmp {
        bool operator()(const State &a, const State &b) const {
            return a.value > b.value;
        }
    };

    void allocateProcessors() {
        int total_stage = processors.size();
        for (auto &processor: processors) {
            stage[processor_maps[processor->id]].sram = proc_space[processor->id].first;
            stage[processor_maps[processor->id]].tcam = proc_space[processor->id].second;
            auto &next_table = processor->gateway->next_table;
            if (next_table.default_entry.get() != nullptr) {
                if (next_table.default_entry->type == GTW_ET_STAGE) {
                    if (next_table.default_entry->getId() != -1) {
                        push(stage[processor_maps[processor->id]].next_stage, processor_maps[next_table.default_entry->getId()]);
                        push(stage[processor_maps[next_table.default_entry->getId()]].last_stage, processor_maps[processor->id]);
                    }
                }
            }
            for (auto &entry: next_table.entries) {
                if (entry.value->type == GTW_ET_STAGE) {
                    if (entry.value->getId() != -1) {
                        push(stage[processor_maps[processor->id]].next_stage, processor_maps[entry.value->getId()]);
                        push(stage[processor_maps[entry.value->getId()]].last_stage, processor_maps[processor->id]);
                    }
                }
            }
            for (auto &matcher: processor->matcher) {
                for (auto &action_to_proc: matcher->action_to_proc) {
                    if (action_to_proc.proc_id == -1)
                        continue;
                    push(stage[processor_maps[processor->id]].next_stage, processor_maps[action_to_proc.proc_id]);
                    push(stage[processor_maps[action_to_proc.proc_id]].last_stage, processor_maps[processor->id]);
                }
            }
        }
        for (int i = 0; i < total_stage; i++)
            set_stage_needed(stage, i, total_stage);
        vector<int> topo_sequence;
        topologicalSort(total_stage, stage, topo_sequence);
        vector<State> bfs[total_stage];

        /* TODO: if the first one in the second cluster? */
        check(topo_sequence[0], State(), 0, 0, bfs[0]);

        for (int i = 1; i < total_stage; i++) {
            vector<State> bfs_now;
            for (auto &state: bfs[i - 1]) {
                auto temp = stage_manager.lookup(processors[topo_sequence[i]]->name);
                if (temp && temp->stage_belongs != -1) {
                    if (state.cluster.size() <= temp->stage_belongs)
                        check(topo_sequence[i], state, temp->stage_belongs, 0, bfs_now);
                    else {
                        for (int pro = 0; pro < state.cluster[temp->stage_belongs].processor.size(); pro++)
                            check(topo_sequence[i], state, temp->stage_belongs, pro, bfs_now);
                        if (state.cluster[temp->stage_belongs].processor.size() < ipsa_configuration::CLUSTER_PROC_COUNT)
                            check(topo_sequence[i], state, temp->stage_belongs, state.cluster[temp->stage_belongs].processor.size(), bfs_now);
                    }
                    continue;
                }
                for (int clus = 0; clus < state.cluster.size(); clus++) {
                    for (int pro = 0; pro < state.cluster[clus].processor.size(); pro++)
                        check(topo_sequence[i], state, clus, pro, bfs_now);
                    if (state.cluster[clus].processor.size() < ipsa_configuration::CLUSTER_PROC_COUNT)
                        check(topo_sequence[i], state, clus, state.cluster[clus].processor.size(), bfs_now);
                }
                if (state.cluster.size() < ipsa_configuration::CLUSTER_COUNT)
                    check(topo_sequence[i], state, state.cluster.size(), 0, bfs_now);
            }
            sort(bfs_now.begin(), bfs_now.end(), IpsaAllocate::cmp());
            double t = 1.0;
            default_random_engine random(time(nullptr));
            uniform_real_distribution dis(0.0, 1.0);
            int sum = 0;
            if (bfs_now.size() < 500) {
                for (auto state: bfs_now)
                    bfs[i].push_back(state);
            } else {
                for (auto &state: bfs_now) {
                    if (dis(random) < t) {
                        bfs[i].push_back(State(state));
                        t *= 0.99;
                        sum++;
                        if (sum == 1000) {
                            break;
                        }
                    }
                }
            }
            bfs_now.clear();
            bfs[i - 1].clear();
        }
        if (!bfs[total_stage - 1].size()) {
            cout << "Error, cannot allocate processors!" << endl;
        } else {
            int maxx = -1;
            State *ans;
            for (auto &state: bfs[total_stage - 1]) {
                if (state.cal() > maxx) {
                    ans = &state;
                    maxx = state.cal();
                }
            }
            ans->toString();
            cout << ans->cal() << endl;
            for (auto [a, b] : processor_maps) {
                if (b == topo_sequence[0]) {
                    initial_stage = a;
                    begin_with[num] = initial_stage;
                    break;
                }
            }
            for (int i = 0; i < ans->cluster.size(); i++) {
                for (int j = 0; j < ans->cluster[i].processor.size(); j++) {
                    real_processors[i * ipsa_configuration::CLUSTER_PROC_COUNT + j]->next_processor = ans->cluster[i].processor[j].next;
                    for (int k: ans->cluster[i].processor[j].stage) {
                        real_processors[i * ipsa_configuration::CLUSTER_PROC_COUNT + j]->processors.push_back(processors[k]);
                        proc_cluster.insert({{k, i}});
                        if (auto temp = stage_manager.lookup(processors[k]->name))
                            temp->stage_belongs = i;
                        if (k == topo_sequence[0])
                            initial_processor = i * ipsa_configuration::CLUSTER_PROC_COUNT + j;
                    }
                }
            }
            bfs[total_stage - 1].clear();
        }
    }
};