#pragma once

#include "rp4_type.h"
#include "rp4_treenode.h"

class Rp4FieldDef : public Rp4TreeNode {
public:
    std::shared_ptr<Rp4Type> type = nullptr;
    std::string name;
    Rp4FieldDef() {}
    Rp4FieldDef(std::shared_ptr<Rp4Type> _type, std::string _name):
        type(std::move(_type)), name(std::move(_name)) {}
    virtual std::string toString() const {
        return "field-def(" + name + ")";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return { type.get() };
    }
};

class Rp4Member : public Rp4TreeNode {
public:
    std::string instance_name;
    std::string member_name;
    Rp4Member() {}
    Rp4Member(std::string _instance_name, std::string _member_name):
        instance_name(std::move(_instance_name)), member_name(std::move(_member_name)) {}
    virtual std::string toString() const {
        return "member(" + instance_name + "." + member_name + ")";
    }
};

class Rp4Field : public Rp4TreeNode {
public:
    std::string instance_name;
    std::string member_name;
    std::string field_name;
    Rp4Field() {}
    Rp4Field(std::string _instance_name, std::string _member_name, std::string _field_name):
        instance_name(std::move(_instance_name)), member_name(std::move(_member_name)), field_name(std::move(_field_name)) {}
    virtual std::string toString() const {
        return "field(" + instance_name + "." + member_name + "." + field_name + ")";
    }
};