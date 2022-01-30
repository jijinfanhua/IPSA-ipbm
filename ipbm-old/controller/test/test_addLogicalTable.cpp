//
// Created by fengyong on 2021/2/10.
//

#include <iostream>

#include "../include/logical_table.h"
#include "../include/sram_mem_config.h"

int main() {
    auto *lt1 = new LogicalTable(1, MatchType::EXACT, 1, 80, 20);
    auto *lt2 = new LogicalTable(2, MatchType::EXACT, 1, 80, 20);
    auto *lt3 = new LogicalTable(3, MatchType::EXACT, 2, 80, 20);

    std::vector<LogicalTable*> v1;
    v1.push_back(lt1);
    v1.push_back(lt2);
    auto *smc = new SRAMMemConfig(2, 5000);
    smc->addMemoryPoolInfo(1, v1);

    std::vector<LogicalTable*> v2;
    v2.push_back(lt3);
    smc->addMemoryPoolInfo(2, v2);

    std::unordered_map<uint32_t, std::vector<LogicalTable*>> a = smc->getMemoryPoolInfo();

    std::vector<LogicalTable*> return_value = a.at(2);

    for (auto & i : return_value) {
        std::cout << i->getLogicalTableId() << std::endl;
    }

    return 0;
}
