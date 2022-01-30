//
// Created by fengyong on 2/21/21.
//

#ifndef GRPC_TEST_EXECUTER_CONFIG_H
#define GRPC_TEST_EXECUTER_CONFIG_H

#include "executer.h"
#include "matcher.h"

/**
 * action set_nhop(bit<48> nhop_dmac, bit<32> nhop_ipv4, bit<9> port) {
 *      hdr.ethernet.dstAddr = nhop_dmac;
 *      hdr.ipv4.dstAddr = nhop_ipv4;
 *      standard_metadata.egress_spec = port;
 *      hdr.ipv4.ttl = hdr.ipv4.ttl - 1;
 * }
 *
 * -> copy_field(ethernet.dstAddr, nhop_dmac);
 * -> copy_field(ipv4.dstAddr, nhop_ipv4);
 * -> copy_field(standard_metadata.egress_spec, port);
 * -> decrement(ipv4.ttl);
 *
 * copy_field 1.1 p.1
 * copy_field 3.19 p.2
 * copy_field s.1 p.3
 * decrement 3.15
 */

class ExecuterConfig {
public:
    ExecuterConfig(int processorNum);
    ExecuterConfig() : cur_action_idx(0) {};

    void setProcessorNum(int processorNum);

    void addAction(Action * action, int processor_id);
    Action* addActionByJson(const std::string &json_filename,
                         Processor* processor);


    void initExecuterFromJson(const std::string &json_filename,
                              MatcherConfig *match_config, Processor* processor);

    void printAllActions();
    void printActionsByExecuter();

    int getActionIdByName(const std::string & action_name);

    std::string searchDesUsingOpCode(OpCode op);
private:
    std::unordered_map<int, Action*> action_map;
    std::unordered_map<int, Executer*> executer_map;

    int cur_action_idx;
    int processor_num;
};

Action* ExecuterConfig::addActionByJson(const std::string &json_filename,
                     Processor* processor) {
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    std::string processor_name = d["processor_name"].GetString();
    int processor_id = processor->allocExecuterToProcessor(processor_name);
    std::string action_name = d["action_name"].GetString();
    int para_num = d["parameter_num"].GetInt();

    auto ac = new Action();
    ac->action_id = cur_action_idx;
    cur_action_idx++;
    ac->action_name = action_name;
    ac->parameter_num = para_num;
    ac->processor_id = processor_id;

    action_map.insert(std::make_pair(ac->action_id, ac));

    auto primitive_array = d["primitives"].GetArray();
    int primitive_num = primitive_array.Size();
    for (int j = 0; j < primitive_num; j++) {
        auto primitive = new Primitive();

        std::string primitive_name = primitive_array[j]["primitive_name"].GetString();
        primitive->op = Op.opcode_map[primitive_name];

        auto parameter_array = primitive_array[j]["parameters"].GetArray();
        int parameter_num = parameter_array.Size();
        for (int k = 0; k < parameter_num; k++) {
            Parameter para = {
                    .type = parameter_array[k]["type"].GetString(),
                    .value = parameter_array[k]["value"].GetString()
            };
//            std::string parameter_type = parameter_array[k]["type"].GetString();
//            std::string parameter_value = parameter_array[k]["value"].GetString();
            primitive->parameters.push_back(para);
        }
        ac->primitives.push_back(primitive);
    }

    auto parameter_length_array = d["parameter_length"].GetArray();
    int parameter_length_num = parameter_length_array.Size();
    for (int j = 0; j < parameter_length_num; j++) {
        ac->parameter_lengths.push_back(parameter_length_array[j].GetInt());
    }

    this->executer_map[processor_id]->addAction(ac);
    return ac;
}

void ExecuterConfig::initExecuterFromJson(const std::string &json_filename,
                                          MatcherConfig *match_config, Processor* processor) {
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
        std::string processor_name = it->name.GetString();
        int processor_id = processor->allocExecuterToProcessor(processor_name);

        auto actions = it->value.GetArray();
        int action_num = actions.Size();
        for(int i = 0; i < action_num; i++) {
            std::string action_name = actions[i]["action_name"].GetString();
            int para_num = actions[i]["parameter_num"].GetInt();

            auto ac = new Action();
            ac->action_id = cur_action_idx;
            cur_action_idx++;
            ac->action_name = action_name;
            ac->parameter_num = para_num;

            action_map.insert(std::make_pair(ac->action_id, ac));

            auto primitive_array = actions[i]["primitives"].GetArray();
            int primitive_num = primitive_array.Size();
            for (int j = 0; j < primitive_num; j++) {
                auto primitive = new Primitive();

                std::string primitive_name = primitive_array[j]["primitive_name"].GetString();
                primitive->op = Op.opcode_map[primitive_name];

//                std::cout << "\t\t" << primitive_name << std::endl;
                auto parameter_array = primitive_array[j]["parameters"].GetArray();
                int parameter_num = parameter_array.Size();
                for (int k = 0; k < parameter_num; k++) {
                    Parameter para = {
                            .type = parameter_array[k]["type"].GetString(),
                            .value = parameter_array[k]["value"].GetString()
                    };
//                    std::string parameter_type = parameter_array[k]["type"].GetString();
//                    std::string parameter_value = parameter_array[k]["value"].GetString();
                    primitive->parameters.push_back(para);
//                    std::cout << "\t\t\t" << parameter_type << "." << parameter_value << std::endl;
                }
                ac->primitives.push_back(primitive);
            }
            this->executer_map[processor_id]->addAction(ac);
//            this->executer_map[match_config->getProcessorIdByFlowTableName(flow_table_name)]->addAction(ac);
        }
    }

}

//void ExecuterConfig::initExecuterFromJson(const std::string &json_filename, MatcherConfig *match_config) {
//    FILE* fp = fopen(json_filename.c_str(), "rb");
//    char readBufer[65536];
//    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));
//
//    rapidjson::Document d;
//    d.ParseStream(is);
//    fclose(fp);
//
//    for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
//        std::string flow_table_name = it->name.GetString();
////        auto executer = new Executer(match_config->getProcessorIdByFlowTableName(flow_table_name));
////        std::cout << flow_table_name << std::endl;
//        for(auto action = it->value.MemberBegin(); action != it->value.MemberEnd(); ++action) {
//            std::string action_name = action->value["action_name"].GetString();
//            int para_num = action->value["parameter_num"].GetInt();
//
//            auto ac = new Action();
//            ac->action_id = cur_action_idx;
//            cur_action_idx++;
//            ac->action_name = action_name;
//            ac->parameter_num = para_num;
//
//            action_map.insert(std::make_pair(ac->action_id, ac));
//
////            std::cout << "\t" << action_name << std::endl;
//            auto primitive_array = action->value["primitives"].GetArray();
//            int primitive_num = primitive_array.Size();
//            for (int i = 0; i < primitive_num; i++) {
//                auto primitive = new Primitive();
//
//                std::string primitive_name = primitive_array[i]["primitive_name"].GetString();
//                primitive->op = Op.opcode_map[primitive_name];
//
////                std::cout << "\t\t" << primitive_name << std::endl;
//                auto parameter_array = primitive_array[i]["parameters"].GetArray();
//                int parameter_num = parameter_array.Size();
//                for (int j = 0; j < parameter_num; j++) {
//                    std::string parameter_type = parameter_array[j]["type"].GetString();
//                    std::string parameter_value = parameter_array[j]["value"].GetString();
//                    primitive->parameters.push_back(parameter_type + "." + parameter_value);
////                    std::cout << "\t\t\t" << parameter_type << "." << parameter_value << std::endl;
//                }
//                ac->primitives.push_back(primitive);
//            }
//            this->executer_map[match_config->getProcessorIdByFlowTableName(flow_table_name)]->addAction(ac);
//        }
////        executer_map.insert(std::make_pair(executer->getResidedProcessorId(), executer));
//    }
//
//}

void ExecuterConfig::addAction(Action *action, int processor_id) {
    action->action_id = cur_action_idx++;
    this->action_map.insert(std::make_pair(action->action_id, action));

    executer_map[processor_id]->addAction(action);
}

void ExecuterConfig::printAllActions() {
    std::cout << "************** Actions **************" << std::endl;
    for(auto it : action_map) {
        auto action = it.second;
        std::cout << "Action " << it.first << "(" << action->action_name << ")" << std::endl;
//        std::cout << "\tProcessor: " << action->processor_id << std::endl;
        std::cout << "\tPrimitives: " << std::endl;
        for(auto ele : action->primitives) {
            std::cout << "\t\t" << Op.opcode_map_inverse[ele->op] << ": ";
            for(auto p : ele->parameters) {
                std::cout << p.type << "." << p.value << " ";
            }
            std::cout << std::endl;
        }
    }
}


void ExecuterConfig::printActionsByExecuter() {
    std::cout << "************** Actions **************" << std::endl;
    for(auto it : executer_map) {
        auto executer = it.second;
        executer->printActions();

//        std::cout << "Action " << it.first << "(" << action->action_name << ")" << std::endl;
//        std::cout << "\tProcessor: " << action->processor_id << std::endl;
//        std::cout << "\tPrimitives: " << std::endl;
//        for(auto ele : action->primitives) {
//            std::cout << "\t\t" <<searchDesUsingOpCode(ele->op) << ": ";
//            for(auto p : ele->parameters) {
//                std::cout << p << " ";
//            }
//            std::cout << std::endl;
//        }
    }
}

ExecuterConfig::ExecuterConfig(int processor_num) {
    cur_action_idx = 0;

    for(int i = 0; i < processor_num; i++) {
        auto executer = new Executer(i);
        this->executer_map.insert(std::make_pair(i, executer));
    }
}

std::string ExecuterConfig::searchDesUsingOpCode(OpCode op) {
    OpCodeMap  pc;
    for(auto it : Op.opcode_map) {
        if(it.second == op){
            return it.first;
        }
    }
}

int ExecuterConfig::getActionIdByName(const std::string & action_name) {
    for(auto it : action_map) {
        if (it.second->action_name == action_name) {
            return it.second->action_id;
        }
    }
    return -1;
}

void ExecuterConfig::setProcessorNum(int processoNum){
    this->processor_num = processoNum;

    for(int i = 0; i < processor_num; i++) {
        auto executer = new Executer(i);
        this->executer_map.insert(std::make_pair(i, executer));
    }
}


#endif //GRPC_TEST_EXECUTER_CONFIG_H
