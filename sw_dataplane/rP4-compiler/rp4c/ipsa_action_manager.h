#pragma once

#include "ipsa_module.h"
#include "ipsa_expression.h"
#include "ipsa_header_manager.h"
#include "rp4_ast.h"

class IpsaAssign : public IpsaModule {
public:
    const IpsaHeaderField* lvalue;
    std::shared_ptr<IpsaOperand> rvalue;
    IpsaAssign() {}
    IpsaAssign(const IpsaHeaderField* _lvalue, std::shared_ptr<IpsaOperand> _rvalue):
        lvalue(_lvalue), rvalue(std::move(_rvalue)) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"lvalue", lvalue->toIpsaValue()},
            {"rvalue", rvalue->toIpsaValue()}
        };
        return makeValue(dst);
    }
};

// keyword noaction is deprecated
class IpsaAction : public IpsaModule {
public:
    int id;
    int parameter_num;
    const Rp4ActionDef* action_def;
    std::vector<int> action_parameters_lengths;
    std::vector<IpsaAssign> primitives;
    IpsaAction(int _id, const Rp4ActionDef* _action_def): id(_id), action_def(_action_def) { }
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"id", makeValue(id)},
            {"parameter_num", makeValue(parameter_num)},
            {"action_parameters_lengths", makeValue(action_parameters_lengths)},
            {"primitives", makeValue(primitives)}
        };
        return makeValue(dst);
    }
};

class IpsaActionManager {
public:
    IpsaHeaderManager* header_manager;
    int global_action_id;
    std::map<std::string, IpsaAction> actions;
    bool concatAction(int id, const IpsaAction* next_action);
    const IpsaAction* lookup(int action_id) const;
    const IpsaAction* lookup(std::string name) const;
    void addAction(const Rp4ActionDef* action_def);
    IpsaActionManager(IpsaHeaderManager* _header_manager): header_manager(_header_manager) {}
    void load(const Rp4Ast* ast);
};

// concat action[id] with next_action, return if successful
bool IpsaActionManager::concatAction(int id, const IpsaAction* next_action) {
    auto can_concat = [](const std::vector<IpsaAssign>& assign1, const std::vector<IpsaAssign>& assign2) {
        for (auto& a1 : assign1) {
            for (auto& a2 : assign2) {
                if (a1.lvalue == a2.lvalue) {
                    return false; // lvalue conflict
                }
            }
        }
        return true;
    };
    for (auto& [_, x] : actions) {
        if (x.id == id) {
            if (can_concat(x.primitives, next_action->primitives)) {
                std::copy(std::begin(next_action->primitives), std::end(next_action->primitives),
                    std::back_inserter(x.primitives));
                return true;
            } else {
                break;
            }
        }
    }
    return false;
}

const IpsaAction* IpsaActionManager::lookup(std::string name) const {
    if (auto x = actions.find(name); x != std::end(actions)) {
        return &(x->second);
    } else {
        return nullptr;
    }
}

const IpsaAction* IpsaActionManager::lookup(int action_id) const {
    for (auto& [name, action] : actions) {
        if (action.id == action_id) {
            return &action;
        }
    }
    return nullptr;
}

void IpsaActionManager::addAction(const Rp4ActionDef* action_def) {
    if (actions.find(action_def->name) == std::end(actions)) {
        IpsaAction action(global_action_id++, action_def);
        action.parameter_num = action_def->parameters.size();
        for (auto& param : action_def->parameters) {
            auto type = param.type;
            int length = 0;
            if (type->isBitType()) {
                length = std::static_pointer_cast<Rp4BitType>(type)->length;
            }
            action.action_parameters_lengths.push_back(length);
        }
        for (auto& expression : action_def->expressions) {
            action.primitives.push_back(IpsaAssign(
                header_manager->lookup(expression.lvalue),
                parse_expr(expression.rvalue, header_manager, action_def)
            ));
        }
        actions.insert({{
            action_def->name, std::move(action)
        }});
    }
}

void IpsaActionManager::load(const Rp4Ast* ast) {
    global_action_id = 0;
    for (auto& action_def : ast->actions_def.actions) {
        addAction(&action_def);
    }
}