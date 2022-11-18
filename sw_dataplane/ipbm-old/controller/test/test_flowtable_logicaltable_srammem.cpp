//
// Created by fengyong on 2021/2/10.
//

#include "../include/sram_mem_config.h"
#include "../include/tcam_mem_config.h"

int main(){
    auto SRAM_handler = new SRAMMemConfig(8, 16777216);

    std::vector<std::vector<int32_t>> config;
    config.push_back({112, 160});
    config.push_back({80, 112, 200});
    config.push_back({160, 320});
    config.push_back({200, 240});
    config.push_back({80, 280});
    config.push_back({360});
    config.push_back({400});
    config.push_back({440});

    std::unordered_map<uint32_t, std::vector<int32_t>> allocConfig;
    int i = 0;
    for(auto & it : config) {
        allocConfig.insert(std::make_pair(i++, it));
    }

    SRAM_handler->allocLogicalTableFromLengthConfig(allocConfig);
    SRAM_handler->printSRAMInfo();

    auto ft = new FlowTable("mac_learning", MatchType::EXACT, MemType::MEM_SRAM, 32, 64, 1, 100);
    FlowTablePureInfo a = SRAM_handler->addPureFlowTable(ft);
    std::cout << a.flow_table_id << " " << a.flow_table_name << " " << a.residedLogicTableId << std::endl;
    SRAM_handler->addFlowEntry(1, "mac_learning");
    SRAM_handler->printSRAMInfo();

#ifdef SRAM
    auto ft1 = new FlowTable(1, MatchType::EXACT, MemType::MEM_SRAM, 1, 32, 64, 2);
    auto ft2 = new FlowTable(2, MatchType::EXACT, MemType::MEM_SRAM, 1, 48, 64, 1);
    auto ft3 = new FlowTable(3, MatchType::EXACT, MemType::MEM_SRAM, 2, 32, 32, 1);
    auto ft4 = new FlowTable(4, MatchType::EXACT, MemType::MEM_SRAM, 3, 64, 32, 1);
    auto ft5 = new FlowTable(5, MatchType::EXACT, MemType::MEM_SRAM, 3, 48, 64, 2);

    auto lt1 = new LogicalTable(1, MatchType::EXACT, 1, 80, 1000);
    std::vector<FlowTable*> v1;
    v1.push_back(ft1);
    v1.push_back(ft2);
    lt1->setInsideFlowTables(v1);

    auto lt2 = new LogicalTable(2, MatchType::EXACT, 1, 80, 1000);
    std::vector<FlowTable*> v2;
    v2.push_back(ft3);
    lt2->setInsideFlowTables(v2);

    auto lt3 = new LogicalTable(3, MatchType::EXACT, 2, 120, 1000);
    std::vector<FlowTable*> v3;
    v3.push_back(ft4);
    v3.push_back(ft5);
    lt3->setInsideFlowTables(v3);

    std::vector<LogicalTable*> ltv1, ltv2;
    ltv1.push_back(lt1);
    ltv1.push_back(lt2);
    ltv2.push_back(lt3);

    auto sram = new SRAMMemConfig(3, 10000);
    sram->addMemoryPoolInfo(1, ltv1);
    sram->addMemoryPoolInfo(2, ltv2);

    auto sram_mem_pool = sram->getMemoryPoolInfo();
    std::cout << sram_mem_pool.size() << std::endl;
    for (auto & ele : sram_mem_pool){
        std::cout << "**************** SRAM no: " << ele.first << " ****************" << std::endl;
        for(auto & it : ele.second){
            std::cout << "logical table ID: " << it->getLogicalTableId() << std::endl;
            auto flow_tables = it->getInsideFlowTables();
            for(auto & ft_it : flow_tables) {
                std::cout << "\t\tflow table ID: " << ft_it->getFlowTableId() << std::endl;
            }
        }
    }
#endif
}