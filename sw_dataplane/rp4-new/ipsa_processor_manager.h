#pragma once

#include "ipsa_action_manager.h"
#include "ipsa_gateway_manager.h"
#include "ipsa_header_manager.h"
#include "ipsa_level_manager.h"
#include "ipsa_stage_manager.h"
#include "ipsa_table_manager.h"

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
    IpsaStageManager *stage_manager;
    IpsaHeaderManager *header_manager;
    IpsaLevelManager *level_manager;
    IpsaActionManager *action_manager;
    IpsaTableManager *table_manager;

    IpsaProcessorManager(
            IpsaStageManager *_stage_manager,
            IpsaHeaderManager *_header_manager,
            IpsaLevelManager *_level_manager,
            IpsaActionManager *_action_manager,
            IpsaTableManager *_table_manager) : stage_manager(_stage_manager),
                                                header_manager(_header_manager),
                                                level_manager(_level_manager),
                                                action_manager(_action_manager),
                                                table_manager(_table_manager) {
    }
    void initializeStages();
    void reorderStages(std::map<int, int> proc_proc);
    void setupStages(IpsaGatewayManager *gateway_manager);
};