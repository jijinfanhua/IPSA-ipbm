#pragma once

#include "rp4_treenode.h"
#include "rp4_key.h"
#include "rp4_lvalue.h"
#include <map>

enum Rp4MatchType {
    MT_EXACT, MT_TERNARY, MT_LPM
};

static inline std::string to_string(Rp4MatchType mt) {
    static const std::map<Rp4MatchType, std::string> m = {
            {MT_EXACT, "EXACT"},
            {MT_TERNARY, "TERNARY"},
            {MT_LPM, "LPM"}
    };
    return m.at(mt);
}

class Rp4TableOptionalStmt : public Rp4TreeNode {
public:
    virtual bool isWidth() const { return false; }
    virtual bool isSize() const { return false; }
    virtual bool isDefault() const { return false; }
};

class Rp4TableSizeStmt : public Rp4TableOptionalStmt {
public:
    int size;
    Rp4TableSizeStmt() {}
    Rp4TableSizeStmt(int _size) : size(_size) {}
    virtual bool isSize() const { return true; }
    virtual std::string toString() const { 
        return "table-size-stmt(" + std::to_string(size) + ")";
    }
};

class Rp4TableValueWidthStmt : public Rp4TableOptionalStmt {
public:
    int width;
    Rp4TableValueWidthStmt() {}
    Rp4TableValueWidthStmt(int _width) : width(_width) {}
    virtual bool isWidth() const { return true; }
    virtual std::string toString() const { 
        return "table-width-stmt(" + std::to_string(width) + ")";
    }
};

class Rp4MatchKey : public Rp4TreeNode {
public:
    std::vector<std::shared_ptr<Rp4Key>> keys;
    Rp4MatchKey() {}
    Rp4MatchKey(std::vector<std::shared_ptr<Rp4Key>> _keys) : keys(std::move(_keys)) {}
    virtual std::string toString() const {
        return "match-key";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(keys);
    }
};

class Rp4MatchEntry : public Rp4TreeNode {
public:
    Rp4MatchKey keys;
    std::string action_name;
    Rp4MatchEntry() {}
    Rp4MatchEntry(Rp4MatchKey _keys, std::string _action_name) : 
        keys(std::move(_keys)), action_name(std::move(_action_name)) {}
    virtual std::string toString() const {
        return "match-entry(" + action_name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return { dynamic_cast<const Rp4TreeNode*>(&keys) };
    }
};

class Rp4TableEntriesStmt : public Rp4TableOptionalStmt {
public:
    std::vector<Rp4MatchEntry> entries;
    Rp4TableEntriesStmt() {}
    Rp4TableEntriesStmt(std::vector<Rp4MatchEntry> _entries) : entries(std::move(_entries)) {}
    virtual std::string toString() const {
        return "table-entries-stmt";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(entries);
    }
};

class Rp4DefaultActionStmt : public Rp4TableOptionalStmt {
public:
    std::string action_name;
    Rp4DefaultActionStmt() {}
    Rp4DefaultActionStmt(std::string _action_name) : action_name(std::move(_action_name)) {}
    virtual bool isDefault() const { return true; }
    virtual std::string toString() const {
        return "default_action_stmt(" + action_name + ")";
    }
};

class Rp4KeyEntry : public Rp4TreeNode {
public:
    std::shared_ptr<Rp4LValue> lvalue;
    Rp4MatchType match_type;
    Rp4KeyEntry() {}
    Rp4KeyEntry(std::shared_ptr<Rp4LValue> _lvalue, Rp4MatchType _match_type):
        lvalue(std::move(_lvalue)), match_type(_match_type) {}
    virtual std::string toString() const {
        return "key-entry[" + to_string(match_type) + "]";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return { dynamic_cast<const Rp4TreeNode*>(lvalue.get()) };
    }
};

class Rp4TableKeyDef : public Rp4TreeNode {
public:
    std::vector<Rp4KeyEntry> entries;
    Rp4TableKeyDef() {}
    Rp4TableKeyDef(std::vector<Rp4KeyEntry> _entries) : entries(std::move(_entries)) {}
    virtual std::string toString() const { 
        return "table-key";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(entries);
    }
};

class Rp4TableDef : public Rp4TreeNode {
public:
    std::string name;
    Rp4TableKeyDef key_def;
    std::vector<std::shared_ptr<Rp4TableOptionalStmt>> optional_stmts;
    Rp4TableDef() {}
    Rp4TableDef(std::string _name, Rp4TableKeyDef _key_def, std::vector<std::shared_ptr<Rp4TableOptionalStmt>> _optional_stmts):
        name(std::move(_name)), key_def(std::move(_key_def)), optional_stmts(std::move(_optional_stmts)) {}
    virtual std::string toString() const { 
        return "table(" + name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        std::vector<const Rp4TreeNode*> dst = {
            dynamic_cast<const Rp4TreeNode*>(&key_def)
        };
        add(dst, optional_stmts);
        return dst;
    }
};

class Rp4TablesDef : public Rp4TreeNode {
public:
    std::vector<Rp4TableDef> tables;
    Rp4TablesDef() {}
    Rp4TablesDef(std::vector<Rp4TableDef> _tables): tables(std::move(_tables)) {}
    virtual std::string toString() const { 
        return "tables";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(tables);
    }
};