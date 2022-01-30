//
// Created by fengyong on 2021/2/13.
//

#ifndef GRPC_TEST_TCAM_H
#define GRPC_TEST_TCAM_H

#include <iostream>
#include <vector>

class TCAM {
public:
    TCAM(): capacity(-1), free_capacity(-1) {};
    explicit TCAM(int configLength) : config_length(configLength) {};
    int getTcamId() const;
    void setTcamId(int tcamId);
    int getCapacity() const;
    void setCapacity(int cap);
    int getFreeCapacity() const;
    void setFreeCapacity(int freeCapacity);
    int getConfigLength() const;
    void setConfigLength(int configLength);
    int getLogicalTableId() const;
    void setLogicalTableId(int logicalTableId);
    int getEntryNum() const;
    void setEntryNum(int entryNum);

private:
    int tcam_id;
    int capacity;
    int free_capacity;
    int config_length;
    int logical_table_id;
    int entry_num;
};

int TCAM::getTcamId() const {
    return tcam_id;
}

void TCAM::setTcamId(int tcamId) {
    tcam_id = tcamId;
}

int TCAM::getCapacity() const {
    return capacity;
}

void TCAM::setCapacity(int cap) {
    this->capacity = cap;
    this->free_capacity = cap;

}

int TCAM::getFreeCapacity() const {
    return free_capacity;
}

void TCAM::setFreeCapacity(int freeCapacity) {
    free_capacity = freeCapacity;
}

int TCAM::getConfigLength() const {
    return config_length;
}

void TCAM::setConfigLength(int configLength) {
    config_length = configLength;
}

int TCAM::getLogicalTableId() const {
    return logical_table_id;
}

void TCAM::setLogicalTableId(int logicalTableId) {
    this->logical_table_id = logicalTableId;
}

int TCAM::getEntryNum() const {
    return this->entry_num;
}

void TCAM::setEntryNum(int entryNum) {
    this->entry_num = entryNum;
}


#endif //GRPC_TEST_TCAM_H
