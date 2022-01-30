//
// Created by fengyong on 2021/2/10.
//

#ifndef GRPC_TEST_EXECUTER_H
#define GRPC_TEST_EXECUTER_H

#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

#include "parser.h"
#include "mem_config.h"

// TODO: draw the relationship about memory resources and headers/fields

enum class OpCode {
    ADD, SUB, MOD, DIV, MUL,
    SHIFT_LEFT, SHIFT_RIGHT, EQ_DATA, NEQ_DATA, GT_DATA, LT_DATA, GET_DATA, LET_DATA,
    AND, OR, NOT, BIT_AND, BIT_OR, BIT_XOR, BIT_NEG,
    TERNARY_OP, COPY_FIELD, SET_FIELD, DECREMENT,
    SET_TTL, DECREMENT_TTL, COPY_TTL_OUTWARDS, COPY_TTL_INWARDS, PUSH, POP
};

struct Parameter {
    std::string type;
    std::string value;
};

struct Primitive {
    OpCode op;
    /**
     * parameter:
     * header.field: [integer].[integer]
     * action parameter: p.[integer]
     * metadata: m.[integer]
     * table/stage: t.[integer] (used in 'goto')
     * standard_metadata: s.[integer]
     */
    std::vector<Parameter> parameters;
};

typedef struct OpCodeMap {
    std::unordered_map<std::string, OpCode> opcode_map = {
        {"+", OpCode::ADD},
        {"-", OpCode::SUB},
        {"%", OpCode::MOD},
        {"/", OpCode::DIV},
        {"*", OpCode::MUL},
        {"<<", OpCode::SHIFT_LEFT},
        {">>", OpCode::SHIFT_RIGHT},
        {"==", OpCode::EQ_DATA},
        {"!=", OpCode::NEQ_DATA},
        {">", OpCode::GT_DATA},
        {"<", OpCode::LT_DATA},
        {">=", OpCode::GET_DATA},
        {"<=", OpCode::LET_DATA},
        {"and", OpCode::AND},
        {"or", OpCode::OR},
        {"not", OpCode::NOT},
        {"&", OpCode::BIT_AND},
        {"|", OpCode::BIT_OR},
        {"^", OpCode::BIT_XOR},
        {"~", OpCode::BIT_NEG},
        {"?", OpCode::TERNARY_OP},
        {"set_field", OpCode::SET_FIELD},
        {"copy_field", OpCode::COPY_FIELD},
        {"set_ttl", OpCode::SET_TTL},
        {"decrement_ttl", OpCode::DECREMENT_TTL},
        {"copy_ttl_outwards", OpCode::COPY_TTL_OUTWARDS},
        {"copy_ttl_inwards", OpCode::COPY_TTL_INWARDS},
        {"push", OpCode::PUSH},
        {"pop", OpCode::POP},
        {"decrement", OpCode::DECREMENT}
    };

    std::unordered_map<OpCode, std::string> opcode_map_inverse = {
            {OpCode::ADD, "+"},
            {OpCode::SUB, "-"},
            {OpCode::MOD, "%"},
            {OpCode::DIV, "/"},
            {OpCode::MUL, "*"},
            {OpCode::SHIFT_LEFT, "<<", },
            {OpCode::SHIFT_RIGHT, ">>",},
            {OpCode::EQ_DATA, "=="},
            {OpCode::NEQ_DATA, "!="},
            {OpCode::GT_DATA, ">"},
            {OpCode::LT_DATA, "<"},
            {OpCode::GET_DATA, ">="},
            {OpCode::LET_DATA, "<="},
            {OpCode::AND, "and"},
            {OpCode::OR, "or"},
            {OpCode::NOT, "not"},
            {OpCode::BIT_AND, "&"},
            {OpCode::BIT_OR, "|"},
            {OpCode::BIT_XOR, "^"},
            {OpCode::BIT_NEG, "~"},
            {OpCode::TERNARY_OP, "?"},
            {OpCode::SET_FIELD, "set_field"},
            {OpCode::COPY_FIELD, "copy_field"},
            {OpCode::SET_TTL, "set_ttl"},
            {OpCode::DECREMENT_TTL, "decrement_ttl"},
            {OpCode::COPY_TTL_OUTWARDS, "copy_ttl_outwards"},
            {OpCode::COPY_TTL_INWARDS, "copy_ttl_inwards"},
            {OpCode::PUSH, "push"},
            {OpCode::POP, "pop"},
            {OpCode::DECREMENT, "decrement"}
    };
} OpCodeMap;
OpCodeMap Op;

struct Action {
    int action_id;
    int processor_id;
    std::string action_name;
    int parameter_num;
//    std::list<Primitive*> primitives;
    std::vector<int> parameter_lengths;
    std::vector<Primitive*> primitives;


//    void addPrimitive(Primitive * p) {
//        this->primitives.push_back(p);
//    }
};

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

class Executer {
public:
    Executer(int processor_id);
    void addAction(Action * action);
    void printActions();
    std::string searchDesUsingOpCode(OpCode op);

    void setResidedProcessorId(int residedProcessorId);
    int getResidedProcessorId() const;
private:
    std::unordered_map<int, Action*> action_map;
    int resided_processor_id; // processor id is the same as executer id
//    std::unordered_map<std::string, OpCode> opcode_map{};
};

void Executer::addAction(Action * action) {
    this->action_map.insert(std::make_pair(action->action_id, action));
}

void Executer::printActions() {
    std::cout << "************** Executer " << this->resided_processor_id << " **************" << std::endl;
    for(auto it : action_map) {
        auto action = it.second;
        std::cout << "Action " << it.first << " (" << action->action_name << ")" << std::endl;
        std::cout << "\tPrimitives: " << std::endl;
        for(auto ele : action->primitives) {
            std::cout << "\t\t" << searchDesUsingOpCode(ele->op) << ": ";
            for(auto p : ele->parameters) {
                std::cout << p.type << "." << p.value << " ";
            }
            std::cout << std::endl;
        }
    }
}

Executer::Executer(int processor_id) {
    this->resided_processor_id = processor_id;
}

std::string Executer::searchDesUsingOpCode(OpCode op) {
    for(auto it : Op.opcode_map) {
        if(it.second == op){
            return it.first;
        }
    }
}

void Executer::setResidedProcessorId(int residedProcessorId) {
    this->resided_processor_id = residedProcessorId;
}

int Executer::getResidedProcessorId() const {
    return this->resided_processor_id;
}

#endif //GRPC_TEST_EXECUTER_H
