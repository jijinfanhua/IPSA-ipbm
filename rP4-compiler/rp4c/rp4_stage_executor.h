#pragma once

#include "rp4_treenode.h"

class Rp4StageExecutorEntry : public Rp4TreeNode {
public:
    std::string action_name;
    std::string stage_name;
    Rp4StageExecutorEntry() {}
    Rp4StageExecutorEntry(std::string _action_name, std::string _stage_name):
        action_name(std::move(_action_name)), stage_name(std::move(_stage_name)) {}
    virtual std::string toString() const {
        return "executor-entry(" + action_name + ": " + stage_name + ")";
    }
};

class Rp4StageExecutor : public Rp4TreeNode {
public:
    std::vector<Rp4StageExecutorEntry> entries;
    Rp4StageExecutor() {}
    Rp4StageExecutor(std::vector<Rp4StageExecutorEntry> _entries): entries(std::move(_entries)) {}
    virtual std::string toString() const {
        return "executor-in-stage";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(entries);
    }
};