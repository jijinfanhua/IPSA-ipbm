#pragma once

#include "rp4_treenode.h"

class Rp4StageParserHeader : public Rp4TreeNode {
public:
    std::string name;
    Rp4StageParserHeader() {}
    Rp4StageParserHeader(std::string _name): name(std::move(_name)) {}
    virtual std::string toString() const {
        return "header-in-stage(" + name + ")";
    }
};

class Rp4StageParser : public Rp4TreeNode {
public:
    std::vector<Rp4StageParserHeader> headers;
    Rp4StageParser() {}
    Rp4StageParser(std::vector<Rp4StageParserHeader> _headers): headers(std::move(_headers)) {}
    virtual std::string toString() const {
        return "parser-in-stage";
    }
    virtual std::vector<const Rp4TreeNode*> children() const {
        return mapped(headers);
    }
};