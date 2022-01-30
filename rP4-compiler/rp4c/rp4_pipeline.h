#pragma once

#include "rp4_stage.h"

class Rp4Pipeline : public Rp4TreeNode {
public:
    std::string name;
    std::vector<Rp4StageDef> stage_defs;
    Rp4Pipeline() {}
    Rp4Pipeline(std::string _name, std::vector<Rp4StageDef> _stage_defs) : 
        name(std::move(_name)), stage_defs(std::move(_stage_defs)) {}
    virtual std::string toString() const {
        return "pipeline(" + name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(stage_defs);
    }
};