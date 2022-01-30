#pragma once

#include "ipsa_value.h"
#include <iostream>

class IpsaOutput {
public:
    int tab;
    std::ostream& out;
    IpsaOutput(std::ostream& _out): tab(0), out(_out) {}
    std::ostream& emit(std::shared_ptr<IpsaValue> value);
};

std::ostream& IpsaOutput::emit(std::shared_ptr<IpsaValue> value) {
    if (value.get() == nullptr) {
        out << "null";
    } else if (value->isInteger()) {
        out << std::static_pointer_cast<IpsaInteger>(value)->getValue();
    } else if (value->isString()) {
        out << "\"" << std::static_pointer_cast<IpsaString>(value)->getValue() << "\"";
    } else if (value->isList()) {
        auto& l = std::static_pointer_cast<IpsaList>(value)->getValue();
        if (l.size() == 0) {
            out << "[]";
        } else {
            out << "[" << std::endl;
            tab += 2;
            std::string offset(tab, ' ');
            for (auto c = std::begin(l); c != std::end(l); ) {
                out << offset;
                emit(*c);
                if (++c != std::end(l)) {
                    out << ",";
                }
                out << std::endl;
            }
            tab -= 2;
            out << std::string(tab, ' ') << "]";
        }
    } else if (value->isDict()) {
        auto& d = std::static_pointer_cast<IpsaDict>(value)->getValue();
        if (d.size() == 0) {
            out << "{}";
        } else {
            out << "{" << std::endl;
            tab += 2;
            std::string offset(tab, ' ');
            for (auto c = std::begin(d); c != std::end(d); ) {
                out << offset;
                out << "\"" << c->first << "\" : ";
                emit(c->second);
                if (++c != std::end(d)) {
                    out << ",";
                }
                out << std::endl;
            }
            tab -= 2;
            out << std::string(tab, ' ') << "}";
        }
    }
    return out;
}