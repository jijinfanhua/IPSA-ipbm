#pragma once

#include "action.h"
#include "field.h"
#include "bytearray.h"
#include "bitmap.h"
#include "defs.h"
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>
#include <memory>

Buffer alloc_logical_sram(int phy_id, int num_bytes);

Buffer alloc_logical_tcam(int phy_id, int num_bytes);

struct LogicTable {
    virtual int key_len() const = 0;

    virtual int val_len() const = 0;

    int entry_len() const { return key_len() + val_len(); }

    virtual Buffer get_entry(int pos) const = 0;

    virtual int end_pos() const = 0;

    virtual void insert(Buffer key, Buffer val) = 0;

    virtual void erase(Buffer key) = 0;

    virtual int match_exact(Buffer key) const = 0;

    // ternary matching
    virtual int match_ternary(Buffer key, Buffer mask) const = 0;

    // longest prefix matching
    virtual int match_lpm(Buffer key) const = 0;
};

// Logical SRAM table, each with different entry length & num
struct LogicSram : public LogicTable {
    Buffer _bitmap;     // bitmap start
    Buffer _data;       // entry start
    int _key_len;       // key length
    int _val_len;       // value length
    int _cap;           // sram capacity

    LogicSram(int phy_id, int key_len, int val_len, int num_entry) {
        _key_len = key_len;
        _val_len = val_len;
        _cap = num_entry;
        int bm_len = _cap / 8;
        int total_len = bm_len + _cap * entry_len();
        _bitmap = alloc_logical_sram(phy_id, total_len);
        Bitmap::init(_bitmap, bm_len);
        _data = _bitmap + bm_len;
    }

    int key_len() const override { return _key_len; }

    int val_len() const override { return _val_len; }

    Buffer get_entry(int pos) const override {
        return _data + pos * entry_len();
    }

    int end_pos() const override { return _cap; }

    int get_bucket_id(InputBuffer key, int key_len) const {
        size_t p = 1e9 + 7;
        size_t hash = 3;
        for (int i = 0; i < key_len; i++) {
            hash = hash * p + key[i];
        }
        int bkt = (int) (hash % _cap);   // hash
        return bkt;
    }

    void insert(Buffer key, Buffer val) override {
        int bkt = get_bucket_id(key, _key_len);
        if (Bitmap::test(_bitmap, bkt)) {
            throw std::runtime_error("Entry already exists");
        }
        Bitmap::set(_bitmap, bkt);
        Buffer entry = get_entry(bkt);
        memcpy(entry, key, _key_len);
        memcpy(entry + _key_len, val, _val_len);
    }

    void erase(Buffer key) override {
        int bkt = get_bucket_id(key, _key_len);
        if (!Bitmap::test(_bitmap, bkt)) {
            throw std::out_of_range("Entry not found");
        }
        Bitmap::reset(_bitmap, bkt);
    }

    int match_exact(Buffer key) const override {
        int bkt = get_bucket_id(key, _key_len);
        if (!Bitmap::test(_bitmap, bkt)) {
            return _cap;
        }
        Buffer entry = get_entry(bkt);
        if (memcmp(entry, key, _key_len) != 0) {
            throw std::runtime_error("Unsolved hash table conflict");
        }
        return bkt;
    }

    int match_ternary(Buffer key, Buffer mask) const override {
        throw std::runtime_error("Not implemented");
    }

    int match_lpm(Buffer key) const override {
        throw std::runtime_error("Not implemented");
    }
};

// Logical TCAM table
struct LogicTcam : public LogicTable {
    int _key_len;   // fixed key length
    int _val_len;   // fixed value length
    int _cap;       // capacity
    int _size;      // current number of entries
    Buffer _buf;

    LogicTcam(int phy_id, int key_len, int val_len, int cap) {
        _key_len = key_len;
        _val_len = val_len;
        _cap = cap;
        _size = 0;
        _buf = alloc_logical_tcam(phy_id, entry_len() * cap);
    }

    int key_len() const override { return _key_len; }

    int val_len() const override { return _val_len; }

    Buffer get_entry(int pos) const override {
        if (pos >= _cap) { throw std::out_of_range("TCAM entry out of range"); }
        return _buf + pos * entry_len();
    }

    int end_pos() const override { return _size; }

    void insert(Buffer key, Buffer val) override {
        if (_size >= _cap) {
            throw std::overflow_error("TCAM overflow!");
        }
        Buffer entry = get_entry(_size);
        memcpy(entry, key, _key_len);
        memcpy(entry + _key_len, val, _val_len);
        _size++;
    }

    void erase(Buffer key) override {
        int pos = match_exact(key);
        Buffer entry = get_entry(pos);
        memmove(entry, entry + entry_len(), entry_len() * (_size - pos - 1));
    }

    // exact matching
    int match_exact(Buffer key) const override {
        int i;
        for (i = 0; i < _size; i++) {
            Buffer entry = get_entry(i);
            if (memcmp(entry, key, _key_len) == 0) {
                break;
            }
        }
        return i;
    }

    static void get_masked_key(InputBuffer key, InputBuffer mask, int len, Buffer out_key) {
        for (int i = 0; i < len; i++) {
            out_key[i] = key[i] & mask[i];
        }
    }

    // ternary matching
    int match_ternary(Buffer key, Buffer mask) const override {
        auto mask_key = new uint8_t[_key_len];
        auto mask_cand = new uint8_t[_key_len];
        get_masked_key(key, mask, _key_len, mask_key);
        int i;
        for (i = 0; i < _size; i++) {
            Buffer entry = get_entry(i);
            get_masked_key(entry, mask, _key_len, mask_cand);
            if (memcmp(mask_key, mask_cand, _key_len) == 0) {
                break;
            }
        }
        delete[] mask_cand;
        delete[] mask_key;
        return i;
    }

    int match_lpm(Buffer key) const override {
        assert(_key_len == 4);   // TODO: only support 4-bytes LPM
        int pos = _size;
        for (uint32_t mask = UINT32_MAX; mask; mask <<= 1) {
            pos = match_ternary(key, (Buffer) &mask);
            if (pos != _size) {
                break;
            }
        }
        return pos;
    }
};

enum MemType {
    MEM_SRAM, MEM_TCAM
};

enum MatchType {
    MATCH_EXACT, MATCH_TERNARY, MATCH_LPM,
};

class FlowTable {
public:
    uint8_t tag;
    int key_len;
    int val_len;
    std::shared_ptr<LogicTable> logic_table;
    MemType mem_type;
    MatchType match_type;
    std::vector<FieldSpec> key_fields;
    bool is_counter;

    FlowTable() = default;

    FlowTable(uint8_t tag, int valLen, std::shared_ptr<LogicTable> logicTable, MemType memType,
              MatchType matchType, std::vector<FieldSpec> keyFields, bool isCounter)
            : tag(tag), key_len(0), val_len(valLen), logic_table(std::move(logicTable)), mem_type(memType),
              match_type(matchType), key_fields(std::move(keyFields)), is_counter(isCounter) {
        for (auto &key_field: key_fields) {
            key_len += key_field.len;
        }
    }

    Buffer get_val(int pos) const {
        return logic_table->get_entry(pos) + logic_table->key_len();
    }

    Buffer get_entry(int pos) const { return logic_table->get_entry(pos); }

    void insert(Buffer key, Buffer val) {
        // Entry should be like
        // | tag | key (padding) | value (padding) |
        auto tag_key = get_tag_key(key);
        auto pad_val = get_pad_val(val);
        logic_table->insert(tag_key.data(), pad_val.data());
    }

    void erase(Buffer key) {
        auto tag_key = get_tag_key(key);
        logic_table->erase(tag_key.data());
    }

    int match_exact(Buffer key) const {
        auto tag_key = get_tag_key(key);
        return logic_table->match_exact(tag_key.data());
    }

    int match_lpm(Buffer key) const {
        throw std::runtime_error("Not implemented");
    }

    int match_ternary(Buffer key, Buffer mask) const {
        throw std::runtime_error("Not implemented");
    }

    int end_pos() const { return logic_table->end_pos(); }

private:
    ByteArray get_tag_key(Buffer key) const {
        if (key_len + 1 > logic_table->key_len()) {
            throw std::overflow_error("Key too long");
        }
        ByteArray tag_key(logic_table->key_len());
        auto key_buf = tag_key.data();
        memset(key_buf, 0, logic_table->key_len());
        key_buf[0] = tag;
        memcpy(key_buf + 1, key, key_len);
        return tag_key;
    }

    ByteArray get_pad_val(Buffer val) {
        if (val_len > logic_table->val_len()) {
            throw std::overflow_error("Value too long!");
        }
        ByteArray pad_val(logic_table->val_len());
        auto val_buf = pad_val.data();
        memset(val_buf, 0, logic_table->val_len());
        memcpy(val_buf, val, val_len);
        return pad_val;
    }
};
