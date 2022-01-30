#pragma once

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <iostream>
#include "p4_rename.h"
#include "p4_field.h"
#include "p4_parameter.h"

class P4Primitive {
public:
    std::string op;
    std::vector<P4Parameter> parameters;
};

class P4Action {
public:
    std::string name;
    int id;
    std::vector<P4RuntimeData> runtime_data;
    std::vector<P4Primitive> primitives;
};

static P4Rename<P4Action>* action_rename = nullptr;

class P4Actions : public std::vector<P4Action> {
public:
    std::string translate_name(const std::string& name, int index) const;
    friend std::ostream & operator<<(std::ostream & out, P4Actions const & actions);
};

std::string P4Actions::translate_name(const std::string& name, int index) const {
    if (action_rename == nullptr) {
        action_rename = new P4Rename<P4Action>(*this, [](auto & act) {
            return act.name;
        });
    }
    return action_rename->get_name(name, index);
}

std::ostream & operator<<(std::ostream & out, P4Actions const & actions) {
    out << "actions {" << std::endl;
    for (auto & action : actions) {
        out << "\taction " << actions.translate_name(action.name, action.id);
        out << "(";
        for (bool first = true; auto & rd : action.runtime_data) {
            if (!first) {
                out << ", ";
            } else {
                first = false;
            }
            out << "bit<" << rd.bitwidth << "> " << rd.name;
        }
        out << ") {" << std::endl;
        for (auto & p : action.primitives) {
            out << "\t\t";
            if (p.op == "assign") {
                p.parameters[0].output(out, &(action.runtime_data)) << " = ";
                p.parameters[1].output(out, &(action.runtime_data)) << ";" << std::endl;
            } else {
                if (p.op == "mark_to_drop") {
                    out << "standard_metadata.drop = 1;" << std::endl;
                } else {
                    out << p.op << "(";
                    for (bool first = true; auto & pp : p.parameters) {
                        if (!first) {
                            out << ", ";
                        } else {
                            first = false;
                        }
                        pp.output(out, &(action.runtime_data));
                    }
                    out << ");" << std::endl;
                }
            }
        }
        out << "\t}" << std::endl;
    }
    return out << "}" << std::endl;
}