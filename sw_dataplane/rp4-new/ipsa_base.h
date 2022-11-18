#pragma once

#include "ipsa_action_manager.h"
#include "ipsa_gateway_manager.h"
#include "ipsa_level_manager.h"
#include "ipsa_processor_manager.h"
#include "ipsa_table_manager.h"

class IpsaBase {
public:
    IpsaHeaderManager header_manager;
    IpsaLevelManager level_manager;
    IpsaActionManager action_manager;
    IpsaTableManager table_manager;
    IpsaStageManager stage_manager;

    void load(const std::unique_ptr<Rp4Ast>& ast) {
        header_manager.load(ast->struct_defs, ast->header_defs);
        level_manager.load(ast->parser_def, &header_manager);
        action_manager.load(ast, &header_manager);
        table_manager.load(ast, &header_manager, &action_manager);
        stage_manager.load(ast);
    }
};