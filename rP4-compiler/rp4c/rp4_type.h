#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#include "rp4_treenode.h"

class Rp4Type : public Rp4TreeNode {
public:
    virtual bool isBitType() const {
        return false;
    }
    virtual bool isNamedType() const {
        return false;
    }
};

class Rp4BitType : public Rp4Type {
public:
    int length;
    virtual bool isBitType() const {
        return true;
    }
    virtual std::string toString() const {
        return "bit<" + std::to_string(length) + ">";
    }
    Rp4BitType(int _length) : length(_length) {}
};

class Rp4NamedType : public Rp4Type {
public:
    std::string name;
    virtual bool isNamedType() const {
        return true;
    }
    virtual std::string toString() const {
        return "class<" + name + ">";
    }
    Rp4NamedType(std::string _name) : name(std::move(_name)) {}
};