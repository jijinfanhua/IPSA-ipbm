//
// Created by fengyong on 2021/2/10.
//

#ifndef GRPC_TEST_TCAM_MEM_CONFIG_H
#define GRPC_TEST_TCAM_MEM_CONFIG_H

#include <iostream>
#include <unordered_map>

#include "../util/json/rapidjson/document.h"
#include "../util/json/rapidjson/filereadstream.h"

#include "tcam.h"
#include "logical_table.h"

// Catalyset 6500 TCAM

/**
 * unlike SRAM, a TCAM can only configured as one type of length,  36bits, 72bits, 144bits, 288bits
 * TCAM with x-length key, should have x-length key and x-length mask and result in CAM.
 * referred to:
 *          https://networkengineering.stackexchange.com/questions/24350/tcam-entry-width/24413
 *          https://www.sciencedirect.com/topics/computer-science/content-addressable-memory
 */

class TCAMMemConfig {
public:
    TCAMMemConfig() : mem_slice_num(-1),  bit_per_tcam(-1) {};
    TCAMMemConfig(int32_t memSliceNum, int32_t bitPerTCAM) : mem_slice_num(memSliceNum), bit_per_tcam(bitPerTCAM) {};

//    void setTCAMLengthConfig(const std::unordered_map<uint32_t, uint32_t> &tcamLengthConfig);
//    void addTCAMLengthConfig(uint32_t tcam_id, uint32_t entry_length);
    void setTCAMInsideLogicTable(const std::unordered_map<uint32_t, LogicalTable*> &tcamInsideLogicTable);
    void addTCAMInsideLogicTable(uint32_t sram_id, LogicalTable* lt);
    int32_t getMemSliceNum() const;
    void setMemSliceNum(int32_t memSliceNum);
    int32_t getBitPerTcam() const;
    void setBitPerTcam(int32_t bitPerTcam);
    const std::unordered_map<uint32_t, uint32_t> &getTcamLengthConfig() const;
    const std::unordered_map<uint32_t, LogicalTable *> &getTcamInsideLogicTable() const;

    void initializeFromJson(const std::string &json_filename);

private:
    int32_t mem_slice_num;
    int32_t bit_per_tcam;

    int cur_tcam_idx;

    std::unordered_map<int, TCAM*> tcam_map;
//    std::unordered_map<uint32_t, uint32_t> tcam_length_config;
    std::unordered_map<uint32_t, LogicalTable*> tcam_inside_logic_table;
};

void TCAMMemConfig::setTCAMLengthConfig(const std::unordered_map<uint32_t, uint32_t> &tcamLengthConfig) {
    this->tcam_length_config = tcamLengthConfig;
}

void TCAMMemConfig::addTCAMLengthConfig(uint32_t tcam_id, uint32_t entry_length){
    this->tcam_length_config.insert(std::make_pair(tcam_id, entry_length));
}

void TCAMMemConfig::setTCAMInsideLogicTable(const std::unordered_map<uint32_t, LogicalTable*> &tcamInsideLogicTable){
    this->tcam_inside_logic_table = tcamInsideLogicTable;
}

void TCAMMemConfig::addTCAMInsideLogicTable(uint32_t sram_id, LogicalTable* lt){
    this->tcam_inside_logic_table.insert(std::make_pair(sram_id, lt));
}

int32_t TCAMMemConfig::getMemSliceNum() const {
    return mem_slice_num;
}

void TCAMMemConfig::setMemSliceNum(int32_t memSliceNum) {
    mem_slice_num = memSliceNum;
}

int32_t TCAMMemConfig::getBitPerTcam() const {
    return bit_per_tcam;
}

void TCAMMemConfig::setBitPerTcam(int32_t bitPerTcam) {
    bit_per_tcam = bitPerTcam;
}

const std::unordered_map<uint32_t, uint32_t> & TCAMMemConfig::getTcamLengthConfig() const {
    return tcam_length_config;
}

const std::unordered_map<uint32_t, LogicalTable *> & TCAMMemConfig::getTcamInsideLogicTable() const {
    return tcam_inside_logic_table;
}

void TCAMMemConfig::initializeFromJson(const std::string &json_filename) {
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    this->mem_slice_num = d["tcam_num"].GetInt();
    this->bit_per_tcam = d["tcam_bit"].GetInt();

    for(int i = 0; i < mem_slice_num; i++) {
        auto tcam = new TCAM();
        tcam->setTcamId(i);
        this->tcam_map.insert(std::make_pair(tcam->getTcamId(), tcam));
    }

    for(auto it = d["tcams"].MemberBegin(); it != d["tcams"].MemberEnd(); ++it) {
        auto lt = new LogicalTable()
    }

}

#endif //GRPC_TEST_TCAM_MEM_CONFIG_H
