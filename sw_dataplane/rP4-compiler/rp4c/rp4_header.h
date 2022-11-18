#pragma once

#include <vector>
#include "rp4_field.h"
#include "rp4_treenode.h"

class Rp4HeaderDef : public Rp4TreeNode {
public:
    std::string name;
    std::vector<Rp4FieldDef> fields;
    Rp4HeaderDef() {}
    Rp4HeaderDef(std::string _name, std::vector<Rp4FieldDef> _fields) :
            name(std::move(_name)), fields(std::move(_fields)) {}
    virtual std::string toString() const {
        return "header-def(" + name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(fields);
    }
};