//
// Created by fengyong on 2/22/21.
//

#include "../include/mem_config.h"

int main() {
    auto mem_config = new MemConfig();

    mem_config->initSRAMFromJson("../config/sram.json");
    mem_config->printSRAMInfo();
    mem_config->initTCAMFromJson("../config/tcam.json");
    mem_config->printTCAMInfo();

    mem_config->initFlowTableFromJson("../config/flow_table.json");
    mem_config->printFlowTableInfo();

    auto ft = new FlowTable("l2_forwarding", MatchType::EXACT, MemType::MEM_SRAM, 80, 64, 2, 100);
    mem_config->addFlowTable(ft);
    mem_config->printFlowTableInfo();

    mem_config->addEntry("mac_learning", 25);

    mem_config->printFlowTableInfo();

    return 0;
}