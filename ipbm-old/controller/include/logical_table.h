//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_LOGICAL_TABLE_H
#define GRPC_TEST_LOGICAL_TABLE_H

#include <iostream>
#include <utility>

#include "enums.h"
#include "flow_table.h"

/**
 * Memory pool is composed of logical tables.
 */
class LogicalTable {
public:
    LogicalTable() : logical_table_id(-1),
                        mem_type(MemType::NONE),
                        resided_hardware_id(-1),
                        supported_max_length(-1),
                        supported_max_num(-1),
                        mem_in_bit(-1),
                        inside_flow_table_ids({}){};
    LogicalTable(int32_t logicalTableId, MemType memType, int32_t residedHardwareId, int32_t supportedMaxLength,
                 int32_t supportedMaxNum) : logical_table_id(logicalTableId), mem_type(memType),
                                             resided_hardware_id(residedHardwareId),
                                             supported_max_length(supportedMaxLength),
                                             supported_max_num(supportedMaxNum),
                                             mem_in_bit(supportedMaxLength*supportedMaxLength),
                                             current_capacity{supportedMaxNum}{};

    int32_t getLogicalTableId() const;
    void setLogicalTableId(int32_t logicalTableId);

    MemType getMemType() const;
    void setMemType(MemType memType);

    int32_t getResidedHardwareId() const;
    void setResidedHardwareId(int32_t residedHardwareId);

    int32_t getSupportedMaxLength() const;
    void setSupportedMaxLength(int32_t supportedMaxLength);

    int32_t getSupportedMaxNum() const;
    void setSupportedMaxNum(int32_t supportedMaxNum);

    int32_t getMemInBit() const;
    int32_t changeCapacity(int change_value);

    void setInsideFlowTables(std::vector<int> flow_tables);
    void addInsideFlowTable(int flow_table_id);
    std::vector<int> getInsideFlowTables();

    int32_t insertOneEntry();
    int32_t insetEntryBatch(uint32_t entry_num);

    int32_t getCurrentCapacity() const;
    void setCurrentCapacity(int32_t currentCapacity);
    int addFlowEntry(int entry_num);

private:
    int32_t logical_table_id;
    MemType mem_type;
    int32_t resided_hardware_id;
    int32_t supported_max_length; // sram: key length + value length; tcam: key length
    int32_t supported_max_num; // max entry num
    int32_t mem_in_bit;
    int32_t current_capacity;
    // in entry num
    std::vector<int> inside_flow_table_ids; // one logical table can have several flow tables
};


void LogicalTable::setInsideFlowTables(std::vector<int> flow_tables){
    this->inside_flow_table_ids = flow_tables;
}
void LogicalTable::addInsideFlowTable(int flow_table_id){
    this->inside_flow_table_ids.push_back(flow_table_id);
}

std::vector<int> LogicalTable::getInsideFlowTables() {
    return this->inside_flow_table_ids;
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

MemType LogicalTable::getMemType() const {
    return mem_type;
}

void LogicalTable::setMemType(MemType memType) {
    mem_type = memType;
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

int32_t LogicalTable::getCurrentCapacity() const {
    return current_capacity;
}

void LogicalTable::setCurrentCapacity(int32_t currentCapacity) {
    current_capacity = currentCapacity;
}

int LogicalTable::addFlowEntry(int entry_num) {
    if(this->current_capacity < entry_num)
        return -1;
    this->current_capacity -= entry_num;
    return this->current_capacity;
}


#endif //GRPC_TEST_LOGICAL_TABLE_H
