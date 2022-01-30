//
// Created by fengyong on 2021/2/18.
//

#include <iostream>
#include <fstream>

#include "../include/matcher.h"

int main() {

    auto processor = new Processor(8);

    auto parser = new Parser();
    parser->initializeFromJson("../config/header.json");
//    p->printHeaderInfo();

    auto mem_config = new MemConfig();
    mem_config->initSRAMFromJson("../config/sram.json");
    mem_config->initTCAMFromJson("../config/tcam.json");

    mem_config->initFlowTableFromJson("../config/flow_table.json");
//    mem_config->printFlowTableInfo();

    auto matcher_config = new MatcherConfig();
    matcher_config->initFromJson("../config/matcher.json", parser, mem_config, processor);
    matcher_config->printMatcher();

//    std::vector<std::vector<int32_t>> config;
//    config.push_back({112, 160});
//    config.push_back({80, 112, 200});
//    config.push_back({160, 320});
//    config.push_back({200, 240});
//    config.push_back({80, 280});
//    config.push_back({360});
//    config.push_back({400});
//    config.push_back({440});

//    std::unordered_map<uint32_t, std::vector<int32_t>> allocConfig;
//    int i = 0;
//    for(auto & it : config) {
//        allocConfig.insert(std::make_pair(i++, it));
//    }
//    SRAM_handler->allocLogicalTableFromLengthConfig(allocConfig);
////    SRAM_handler->printSRAMInfo();
//
//    auto ft = new FlowTable("mac_learning", MatchType::EXACT, MemType::MEM_SRAM, 32, 64, 1, 100);
//    FlowTablePureInfo a = SRAM_handler->addPureFlowTable(ft);
//    std::cout << "New flow table " << a.flow_table_name << "'s id is " << a.flow_table_id << std::endl;
//    SRAM_handler->printSRAMInfo();


//    auto matcher_config = new MatcherConfig();
//
//    auto matcher1 = new Matcher();
//    matcher1->flow_table_name = "mac_learning";
//
//    std::string match_fields[2] = {"Ethernet dmac exact", "VLAN vid exact"};
//    char match_type[50];
//    auto *head = new MatchFieldNode();
//    MatchFieldNode *cur = head;
//    for(auto & match_field : match_fields){
//        auto mfn = new MatchFieldNode();
//        sscanf(match_field.c_str(), "%s %s %s", mfn->header_name, mfn->field_name, match_type);
//        if(std::strcmp(match_type, "exact") == 0) {
//            mfn->matchType = MatchType::EXACT;
//        }
//        cur->next = mfn;
//        cur = mfn;
//    }
//
//    matcher_config->addMatcher(head, matcher1, p, mem_config);
//    matcher_config->printMatcher();

    return 0;
}
