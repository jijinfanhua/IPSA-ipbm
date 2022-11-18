#pragma once

#include "ipsa_allocate.h"
#include "ipsa_builder.h"
#include "ipsa_output.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <grpcpp/grpcpp.h>
#include <google/protobuf/text_format.h>
#include <glog/logging.h>

using namespace std;

int yyparse();
extern int yydebug;
IpsaBase base[ipsa_configuration::MAX_PORT];
int num;
int begin_with[ipsa_configuration::MAX_PORT];
std::unique_ptr<Rp4Ast> ast[ipsa_configuration::MAX_PORT];
std::unique_ptr<Rp4Ast> tree;
extern FILE *yyin;
void insertConfig(int port, std::string input_filename);
void insertEntry(int port, std::string input_filename);
// extern IpsaBase base;

class Rp4Task {
public:
    Rp4Task() {}

    void emitTask(string json_filename, IpsaBuilder &builder) {
        ofstream output(json_filename.c_str());
        IpsaOutput out(output);
        out.emit(builder.ipsa.toIpsaValue());
        output.close();
    }

    int newTask(int port, string rp4_filename, string json_filename) {
        yydebug = 0;
        base[port] = IpsaBase();
        FILE *fp = fopen(rp4_filename.c_str(), "r");
        rewind(fp);
        yyin = fp;

        num = port;
        int ret = yyparse();
        if (ret)
            return ret;
        ast[port] = std::move(tree);
        base[port].load(ast[port]);
        fclose(fp);
        return 0;
    }

    void buildTask(int port, string json_filename) {
        IpsaBuilder builder(base[port]);
        builder.allocateLogicalProcessors();
        builder.calculateTableSpace();
        IpsaAllocate allocate(builder.ipsa.real_processors, builder.processors, builder.proc_space,
                              builder.proc_cluster, base[port].stage_manager, builder.ipsa.initial_processor,
                              builder.ipsa.initial_stage, builder.processor_maps);
        allocate.allocatePhysicalProcessors();
        builder.allocateMemory();
        emitTask(json_filename, builder);
    }

    void updateTask(int port, string update_filename, string json_filename) {
        FILE *fp = fopen(update_filename.c_str(), "r");
        rewind(fp);
        yyin = fp;
        num = port;
        int ret = yyparse();
        if (ret)
            exit(ret);
        fclose(fp);

        buildTask(port, json_filename);
    }
};

class Rp4Runtime {
public:
    unique_ptr<Rp4Task> task;

    Rp4Runtime() : task(nullptr) {
        google::InitGoogleLogging("/dev/null");

    }

    void emitNewTask(string rp4_filename, int port) {
        task = make_unique<Rp4Task>();
        string json_filename = "a" + std::to_string(port) + ".json";
        if (task->newTask(port, rp4_filename, json_filename)) {
            cout << "error" << endl;
            return;
        } else
            task->buildTask(port, json_filename);
        insertConfig(port, json_filename);
    }

    void emitUpdate(string update_filename, int port) {
        string json_filename = "update" + std::to_string(port) + ".json";
        task->updateTask(port, update_filename, json_filename);
        insertConfig(port, json_filename);
        cout << "updated!" << endl;
    }
};