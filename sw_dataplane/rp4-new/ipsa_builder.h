#pragma once

#include "ipsa_action_manager.h"
#include "ipsa_base.h"
#include "ipsa_gateway_manager.h"
#include "ipsa_level_manager.h"
#include "ipsa_processor_manager.h"
#include "ipsa_table_manager.h"

#include <bitset>

class IpsaParser : public IpsaModule {
public:
    int parser_level = 0;
    std::vector<std::shared_ptr<IpsaValue>> levels;
    IpsaParser() {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"parser_level", makeValue(parser_level)},
                {"levels", makeValue(levels)}};
        return makeValue(dst);
    }
};

class IpsaProcessor : public IpsaModule {
public:
    int id;
    std::string name;
    IpsaParser parser;
    IpsaGateway *gateway = nullptr;
    std::vector<IpsaTable *> matcher;
    std::vector<const IpsaAction *> executor;
    int cluster_belongs = -1;

    IpsaProcessor(int _id) : id(_id) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"id", makeValue(id)},
                {"name", makeValue(name)},
                {"parser", parser.toIpsaValue()},
                {"gateway", gateway->toIpsaValue()}};
        std::vector<std::shared_ptr<IpsaValue>> matcher_list;
        for (auto table: matcher) {
            matcher_list.push_back(table->toIpsaValue());
            table->set_changed();
        }
        dst.insert({{"matcher", makeValue(matcher_list)}});
        std::vector<std::shared_ptr<IpsaValue>> executor_list;
        for (auto action: executor) {
            executor_list.push_back(action->toIpsaValue());
        }
        dst.insert({{"executor", makeValue(executor_list)}});
        return makeValue(dst);
    }
};

class IpsaRealProcessor : public IpsaModule {
public:
    int next_processor;
    std::vector<std::shared_ptr<IpsaProcessor>> processors;

    IpsaRealProcessor() : next_processor(-1) {}

    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"next_processor", makeValue(next_processor)}};
        std::vector<std::shared_ptr<IpsaValue>> processor_list;
        for (auto &processor: processors) {
            processor_list.push_back(processor->toIpsaValue());
        }
        dst.insert({"stage_list", makeValue(processor_list)});
        return makeValue(dst);
    }
};

class Ipsa : public IpsaModule {
public:
    int initial_processor = 0;
    int initial_stage = 0;
    const IpsaMetadata *metadata;
    std::vector<std::shared_ptr<IpsaRealProcessor>> real_processors;
    Ipsa(const IpsaMetadata *_metadata) : metadata(_metadata) {
        real_processors.resize(ipsa_configuration::PROC_COUNT);
        for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++)
            real_processors[i] = std::make_shared<IpsaRealProcessor>();
    }
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"initial_processor", makeValue(initial_processor)},
                {"initial_stage", makeValue(initial_stage)},
                {"metadata", metadata->toIpsaValue()},
        };
        for (int i = 0; i < ipsa_configuration::PROC_COUNT; i++) {
            dst.insert({{"processor_" + std::to_string(i),
                         real_processors[i]->processors.size() ? real_processors[i]->toIpsaValue() : nullptr}});
        }
        return makeValue(dst);
    }
};

class IpsaBuilder {
public:
    IpsaBase &base;
    Ipsa ipsa;
    IpsaProcessorManager processor_manager;
    IpsaGatewayManager gateway_manager;
    IpsaBuilder(IpsaBase &_base) : base(_base),
                                   gateway_manager(),
                                   processor_manager(
                                           &base.stage_manager,
                                           &base.header_manager,
                                           &base.level_manager,
                                           &base.action_manager,
                                           &base.table_manager),
                                   ipsa(&(base.header_manager.Metadata())) {
        processor_manager.initializeStages();
        gateway_manager.load(&base.header_manager,
                             &base.stage_manager,
                             &base.table_manager);
        processor_manager.setupStages(&gateway_manager);
    }

    std::vector<std::shared_ptr<IpsaProcessor>> processors;
    std::map<int, std::pair<int, int>> proc_space;
    std::map<int, int> proc_cluster;
    std::map<int, int> processor_maps;

    void allocateMemory() {
        // set unchanged configs
        for (auto &[name, stage]: processor_manager.stage_manager->logical_stages) {
            for (int table_id: stage.table_id) {
                auto table = tables[table_id];
                if (table->changed)
                    continue;
                auto &key_memory = table->key_memory;
                if (key_memory.type == MEM_TCAM) {
                    for (int i: key_memory.config) {
                        clusters[proc_cluster[stage.stage_id]].tcam[i] = true;
                    }
                } else {
                    for (int i: key_memory.config) {
                        clusters[proc_cluster[stage.stage_id]].sram[i] = true;
                    }
                }
                for (int i: table->value_memory.config) {
                    clusters[proc_cluster[stage.stage_id]].sram[i] = true;
                }
            }
        }

        // set configs
        for (auto &[name, stage]: processor_manager.stage_manager->logical_stages) {
            for (int table_id: stage.table_id) {
                auto table = tables[table_id];
                if (!table->changed)
                    continue;
                auto &key_memory = table->key_memory;
                if (key_memory.type == MEM_TCAM) {
                    for (int i = 0; i < key_memory.depth * key_memory.width; i++) {
                        key_memory.config.push_back(searchFree(proc_cluster[stage.stage_id], false));
                    }
                } else {
                    for (int i = 0; i < key_memory.depth * key_memory.width; i++) {
                        key_memory.config.push_back(searchFree(proc_cluster[stage.stage_id], true));
                    }
                }
                auto &value_memory = table->value_memory;
                for (int i = 0; i < value_memory.depth * value_memory.width; i++) {
                    value_memory.config.push_back(searchFree(proc_cluster[stage.stage_id], true));
                }
            }
        }
    }
    void calculateTableSpace() {
        // table_id -> space
        for (auto &[name, table]: processor_manager.table_manager->tables) {
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
        for (auto &[name, stage]: processor_manager.stage_manager->logical_stages) {
            int sram_number = 0;
            int tcam_number = 0;
            for (int table_id: stage.table_id) {
                sram_number += table_space[table_id].first;
                tcam_number += table_space[table_id].second;
            }
            proc_space.insert({{stage.stage_id, {sram_number, tcam_number}}});
        }
    }
    void allocateLogicalProcessors() {
        for (auto &[name, stage]: processor_manager.stage_manager->logical_stages) {
            int id = stage.stage_id;
            processor_maps[id] = processors.size();
            processors.push_back(std::make_shared<IpsaProcessor>(id));
            auto &processor = processors[processor_maps[id]];

            processor->name = stage.def.name;
            // gateway
            processor->gateway = &(gateway_manager.gateways[stage.gateway_id]);
            // matcher
            for (int table_id: stage.table_id) {
                processor->matcher.push_back(base.table_manager.lookup(table_id));
            }
            for (auto matcher: processor->matcher) {
                if (!matcher->changed)
                    processor->cluster_belongs = base.stage_manager.lookup(processor->id)->stage_belongs;
            }

            // executor
            for (auto [action_id, _]: stage.action_proc) {
                processor->executor.push_back(base.action_manager.lookup(action_id));
            }
        }
        total_stage = processors.size();
        headerPretreatment();
        processorPretreatment();
        int max_id = 0;
        for (auto processor: processors) {
            if (processor->id > max_id)
                max_id = processor->id;
        }
        for (int num = 0; num < total_stage; num++) {
            std::vector<std::vector<int>> parser_levels;
            setParsers(num, parser_levels);
            if (!parser_levels.size())
                continue;
            int len = parser_levels.size() - 1;
            if (len / ipsa_configuration::MAX_LEVEL) {
                int target = max_id;
                for (int i = 0; i < len / ipsa_configuration::MAX_LEVEL; i++) {
                    processor_maps[++max_id] = processors.size();
                    processors.push_back(std::make_shared<IpsaProcessor>(max_id));
                }
                std::shared_ptr<IpsaProcessor> temp = processors[num];
                int temp_id = processors[num]->id;
                processors[num] = processors[processor_maps[max_id]];
                processors[processor_maps[max_id]] = temp;
                processors[processor_maps[max_id]]->id = processors[num]->id;
                processors[num]->id = temp_id;
                auto &parser = processors[num]->parser;
                for (int k = 0; k < ipsa_configuration::MAX_LEVEL; k++) {
                    parser.parser_level++;
                    for (int level_id: parser_levels[k]) {
                        parser.levels.push_back(
                                base.level_manager.levels[level_id].toIpsaValue());
                    }
                }
                for (int k = ipsa_configuration::MAX_LEVEL; k < parser_levels.size(); k++) {
                    auto &parser = processors[processor_maps[target + k / ipsa_configuration::MAX_LEVEL]]->parser;
                    parser.parser_level++;
                    for (int level_id: parser_levels[k]) {
                        parser.levels.push_back(
                                base.level_manager.levels[level_id].toIpsaValue());
                    }
                }
                auto &gateway = processors[num]->gateway = new IpsaGateway();
                gateway->next_table.default_entry = std::make_shared<IpsaGatewayStageEntry>(++target);
                for (int i = 0; i < len / ipsa_configuration::MAX_LEVEL - 1; i++) {
                    auto &gateway = processors[processor_maps[target]]->gateway = new IpsaGateway();
                    gateway->next_table.default_entry = std::make_shared<IpsaGatewayStageEntry>(++target);
                }
            } else {
                auto &parser = processors[num]->parser;
                for (int k = 0; k < parser_levels.size(); k++) {
                    parser.parser_level++;
                    for (int level_id: parser_levels[k]) {
                        parser.levels.push_back(
                                base.level_manager.levels[level_id].toIpsaValue());
                    }
                }
            }
            parser_levels.clear();
        }
    }

private:
    std::map<int, IpsaTable *> tables;
    std::map<int, std::pair<int, int>> table_space;
    struct Level {
        std::bitset<ipsa_configuration::MAX_HEADER> next, last, needed;
        bool pretreat = false;
    };
    struct Stage {
        std::bitset<ipsa_configuration::MAX_STAGE> last, next;
        std::bitset<ipsa_configuration::MAX_HEADER> header, parser, parserd, pre_parserd;
        bool pretreat = false;
    };
    struct Cluster {
        bool sram[ipsa_configuration::CLUSTER_SRAM_COUNT] = {0};
        bool tcam[ipsa_configuration::CLUSTER_TCAM_COUNT] = {0};
    };
    int total_state, total_stage;
    Cluster clusters[ipsa_configuration::CLUSTER_COUNT];
    struct Level level[ipsa_configuration::MAX_HEADER];
    struct Stage stage[ipsa_configuration::MAX_STAGE];
    int header_to_level[ipsa_configuration::MAX_HEADER] = {0};
    // std::bitset<ipsa_configuration::MAX_HEADER> set_header_needed(int state);
    void setProcessorParserd(int processor) {
        if (stage[processor].pretreat)
            return;
        for (int i = 1; i < total_state; i++)
            if (stage[processor].header[i])
                stage[processor].parser |= level[i].needed;
        stage[processor].parserd = stage[processor].parser;
        for (int i = 0; i < total_stage; i++) {
            if (stage[processor].last[i]) {
                setProcessorParserd(i);
                if (stage[processor].pre_parserd.any())
                    stage[processor].pre_parserd &= stage[i].parserd;
                else
                    stage[processor].pre_parserd = stage[i].parserd;
            }
        }
        stage[processor].parserd |= stage[processor].pre_parserd;
        stage[processor].parser &= (~stage[processor].pre_parserd);
        stage[processor].pretreat = true;
    }
    // void headerPretreatment();
    // void processorPretreatment();
    // void setParsers(int id, std::vector<std::vector<int>> &parser_levels);
    void push(std::bitset<ipsa_configuration::MAX_STAGE> &a, int b) {
        if (b == -1)
            return;
        a[b] = 1;
    }
    int searchFree(int cluster_num, bool sram) {
        if (sram) {
            for (int i = 0; i < ipsa_configuration::CLUSTER_SRAM_COUNT; i++)
                if (!clusters[cluster_num].sram[i]) {
                    clusters[cluster_num].sram[i] = true;
                    return i;
                }
        } else {
            for (int i = 0; i < ipsa_configuration::CLUSTER_SRAM_COUNT; i++)
                if (!clusters[cluster_num].tcam[i]) {
                    clusters[cluster_num].tcam[i] = true;
                    return i;
                }
        }
    }

    void setParsers(int id, std::vector<std::vector<int>> &parser_levels) {
        std::map<int, std::bitset<ipsa_configuration::MAX_HEADER>> topo_headers;
        for (int i = 0; i < total_state; i++) {
            if (stage[id].parser[i])
                topo_headers.insert({i, level[i].needed & (~stage[id].pre_parserd)});
        }
        int k = 0;
        while (topo_headers.size()) {
            parser_levels.push_back({});
            for (auto &[header, needed]: topo_headers) {
                if (needed.none()) {
                    parser_levels[k].push_back(header);
                }
            }
            for (auto &parsed: parser_levels[k]) {
                topo_headers.erase(parsed);
            }
            for (auto &parsed: parser_levels[k]) {
                for (auto &[header, needed]: topo_headers) {
                    needed[parsed] = 0;
                }
            }
            k++;
        }
    }

    std::bitset<ipsa_configuration::MAX_HEADER> set_header_needed(int state) {
        if (level[state].pretreat)
            return level[state].needed;
        level[state].needed = level[state].last;
        for (int i = 0; i < total_state; i++) {
            if (level[state].last[i]) {
                level[state].needed |= set_header_needed(i);
            }
        }
        level[state].pretreat = true;
        return level[state].needed;
    }

    void headerPretreatment() {
        int state = 0;
        total_state = processor_manager.level_manager->levels.size();
        for (auto &Level: processor_manager.level_manager->levels) {
            for (auto &Entry: Level.entries) {
                if (Entry.parser_sram_entry.accept_drop < 2)
                    continue;
                int next = Entry.parser_sram_entry.next_state;
                level[state].next[next] = 1;
                level[next].last[state] = 1;
                header_to_level[Entry.parser_sram_entry.header_id] = next;
            }
            state++;
        }
        for (int i = 0; i < total_state; i++) {
            set_header_needed(i);
        }
    }

    void processorPretreatment() {
        for (auto &processor: processors) {
            auto &next_table = processor->gateway->next_table;
            if (next_table.default_entry.get() != nullptr) {
                if (next_table.default_entry->type == GTW_ET_STAGE) {
                    if (next_table.default_entry->getId() != -1) {
                        push(stage[processor_maps[processor->id]].next, processor_maps[next_table.default_entry->getId()]);
                        push(stage[processor_maps[next_table.default_entry->getId()]].last, processor_maps[processor->id]);
                    }
                }
            }
            for (auto &entry: next_table.entries) {
                if (entry.value->type == GTW_ET_STAGE) {
                    if (entry.value->getId() != -1) {
                        push(stage[processor_maps[processor->id]].next, processor_maps[entry.value->getId()]);
                        push(stage[processor_maps[entry.value->getId()]].last, processor_maps[processor->id]);
                    }
                }
            }
            for (auto &matcher: processor->matcher) {
                for (auto &action_to_proc: matcher->action_to_proc) {
                    if (action_to_proc.proc_id != -1) {
                        push(stage[processor_maps[processor->id]].next, processor_maps[action_to_proc.proc_id]);
                        push(stage[processor_maps[action_to_proc.proc_id]].last, processor_maps[processor->id]);
                    }
                }
            }
        }
        for (auto &[name, Stage]: processor_manager.stage_manager->logical_stages) {
            for (auto &header: Stage.def.parser.headers) {
                int header_id = processor_manager.header_manager->get_header(header.name)->header_id;
                stage[processor_maps[Stage.stage_id]].header[header_to_level[header_id]] = 1;
            }
        }
        for (int i = 0; i < total_stage; i++)
            setProcessorParserd(i);
    }
};