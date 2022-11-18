#pragma once

#include "ipsa_action_manager.h"
#include "ipsa_configuration.h"
#include "ipsa_header_manager.h"
#include "ipsa_table.h"

class IpsaTableManager {

public:
    int global_table_id = 0;
    std::map<std::string, IpsaTable> tables;

    void load(const std::unique_ptr<Rp4Ast>& ast, const IpsaHeaderManager *, const IpsaActionManager *);
    void addTable(const Rp4TableDef &table_def, const IpsaHeaderManager *, const IpsaActionManager *);
    void reloadHeader(const IpsaHeaderManager *header_manager);
    IpsaTable *lookup(int table_id);
    IpsaTable *lookup(std::string name);
    void setMatcherId(int table_id, int proc_id, int matcher_id, const std::vector<std::pair<int, int>> &action_proc);
};