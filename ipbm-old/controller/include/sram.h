//
// Created by fengyong on 2021/2/13.
//

#ifndef GRPC_TEST_SRAM_H
#define GRPC_TEST_SRAM_H

#include <iostream>
#include <utility>
#include <vector>

//#include "logical_table.h"

class SRAM {
public:
    SRAM() : capacity(-1), free_capacity(-1) {};
    explicit SRAM(int32_t cap) : capacity(cap), free_capacity(cap){};
    //SRAM(int32_t cap, std::vector<LogicalTable*> logicalTables)
    //            : capacity(cap), free_capacity(cap), logical_tables(std::move(logicalTables)) {};

    void setSramId(int sramId);
    int getSramId();

    std::vector<int> getLogicalTables();
    void setLogicalTables(std::vector<int> lts);
    void addLogicalTable(int logical_table_id);

    int getCapacity();
    int getFreeCapacity();


//    std::vector<FlowTable*> getFlowTablesObj();
//    std::vector<int32_t> getFlowTablesNo();
//    std::vector<FlowTable*> getFlowTablesWithCluster();

private:
    int32_t sram_id;
    int32_t capacity; //in bits
    int32_t free_capacity; //in bits
    std::vector<int> logical_table_ids;
    //std::vector<int> flow_table_ids;
    //std::vector<LogicalTable*> logical_tables;
};

void SRAM::setSramId(int sramId) {
    this->sram_id = sramId;
}

int SRAM::getSramId() {
    return this->sram_id;
}

void SRAM::setLogicalTables(std::vector<int> lts) {
    this->logical_table_ids = lts;
}

std::vector<int> SRAM::getLogicalTables() {
    return this->logical_table_ids;
}

void SRAM::addLogicalTable(int logical_table_id) {
    this->logical_table_ids.push_back(logical_table_id);
}

int SRAM::getCapacity() {
    return this->capacity;
}

int SRAM::getFreeCapacity() {
    return this->free_capacity;
}
//
//std::vector<FlowTable *> SRAM::getFlowTablesObj() {
//    std::vector<FlowTable*> fts;
//    for(auto & it : this->logical_tables) {
//        for (auto & its : it->getInsideFlowTables()) {
//            fts.push_back(its);
//        }
//    }
//    return fts;
//}
//
//std::vector<int32_t> SRAM::getFlowTablesNo() {
//    std::vector<int> fts;
//    for(auto & it : this->logical_tables){
//        for (auto & its : it->getInsideFlowTables()){
//            fts.push_back(its->getFlowTableId());
//        }
//    }
//    return fts;
//}
//
//std::unordered_map<int32_t, std::vector<FlowTable*>> SRAM::getFlowTablesWithCluster() {
//    std::vector<int> fts;
//    for(auto & it : this->logical_tables){
//        for (auto & its : it->getInsideFlowTables()){
//            fts.push_back(its->getFlowTableId());
//        }
//    }
//    return fts;
//    return std::vector<FlowTable *>();
//}

#endif //GRPC_TEST_SRAM_H
