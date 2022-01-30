#pragma once

#include "ipsa_header_manager.h"

enum IpsaOperandType {
    OPT_CONST, OPT_EXPR, OPT_FIELD, OPT_PARAM
};

static inline std::string to_string(IpsaOperandType op) {
    static const std::map<IpsaOperandType, std::string> m = {
        {OPT_CONST, "CONST"},
        {OPT_EXPR, "EXPR"},
        {OPT_FIELD, "FIELD"},
        {OPT_PARAM, "PARAM"}
    };
    return m.at(op);
}

class IpsaOperand : public IpsaModule {
public:
    IpsaOperandType type;
    virtual bool isConstant() const { return false; }
    virtual bool isField() const { return false; }
    virtual bool isExpression() const { return false; }
    virtual bool isParameter() const { return false; }

    virtual bool isGateParameter() const { return false; }
    virtual std::shared_ptr<IpsaValue> getValue() const = 0;
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"type", makeValue(to_string(type))},
            {"value", getValue()}
        };
        return makeValue(dst);
    }
};

class IpsaConstant : public IpsaOperand {
public:
    int value;
    IpsaConstant(int _value) : value(_value) {
        type = OPT_CONST;
    }
    virtual bool isConstant() const { return true; }
    virtual bool isGateParameter() const { return true; }
    virtual std::shared_ptr<IpsaValue> getValue() const {
        return makeValue(value);
    }
};

class IpsaParameter : public IpsaOperand {
public:
    int parameter_id;
    IpsaParameter(int _parameter_id) : parameter_id(_parameter_id) {
        type = OPT_PARAM;
    }
    virtual bool isParameter() const { return true; }
    virtual std::shared_ptr<IpsaValue> getValue() const {
        return makeValue(parameter_id);
    }
};

class IpsaField : public IpsaOperand {
public:
    const IpsaHeaderField* header_field;
    IpsaField(const IpsaHeaderField* _header_field): header_field(_header_field) {
        type = OPT_FIELD;
    }
    virtual bool isField() const { return true; }
    virtual bool isGateParameter() const { return true; }
    virtual std::shared_ptr<IpsaValue> getValue() const {
        if (header_field != nullptr) {
            return header_field->toIpsaValue();
        } else {
            return nullptr;
        }
    }
};

class IpsaExpression : public IpsaOperand {
public:
    Rp4Operator op;
    std::shared_ptr<IpsaOperand> left;
    std::shared_ptr<IpsaOperand> right;
    IpsaExpression(Rp4Operator _op, std::shared_ptr<IpsaOperand> _left, std::shared_ptr<IpsaOperand> _right):
        op(_op), left(std::move(_left)), right(std::move(_right)) {
            type = OPT_EXPR;
        }
    virtual bool isExpression() const { return true; }
    virtual std::shared_ptr<IpsaValue> getValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"op", makeValue(to_string(op))},
            {"left", left != nullptr ? left->toIpsaValue() : nullptr},
            {"right", right != nullptr ? right->toIpsaValue() : nullptr}
        };
        return makeValue(dst);
    }
};

std::shared_ptr<IpsaOperand> parse_expr(
    std::shared_ptr<Rp4Operation> v,
    IpsaHeaderManager* header_manager,
    const Rp4ActionDef* action_def
) {
    if (v->isLiteral()) {
        return std::make_shared<IpsaConstant>(std::static_pointer_cast<Rp4Literal>(v)->value);
    } else if (v->isLValue()) {
        return std::make_shared<IpsaField>(
            header_manager->lookup(std::static_pointer_cast<Rp4LValue>(v))
        );
    } else if (v->isParameter()) {
        auto& name = std::static_pointer_cast<Rp4Parameter>(v)->name;
        for (int i = 0; i < action_def->parameters.size(); i++) {
            if (action_def->parameters[i].name == name) {
                return std::make_shared<IpsaParameter>(i);
            }
        }
    } else if (v->isBinary()) {
        auto x = std::static_pointer_cast<Rp4Binary>(v);
        return std::make_shared<IpsaExpression>(
            x->op, 
            parse_expr(x->left, header_manager, action_def), 
            parse_expr(x->right, header_manager, action_def)
        );
    }
    return nullptr;
}

IpsaExpression expr_to_expr(std::shared_ptr<Rp4Operation> v, IpsaHeaderManager* header_manager) {
    auto x = parse_expr(v, header_manager, nullptr);
    if (x->isConstant() || x->isField()) {
        return IpsaExpression(
            OP_EQ, std::make_shared<IpsaConstant>(1), x
        );
    } else {
        return IpsaExpression(
            *std::static_pointer_cast<IpsaExpression>(x).get()
        );
    }
}