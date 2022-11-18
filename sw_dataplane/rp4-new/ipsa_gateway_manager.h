#pragma once

#include "ipsa_gateway.h"
#include "ipsa_stage_manager.h"
#include "ipsa_table_manager.h"

class IpsaGatewayManager {
public:
    int global_gateway_id;
    std::vector<IpsaGateway> gateways;

    void load(IpsaHeaderManager *header_manager,
              IpsaStageManager *stage_manager,// after initialization
              IpsaTableManager *table_manager);
};