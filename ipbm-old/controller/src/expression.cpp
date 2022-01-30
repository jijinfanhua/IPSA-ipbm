//
// Created by fengyong on 2021/2/9.
//

#include "../include/expression.h"

OpCodeMap::OpCodeMap() {
    //SET_FIELD, COPY_FIELD, SET_TTL, DECREMENT_TTL, COPY_TTL_OUTWARDS, COPY_TTL_INWARDS, PUSH, POP
    opcode_map = {
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
            {"pop", OpCode::POP}
    };

    opcode_parameter_num_map = {
            {OpCode::ADD, 3},
            {OpCode::SUB, 3},
            {OpCode::MOD, 3},
            {OpCode::DIV, 3},
            {OpCode::MUL, 3},
            {OpCode::SHIFT_LEFT, 3},
            {OpCode::SHIFT_RIGHT, 3},
            {OpCode::EQ_DATA, 2},
            {OpCode::NEQ_DATA, 2},
            {OpCode::GT_DATA, 2},
            {OpCode::LT_DATA, 2},
            {OpCode::GET_DATA, 2},
            {OpCode::LET_DATA, 2},
            {OpCode::AND, 2},
            {OpCode::OR, 2},
            {OpCode::NOT, 2},
            { OpCode::BIT_AND, 2},
            {OpCode::BIT_OR, 2},
            {OpCode::BIT_XOR, 2},
            {OpCode::BIT_NEG, 1},
            {OpCode::TERNARY_OP, 3},
            {OpCode::SET_FIELD, 2},
            {OpCode::COPY_FIELD, 2},
            {OpCode::SET_TTL, 2},
            {OpCode::DECREMENT_TTL, 1},
            {OpCode::COPY_TTL_OUTWARDS, 2},
            {OpCode::COPY_TTL_INWARDS, 2},
            {OpCode::PUSH, 2},
            {OpCode::POP, 1}
    };
}

OpCodeMap *OpCodeMap::get_instance() {
    static OpCodeMap instance;
    return &instance;
}

OpCode OpCodeMap::get_opcode(std::string op_name) {
    OpCodeMap *instance = get_instance();
    return instance->opcode_map[op_name];
}

uint32_t OpCodeMap::get_parameter_num(OpCode op_code) {
    OpCodeMap *instance = get_instance();
    return instance->opcode_parameter_num_map[op_code];
}