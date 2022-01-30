#pragma once

#include <vector>
#include "rp4_treenode.h"

class Rp4Key : public Rp4TreeNode {
public:
    virtual int get_key() const = 0;
    virtual int get_mask() const = 0;
    virtual bool isDefault() const { return false; }
};

class Rp4ExactKey : public Rp4Key {
public:
    int key;
    Rp4ExactKey(int _key) : key(_key) { }
    virtual int get_key() const { return key; }
    virtual int get_mask() const { return -1; }
    virtual std::string toString() const {
        return "exact-key(" + std::to_string(key) + ")";
    }
};

class Rp4LpmKey : public Rp4Key {
public:
    int key;
    int prelen;
    Rp4LpmKey(int _key, int _prelen) : key(_key), prelen(_prelen) {}
    virtual int get_key() const { return key; }
    virtual int get_mask() const { return -1 << 32-prelen; }
    virtual std::string toString() const {
        return "lpm-key(" + std::to_string(key) + " /// " + std::to_string(prelen) + ")";
    }
};

class Rp4TernaryKey : public Rp4Key {
public:
    int key;
    int mask;
    Rp4TernaryKey(int _key, int _mask) : key(_key), mask(_mask) { }
    virtual int get_key() const { return key; }
    virtual int get_mask() const { return mask; }
    virtual std::string toString() const {
        return "ternary-key(" + std::to_string(key) + " &&& " + std::to_string(mask) + ")";
    }
};

class Rp4DefaultKey : public Rp4Key {
public:
    virtual bool isDefault() const { return true; }
    virtual int get_key() const { return 0; }
    virtual int get_mask() const { return 0; }
    virtual std::string toString() const {
        return "default-key";
    }
};