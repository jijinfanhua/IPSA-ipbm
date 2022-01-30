#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <tuple>
#include <map>
#include <set>
#include <functional>
#include <iostream>
#include "p4_field.h"
#include "p4_action.h"
#include "p4_rename.h"

class P4Key {
public:
    std::string match_type;
    std::string name;
    std::vector<std::string> target;
    std::string mask;
};

class P4DefaultEntry {
public:
    int action_id;
    std::vector<std::string> action_data;
};

class P4MatchKey {
public:
    std::string match_type;
    std::string key;
    int prefix_length;
    std::string mask;
    friend std::ostream & operator<<(std::ostream & out, P4MatchKey const & mk);
};

std::ostream & operator<<(std::ostream & out, P4MatchKey const & mk) {
    out << mk.key;
    if (mk.match_type == "lpm") {
        out << " /// " << mk.prefix_length;
    }
    if (mk.match_type == "ternary") {
        out << " &&& " << mk.mask;
    }
    return out;
}

class P4Entry : public P4DefaultEntry {
public:
    std::vector<P4MatchKey> match_key;
};

class P4Table {
public:
    std::string name;
    std::vector<P4Key> key;
    int max_size;
    std::vector<int> action_ids;
    std::vector<std::string> actions;
    std::map<std::string, std::string> next_tables;
    P4DefaultEntry default_entry;
    std::vector<P4Entry> entries;
    bool is_empty() const { 
        return key.size() == 0 && actions.size() == 1;
    }
};

class P4Conditional {
public:
    std::string name;
    P4Parameter expression;
    std::string true_next;
    std::string false_next;
};

class P4Pipeline {
public:
    std::string name;
    std::string init_table;
    std::vector<P4Table> tables;
    std::vector<P4Conditional> conditionals;
};

static P4Rename<P4Conditional>* stage_rename = nullptr;
static P4Rename<P4Table>* table_rename = nullptr;

class P4Pipelines : public std::vector<P4Pipeline> {
public:
    static P4Actions& actions;
    std::string translate_stage_name(const std::string& name) const;
    std::string translate_table_name(const std::string& name) const;
    std::ostream & out_default_action(std::ostream & out, const P4Table& table, const P4DefaultEntry& default_entry) const;
    std::ostream & out_action(std::ostream & out, const P4Table& table, const P4Entry& entry) const;
    friend std::ostream & operator<<(std::ostream & out, P4Pipelines const & vp);
    void create_hit_miss_actions();
private:
    void initialize_stage_rename() const;
};

void P4Pipelines::create_hit_miss_actions() {
    int action_id = 0xffffff;
    for (auto& pipe : *this) {
        for (auto& t : pipe.tables) {
            for (auto [key, value] : t.next_tables) {
                if (key == "__HIT__" || key == "__MISS__") {
                    P4Action action;
                    action.id = action_id++;
                    action.name = t.name + key;
                    t.default_entry.action_id = action.id;
                    actions.push_back(action);
                }
            }
        }
    }
}

void P4Pipelines::initialize_stage_rename() const {
    stage_rename = new P4Rename<P4Conditional>();
    auto& onames = stage_rename->original_names;
    for (auto& pipe : *this) {
        stage_rename->add_names(pipe.conditionals, [](auto& c) { return c.name; });
        auto& init = pipe.init_table;
        if (std::find(std::begin(onames), std::end(onames), init) == std::end(onames)) {
            stage_rename->add_name(init, true);
        }
        for (auto& t : pipe.tables) { // direct tables without gateway
            for (auto [key, value] : t.next_tables) {
                if (std::find(std::begin(onames), std::end(onames), value) == std::end(onames)) {
                    stage_rename->add_name(value, true);
                }
            }
        }
    }
}

std::ostream & P4Pipelines::out_default_action(std::ostream & out, const P4Table& table, const P4DefaultEntry& default_entry) const {
    if (auto pos = std::find_if(std::begin(actions), std::end(actions), [&](auto& x) {
        return x.id == default_entry.action_id;
    }); pos != std::end(actions)) {
        out << actions.translate_name(pos->name, pos->id) /*<< "(";
        for (bool first = true; auto& d : default_entry.action_data) {
            if (!first) {
                out << ", ";
            } else {
                first = false;
            }
            out << d;
        }
        out << ")"
        NOTE: we do not support parameters in default actions or actions in const entries
        */
        ;
    }
    return out;
}

std::ostream & P4Pipelines::out_action(std::ostream & out, const P4Table& table, const P4Entry& entry) const {
    out << "(";
    for (bool first = true; auto& k : entry.match_key) {
        if (!first) {
            out << ", ";
        } else {
            first = false;
        }
        out << k;
    }
    return out_default_action(out << "): ", table, entry);
}

std::string P4Pipelines::translate_table_name(const std::string& name) const {
    if (table_rename == nullptr) {
        table_rename = new P4Rename<P4Table>();
        for (auto & pipe : *this) {
            table_rename->add_names(pipe.tables, [](auto& t) { return t.name; });
        }
    }
    return table_rename->get_name(name, 0);
}

std::string P4Pipelines::translate_stage_name(const std::string& name) const {
    if (stage_rename == nullptr) {
        initialize_stage_rename();
    }
    return stage_rename->get_name(name, 0);
}

std::ostream & operator<<(std::ostream & out, P4Pipelines const & vp) {
    vp.initialize_stage_rename();
    auto is_stage = [&](std::string name) {
        return std::find_if(std::begin(stage_rename->original_names), std::end(stage_rename->original_names), [&](auto& s) {
            return s == name;
        }) != std::end(stage_rename->original_names);
    };
    auto is_true_stage = [&](std::string name, P4Pipeline const & p) {
        return std::find_if(std::begin(p.conditionals), std::end(p.conditionals), [&](auto& c) {
            return c.name == name;
        }) != std::end(p.conditionals);
    };
    out << "tables {" << std::endl;
    for (auto & pipe : vp) {
        for (auto & table : pipe.tables) {
            out << "\ttable " << vp.translate_table_name(table.name) << " {" << std::endl;
            out << "\t\tkey = {" << std::endl;
            for (auto & key : table.key) {
                out << "\t\t\t" << P4Field(key.target) << ": " << key.match_type << ";" << std::endl;
            }
            out << "\t\t}" << std::endl;
            out << "\t\tsize = " << table.max_size << ";" << std::endl;
            int value_width = 16;
            int action_param_width = 0;
            for (auto & action_id : table.action_ids) {
                for (auto & action : vp.actions) {
                    if (action.id == action_id) {
                        int temp = 0;
                        for (auto & param : action.runtime_data) {
                            temp += param.bitwidth;
                        }
                        action_param_width = std::max(action_param_width, temp);
                        break;
                    }
                }
            }
            value_width += action_param_width;
            out << "\t\tvalue_width = " << value_width << ";" << std::endl;
            vp.out_default_action(out << "\t\tdefault_action = ", table, table.default_entry) << ";" << std::endl;
            if (table.entries.size() > 0) {
                out << "\t\tentries = {" << std::endl;
                for (auto& entry : table.entries) {
                    vp.out_action(out << "\t\t\t", table, entry) << ";" << std::endl;
                }
                out << "\t\t}" << std::endl;
            }
            out << "\t}" << std::endl;
        }
    }
    out << "}" << std::endl << std::endl;
    for (auto & p : vp) {
        auto out_next = [&](std::string text) {
            if (text.size() == 0) {
                out << "None";
            } else {
                if (/*std::find_if(std::begin(p.conditionals), std::end(p.conditionals), [&](auto& cond) {
                    return cond.name == text;
                }) != std::end(p.conditionals)*/
                    is_stage(text)) {
                    out << "stage(" << vp.translate_stage_name(text) << ")";
                } else {
                    out << "table(" << vp.translate_table_name(text) << ")";
                }
            }
        };
        out << "control ";
        if (p.name == "ingress") {
            out << "MyIngress";
        } else if (p.name == "egress") {
            out << "MyEgress";
        } else {
            out << p.name;
        }
        out << " {" << std::endl;
        // initial state
        std::vector<const P4Conditional*> conditionals;
        for (auto& c : p.conditionals) {
            conditionals.push_back(&c);
        }
        std::vector<const P4Table*> tables;
        for (auto& t : p.tables) {
            tables.push_back(&t);
        }
        if (is_true_stage(p.init_table, p)) {
            // swap to first
            int x = -1;
            for (int i = 0; i < p.conditionals.size(); i++) {
                if (p.conditionals[i].name == p.init_table) {
                    x = i; break;
                }
            }
            if (x >= 0) {
                std::swap(conditionals[0], conditionals[x]);
            }
        } else {
            int x = -1;
            for (int i = 0; i < p.tables.size(); i++) {
                if (p.tables[i].name == p.init_table) {
                    x = i; break;
                }
            }
            if (x >= 0) {
                std::swap(tables[0], tables[x]);
            }
        }
        auto conditionals_output = [&]() {
            for (auto cp : conditionals) {
                auto& c = *cp;
                out << "\tstage " << vp.translate_stage_name(c.name) << " {" << std::endl;
                std::set<std::string> hdr_names;
                for (auto table_name : {c.true_next, c.false_next}) {
                    if (auto t = std::find_if(std::begin(p.tables), std::end(p.tables), [&](auto& table) {
                        return table.name == table_name;
                    }); t != std::end(p.tables)) {
                        for (auto & key : t->key) {
                            P4Field temp(key.target);
                            if (temp.is_header()) {
                                hdr_names.insert(temp[0]);
                            }
                        }
                    }
                }
                auto [v, b] = c.expression.parse();
                for (auto pp : v) {
                    pp->add_to_hdr_names_set(hdr_names);
                }
                out << "\t\tparser {" << std::endl;
                for (auto& hn : hdr_names) {
                    out << "\t\t\t" << hn << ";" << std::endl;
                }
                out << "\t\t};" << std::endl;
                out << "\t\tmatcher {" << std::endl;
                out << "\t\t\tswitch (";
                for (bool first = true; auto pp : v) {
                    if (!first) {
                        out << ", ";
                    } else {
                        first = false;
                    }
                    pp->output(out);
                }
                out << ") {" << std::endl;
                for (auto& cond : b) {
                    out << "\t\t\t\t0b" << cond << ": ";
                    out_next(c.true_next);
                    out << ";" << std::endl;
                }
                out << "\t\t\t\t*: ";
                out_next(c.false_next);
                out << ";" << std::endl;
                out << "\t\t\t}" << std::endl;
                out << "\t\t};" << std::endl;
                out << "\t\texecutor {" << std::endl;
                for (auto t = std::begin(p.tables); t != std::end(p.tables); t++) {
                    if ((t->name == c.true_next || t->name == c.false_next) && !is_stage(t->name)) {
                        for (auto [key, value] : t->next_tables) {
                            int action_id = 0;
                            for (int i = 0; i < t->action_ids.size(); i++) {
                                if (t->actions[i] == key) {
                                    action_id = t->action_ids[i];
                                }
                            }
                            out << "\t\t\t";
                            if (key == "__HIT__" || key == "__MISS__") {
                                out << vp.actions.translate_name(t->name + key, 0);
                            } else {
                                out << vp.actions.translate_name(key, action_id);
                            }
                            out << ": ";
                            if (value.size() == 0) {
                                out << "None";
                            } else {
                                out << vp.translate_stage_name(value);
                            }
                            out << ";" << std::endl;
                        }
                    }
                }
                out << "\t\t};" << std::endl;
                out << "\t}" << std::endl;
            }
        };
        auto tables_output = [&]() {
            for (auto pt : tables) {
                auto& t = *pt;
                if (is_stage(t.name)) {
                    out << "\tstage " << vp.translate_stage_name(t.name) << " {" << std::endl;
                    std::set<std::string> hdr_names;
                    for (auto & key : t.key) {
                        P4Field temp(key.target);
                        if (temp.is_header()) {
                            hdr_names.insert(temp[0]);
                        }
                    }
                    out << "\t\tparser {" << std::endl;
                    for (auto& hn : hdr_names) {
                        out << "\t\t\t" << hn << ";" << std::endl;
                    }
                    out << "\t\t};" << std::endl;
                    out << "\t\tmatcher {" << std::endl;
                    out << "\t\t\tswitch (1) {" << std::endl;
                    out << "\t\t\t\t0b1: table(" << vp.translate_table_name(t.name) << ");" << std::endl;
                    out << "\t\t\t}" << std::endl;
                    out << "\t\t};" << std::endl;
                    out << "\t\texecutor {" << std::endl;
                    for (auto [key, value] : t.next_tables) {
                        int action_id = 0;
                        for (int i = 0; i < t.action_ids.size(); i++) {
                            if (t.actions[i] == key) {
                                action_id = t.action_ids[i];
                            }
                        }
                        out << "\t\t\t";
                        if (key == "__HIT__" || key == "__MISS__") {
                            out << vp.actions.translate_name(t.name + key, 0);
                        } else {
                            out << vp.actions.translate_name(key, action_id);
                        }
                        out << ": ";
                        if (value.size() == 0) {
                            out << "None";
                        } else {
                            out << vp.translate_stage_name(value);
                        }
                        out << ";" << std::endl;
                    }
                    out << "\t\t};" << std::endl;
                    out << "\t}" << std::endl;
                }
            }
        };
        if (is_true_stage(p.init_table, p)) {
            conditionals_output();
            tables_output();
        } else {
            tables_output();
            conditionals_output();
        }
        out << "}" << std::endl;
    }
    return out;
}