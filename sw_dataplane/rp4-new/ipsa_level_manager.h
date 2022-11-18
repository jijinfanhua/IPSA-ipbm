#pragma once

#include "ipsa_header_manager.h"
#include "ipsa_module.h"

class IpsaParserTcamEntry : public IpsaModule {
public:
    int current_state;
    int entry;
    int mask;
    IpsaParserTcamEntry(int _current_state, int _entry, int _mask) : current_state(_current_state), entry(_entry), mask(_mask) {}
    IpsaParserTcamEntry(int _current_state, const Rp4Key *key) : current_state(_current_state), entry(key->get_key()), mask(key->get_mask()) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"current_state", makeValue(current_state)},
                {"entry", makeValue(entry)},
                {"mask", makeValue(mask)}};
        return makeValue(dst);
    }
    void toString() const {
        std::cout << "TcamEntry" << std::endl;
        std::cout << "current_state " << current_state << " "
                  << "entry " << entry << " "
                  << "mask " << mask << std::endl;
    }
};

class IpsaParserSramEntry : public IpsaModule {
public:
    int header_id;
    int header_length;
    int next_state;
    std::vector<const IpsaHeaderField *> transition_fields;
    int accept_drop;
    IpsaParserSramEntry(int _header_id, int _header_length, int _next_state, std::vector<const IpsaHeaderField *> _transition_fields, int _accept_drop) : header_id(_header_id), header_length(_header_length), next_state(_next_state), transition_fields(std::move(_transition_fields)), accept_drop(_accept_drop) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"header_id", makeValue(header_id)},
                {"header_length", makeValue(header_length)},
                {"next_state", makeValue(next_state)},
                {"transition_field_num", makeValue(transition_fields.size())},
                {"transition_fields", makeValue(transition_fields)},
                {"accept_drop", makeValue(accept_drop)}};
        return makeValue(dst);
    }
    void toString() const {
        std::cout << "SramEntry " << std::endl;
        std::cout << "header_id " << header_id << " "
                  << "header_length " << header_id << " "
                  << "next_state " << next_state << " "
                  << "accept_drop " << accept_drop << std::endl;
    }
};

class IpsaLevelEntry : public IpsaModule {
public:
    IpsaParserTcamEntry parser_tcam_entry;
    IpsaParserSramEntry parser_sram_entry;
    IpsaLevelEntry(IpsaParserTcamEntry _parser_tcam_entry, IpsaParserSramEntry _parser_sram_entry) : parser_tcam_entry(std::move(_parser_tcam_entry)), parser_sram_entry(std::move(_parser_sram_entry)) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"parser_tcam_entry", parser_tcam_entry.toIpsaValue()},
                {"parser_sram_entry", parser_sram_entry.toIpsaValue()}};
        return makeValue(dst);
    }
    void toString() const {
        parser_tcam_entry.toString();
        parser_sram_entry.toString();
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
    int global_level_id = 0;
    std::vector<IpsaLevel> levels;
    std::map<std::string, int> states;

    void load(const Rp4ParserDef &, const IpsaHeaderManager *header_manager);
};