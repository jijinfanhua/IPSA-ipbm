//
// Created by fengyong on 2021/2/21.
//

#ifndef GRPC_TEST_MEM_CONFIG_H
#define GRPC_TEST_MEM_CONFIG_H


#include "../util/json/rapidjson/document.h"
#include "../util/json/rapidjson/filereadstream.h"

#include "tcam.h"
#include "sram.h"
#include "logical_table.h"


class MemConfig {
public:

    MemConfig() : cur_sram_idx(0), cur_tcam_idx(0), cur_logical_table_idx(0), cur_flow_table_idx(0) {};
    void initSRAMFromJson(const std::string &json_filename);
    void initTCAMFromJson(const std::string &json_filename);
    void printSRAMInfo();
    void printTCAMInfo();


    void initFlowTableFromJson(const std::string &json_filename);
    void printFlowTableInfo();
    void addFlowTable(FlowTable *ft);
    FlowTable* addPureFlowTable(const std::string& flow_table_name, const std::string& matchType,
                          int key_length, int value_length, int action_data_num, int possible_entry_num);

    int addEntry(std::string flow_table_name, int entry_num);

    MatchType getMatchType(const std::string& s_match_type);
    MemType getMemType(const std::string& s_mem_type);
    int getFlowTableIdByName(const std::string& flow_table_name);

    LogicalTable * calLogicalTableByFlowTable(MemType mem_type, int key_length, int value_length, int possible_entry_num);

    int getTcamNum() const;
    void setTcamNum(int tcamNum);
    int getSramNum() const;
    void setSramNum(int sramNum);
    int getBitPerTcam() const;
    void setBitPerTcam(int bitPerTcam);
    int getBitPerSram() const;
    void setBitPerSram(int bitPerSram);
    const std::unordered_map<int, SRAM *> &getSramMap() const;
    void setSramMap(const std::unordered_map<int, SRAM *> &sramMap);
    const std::unordered_map<int, TCAM *> &getTcamMap() const;
    void setTcamMap(const std::unordered_map<int, TCAM *> &tcamMap);
    const std::unordered_map<int, LogicalTable *> &getLogicalTableMap() const;
    void setLogicalTableMap(const std::unordered_map<int, LogicalTable *> &logicalTableMap);
    const std::unordered_map<int, FlowTable *> &getFlowTableMap() const;
    void setFlowTableMap(const std::unordered_map<int, FlowTable *> &flowTableMap);
    const std::unordered_map<std::string, int> &getFlowTableNameIdMap() const;
    void setFlowTableNameIdMap(const std::unordered_map<std::string, int> &flowTableNameIdMap);

private:
    int tcam_num;
    int sram_num;

    int bit_per_tcam;
    int bit_per_sram;

    int cur_sram_idx;
    int cur_tcam_idx;
    int cur_logical_table_idx;
    int cur_flow_table_idx;

    std::unordered_map<int, SRAM*> sram_map;
    std::unordered_map<int, TCAM*> tcam_map;
    std::unordered_map<int, LogicalTable*> logical_table_map;
    std::unordered_map<int, FlowTable*> flow_table_map;
    std::unordered_map<std::string, int> flow_table_name_id_map;
};

void MemConfig::initSRAMFromJson(const std::string &json_filename) {
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    this->sram_num = d["sram_num"].GetInt();
    this->bit_per_sram = d["sram_bit"].GetInt();

    for(int i = 0; i < this->sram_num; i++){
        auto sram = new SRAM(bit_per_sram);
        sram->setSramId(i);
        this->sram_map.insert(std::make_pair(i, sram));
    }

    for(auto it = d["srams"].MemberBegin(); it != d["srams"].MemberEnd(); ++it) {
        for(auto ele = it->value.MemberBegin(); ele != it->value.MemberEnd(); ++ele){
            auto lt = new LogicalTable(cur_logical_table_idx,
                                       MemType::MEM_SRAM,
                                       cur_sram_idx,
                                       ele->value["entry_width"].GetInt(),
                                       ele->value["entry_num"].GetInt());
            logical_table_map.insert(std::make_pair(lt->getLogicalTableId(), lt));
            sram_map[lt->getResidedHardwareId()]->addLogicalTable(lt->getLogicalTableId());
            cur_logical_table_idx++;
        }
        cur_sram_idx++;
    }
}

void MemConfig::initTCAMFromJson(const std::string &json_filename) {
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    this->tcam_num = d["tcam_num"].GetInt();
    this->bit_per_tcam = d["tcam_bit"].GetInt();

    for(int i = 0; i < this->tcam_num; i++) {
        auto tcam = new TCAM();
        tcam->setTcamId(i);
        tcam->setCapacity(bit_per_tcam);
        this->tcam_map.insert(std::make_pair(tcam->getTcamId(), tcam));
    }

    for(auto it = d["tcams"].MemberBegin(); it != d["tcams"].MemberEnd(); ++it) {
        auto lt = new LogicalTable(cur_logical_table_idx,
                                    MemType::MEM_TCAM,
                                    cur_tcam_idx++,
                                    it->value["entry_length"].GetInt(),
                                    it->value["entry_num"].GetInt());
        logical_table_map.insert(std::make_pair(lt->getLogicalTableId(), lt));
        tcam_map[lt->getResidedHardwareId()]->setLogicalTableId(cur_logical_table_idx);
        cur_logical_table_idx++;
    }
}

MatchType MemConfig::getMatchType(const std::string& s_match_type) {
    if(s_match_type == "exact") {
        return MatchType::EXACT;
    }else if(s_match_type == "lpm") {
        return MatchType::LPM;
    }else if(s_match_type == "ternary") {
        return MatchType::TERNARY;
    }else if(s_match_type == "range") {
        return MatchType::RANGE;
    } else{
        return MatchType::NONE;
    }
}

MemType MemConfig::getMemType(const std::string& s_mem_type) {
    if(s_mem_type == "SRAM") {
        return MemType::MEM_SRAM;
    } else if(s_mem_type == "TCAM") {
        return MemType::MEM_TCAM;
    } else {
        return MemType::NONE;
    }
}

int MemConfig::getFlowTableIdByName(const std::string& flow_table_name) {
    return flow_table_name_id_map[flow_table_name];
}


LogicalTable * MemConfig::calLogicalTableByFlowTable(MemType mem_type, int key_length,
                                                            int value_length, int possible_entry_num){
    // search strategy: left most
    // min(supported_length - key_length)
    // max(current_capacity - possible_entry_num)
    // free logical table first

    LogicalTable * logical_table;
    int min_supported_length = 1000;

    if (mem_type == MemType::MEM_TCAM) {
        for (auto & it : logical_table_map) {
            auto lt = it.second;
            if(lt->getMemType() != mem_type)
                continue;

            int supportedMaxLength = lt->getSupportedMaxLength();
            if(lt->getCurrentCapacity() >= possible_entry_num
                        && (supportedMaxLength - key_length) >= 0
                        && (supportedMaxLength - key_length) <= min_supported_length){
                min_supported_length = supportedMaxLength - key_length;
                logical_table = lt;
            }
        }
        return logical_table;
    } else if(mem_type == MemType::MEM_SRAM) {
        for (auto & it : logical_table_map) {
            auto lt = it.second;
            if(lt->getMemType() != mem_type)
                continue;

            int supportedMaxLength = lt->getSupportedMaxLength();
            if(lt->getCurrentCapacity() >= possible_entry_num
               && (supportedMaxLength - key_length - value_length) >= 0
               && (supportedMaxLength - key_length - value_length) <= min_supported_length){
                min_supported_length = supportedMaxLength - key_length - value_length;
                logical_table = lt;
            }
        }
        return logical_table;
    }
}

void MemConfig::initFlowTableFromJson(const std::string &json_filename) {
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    for(auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
        std::string flow_table_name = it->value["flow_table_name"].GetString();
        std::string match_type = it->value["match_type"].GetString();
        std::string mem_type = it->value["mem_type"].GetString();
        int key_length = it->value["key_length"].GetInt();
        int value_length = it->value["value_length"].GetInt();
        int action_data_num = it->value["action_data_num"].GetInt();
        int possible_entry_num = it->value["possible_entry_num"].GetInt();

        LogicalTable* lt = this->calLogicalTableByFlowTable(getMemType(mem_type), key_length, value_length, possible_entry_num);

        FlowTable* ft = new FlowTable(cur_flow_table_idx, flow_table_name, getMatchType(match_type),
                                getMemType(mem_type), lt->getLogicalTableId(),
                                key_length, value_length, action_data_num, possible_entry_num);

        lt->addInsideFlowTable(cur_flow_table_idx);
        this->flow_table_map.insert(std::make_pair(ft->getFlowTableId(), ft));
        flow_table_name_id_map.insert(std::make_pair(ft->getFlowTableName(), ft->getFlowTableId()));
        cur_flow_table_idx++;
    }

}

FlowTable* MemConfig::addPureFlowTable(const std::string& flow_table_name, const std::string& matchType,
                      int key_length, int value_length, int action_data_num, int possible_entry_num) {
    MatchType match_type;
    MemType mem_type = MemType::MEM_TCAM;
    if(matchType == "exact"){
        match_type = MatchType::EXACT;
        mem_type = MemType::MEM_SRAM;
    } else if(matchType == "lpm") {
        match_type = MatchType::LPM;
    } else if(matchType == "ternary") {
        match_type = MatchType::TERNARY;
    } else if(matchType == "range") {
        match_type = MatchType::RANGE;
    } else{

    }

    auto ft = new FlowTable(flow_table_name, match_type, mem_type, key_length, value_length, action_data_num, possible_entry_num);
    this->addFlowTable(ft);
    return ft;
}

void MemConfig::addFlowTable(FlowTable *ft){
    ft->setFlowTableId(cur_flow_table_idx);
    cur_flow_table_idx++;
    flow_table_map.insert(std::make_pair(ft->getFlowTableId(), ft));
    flow_table_name_id_map.insert(std::make_pair(ft->getFlowTableName(), ft->getFlowTableId()));

    LogicalTable* lt = this->calLogicalTableByFlowTable(ft->getMemType(),
                                                        ft->getKeyLength(),
                                                        ft->getValueLength(),
                                                        ft->getPossibleEntryNum());

    ft->setResidedLogicTableId(lt->getLogicalTableId());
}

int MemConfig::addEntry(std::string flow_table_name, int entry_num) {
    int flow_table_id = flow_table_name_id_map[flow_table_name];
    auto ft = flow_table_map[flow_table_id];
    MemType mem_type = ft->getMemType();

    auto lt = logical_table_map[ft->getResidedLogicTableId()];
    int current_capacity = lt->getCurrentCapacity();
    if (current_capacity < entry_num) {
        return -1;
    } else {
        lt->setCurrentCapacity(current_capacity - entry_num);
        ft->setCurrentEntryNum(ft->getCurrentEntryNum() + entry_num);
        return 0;
    }
    return 0;
}

int MemConfig::getTcamNum() const {
    return tcam_num;
}

void MemConfig::setTcamNum(int tcamNum) {
    tcam_num = tcamNum;
}

int MemConfig::getSramNum() const {
    return sram_num;
}

void MemConfig::setSramNum(int sramNum) {
    sram_num = sramNum;
}

int MemConfig::getBitPerTcam() const {
    return bit_per_tcam;
}

void MemConfig::setBitPerTcam(int bitPerTcam) {
    bit_per_tcam = bitPerTcam;
}

int MemConfig::getBitPerSram() const {
    return bit_per_sram;
}

void MemConfig::setBitPerSram(int bitPerSram) {
    bit_per_sram = bitPerSram;
}

const std::unordered_map<int, SRAM *> &MemConfig::getSramMap() const {
    return sram_map;
}

void MemConfig::setSramMap(const std::unordered_map<int, SRAM *> &sramMap) {
    sram_map = sramMap;
}

const std::unordered_map<int, TCAM *> &MemConfig::getTcamMap() const {
    return tcam_map;
}

void MemConfig::setTcamMap(const std::unordered_map<int, TCAM *> &tcamMap) {
    tcam_map = tcamMap;
}

const std::unordered_map<int, LogicalTable *> &MemConfig::getLogicalTableMap() const {
    return logical_table_map;
}

void MemConfig::setLogicalTableMap(const std::unordered_map<int, LogicalTable *> &logicalTableMap) {
    logical_table_map = logicalTableMap;
}

const std::unordered_map<int, FlowTable *> &MemConfig::getFlowTableMap() const {
    return flow_table_map;
}

void MemConfig::setFlowTableMap(const std::unordered_map<int, FlowTable *> &flowTableMap) {
    flow_table_map = flowTableMap;
}

const std::unordered_map<std::string, int> &MemConfig::getFlowTableNameIdMap() const {
    return flow_table_name_id_map;
}

void MemConfig::setFlowTableNameIdMap(const std::unordered_map<std::string, int> &flowTableNameIdMap) {
    flow_table_name_id_map = flowTableNameIdMap;
}

void MemConfig::printFlowTableInfo() {
    for(auto & it : flow_table_map) {
        std::cout << "************** Flow Table " << it.first << " **************" << std::endl;
        auto ft = it.second;
        std::cout << "flow table name: " << ft->getFlowTableName() << std::endl;
        std::cout << "logical table id: " << ft->getResidedLogicTableId() << std::endl;
        std::cout << "logical table support length: " << logical_table_map[ft->getResidedLogicTableId()]->getSupportedMaxLength() << std::endl;
        std::cout << "logical table support num: " << logical_table_map[ft->getResidedLogicTableId()]->getSupportedMaxNum() << std::endl;
        std::cout << "logical table current capacity: " << logical_table_map[ft->getResidedLogicTableId()]->getCurrentCapacity() << std::endl;
        std::cout << "key length: " << ft->getKeyLength() << std::endl;
        std::cout << "value length: " << ft->getValueLength() << std::endl;
        std::cout << "action data num: " << ft->getActionDataNum() << std::endl;
        std::cout << "possible entry num: " << ft->getPossibleEntryNum() << std::endl;
        std::cout << "current entry num: " << ft->getCurrentEntryNum() << std::endl;
    }
}

void MemConfig::printSRAMInfo() {
    for(auto & sram : sram_map) {
        std::cout << "************** SRAM " << sram.first << " **************" << std::endl;
        std::cout << "Capacity: " << sram.second->getCapacity() << std::endl;
        std::cout << "Free Capacity: " << sram.second->getFreeCapacity() << std::endl;
        for(auto lt_id : sram.second->getLogicalTables()){
            std::cout << "\tLogical Table " << lt_id << ": " << std::endl;
            auto lt = logical_table_map[lt_id];
            std::cout << "\t\tMatchType: " << static_cast<bool>(lt->getMemType()) << std::endl;
            std::cout << "\t\tResidedSRAMId: " << lt->getResidedHardwareId() << std::endl;
            std::cout << "\t\tSupportedMaxLength: " << lt->getSupportedMaxLength() << std::endl;
            std::cout << "\t\tSupportedMaxNum: " << lt->getSupportedMaxNum() << std::endl;
            std::cout << "\t\tCurrentCapacity: " << lt->getCurrentCapacity() << std::endl;
        }
    }
}

void MemConfig::printTCAMInfo() {
    for(auto & tcam : tcam_map) {
        std::cout << "************** TCAM " << tcam.first << " **************" << std::endl;
        std::cout << "Capacity: " << tcam.second->getCapacity() << std::endl;
        std::cout << "Free Capacity: " << tcam.second->getFreeCapacity() << std::endl;

        auto lt_id = tcam.second->getLogicalTableId();
        std::cout << "\tLogical Table " << lt_id << ": " << std::endl;
        auto lt = logical_table_map[lt_id];
        std::cout << "\t\tMatchType: " << static_cast<bool>(lt->getMemType()) << std::endl;
        std::cout << "\t\tResidedTCAMId: " << lt->getResidedHardwareId() << std::endl;
        std::cout << "\t\tSupportedMaxLength: " << lt->getSupportedMaxLength() << std::endl;
        std::cout << "\t\tSupportedMaxNum: " << lt->getSupportedMaxNum() << std::endl;
        std::cout << "\t\tCurrentCapacity: " << lt->getCurrentCapacity() << std::endl;
    }
}

#endif //GRPC_TEST_MEM_CONFIG_H
