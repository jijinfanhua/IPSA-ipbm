//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_SRAM_MEM_CONFIG_H
#define GRPC_TEST_SRAM_MEM_CONFIG_H

#define DEBUG

#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cmath>

#include "sram.h"
#include "../util/json/rapidjson/document.h"
#include "../util/json/rapidjson/filereadstream.h"

struct FlowTablePureInfo {
    int flow_table_id;
    int residedLogicTableId;
    std::string flow_table_name;
};

class SRAMMemConfig {
    //friend class MatcherConfig;
public:
    SRAMMemConfig() : mem_slice_num(0), bit_per_sram(0),
                            cur_logical_table_idx(0),
                            cur_flow_table_idx(0),
                            cur_sram_idx(0) {};
    SRAMMemConfig(uint32_t memSliceNum, uint32_t bitPerSRAM) :
                        mem_slice_num(memSliceNum), bit_per_sram(bitPerSRAM),
                        cur_logical_table_idx(0),
                        cur_flow_table_idx(0) {
        for(int i = 0; i < memSliceNum; i++){
            auto sram = new SRAM(bitPerSRAM);
            sram->setSramId(i);
            this->sram_map.insert(std::make_pair(i, sram));
        }
    };

    void initiateSRAM(std::vector<SRAM*> srams);

    void initializeFromJson(const std::string &json_filename);
    void initiateLogicalTable(std::vector<LogicalTable*> logical_tables);
    void allocLogicalTableFromLengthConfig(std::unordered_map<uint32_t, std::vector<int32_t>> allocConfig);

    void addFlowTable(FlowTable* flow_table);
    FlowTablePureInfo addPureFlowTable(FlowTable* flow_table);

//    int insertFlowEntry(int flow_table_id);
    int getMaxFlowTableID();
    int getFlowTableIdByName(std::string flow_table_name);

    void printSRAMInfo();

    void addFlowEntry(int flow_entry_num, std::string flow_table_name);

private:
    uint32_t mem_slice_num;
    uint32_t bit_per_sram;

    int cur_logical_table_idx;
    int cur_flow_table_idx;
    int cur_sram_idx;

    std::unordered_map<int, SRAM*> sram_map;
    std::unordered_map<int, LogicalTable*> logical_table_map;
    std::unordered_map<int, FlowTable*> flow_table_map;
    std::unordered_map<std::string, int> flow_table_name_id_map;
};


void SRAMMemConfig::initiateSRAM(std::vector<SRAM*> srams) {
    for(auto it : srams) {
        sram_map.insert(std::make_pair(it->getSramId(), it));
    }
}

void SRAMMemConfig::initiateLogicalTable(std::vector<LogicalTable*> logical_tables) {
    for(auto it : logical_tables) {
        logical_table_map.insert(std::make_pair(it->getLogicalTableId(), it));
        sram_map[it->getResidedHardwareId()]->addLogicalTable(it->getLogicalTableId());
    }
}

void SRAMMemConfig::printSRAMInfo() {
    for(auto & sram : sram_map) {
        std::cout << "************** SRAM " << sram.first << " **************" << std::endl;
        std::cout << "Capacity: " << sram.second->getCapacity() << std::endl;
        std::cout << "Free Capacity: " << sram.second->getFreeCapacity() << std::endl;
        for(auto lt_id : sram.second->getLogicalTables()){
            std::cout << "\tLogical Table " << lt_id << ": " << std::endl;
            auto lt = logical_table_map[lt_id];
            std::cout << "\t\tMatchType: " << static_cast<bool>(lt->getMatchType()) << std::endl;
            std::cout << "\t\tResidedSRAMId: " << lt->getResidedHardwareId() << std::endl;
            std::cout << "\t\tSupportedMaxLength: " << lt->getSupportedMaxLength() << std::endl;
            std::cout << "\t\tSupportedMaxNum: " << lt->getSupportedMaxNum() << std::endl;
            std::cout << "\t\tCurrentCapacity: " << lt->getCurrentCapacity() << std::endl;
        }
    }
}

int SRAMMemConfig::getMaxFlowTableID(){
    int max = 0;
    for(auto it : flow_table_map) {
        if(it.first > max) {
            max = it.first;
        }
    }
    return max;
}

void SRAMMemConfig::addFlowTable(FlowTable *flow_table) {
    flow_table_map.insert(std::make_pair(flow_table->getFlowTableId(), flow_table));

    for(auto it: logical_table_map) {
        if((it.second->getSupportedMaxLength() >= flow_table->getKeyLength() + flow_table->getValueLength()) &&
                (it.second->getCurrentCapacity() > flow_table->getPossibleEntryNum())) {
            flow_table->setResidedLogicTableId(it.first);
            it.second->addInsideFlowTable(flow_table->getFlowTableId());
        }
    }
}

//int SRAMMemConfig::insertFlowEntry(int flow_table_id) {
//    int res = logical_table_map[flow_table_map[flow_table_id]->getResidedLogicTableId()]->addFlowEntry();
//    if (res < 0){
//        return -1;
//    }
//    else{
//        flow_table_map[flow_table_id]->addFlowEntry();
//        return 0;
//    }
//}

void SRAMMemConfig::allocLogicalTableFromLengthConfig(std::unordered_map<uint32_t,
        std::vector<int32_t>> allocConfig) {
    for(auto & it : allocConfig) {
        uint32_t size = it.second.size();
        uint32_t bit_per_size = floor(this->bit_per_sram * 1.0 / size);
        for(auto ele : it.second) {
            int32_t supportedMaxNum = floor(bit_per_size * 1.0 / ele);
            auto lt = new LogicalTable(cur_logical_table_idx++, MatchType::EXACT, it.first,
                                       ele, supportedMaxNum);
            logical_table_map.insert(std::make_pair(lt->getLogicalTableId(), lt));
            sram_map[it.first]->addLogicalTable(lt->getLogicalTableId());
        }
    }
}

FlowTablePureInfo SRAMMemConfig::addPureFlowTable(FlowTable *flow_table) {
    flow_table->setFlowTableId(cur_flow_table_idx++);
    flow_table_map.insert(std::make_pair(flow_table->getFlowTableId(), flow_table));
    flow_table_name_id_map.insert(std::make_pair(flow_table->getFlowTableName(), flow_table->getFlowTableId()));

    for(auto it: logical_table_map) {
        if((it.second->getSupportedMaxLength() >= flow_table->getKeyLength() + flow_table->getValueLength()) &&
           (it.second->getCurrentCapacity() > flow_table->getPossibleEntryNum())) {
            flow_table->setResidedLogicTableId(it.first);
            it.second->addInsideFlowTable(flow_table->getFlowTableId());
        }
    }

    FlowTablePureInfo ftpi;
    ftpi.flow_table_name = flow_table->getFlowTableName();
    ftpi.flow_table_id = flow_table->getFlowTableId();
    ftpi.residedLogicTableId = flow_table->getResidedLogicTableId();
    return ftpi;
}

void SRAMMemConfig::addFlowEntry(int flow_entry_num, std::string flow_table_name) {
    auto ft = flow_table_map[flow_table_name_id_map[flow_table_name]];
    ft->addFlowEntry(flow_entry_num);
    auto lt = logical_table_map[ft->getResidedLogicTableId()];
    lt->addFlowEntry(flow_entry_num);
}

int SRAMMemConfig::getFlowTableIdByName(std::string flow_table_name) {
    for(auto it : flow_table_map) {
        if(std::strcmp(it.second->getFlowTableName().c_str(), flow_table_name.c_str()) == 0){
            return it.first;
        }
    }
    return 0;
}

void SRAMMemConfig::initializeFromJson(const std::string &json_filename) {
    std::cout << json_filename.c_str() << std::endl;
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    this->mem_slice_num = d["sram_num"].GetInt();
    this->bit_per_sram = d["sram_bit"].GetInt();

    for(int i = 0; i < mem_slice_num; i++){
        auto sram = new SRAM(bit_per_sram);
        sram->setSramId(i);
        this->sram_map.insert(std::make_pair(i, sram));
    }

    for(auto it = d["srams"].MemberBegin(); it != d["srams"].MemberEnd(); ++it) {
        for(auto ele = it->value.MemberBegin(); ele != it->value.MemberEnd(); ++ele){
            auto lt = new LogicalTable(cur_logical_table_idx++,
                                       MatchType::EXACT,
                                       cur_sram_idx,
                                       ele->value["entry_width"].GetInt(),
                                       ele->value["entry_num"].GetInt());
            logical_table_map.insert(std::make_pair(lt->getLogicalTableId(), lt));
            sram_map[lt->getResidedHardwareId()]->addLogicalTable(lt->getLogicalTableId());
        }
        cur_sram_idx++;
    }
}

//std::unordered_map<uint32_t, std::vector<LogicalTable*>>
//    SRAMMemConfig::allocLogicalTableWithWidths(std::unordered_map<uint32_t, std::vector<int32_t>> allocConfig){
//
//    for (auto & ele : allocConfig){
//        uint32_t sram_id = ele.first;
//#ifdef DEBUG
//        std::cout << "SRAM " << sram_id << ": " << this->bit_per_sram << "b" << std::endl;
//#endif
//        uint32_t size = ele.second.size();
//        uint32_t bit_per_size = floor(this->bit_per_sram * 1.0 / size);
//        int i = 0;
//        std::vector<LogicalTable*> logical_tables;
//        for (auto & it : ele.second) {
//            int32_t supportedMaxNum = floor(bit_per_size * 1.0 / it);
//#ifdef DEBUG
//            std::cout << "\t---- Logical Table " << i << ": width(" << it
//                        << ")  length(" << supportedMaxNum << "). Total: " << it * supportedMaxNum << std::endl;
//#endif
//            auto lt = new LogicalTable(i++, MatchType::EXACT, sram_id, it, supportedMaxNum);
//            logical_tables.push_back(lt);
//
//        }
//        sram_memory_pool.insert(std::make_pair(sram_id, logical_tables));
//    }
//    return this->sram_memory_pool;
//}

#endif //GRPC_TEST_SRAM_MEM_CONFIG_H
