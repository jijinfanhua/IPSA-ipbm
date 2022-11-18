//
// Created by fengyong on 2021/2/19.
//

#include "../include/executer_config.h"

int main() {
    auto processor = new Processor(8);

    auto parser = new Parser();
    parser->initializeFromJson("../config/header.json");
//    p->printHeaderInfo();

    auto mem_config = new MemConfig();
    mem_config->initSRAMFromJson("../config/sram.json");
    mem_config->initTCAMFromJson("../config/tcam.json");

    mem_config->initFlowTableFromJson("../config/flow_table.json");
//    mem_config->printFlowTableInfo();

    auto matcher_config = new MatcherConfig();
    matcher_config->initFromJson("../config/matcher.json", parser, mem_config, processor);
//    matcher_config->printMatcher();

    auto executer_config = new ExecuterConfig(8);
    executer_config->initExecuterFromJson("../config/executer.json", matcher_config, processor);
    executer_config->printActionsByExecuter();

//    auto executer_config = new ExecuterConfig(8);
//
//    auto action = new Action();
//    action->action_name = "set_nhop";
//    action->parameter_num = 3;
//
//    auto p1 = new Primitive();
//    p1->op = OpCode::COPY_FIELD;
//    p1->parameters.emplace_back("1.1");
//    p1->parameters.emplace_back("p.1");
//    action->addPrimitive(p1);
//
//    auto p2 = new Primitive();
//    p2->op = OpCode::COPY_FIELD;
//    p2->parameters.emplace_back("3.19");
//    p2->parameters.emplace_back("p.2");
//    action->addPrimitive(p2);
//
//    auto p3 = new Primitive();
//    p3->op = OpCode::COPY_FIELD;
//    p3->parameters.emplace_back("s.1");
//    p3->parameters.emplace_back("p.3");
//    action->addPrimitive(p3);
//
//    auto p4 = new Primitive();
//    p4->op = OpCode::DECREMENT;
//    p4->parameters.emplace_back("3.15");
//    action->addPrimitive(p4);
//
//    executer_config->addAction(action, 1);
//
////    executer_config->printAllActions();
//    executer_config->printActionsByExecuter();

    return 0;
}