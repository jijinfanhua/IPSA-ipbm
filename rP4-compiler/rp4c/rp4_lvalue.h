#pragma once

#include "rp4_treenode.h"
#include "rp4_field.h"

class Rp4Operation : public Rp4TreeNode {
public:
    virtual bool isLValue() const { return false; }
    virtual bool isLiteral() const { return false; }
    virtual bool isBinary() const { return false; }
    virtual bool isParameter() const { return false; }
};

class Rp4Operand : public Rp4Operation {
public:
    
};

class Rp4LValue : public Rp4Operand {
public:
    virtual bool isLValue() const { return true; }
    virtual bool isMeta() const { return false; }
    virtual bool isHeader() const { return false; }
};

class Rp4MetaLValue : public Rp4LValue {
public:
    Rp4Member field;
    Rp4MetaLValue() {}
    Rp4MetaLValue(Rp4Member _field) : field(std::move(_field)) {}
    virtual std::string toString() const {
        return "meta-lvalue";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return { dynamic_cast<const Rp4TreeNode*>(&field) };
    }
    virtual bool isMeta() const { return true; }
};

class Rp4HeaderLValue : public Rp4LValue {
public:
    Rp4Field field;
    Rp4HeaderLValue() {}
    Rp4HeaderLValue(Rp4Field _field) : field(std::move(_field)) {}
    virtual std::string toString() const {
        return "hdr-lvalue";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return { dynamic_cast<const Rp4TreeNode*>(&field) };
    }
    virtual bool isHeader() const { return true; }
};