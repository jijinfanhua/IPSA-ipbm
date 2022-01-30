//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_MATCHER_H
#define GRPC_TEST_MATCHER_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

//#include "sram_mem_config.h"
#include "mem_config.h"
#include "parser.h"
#include "processor.h"

struct MatchField {
    int header_id;
    std::string header_name;
    int field_id;
    std::string field_name;
    MatchType matchType;
    int field_length;
//    MatchFieldNode *next = nullptr;
};

//struct MatchFieldName {
//    char header_name[255];
//    char field_name[255];
//    MatchType matchType;
//    MatchFieldName *next = nullptr;
//};

struct Matcher {
    uint32_t matcher_id;
    uint32_t resided_processor_id;

    std::vector<MatchField*> match_fields;

    std::string flow_table_name;
    int flow_table_id;
    MatchType match_type;
};

class MatcherConfig {
public:
    MatcherConfig() : cur_matcher_id(0) {};
    void setMatcherMap(std::unordered_map<int, Matcher*> matcherMap);
//    void addMatcher(MatchFieldNode* match_field, Matcher* matcher,
//                            Parser* parser, MemConfig * mem_handler);

    int initFromJson(const std::string &json_filename, Parser *parser,
                      MemConfig *mem_config, Processor* processor);

    std::string getMemTypeName(MemType mem_type);
    std::string getMatchTypeName(MatchType mem_type);

    int getProcessorIdByFlowTableName(const std::string& flow_table_name);

    void addMatcherAtRuntime(const std::string& processor_name, Matcher *matcher,
                             Parser *parser,
                             MemConfig *mem_config, Processor* processor);

    void printMatcher();

private:
    std::unordered_map<int, Matcher*> matcher_map;
    int cur_matcher_id;
};

void MatcherConfig::setMatcherMap(std::unordered_map<int, Matcher *> matcherMap) {
    this->matcher_map = std::move(matcherMap);
}

int MatcherConfig::getProcessorIdByFlowTableName(const std::string& flow_table_name) {
    for(auto it : matcher_map) {
        if(it.second->flow_table_name == flow_table_name) {
            return it.second->resided_processor_id;
        }
    }
    return 0;
}

void MatcherConfig::addMatcherAtRuntime(const std::string& processor_name, Matcher *matcher,
                                        Parser *parser,
                                        MemConfig *mem_config, Processor* processor) {
    int proc_id = processor->allocMatcherToProcessor(processor_name);
    matcher->matcher_id = proc_id;
    matcher->resided_processor_id = proc_id;

    this->matcher_map.insert(std::make_pair(matcher->matcher_id, matcher));
}

int MatcherConfig::initFromJson(const std::string &json_filename, Parser *parser,
                                 MemConfig *mem_config, Processor* processor) {
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    for(auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
        std::string processor_name = it->name.GetString();
        int proc_id = processor->allocMatcherToProcessor(processor_name);

        auto matcher = new Matcher();
        matcher->matcher_id = proc_id;
        matcher->resided_processor_id = proc_id; // TODO: add cal processor id function? done!

        std::string flow_table_name = it->value["table_name"].GetString();
        int flow_table_id = mem_config->getFlowTableIdByName(flow_table_name);
        matcher->flow_table_name = flow_table_name;
        matcher->flow_table_id = flow_table_id;

        auto field_array = it->value["fields"].GetArray();
        int size = field_array.Size();
        for(int i = 0; i < size; i++) {
            std::string s_header_name = field_array[i]["header_name"].GetString();
            std::string s_field_name = field_array[i]["field_name"].GetString();
            std::string s_match_type = field_array[i]["match_type"].GetString();

            int header_id = parser->getHeaderIdByName(s_header_name);
            int field_id = parser->getFieldIdByName(s_field_name);
            MatchType match_type = mem_config->getMatchType(s_match_type);

            auto match_field = new MatchField();
            match_field->field_id = field_id;
            match_field->field_name = s_field_name;
            match_field->header_id = header_id;
            match_field->header_name = s_header_name;
            match_field->matchType = match_type;

            matcher->match_fields.push_back(match_field);
        }

        this->matcher_map.insert(std::make_pair(matcher->matcher_id, matcher));
    }
    return 0;
}

//void MatcherConfig::addMatcher(MatchFieldNode* match_field, Matcher *matcher,
//                               Parser* parser, MemConfig * mem_handler) {
////    parser->printHeaderNameIDMap();
//    matcher->matcher_id = cur_matcher_id++;
//    matcher->resided_processor_id = 0; // TODO: add processor class and function
//    matcher->flow_table_id = mem_handler->getFlowTableIdByName(matcher->flow_table_name);
//    MatchFieldNode * cur;
//    cur = match_field->next;
//    while(cur) {
////        std::cout << cur->header_name << " : " << cur->field_name << std::endl;
//        cur->header_id = parser->getHeaderIdByName(cur->header_name);
//        cur->field_id = parser->getFieldIdByName(cur->field_name);
//        cur = cur->next;
//    }
//    matcher->match_field_start = match_field;
//    matcher_map.insert(std::make_pair(matcher->matcher_id, matcher));
//}
//

void MatcherConfig::printMatcher() {
    std::cout << "*********** Matcher ************" << std::endl;
    for(auto it : matcher_map) {
        auto matcher = it.second;
        std::cout << "Matcher ID: " << it.first << std::endl;
        std::cout << "\tFlow_table_name: " << matcher->flow_table_name << std::endl;
        std::cout << "\tFlow_table_id: " << matcher->flow_table_id << std::endl;
        std::cout << "\tResided_processor_id: " << matcher->resided_processor_id << std::endl;
        std::cout << "\tFields: " << std::endl;

        auto match_fields = matcher->match_fields;
        for(int i = 0; i < match_fields.size(); i++) {
            std::cout << "\t\t" << match_fields[i]->header_name << "." ;
            std::cout << match_fields[i]->field_name << " : " ;
            std::cout << this->getMatchTypeName(match_fields[i]->matchType) << std::endl ;
        }
    }
}

std::string MatcherConfig::getMemTypeName(MemType mem_type) {
    if(mem_type == MemType::MEM_SRAM) {
        return "SRAM";
    } else {
      return "TCAM";
    }
}

std::string MatcherConfig::getMatchTypeName(MatchType match_type) {
    if(match_type == MatchType::EXACT) {
        return "exact";
    } else if(match_type == MatchType::TERNARY) {
        return "ternary";
    } else if(match_type == MatchType::LPM) {
        return "lpm";
    } else if(match_type == MatchType::RANGE) {
        return "range";
    }
    return "";
}

#endif //GRPC_TEST_MATCHER_H
