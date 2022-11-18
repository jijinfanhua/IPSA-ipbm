#pragma once

#include "ipsa_expression.h"
#include "ipsa_header_manager.h"
#include "ipsa_module.h"
#include "rp4_ast.h"

class IpsaAssign : public IpsaModule {
public:
    const IpsaHeaderField *lvalue;
    std::shared_ptr<IpsaOperand> rvalue;
    //    IpsaAssign() {}
    IpsaAssign(const IpsaHeaderField *_lvalue, std::shared_ptr<IpsaOperand> _rvalue) : lvalue(_lvalue), rvalue(std::move(_rvalue)) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"lvalue", lvalue->toIpsaValue()},
                {"rvalue", rvalue->toIpsaValue()}};
        return makeValue(dst);
    }
    const IpsaHeaderField *LValue() const {
        return lvalue;
    }
    const IpsaOperand *RValue() const {
        return rvalue.get();
    }

    bool isConflict(const IpsaAssign &assign) const {
        return this->lvalue == assign.lvalue;
    }
};

// keyword noaction is deprecated
class IpsaAction : public IpsaModule {


public:
    std::vector<IpsaAssign> primitives;
    int id;
    int parameter_num;
    Rp4ActionDef action_def;
    std::vector<int> action_parameters_lengths;

    IpsaAction(int _id, const Rp4ActionDef &_action_def) : id(_id), action_def(_action_def) {}
    void rebuildExpressions(const std::vector<Rp4Expression> &_expressions) {
        action_def.expressions = _expressions;
    }

    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"id", makeValue(id)},
                {"parameter_num", makeValue(parameter_num)},
                {"action_parameters_lengths", makeValue(action_parameters_lengths)},
                {"primitives", makeValue(primitives)}};
        return makeValue(dst);
    }
};

class IpsaActionManager {
public:
    int global_action_id;
    std::map<std::string, IpsaAction> actions;
    IpsaAction *lookup(int action_id);
    IpsaAction *lookup(std::string name);
    const IpsaAction *lookupConst(std::string name) const;
    void addAction(const Rp4ActionDef &action_def, const IpsaHeaderManager *header_manager);
    void reloadPrimitives(const IpsaHeaderManager *header_manager);
    void load(const std::unique_ptr<Rp4Ast>& ast, const IpsaHeaderManager *header_manager) {
        global_action_id = 0;
        for (auto &action_def: ast->actions_def.actions) {
            addAction(action_def, header_manager);
        }
    }
};