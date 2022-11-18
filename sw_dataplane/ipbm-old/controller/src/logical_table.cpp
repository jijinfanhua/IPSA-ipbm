//
// Created by fengyong on 2021/2/9.
//

#include "../include/logical_table.h"

void LogicalTable::setInsideFlowTables(std::vector<FlowTable*> flow_tables){
    this->inside_flow_tables = std::move(flow_tables);
}
void LogicalTable::addInsideFlowTable(FlowTable* flow_table){
    this->inside_flow_tables.push_back(flow_table);
}

std::vector<FlowTable*> LogicalTable::getInsideFlowTables() {
    return this->inside_flow_tables;
}

int32_t LogicalTable::getMemInBit() const {
    if (this->supported_max_length == -1 || this->supported_max_num == -1) {
        return -1;
    } else{
        return this->supported_max_num * this->supported_max_length;
    }
}

int32_t LogicalTable::changeCapacity(int change_value) {
    this->current_capacity = this->current_capacity + change_value;
    return this->current_capacity;
}

int32_t LogicalTable::getLogicalTableId() const {
    return logical_table_id;
}

void LogicalTable::setLogicalTableId(int32_t logicalTableId) {
    logical_table_id = logicalTableId;
}

MatchType LogicalTable::getMatchType() const {
    return match_type;
}

void LogicalTable::setMatchType(MatchType matchType) {
    match_type = matchType;
}

int32_t LogicalTable::getResidedHardwareId() const {
    return resided_hardware_id;
}

void LogicalTable::setResidedHardwareId(int32_t residedHardwareId) {
    resided_hardware_id = residedHardwareId;
}

int32_t LogicalTable::getSupportedMaxLength() const {
    return supported_max_length;
}

void LogicalTable::setSupportedMaxLength(int32_t supportedMaxLength) {
    supported_max_length = supportedMaxLength;
    if (this->supported_max_length!=-1 && this->supported_max_num!=-1){
        this->mem_in_bit = this->supported_max_num * this->supported_max_length;
    }
}

int32_t LogicalTable::getSupportedMaxNum() const {
    return supported_max_num;
}

void LogicalTable::setSupportedMaxNum(int32_t supportedMaxNum) {
    supported_max_num = supportedMaxNum;
    current_capacity = supportedMaxNum;
    if (this->supported_max_length!=-1 && this->supported_max_num!=-1){
        this->mem_in_bit = this->supported_max_num * this->supported_max_length;
    }
}

int32_t LogicalTable::insertOneEntry() {
    if (current_capacity <= 0) {
        return -1;
    }
    current_capacity -= 1;
    return current_capacity;
}

int32_t LogicalTable::insetEntryBatch(uint32_t entry_num) {
    if (current_capacity < entry_num) {
        return -1;
    }
    current_capacity -= entry_num;
    return current_capacity;
}
