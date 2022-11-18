#pragma once

#include "rp4_treenode.h"
#include "rp4_field.h"
#include "rp4_expression.h"

class Rp4ActionDef : public Rp4TreeNode {
public:
    std::string name;
    std::vector<Rp4FieldDef> parameters;
    std::vector<Rp4Expression> expressions;
    bool is_noaction;

    Rp4ActionDef() {}
    Rp4ActionDef(std::string _name, std::vector<Rp4FieldDef> _parameters, std::vector<Rp4Expression> _expressions, bool _is_noaction = false):
        name(std::move(_name)), parameters(std::move(_parameters)), expressions(std::move(_expressions)), is_noaction(_is_noaction) {}
    virtual std::string toString() const { 
        return "action(" + name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        auto dst = mapped(parameters);
        add(dst, expressions);
        return std::move(dst);
    }
};

class Rp4ActionsDef : public Rp4TreeNode {
public:
    std::vector<Rp4ActionDef> actions;
    Rp4ActionsDef() {}
    Rp4ActionsDef(std::vector<Rp4ActionDef> _actions): actions(std::move(_actions)) {}
    virtual std::string toString() const { 
        return "actions";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(actions);
    }
};