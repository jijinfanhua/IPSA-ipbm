//
// Created by fengyong on 2021/2/9.
//

#ifndef CONTROLLER_PRIMITIVE_H
#define CONTROLLER_PRIMITIVE_H

#include <iostream>
#include <unordered_map>

enum class OpCode {
    ADD, SUB, MOD, DIV, MUL,
    SHIFT_LEFT, SHIFT_RIGHT, EQ_DATA, NEQ_DATA, GT_DATA, LT_DATA, GET_DATA, LET_DATA,
    AND, OR, NOT, BIT_AND, BIT_OR, BIT_XOR, BIT_NEG,
    TERNARY_OP
};

/**
 * define basic operations
 */
class Primitive {
public:
    Primitive();
private:
    uint32_t primitive_id;

};

#endif //CONTROLLER_PRIMITIVE_H
