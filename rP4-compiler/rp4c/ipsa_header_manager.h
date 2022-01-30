#pragma once

#include "ipsa_module.h"
#include "rp4_ast.h"
#include <algorithm>

enum IpsaFieldType {
    FT_FIELD, FT_VALID, FT_HIT, FT_MISS
};

static inline std::string to_string(IpsaFieldType op) {
    static const std::map<IpsaFieldType, std::string> m = {
        {FT_FIELD, "FIELD"},
        {FT_VALID, "VALID"},
        {FT_HIT, "HIT"},
        {FT_MISS, "MISS"}
    };
    return m.at(op);
}

class IpsaHeaderField : IpsaModule {
public:
    IpsaFieldType type;
    int header_id;
    int field_internal_offset;
    int field_length;
    IpsaHeaderField(IpsaFieldType _type, int _header_id = 0, int _field_internal_offset = 0, int _field_length = 0):
        type(_type), header_id(_header_id), field_internal_offset(_field_internal_offset), field_length(_field_length) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"type", makeValue(to_string(type))},
            {"header_id", makeValue(header_id)},
            {"field_internal_offset", makeValue(field_internal_offset)},
            {"field_length", makeValue(field_length)}
        };
        return makeValue(dst);
    }
};

// includes metadata
class IpsaHeader {
public:
    bool is_header;
    bool is_standard_metadata = false;
    int header_id;
    int offset;
    std::map<std::string, IpsaHeaderField> fields;
    int getLength() const;
    const IpsaHeaderField* lookup(std::string field_name) const;
    IpsaHeader(const Rp4HeaderDef* header_def, int id);
    IpsaHeader(const Rp4StructDef* struct_def, int id);
};

int IpsaHeader::getLength() const {
    int width = 0;
    for (auto& [name, field] : fields) {
        width += field.field_length;
    }
    return (width + 7) / 8;
}

const IpsaHeaderField* IpsaHeader::lookup(std::string field_name) const {
    if (auto x = fields.find(field_name); x != std::end(fields)) {
        return &(x->second);
    } else {
        std::cout << "error invalid field: " << field_name << std::endl;
        return nullptr;
    }
}

IpsaHeader::IpsaHeader(const Rp4HeaderDef* header_def, int id):
    header_id(id), is_header(true), is_standard_metadata(false) {
    fields.insert({{
        "isValid", IpsaHeaderField(FT_VALID, id, 0, 0)
    }});
    for (offset = 0; auto& field : header_def->fields) {
        if (auto x = field.type; x->isBitType()) {
            auto y = dynamic_cast<const Rp4BitType*>(x.get());
            fields.insert({{field.name, IpsaHeaderField(
                FT_FIELD, id, offset, y->length
            )}});
            offset += y->length;
        } else {
            std::cout << "error header: " << header_def->name << std::endl;
        }
    }
}

IpsaHeader::IpsaHeader(const Rp4StructDef* struct_def, int id):
    header_id(id), is_header(false) {
    if (struct_def->name == "standard_metadata") {
        is_standard_metadata = true;
    }
    fields.insert({{
        "isValid", IpsaHeaderField(FT_VALID, id, 0, 0)
    }});
    for (offset = 0; auto& field : struct_def->fields) {
        if (auto x = field.type; x->isBitType()) {
            auto y = dynamic_cast<const Rp4BitType*>(x.get());
            fields.insert({{field.name, IpsaHeaderField(
                FT_FIELD, id, offset, y->length
            )}});
            offset += y->length;
        } else {
            std::cout << "error metadata: " << struct_def->struct_name << std::endl;
        }
    }
}

class IpsaMetadataEntry : public IpsaModule {
public:
    int id;
    int offset;
    int length;
    IpsaMetadataEntry(int _id, int _offset, int _length):
        id(_id), offset(_offset), length(_length) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"id", makeValue(id)},
            {"offset", makeValue(offset)},
            {"length", makeValue(length)}
        };
        return makeValue(dst);
    }
};

class IpsaMetadata : public IpsaModule {
public:
    std::vector<IpsaMetadataEntry> entries;
    IpsaMetadata() {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        return makeValue(entries);
    }
};

// includes metadata
class IpsaHeaderManager {
public:
    int global_header_id;
    std::string header_name;
    std::map<std::string, IpsaHeader> headers;
    IpsaMetadata metadata;
    IpsaHeaderManager() {}
    void load(const Rp4Ast* ast);
    const IpsaHeader* get_header(int header_id) const;
    const IpsaHeader* get_header(std::string name) const;
    const IpsaHeader* get_header(const Rp4Member* member) const;
    const IpsaHeaderField* lookup(std::shared_ptr<Rp4LValue> lvalue) const;
    const IpsaHeaderField* lookup(std::string header_name, std::string field_name) const;
    void addHeader(const Rp4HeaderDef* header_def, std::string name);
    void addMetadata(const Rp4StructDef* struct_def);
};

const IpsaHeader* IpsaHeaderManager::get_header(int header_id) const {
    for (auto& [name, header] : headers) {
        if (header.header_id == header_id) {
            return &header;
        }
    }
    return nullptr;
}

const IpsaHeader* IpsaHeaderManager::get_header(std::string name) const {
    if (auto x = headers.find(name); x != std::end(headers)) {
        return &(x->second);
    } else {
        std::cout << "error wrong header name: " << name << std::endl;
        return nullptr;
    }
}

const IpsaHeader* IpsaHeaderManager::get_header(const Rp4Member* member) const {
    if (member->instance_name == this->header_name) {
        return get_header(member->member_name);
    } else {
        std::cout << "error wrong headers name: " << member->instance_name << std::endl;
        return nullptr;
    }
}

const IpsaHeaderField* IpsaHeaderManager::lookup(std::string header_name, std::string field_name) const {
    if (auto x = headers.find(header_name); x != std::end(headers)) {
        return x->second.lookup(field_name);
    } else {
        std::cout << "error header name: " << header_name << std::endl;
        return nullptr;
    }
}

const IpsaHeaderField* IpsaHeaderManager::lookup(std::shared_ptr<Rp4LValue> lvalue) const {
    std::string header_name, field_name;
    if (lvalue->isMeta()) {
        auto member = dynamic_cast<const Rp4MetaLValue*>(lvalue.get())->field;
        header_name = member.instance_name;
        field_name = member.member_name;        
    } else if (lvalue->isHeader()) {
        auto header = dynamic_cast<const Rp4HeaderLValue*>(lvalue.get())->field;
        if (header.instance_name == this->header_name) {
            header_name = header.member_name;
            field_name = header.field_name;
        } else {
            std::cout << "error wrong headers name: " << header.instance_name << std::endl;
            return nullptr;
        }
    } else {
        return nullptr;
    }
    return lookup(std::move(header_name), std::move(field_name));
}

void IpsaHeaderManager::addHeader(const Rp4HeaderDef* header_def, std::string name) {
    if (headers.find(name) == headers.end()) {
        headers.insert({
            {name, IpsaHeader(header_def, global_header_id++)}
        });
    }
}

void IpsaHeaderManager::addMetadata(const Rp4StructDef* struct_def) {
    if (headers.find(struct_def->name) == headers.end()) {
        int header_id = global_header_id;
        if (struct_def->name == "standard_metadata") {
            header_id = 31; // fixed
        } else {
            global_header_id++;
        }
        headers.insert({
            {struct_def->name, IpsaHeader(struct_def, header_id)},
        });
    }
}

void IpsaHeaderManager::load(const Rp4Ast* ast) {
    global_header_id = 0;
    for (auto& struct_def : ast->struct_defs) {
        if (struct_def.is_header) {
            header_name = struct_def.name;
            for (auto& field_def : struct_def.fields) {
                if (auto x = field_def.type; x->isNamedType()) {
                    auto y = dynamic_cast<const Rp4NamedType*>(x.get());
                    if (auto header_def = std::find_if(std::begin(ast->header_defs), std::end(ast->header_defs), [&](auto& h) {
                        return h.name == y->name;
                    }); header_def != std::end(ast->header_defs)) {
                        addHeader(header_def.base(), field_def.name);
                    } else {
                        std::cout << "error no header type: " << y->name << std::endl;
                    }
                } else {
                    std::cout << "error bit type in headers: " << field_def.name << std::endl;
                }
            }
        } else {
            addMetadata(&struct_def);
        }
    }
    // generate metadata
    metadata.entries.push_back(IpsaMetadataEntry(31, 0, 20));
    int offset = 20;
    for (auto& [name, header] : headers) {
        if (!header.is_header && !header.is_standard_metadata) {
            int length = std::max(8, header.getLength() * 2);
            metadata.entries.push_back(IpsaMetadataEntry(
                header.header_id, offset, length
            ));
            offset += length;
        }
    }
}