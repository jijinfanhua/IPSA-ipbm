#pragma once

#include "ipsa_module.h"
#include "ipsa_header_manager.h"

class IpsaParserTcamEntry : public IpsaModule {
public:
    int current_state;
    int entry;
    int mask;
    IpsaParserTcamEntry(int _current_state, int _entry, int _mask):
        current_state(_current_state), entry(_entry), mask(_mask) {}
    IpsaParserTcamEntry(int _current_state, const Rp4Key* key) :
        current_state(_current_state), entry(key->get_key()), mask(key->get_mask()) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"current_state", makeValue(current_state)},
            {"entry", makeValue(entry)},
            {"mask", makeValue(mask)}
        };
        return makeValue(dst);
    }
};

class IpsaParserSramEntry : public IpsaModule {
public:
    int header_id;
    int header_length;
    int next_state;
    std::vector<IpsaHeaderField> transition_fields;
    int accept_drop;
    IpsaParserSramEntry(int _header_id, int _header_length, int _next_state, std::vector<IpsaHeaderField> _transition_fields, int _accept_drop):
        header_id(_header_id), header_length(_header_length), next_state(_next_state), transition_fields(std::move(_transition_fields)), accept_drop(_accept_drop) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const { 
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"header_id", makeValue(header_id)},
            {"header_length", makeValue(header_length)},
            {"next_state", makeValue(next_state)},
            {"transition_field_num", makeValue(transition_fields.size())},
            {"transition_fields", makeValue(transition_fields)},
            {"accept_drop", makeValue(accept_drop)}
        };
        return makeValue(dst);
    }
};

class IpsaLevelEntry : public IpsaModule {
public:
    IpsaParserTcamEntry parser_tcam_entry;
    IpsaParserSramEntry parser_sram_entry;
    IpsaLevelEntry(IpsaParserTcamEntry _parser_tcam_entry, IpsaParserSramEntry _parser_sram_entry):
        parser_tcam_entry(std::move(_parser_tcam_entry)), parser_sram_entry(std::move(_parser_sram_entry)) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"parser_tcam_entry", parser_tcam_entry.toIpsaValue()},
            {"parser_sram_entry", parser_sram_entry.toIpsaValue()}
        };
        return makeValue(dst);
    }
};

class IpsaLevel : public IpsaModule {
public:
    std::vector<IpsaLevelEntry> entries;
    IpsaLevel() {}
    void add(IpsaLevelEntry entry) { entries.push_back(std::move(entry)); }
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        return makeValue(entries);
    }
};

class IpsaLevelManager {
public:
    IpsaHeaderManager* header_manager = nullptr;
    int global_level_id = 0;
    std::vector<IpsaLevel> levels;
    std::map<std::string, int> states;
    IpsaLevelManager(IpsaHeaderManager* _header_manager): 
        header_manager(_header_manager) {}
    void load(const Rp4Ast* ast);
};

void IpsaLevelManager::load(const Rp4Ast* ast) {
    auto& state_defs = ast->parser_def.states;
    global_level_id = 0;
    auto get_state = [&](std::string state_name) -> const Rp4StateDef* {
        if (auto state_def = std::find_if(std::begin(state_defs), std::end(state_defs), [&](auto& state) {
            return state.name == state_name;
        }); state_def != std::end(state_defs)) {
            return state_def.base();
        } else {
            return nullptr;
        }
    };
    // auto get_miss_act = [&](const Rp4StateDef* state_def) {
    //     if (state_def != nullptr) {
    //         auto trans = state_def->transition;
    //         Rp4TransitionEntry* default_entry = nullptr;
    //         if (trans->isDirect()) {
    //             default_entry = &(std::static_pointer_cast<Rp4DirectTransition>(trans)->entry);
    //         } else if (trans->isSelect()) {
    //             auto& entries = std::static_pointer_cast<Rp4SelectTransition>(trans)->entries;
    //             if (auto de = std::find_if(std::begin(entries), std::end(entries), [](auto& entry) {
    //                 return entry.key->isDefault();
    //             }); de != std::end(entries)) {
    //                 if (de->is_accept) {
    //                     return 0;
    //                 }
    //             }
    //         }
    //     }
    //     return 1;
    // };
    auto get_select = [&](const Rp4StateDef* state_def) -> std::vector<IpsaHeaderField> {
        if (state_def == nullptr) { return {}; }
        auto trans = state_def->transition;
        if (trans->isDirect()) { return {}; }
        auto& targets = std::static_pointer_cast<Rp4SelectTransition>(trans)->targets;
        std::vector<IpsaHeaderField> fields;
        for (auto& target : targets) {
            fields.push_back(IpsaHeaderField(*(header_manager->lookup(
                target.member_name, target.field_name
            ))));
        }
        return std::move(fields);
    };
    auto build_entries = [&](IpsaLevel& level, int state, const std::vector<Rp4TransitionEntry>& entries) {
        for (auto& entry : entries) {
            if (entry.accept_drop == 2) {
                auto next_state = get_state(entry.next);
                auto m = header_manager->get_header(&(next_state->extract.target));
                level.add(IpsaLevelEntry(
                    IpsaParserTcamEntry(state, entry.key.get()),
                    IpsaParserSramEntry(
                        m->header_id, m->offset, 
                        states[next_state->name],
                        get_select(next_state), 2
                    )
                ));
            } else {
                level.add(IpsaLevelEntry(
                    IpsaParserTcamEntry(state, entry.key.get()),
                    IpsaParserSramEntry(0, 0, 0, {}, entry.accept_drop)
                ));
            }
        }
    };
    if (state_defs.size() > 0) {
        auto first_state = &(state_defs[0]);
        auto& first_header = state_defs[0].extract.target;
        auto m = header_manager->get_header(&first_header);
        states.insert({{"", global_level_id++}});
        levels.push_back(IpsaLevel());
        for (auto& state_def : state_defs) {
            states.insert({{state_def.name, global_level_id++}});
            levels.push_back(IpsaLevel());
        }
        levels[0].add(IpsaLevelEntry(
            IpsaParserTcamEntry(0, 0, 0),
            IpsaParserSramEntry(
                m->header_id, m->offset, states[first_state->name],
                get_select(first_state), 2
            )
        ));
        for (auto& state_def : state_defs) {
            if (auto x = states.find(state_def.name); x != std::end(states)) {
                auto& level = levels[x->second];
                auto trans = state_def.transition;
                if (trans->isDirect()) {
                    build_entries(level, x->second, { std::static_pointer_cast<Rp4DirectTransition>(trans)->entry });
                } else if (trans->isSelect()) {
                    build_entries(level, x->second, std::static_pointer_cast<Rp4SelectTransition>(trans)->entries );
                }
            }
        }
    }
}