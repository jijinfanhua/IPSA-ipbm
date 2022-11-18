#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

class P4HeaderField {
public:
    std::string name;
    int width;
    std::string proper_name() const;
};

std::string P4HeaderField::proper_name() const {
    std::string temp = name;
    for (unsigned int i = 0; i < temp.size(); i++) {
        if (temp[i] == '.') {
            temp[i] = '_';
        }
    }
    return temp;
}

class P4HeaderType {
public:
    std::string name;
    std::vector<P4HeaderField> fields;
};

class P4Header {
public:
    std::string name;
    int id;
    std::string header_type_name;
    bool metadata;
    std::string translated_prefix() const;
    std::string translated_typename() const;
};

std::string P4Header::translated_typename() const {
    if (header_type_name == "scalars_0") {
        return "metadata";
    } else {
        return header_type_name;
    }
}

std::string P4Header::translated_prefix() const {
    if (!metadata) {
        return std::string("hdr.") + name;
    } else if (header_type_name == "scalars_0") {
        return "meta";
    } else {
        return name;
    }
}

class P4Headers {
public:
    std::vector<P4HeaderType> header_types;
    std::vector<P4Header> headers;
    bool is_header(std::string name) const;
    friend std::ostream & operator<<(std::ostream & out, P4Headers const & v);
};

bool P4Headers::is_header(std::string name) const {
    if (auto i = std::find_if(std::begin(headers), std::end(headers), [&](auto& h) { 
        return h.header_type_name == name; 
    }); i != std::end(headers) && !i->metadata) {
        return true;
    }
    return false;
}

std::ostream & operator<<(std::ostream & out, P4Headers const & v) {
    for (auto & h : v.header_types) {
        if (v.is_header(h.name)) {
            out << "header " << h.name << " {" << std::endl;
            for (auto & f : h.fields) {
                out << "\tbit<" << f.width << ">\t" << f.proper_name() << ";" << std::endl;
            }
            out << "}" << std::endl;
        }
    }
    for (auto & h : v.headers) {
        if (h.metadata && h.header_type_name != "standard_metadata") {
            out << "struct " << h.translated_typename() << " {" << std::endl;
            if (auto hi = std::find_if(std::begin(v.header_types), std::end(v.header_types), [&](auto & ht) {
                return ht.name == h.header_type_name;
            }); hi != std::end(v.header_types)) {
                for (auto & f : hi->fields) {
                    if (f.name.size() > 0 && f.name[0] != '_') {
                        out << "\tbit<" << f.width << ">\t" << f.proper_name() << ";" << std::endl;
                    }
                }
            }
            out << "} " << h.translated_prefix() << ";" << std::endl;
        }
    }
    out << "struct headers {" << std::endl;
    for (auto & h : v.headers) {
        if (!h.metadata) {
            out << "\t" << h.header_type_name << "\t" << h.name << ";" << std::endl;
        }
    }
    return out << "} hdr;" << std::endl;
}