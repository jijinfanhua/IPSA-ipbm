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

struct MatchFieldNode {
    int header_id;
    char header_name[255];
    int field_id;
    char field_name[255];
    MatchType matchType;
    MatchFieldNode *next = nullptr;
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

    MatchFieldNode * match_field_start;

    std::string flow_table_name;
    int flow_table_id;
};

class MatcherConfig {
public:
    MatcherConfig() : cur_matcher_id(0) {};
    void setMatcherMap(std::unordered_map<int, Matcher*> matcherMap);
    void addMatcher(MatchFieldNode* match_field, Matcher* matcher,
                            Parser* parser, MemConfig * mem_handler);

    void printMatcher();

private:
    std::unordered_map<int, Matcher*> matcher_map;
    int cur_matcher_id;
};

void MatcherConfig::setMatcherMap(std::unordered_map<int, Matcher *> matcherMap) {
    this->matcher_map = std::move(matcherMap);
}

void MatcherConfig::addMatcher(MatchFieldNode* match_field, Matcher *matcher,
                               Parser* parser, MemConfig * mem_handler) {
//    parser->printHeaderNameIDMap();
    matcher->matcher_id = cur_matcher_id++;
    matcher->resided_processor_id = 0; // TODO: add processor class and function
    matcher->flow_table_id = mem_handler->getFlowTableIdByName(matcher->flow_table_name);
    MatchFieldNode * cur;
    cur = match_field->next;
    while(cur) {
//        std::cout << cur->header_name << " : " << cur->field_name << std::endl;
        cur->header_id = parser->getHeaderIdByName(cur->header_name);
        cur->field_id = parser->getFieldIdByName(cur->field_name);
        cur = cur->next;
    }
    matcher->match_field_start = match_field;
    matcher_map.insert(std::make_pair(matcher->matcher_id, matcher));
}

void MatcherConfig::printMatcher() {
    std::cout << "*********** Matcher ************" << std::endl;
    for(auto it : matcher_map) {
        auto matcher = it.second;
        std::cout << "\tMatcher ID: " << it.first << std::endl;
        std::cout << "\tFlow_table_name: " << matcher->flow_table_name << std::endl;
        std::cout << "\tFlow_table_id: " << matcher->flow_table_id << std::endl;
        std::cout << "\tResided_processor_id: " << matcher->resided_processor_id << std::endl;
        auto cur = matcher->match_field_start->next;
        while(cur) {
            std::cout << "\t\t" << cur->header_name << "(" << cur->header_id << ").";
            std::cout << cur->field_name << "(" << cur->field_id << "):";
            std::cout << static_cast<bool>(cur->matchType) << std::endl;
            cur = cur->next;
        }
    }
}

#endif //GRPC_TEST_MATCHER_H
