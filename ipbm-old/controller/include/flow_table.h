//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_FLOW_TABLE_H
#define GRPC_TEST_FLOW_TABLE_H

#include <iostream>
#include "enums.h"

#include <unordered_map>
#include <vector>
#include <cstring>

class FlowTable {
public:
    FlowTable():flow_table_id(-1), table_match_type(MatchType::NONE),
                    mem_type(MemType::NONE), resided_logic_table_id(-1), key_length(-1),
                    value_length(-1), action_data_num(-1), current_entry_num(0) {};

    FlowTable(int32_t flowTableId, std::string flowTableName, MatchType tableMatchType, MemType memType, int32_t residedLogicTableId,
              int32_t keyLength, int32_t valueLength, int32_t actionDataNum, int32_t possibleEntryNum);

    FlowTable(std::string flowTableName, MatchType tableMatchType, MemType memType,
              int32_t keyLength, int32_t valueLength, int32_t actionDataNum, int32_t possibleEntryNum);

    int32_t getFlowTableId() const;
    void setFlowTableId(int32_t flowTableId);

    MatchType getTableMatchType() const;
    void setTableMatchType(MatchType tableMatchType);

    MemType getMemType() const;
    void setMemType(MemType memType);

    int32_t getResidedLogicTableId() const;
    void setResidedLogicTableId(int32_t residedLogicTableId);

    int32_t getKeyLength() const;
    void setKeyLength(int32_t keyLength);

    int32_t getValueLength() const;
    void setValueLength(int32_t valueLength);

    int32_t getActionDataNum() const;
    void setActionDataNum(int32_t actionDataNum);

    int32_t getPossibleEntryNum() const;
    void setPossibleEntryNum(int num);

    std::string getFlowTableName();
    void setFlowTableName(std::string flowTableName);

    int32_t getCurrentEntryNum() const;
    void setCurrentEntryNum(int32_t currentEntryNum);
    void addFlowEntry(int entry_num);

private:
    int32_t flow_table_id;
    //char flow_table_name[255];
    std::string flow_table_name;
    MatchType table_match_type;
    MemType mem_type;
    int32_t resided_logic_table_id;
    int32_t key_length;
    int32_t value_length;
    int32_t action_data_num;
    int32_t possible_entry_num;
    int32_t current_entry_num;

};

FlowTable::FlowTable(int32_t flowTableId, std::string flowTableName, MatchType tableMatchType, MemType memType, int32_t residedLogicTableId,
                     int32_t keyLength, int32_t valueLength, int32_t actionDataNum, int32_t possibleEntryNum) : flow_table_id(flowTableId),
                                                                                      table_match_type(tableMatchType),
                                                                                      mem_type(memType),
                                                                                      resided_logic_table_id(
                                                                                              residedLogicTableId),
                                                                                      key_length(keyLength),
                                                                                      value_length(valueLength),
                                                                                      action_data_num(actionDataNum),
                                                                                      possible_entry_num(possibleEntryNum),
                                                                                      current_entry_num(0) {
    //std::strcpy(flow_table_name, flowTableName);
    flow_table_name = flowTableName;
}

int32_t FlowTable::getFlowTableId() const {
    return flow_table_id;
}

void FlowTable::setFlowTableId(int32_t flowTableId) {
    flow_table_id = flowTableId;
}

MatchType FlowTable::getTableMatchType() const {
    return table_match_type;
}

void FlowTable::setTableMatchType(MatchType tableMatchType) {
    table_match_type = tableMatchType;
}

MemType FlowTable::getMemType() const {
    return mem_type;
}

void FlowTable::setMemType(MemType memType) {
    mem_type = memType;
}

int32_t FlowTable::getResidedLogicTableId() const {
    return resided_logic_table_id;
}

void FlowTable::setResidedLogicTableId(int32_t residedLogicTableId) {
    resided_logic_table_id = residedLogicTableId;
}

int32_t FlowTable::getKeyLength() const {
    return key_length;
}

void FlowTable::setKeyLength(int32_t keyLength) {
    key_length = keyLength;
}

int32_t FlowTable::getValueLength() const {
    return value_length;
}

void FlowTable::setValueLength(int32_t valueLength) {
    value_length = valueLength;
}

int32_t FlowTable::getActionDataNum() const {
    return action_data_num;
}

void FlowTable::setActionDataNum(int32_t actionDataNum) {
    action_data_num = actionDataNum;
}

int32_t FlowTable::getPossibleEntryNum() const {
    return this->possible_entry_num;
}

void FlowTable::setPossibleEntryNum(int num) {
    this->possible_entry_num = num;
}

int32_t FlowTable::getCurrentEntryNum() const {
    return current_entry_num;
}

void FlowTable::setCurrentEntryNum(int32_t currentEntryNum) {
    current_entry_num = currentEntryNum;
}

void FlowTable::addFlowEntry(int entry_num) {
    this->current_entry_num += entry_num;
}

std::string FlowTable::getFlowTableName() {
    return this->flow_table_name;
}

void FlowTable::setFlowTableName(std::string flowTableName) {
    //std::strcpy(this->flow_table_name, flowTableName);
    this->flow_table_name = std::move(flowTableName);
}

FlowTable::FlowTable(std::string flowTableName, MatchType tableMatchType, MemType memType, int32_t keyLength,
                     int32_t valueLength, int32_t actionDataNum, int32_t possibleEntryNum) {
    flow_table_name = std::move(flowTableName);
    table_match_type = tableMatchType;
    mem_type = memType;
    key_length = keyLength;
    value_length = valueLength;
    action_data_num = actionDataNum;
    possible_entry_num = possibleEntryNum;
    current_entry_num = 0;
}


#endif //GRPC_TEST_FLOW_TABLE_H
