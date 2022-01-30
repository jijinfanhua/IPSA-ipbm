#pragma once

#include "rp4_compiler.h"
#include "ipsa_output.h"
#include "ipsa_builder.h"
#include "ipsa_tableupdate.h"
#include <fstream>

class Rp4Task {
public:
    IpsaBuilder builder;
    Rp4Compiler compiler;
    Rp4Task() {}
    void allocateParsers() { builder.allocateParsers(); }
    void allocateMemory() { builder.allocateMemory(); }
    void allocateProcessors() { builder.allocateProcessors(); }
    int load(std::string rp4_filename) {
        if (int x = compiler.parse(rp4_filename.c_str()); x == 0) {
            builder.load(compiler.ast.get());
            return 0;
        } else {
            return x;
        }
    }
    void output_all(std::string json_filename) {
        std::ofstream output(json_filename.c_str());
        IpsaOutput out(output);
        out.emit(builder.ipsa.toIpsaValue());
        output.close();
    }
};

class Rp4Runtime {
public:
    std::unique_ptr<Rp4Task> now_task;
    std::unique_ptr<Rp4Task> next_task;
    std::string update_json_filename = "update.json";
    Rp4Runtime(): now_task(nullptr), next_task(nullptr) {}
    void emitTask(std::string rp4_filename, std::string json_filename);
    void emitUpdate(std::vector<std::string> params);
};

void Rp4Runtime::emitTask(std::string rp4_filename, std::string json_filename) {
    if (now_task.get() == nullptr) {
        // never be called previously
        now_task = std::make_unique<Rp4Task>();
        now_task->load(rp4_filename);
        now_task->allocateParsers();
        now_task->allocateMemory();
        now_task->allocateProcessors();
        now_task->output_all(json_filename);
    } else {
        // has been called previously
        next_task = std::make_unique<Rp4Task>();
        next_task->load(rp4_filename);
        next_task->allocateParsers();
        if (!next_task->builder.allocateMemory(now_task->builder)) {
            next_task->builder.memory.removeAllEntries();
            next_task->allocateMemory();
        } else {
            next_task->builder.ipsa.is_incremental = 1;
        }
        next_task->allocateProcessors();
        next_task->output_all(json_filename);
        now_task = std::move(next_task);
    }
}

void Rp4Runtime::emitUpdate(std::vector<std::string> params) {
    std::ofstream output(update_json_filename);
    IpsaOutput out(output);
    if (now_task.get() == nullptr) {
        return; // does not has a rp4 task
    }
    // parse update type
    auto cmd = params[0];
    IpsaTableUpdateType type = (
        cmd == "table_add" ? IpsaTableUpdateType::TBL_UPD_ADD : (
        cmd == "table_mod" ? IpsaTableUpdateType::TBL_UPD_MOD : (
        cmd == "table_del" ? IpsaTableUpdateType::TBL_UPD_DEL :
                             IpsaTableUpdateType::TBL_UPD_ADD
    )));
    // parse update table
    auto table_name = params[1];
    auto table = now_task->builder.table_manager.lookup(table_name);
    int proc_id = table->proc_id;
    int matcher_id = table->id;
    // parse action
    auto action_name = params[2];
    auto action = now_task->builder.action_manager.lookup(action_name);
    int action_id = action->id;
    IpsaTableUpdater updater(type, proc_id, matcher_id, action_id);
    updater.match_type = table->match_type;
    // parse keys
    int i;
    for (i = 3; i < params.size(); i++) {
        if (params[i] == "=>") { i++; break; }
        int width = 32, index = i - 3;
        if (index < table->key_width_vec.size()) {
            width = table->key_width_vec[index];
        }
        updater.keys.push_back(IpsaTableUpdateKey(params[i], width));
    }
    // parse action_paras
    for (int j = i; j < params.size(); j++) {
        int width = 32, index = j - i;
        if (index < action->action_parameters_lengths.size()) {
            width = action->action_parameters_lengths[index];
        }
        updater.action_para.push_back(IpsaTableUpdateKey(params[j], width).abandonMask());
    }
    out.emit(updater.toIpsaValue());
    output.close();
}