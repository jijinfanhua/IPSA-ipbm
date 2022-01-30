#pragma once

#include <vector>
#include "rp4_field.h"
#include "rp4_key.h"
#include "rp4_treenode.h"
#include <algorithm>

class Rp4Extract : public Rp4TreeNode {
public:
    Rp4Member target;
    Rp4Extract() {}
    Rp4Extract(Rp4Member _target) : target(std::move(_target)) {}
    virtual std::string toString() const {
        return "extract";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return { dynamic_cast<const Rp4TreeNode*>(&target) };
    }
};

class Rp4TransitionEntry : public Rp4TreeNode {
public:
    std::shared_ptr<Rp4Key> key = nullptr;
    std::string next;
    int accept_drop = 2;
    Rp4TransitionEntry() {}
    Rp4TransitionEntry(std::shared_ptr<Rp4Key> _key, std::string _next, int _accept_drop = 2):
        key(std::move(_key)), next(std::move(_next)), accept_drop(_accept_drop) {}
    virtual std::vector<const Rp4TreeNode*> children() const {
        return { key.get() };
    }
    virtual std::string toString() const {
        return "transition-entry( " + next + ")";
    }
};

class Rp4Transition : public Rp4TreeNode {
public:
    virtual bool isSelect() const { return false; }
    virtual bool isDirect() const { return false; }
};

class Rp4SelectTransition : public Rp4Transition {
public:
    std::vector<Rp4Field> targets;
    std::vector<Rp4TransitionEntry> entries;
    Rp4SelectTransition() {}
    Rp4SelectTransition(std::vector<Rp4Field> _targets, std::vector<Rp4TransitionEntry> _entries):
        targets(std::move(_targets)), entries(std::move(_entries)) {}
    virtual bool isSelect() const { return true; }
    virtual std::string toString() const {
        return "transition-select";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        auto dst = mapped(targets);
        add(dst, entries);
        return std::move(dst);
    }
};

class Rp4DirectTransition : public Rp4Transition {
public:
    Rp4TransitionEntry entry;
    Rp4DirectTransition() {}
    Rp4DirectTransition(Rp4TransitionEntry _entry): entry(std::move(_entry)) {}
    virtual bool isDirect() const { return true; }
    virtual std::string toString() const {
        return "transition-direct";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return { dynamic_cast<const Rp4TreeNode*>(&entry) };
    }
};

class Rp4StateDef : public Rp4TreeNode {
public:
    std::string name;
    Rp4Extract extract;
    std::shared_ptr<Rp4Transition> transition;
    Rp4StateDef() {}
    Rp4StateDef(std::string _name, Rp4Extract _extract, std::shared_ptr<Rp4Transition> _transition):
        name(std::move(_name)), extract(std::move(_extract)), transition(std::move(_transition)) {}
    virtual std::string toString() const {
        return "state-def(" + name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return {
            dynamic_cast<const Rp4TreeNode*>(&extract), 
            dynamic_cast<const Rp4TreeNode*>(transition.get())
        };
    }
    // should be deprecated
    int get_accept_drop() const {
        auto is_accept = [](const Rp4TransitionEntry& entry) {
            return entry.accept_drop == 0;
        };
        auto is_drop = [](const Rp4TransitionEntry& entry) {
            return entry.accept_drop == 1;
        };
        if (transition->isSelect()) {
            auto& entries = std::static_pointer_cast<Rp4SelectTransition>(transition)->entries;
            if (std::all_of(std::begin(entries), std::end(entries), is_accept)) {
                return 0;
            } else if (std::all_of(std::begin(entries), std::end(entries), is_drop)) {
                return 1;
            }
        } else if (transition->isDirect()) {
            auto& entry = std::static_pointer_cast<Rp4DirectTransition>(transition)->entry;
            if (is_accept(entry)) {
                return 0;
            } else {
                return 1;
            }
        }
        return 2;
    }
};
