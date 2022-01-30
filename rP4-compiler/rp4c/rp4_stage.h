#pragma once

#include "rp4_treenode.h"
#include "rp4_stage_parser.h"
#include "rp4_stage_matcher.h"
#include "rp4_stage_executor.h"

class Rp4StageDef : public Rp4TreeNode {
public:
    std::string name;
    Rp4StageParser parser;
    Rp4StageMatcher matcher;
    Rp4StageExecutor executor;
    Rp4StageDef() {}
    Rp4StageDef(std::string _name, Rp4StageParser _parser, Rp4StageMatcher _matcher, Rp4StageExecutor _executor) : 
        name(std::move(_name)), parser(std::move(_parser)), matcher(std::move(_matcher)), executor(std::move(_executor)) {}
    virtual std::string toString() const {
        return "stage(" + name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return {
            dynamic_cast<const Rp4TreeNode*>(&parser),
            dynamic_cast<const Rp4TreeNode*>(&matcher),
            dynamic_cast<const Rp4TreeNode*>(&executor)
        };
    }
    const Rp4StageExecutorEntry* get_virtual_action() const {
        // empty stage = 
        // 1. all trans entries are table entries
        // 2. has only 1 action
        // this stage can be removed
        // and add the action to the prior one
        //     as long as the lvalue does not conflict
        for (auto& entry : matcher.switch_entries) {
            if (!entry.value->isTableStmt()) {
                return nullptr;
            }
        }
        if (executor.entries.size() == 1) {
            return &(executor.entries[0]);
        }
        return nullptr;
    }
};