#pragma once

#include <vector>
#include <map>
#include "rp4_treenode.h"
#include "rp4_lvalue.h"

enum Rp4Operator {
    OP_PLUS, OP_MINUS, OP_AND, OP_OR, OP_XOR,
    OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE,
    OP_NOT, OP_NOTL, OP_MUL, OP_DIV
};

static inline std::string to_string(Rp4Operator op) {
    static const std::map<Rp4Operator, std::string> m = {
            {OP_EQ, "=="},  {OP_PLUS, "+"},     {OP_MUL, "*"},
            {OP_NE, "!="},  {OP_MINUS, "-"},    {OP_DIV, "/"},
            {OP_LT, "<"},   {OP_AND, "&"},      {OP_NOTL, "!"},
            {OP_GT, ">"},   {OP_OR, "|"},
            {OP_LE, "<="},  {OP_XOR, "^"},
            {OP_GE, ">="},  {OP_NOT, "~"}
    };
    return m.at(op);
}

class Rp4Literal : public Rp4Operand {
public:
    int value;

    Rp4Literal(int _value) : value(_value) {}
    virtual std::string toString() const {
        return "literal(" + std::to_string(value) + ")";
    }
    virtual bool isLiteral() const { return true; }
};

class Rp4Parameter : public Rp4Operand {
public:
    std::string name;
    Rp4Parameter() {}
    Rp4Parameter(std::string _name) : name(std::move(_name)) {}
    virtual std::string toString() const {
        return "parameter(" + name + ")";
    }
    virtual bool isParameter() const { return true; }
};

// single-operand operators use left as nullptr
class Rp4Binary : public Rp4Operation {
public:
    Rp4Operator op;
    std::shared_ptr<Rp4Operation> left = nullptr;
    std::shared_ptr<Rp4Operation> right = nullptr;
    Rp4Binary() {}
    Rp4Binary(Rp4Operator _op, std::shared_ptr<Rp4Operation> _left, std::shared_ptr<Rp4Operation> _right):
        op(_op), left(std::move(_left)), right(std::move(_right)) {}
    virtual std::string toString() const {
        return "operator[" + to_string(op) + "]";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        std::vector<const Rp4TreeNode*> dst;
        for (auto& ptr : { left, right }) {
            if (auto x = dynamic_cast<const Rp4TreeNode*>(ptr.get()); x != nullptr) {
                dst.push_back(x);
            }
        }
        return std::move(dst);
    }
    virtual bool isBinary() const { return true; }
};

class Rp4Expression : public Rp4TreeNode {
public:
    std::shared_ptr<Rp4LValue> lvalue;
    std::shared_ptr<Rp4Operation> rvalue;
    Rp4Expression() {}
    Rp4Expression(std::shared_ptr<Rp4LValue> _lvalue, std::shared_ptr<Rp4Operation> _rvalue):
        lvalue(std::move(_lvalue)), rvalue(std::move(_rvalue)) {}
    virtual std::string toString() const {
        return "expression";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return {
            dynamic_cast<const Rp4TreeNode*>(lvalue.get()),
            dynamic_cast<const Rp4TreeNode*>(rvalue.get())
        };
    }
};