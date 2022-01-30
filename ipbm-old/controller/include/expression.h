//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_EXPRESSION_H
#define GRPC_TEST_EXPRESSION_H

#include <iostream>
#include <unordered_map>

enum class OpCode {
    ADD, SUB, MOD, DIV, MUL,
    SHIFT_LEFT, SHIFT_RIGHT, EQ_DATA, NEQ_DATA, GT_DATA, LT_DATA, GET_DATA, LET_DATA,
    AND, OR, NOT, BIT_AND, BIT_OR, BIT_XOR, BIT_NEG,
    TERNARY_OP,
    SET_FIELD, COPY_FIELD, SET_TTL, DECREMENT_TTL, COPY_TTL_OUTWARDS, COPY_TTL_INWARDS, PUSH, POP
};

class OpCodeMap {
public:
    static OpCode get_opcode(std::string op_name);
    static uint32_t get_parameter_num(OpCode op_code);
private:
    OpCodeMap();
    static OpCodeMap *get_instance();

    std::unordered_map<std::string, OpCode> opcode_map{};
    std::unordered_map<OpCode, uint32_t> opcode_parameter_num_map{};
};

#endif //GRPC_TEST_EXPRESSION_H
