#pragma once

#include "ipsa_module.h"
#include "rp4_ast.h"
#include <algorithm>

enum IpsaFieldType {
    FT_FIELD,
    FT_VALID,
    FT_HIT,
    FT_MISS
};

std::string to_string(IpsaFieldType op);
// {
//     static const std::map<IpsaFieldType, std::string> m = {
//         {FT_FIELD, "FIELD"},
//         {FT_VALID, "VALID"},
//         {FT_HIT, "HIT"},
//         {FT_MISS, "MISS"}
//     };
//     return m.at(op);
// }

class IpsaHeaderField : IpsaModule {
private:
    IpsaFieldType type;
    int header_id;
    int field_internal_offset;
    int field_length;
    IpsaHeaderField(const IpsaHeaderField &ipsaHeaderField) = delete;
    IpsaHeaderField(IpsaFieldType _type, int _header_id = 0, int _field_internal_offset = 0, int _field_length = 0) : type(_type), header_id(_header_id), field_internal_offset(_field_internal_offset), field_length(_field_length) {}
    static std::vector<IpsaHeaderField *> ipsaHeaderFields;

public:
    static IpsaHeaderField *CreateHeaderField(IpsaFieldType _type, int _header_id = 0, int _field_internal_offset = 0, int _field_length = 0) {
        ipsaHeaderFields.push_back(
                new IpsaHeaderField(_type, _header_id, _field_internal_offset, _field_length));
        return ipsaHeaderFields.back();
    }

    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"type", makeValue(to_string(type))},
                {"header_id", makeValue(header_id)},
                {"field_internal_offset", makeValue(field_internal_offset)},
                {"field_length", makeValue(field_length)}};
        return makeValue(dst);
    }
    int FieldLength() const { return field_length; }
};

// std::vector<IpsaHeaderField*> IpsaHeaderField::ipsaHeaderFields;
// includes metadata
class IpsaHeader {
public:
    bool is_header;
    bool is_standard_metadata = false;
    int header_id;
    int offset;
    std::map<std::string, const IpsaHeaderField *const> fields;
    int getLength() const;
    const IpsaHeaderField *lookup(std::string field_name) const;
    IpsaHeader(const Rp4HeaderDef *header_def, int id);
    IpsaHeader(const Rp4StructDef *struct_def, int id);
};

class IpsaMetadataEntry : public IpsaModule {
public:
    int id;
    int offset;
    int length;
    IpsaMetadataEntry(int _id, int _offset, int _length) : id(_id), offset(_offset), length(_length) {}
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
                {"id", makeValue(id)},
                {"offset", makeValue(offset)},
                {"length", makeValue(length)}};
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
    void load(const std::vector<Rp4StructDef> &struct_defs, const std::vector<Rp4HeaderDef> &header_defs);
    const IpsaHeader *get_header(int header_id) const;
    const IpsaHeader *get_header(const std::string &name) const;
    const IpsaHeader *get_header(const Rp4Member *member) const;
    const IpsaHeaderField *lookup(std::shared_ptr<Rp4LValue> lvalue) const;
    const IpsaHeaderField *lookup(std::string header_name, std::string field_name) const;
    void addHeader(const Rp4HeaderDef *header_def, std::string name);
    void addMetadata(const Rp4StructDef *struct_def);

    const IpsaMetadata &Metadata() { return metadata; }
};