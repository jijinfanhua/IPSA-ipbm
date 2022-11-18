#pragma once

#include <vector>
#include "rp4_field.h"
#include "rp4_treenode.h"
#include "rp4_state.h"

class Rp4ParserDef : public Rp4TreeNode {
public:
    std::string name;
    std::vector<Rp4StateDef> states;

    Rp4ParserDef() = default;
    Rp4ParserDef(std::string _name, std::vector<Rp4StateDef> _states) : 
        name(std::move(_name)), states(std::move(_states)) {}

    virtual std::string toString() const {
        return "parser-def(" + name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(states);
    }
};