#pragma once

#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <iostream>
#include <set>
#include "p4_field.h"

class P4RuntimeData {
public:
    std::string name;
    int bitwidth;
};

class P4Parameter;
class P4Expression {
public:
    std::string op;
    P4Parameter* left;
    P4Parameter* right;
};

class P4Parameter {
public:
    std::string type;
    bool bool_value;
    int runtime_data_value;
    std::string header_value;
    std::string hexstr_value;
    P4Field field_value;
    P4Expression* expression_value;

    std::ostream& output(std::ostream& out, const std::vector<P4RuntimeData>* runtime_data) const;
    void add_to_hdr_names_set(std::set<std::string>& dst) const;
    std::tuple<std::vector<const P4Parameter*>, std::vector<std::string>> parse() const;
};

std::ostream & P4Parameter::output(std::ostream & out, const std::vector<P4RuntimeData>* runtime_data = nullptr) const {
    if (type == "field") {
        out << field_value;
    } else if (type == "runtime_data") {
        if (runtime_data != nullptr) {
            out << (*runtime_data)[runtime_data_value].name;
        }
    } else if (type == "hexstr") {
        out << hexstr_value;
    } else if (type == "bool") {
        out << (bool_value ? "true" : "false");
    } else if (type == "header") {
        out << header_value;
    } else if (type == "expression") {
        if (auto & exp = expression_value; exp != nullptr) {
            if (exp->op != "b2d") {
                out << "(";
                if (exp->left != nullptr) {
                    exp->left->output(out) << " ";
                }
                out << exp->op;
                if (exp->right != nullptr) {
                    exp->right->output(out << " ");
                }
                out << ")";
            } else {
                exp->right->output(out);
            }
        }
    }
    return out;
}

void P4Parameter::add_to_hdr_names_set(std::set<std::string>& dst) const {
    if (type == "field") {
        if (field_value.is_header()) {
            dst.insert(field_value[0]);
        }
    } else if (type == "expression" && expression_value != nullptr) {
        if (expression_value->left != nullptr) {
            expression_value->left->add_to_hdr_names_set(dst);
        }
        if (expression_value->right != nullptr) {
            expression_value->right->add_to_hdr_names_set(dst);
        }
    }
}

std::tuple<std::vector<const P4Parameter*>, std::vector<std::string>> P4Parameter::parse() const {
    if (type == "expression" && expression_value != nullptr) {
        if (expression_value->op == "d2b") {
            return expression_value->right->parse();
        }
        auto not_apply = [](const std::vector<std::string> & v) {
            auto vs = v[0].size();
            std::vector<std::string> ans;
            std::string temp(vs, '1');
            std::string zero(vs, '0');
            auto inc_temp = [&]() {
                for (int k = vs - 1; k >= 0; --k) {
                    if (temp[k] == '1') {
                        temp[k] = '0';
                    } else {
                        temp[k] = '1';
                        break;
                    }
                }
            };
            for (auto & s : v) {
                for (inc_temp(); s != temp; inc_temp()) {
                    ans.push_back(temp);
                }
            }
            for (inc_temp(); zero != temp; inc_temp()) {
                ans.push_back(temp);
            }
            return ans;
        };
        auto and_apply = [](const std::vector<std::string> & v1, const std::vector<std::string> & v2) {
            std::vector<std::string> ans;
            for (auto i1 = std::begin(v1); i1 != std::end(v1); i1++) {
                for (auto i2 = std::begin(v2); i2 != std::end(v2); i2++) {
                    ans.push_back(*i1 + *i2);
                }
            }
            return ans;
        };
        auto or_apply = [&](const std::vector<std::string> & v1, const std::vector<std::string> & v2) {
            std::vector<std::string> ans;
            auto a1 = and_apply(v1, not_apply(v2));
            auto a2 = and_apply(not_apply(v1), v2);
            auto a3 = and_apply(v1, v2);
            for (auto & a : {a1, a2, a3}) {
                ans.insert(std::end(ans), std::begin(a), std::end(a));
            }
            std::sort(std::begin(ans), std::end(ans));
            return ans;
        };
        auto concat_apply = [](const std::vector<const P4Parameter*> & v1, const std::vector<const P4Parameter*> & v2) {
            std::vector<const P4Parameter*> ans;
            for (auto & v : {v1, v2}) {
                ans.insert(std::end(ans), std::begin(v), std::end(v));
            }
            return ans;
        };
        if (expression_value->op == "not") {
            auto [v, b] = expression_value->right->parse();
            return std::tuple(v, not_apply(b));
        }
        if (expression_value->op == "and") {
            auto [vl, bl] = expression_value->left->parse();
            auto [vr, br] = expression_value->right->parse();
            return std::tuple(concat_apply(vl, vr), and_apply(bl, br));
        }
        if (expression_value->op == "or") {
            auto [vl, bl] = expression_value->left->parse();
            auto [vr, br] = expression_value->right->parse();
            return std::tuple(concat_apply(vl, vr), or_apply(bl, br));
        }
    }
    /* else if (type == "field") or other expressions */
    return std::tuple(std::vector<const P4Parameter*> { this }, std::vector<std::string> { "1" });        
}