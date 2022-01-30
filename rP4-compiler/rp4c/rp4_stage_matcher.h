#pragma once

#include "rp4_treenode.h"
#include "rp4_expression.h"
#include "rp4_key.h"

class Rp4SwitchValue : public Rp4TreeNode {
public:
    virtual bool isTableStmt() const { return false; }
    virtual bool isStageStmt() const { return false; }
};

class Rp4SwitchTableStmt : public Rp4SwitchValue {
public:
    std::string name;
    Rp4SwitchTableStmt() {}
    Rp4SwitchTableStmt(std::string _name) : name(std::move(_name)) {}
    virtual std::string toString() const { 
        return "switch-value:table(" + name + ")";
    }
    virtual bool isTableStmt() const { return true; }
};

class Rp4SwitchStageStmt : public Rp4SwitchValue {
public:
    std::string name;
    bool to_none;
    Rp4SwitchStageStmt() {}
    Rp4SwitchStageStmt(std::string _name, bool _to_none = false) : name(std::move(_name)), to_none(_to_none) {}
    virtual std::string toString() const { 
        return "switch-value:stage(" + name + ")";
    }
    virtual bool isStageStmt() const { return true; }
};

// this grammar is deprecated
class Rp4SwitchActionStmt : public Rp4SwitchValue {
public:
    std::string name;
    Rp4SwitchActionStmt() {}
    Rp4SwitchActionStmt(std::string _name) : name(std::move(_name)) {}
    virtual std::string toString() const {
        return "switch-value:action(" + name + ")";
    }
};

class Rp4SwitchEntry : public Rp4TreeNode {
public:
    std::shared_ptr<Rp4Key> key;
    std::shared_ptr<Rp4SwitchValue> value;
    Rp4SwitchEntry() {}
    Rp4SwitchEntry(std::shared_ptr<Rp4Key> _key, std::shared_ptr<Rp4SwitchValue> _value):
        key(std::move(_key)), value(std::move(_value)) {}
    virtual std::string toString() const {
        return "switch-entry";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return {
            dynamic_cast<const Rp4TreeNode*>(key.get()),
            dynamic_cast<const Rp4TreeNode*>(value.get())
        };
    }
};

class Rp4StageMatcher : public Rp4TreeNode {
public:
    std::vector<std::shared_ptr<Rp4Operation>> ops;
    std::vector<Rp4SwitchEntry> switch_entries;
    Rp4StageMatcher() {}
    Rp4StageMatcher(std::vector<std::shared_ptr<Rp4Operation>> _ops, std::vector<Rp4SwitchEntry> _switch_entries):
        ops(std::move(_ops)), switch_entries(std::move(_switch_entries)) {}
    virtual std::string toString() const {
        return "matcher-in-stage";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        auto dst = mapped(ops);
        add(dst, switch_entries);
        return dst;
    }
};