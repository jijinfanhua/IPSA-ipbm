#pragma once
#ifdef __linux__
#include <arpa/inet.h>
#endif

#include "ipsa_module.h"

static inline uint8_t convert(char c) {
    static const std::map<char, uint8_t> m = {
        {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6' , 6}, {'7', 7}, {'8', 8}, {'9', 9}, 
        {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15},
        {'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}
    };
    return m.at(c);
}

enum IpsaTableUpdateType {
    TBL_UPD_ADD, TBL_UPD_MOD, TBL_UPD_DEL
};

static inline std::string to_string(IpsaTableUpdateType c) {
    static const std::map<IpsaTableUpdateType, std::string> m = {
        {TBL_UPD_ADD, "ADD"},
        {TBL_UPD_MOD, "MOD"},
        {TBL_UPD_DEL, "DEL"}
    };
    return m.at(c);
}

class IpsaTableUpdateActionParameter : public IpsaModule {
public:
    int width;
    std::vector<uint8_t> entry;
    IpsaTableUpdateActionParameter() {}
protected:
    std::map<std::string, std::shared_ptr<IpsaValue>> getDst() const {
        std::map<std::string, std::shared_ptr<IpsaValue>> dst = {
            {"width", makeValue(width)}
        };
        std::vector<std::shared_ptr<IpsaValue>> entries;
        for (auto x : entry) {
            entries.push_back(makeValue(x));
        }
        dst.insert({{"value", makeValue(entries)}});
        return std::move(dst);
    }
public:
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        return makeValue(getDst());
    }
};

class IpsaTableUpdateKey : public IpsaTableUpdateActionParameter {
public:
    std::vector<uint8_t> mask;
    std::vector<uint8_t> parseNumber(std::string value, int width);
    std::vector<uint8_t> generateMask(int width, int prelen);

    IpsaTableUpdateKey(std::string value, int _width);
    IpsaTableUpdateActionParameter abandonMask() {
        IpsaTableUpdateActionParameter result;
        result.width = width;
        result.entry = std::move(entry);
        return std::move(result);
    }
    virtual std::shared_ptr<IpsaValue> toIpsaValue() const {
        auto dst = getDst();
        std::vector<std::shared_ptr<IpsaValue>> masks;
        for (auto y : mask) {
            masks.push_back(makeValue(y));
        }
        dst.insert({{"mask", makeValue(masks)}});
        return makeValue(dst);
    }
};

IpsaTableUpdateKey::IpsaTableUpdateKey(std::string value, int _width) {
    width = _width;
    auto has_dot = value.find('.') != std::string::npos;
    auto has_slide = value.find('/') != std::string::npos;
    auto has_colon = value.find(':') != std::string::npos;
    auto has_dash = value.find('-') != std::string::npos;
    auto has_and = value.find('&') != std::string::npos;
    if (has_and) {
        // literal&&&literal
        int and_pos = value.find('&');
        auto entry = value.substr(0, and_pos);
        auto mask = value.substr(and_pos+3);
        this->entry = parseNumber(entry, width);
        this->mask = parseNumber(mask, width);
    } else {
        if (has_dash) {
            // AA-BB-CC-DD-EE-FF
            for (int i = 0; i < 6; i++) {
                uint8_t c = (convert(value[i*3]) << 4 | convert(value[i*3+1])) & 0xFF;
                entry.push_back(c);
            }
            mask = generateMask(width, -1);
        } else if (has_dot) {
            // ipv4
            entry.resize(4, 0);
            int prelen = -1;
            if (has_slide) {
                int slide_pos = value.find('/');
                prelen = std::stoi(value.substr(slide_pos+1));
                value = value.substr(0, slide_pos);
            }
            #ifdef __linux__
            inet_pton(AF_INET, value.c_str(), &entry[0]);
            #else
            for (int i = 0; i < 4; i++) {
                int next_pos = value.find('.');
                entry[i] = std::stoi(value.substr(0, next_pos)) & 0xFF;
                value = value.substr(next_pos+1);
            }
            #endif
            mask = generateMask(width, prelen);
        } else if (has_colon) {
            // ipv6
            entry.resize(16, 0);
            int prelen = -1;
            if (has_slide) {
                int slide_pos = value.find('/');
                prelen = std::stoi(value.substr(slide_pos+1));
                value = value.substr(0, slide_pos);
            }
            #ifdef __linux__
            inet_pton(AF_INET6, value.c_str(), &entry[0]);
            #else
            // only support AAAA:BBBB:CCCC:DDDD:EEEE:FFFF:GGGG:HHHH
            for (int i = 0; i < 8; i++) {
                uint8_t c = (convert(value[i*5]) << 4 | convert(value[i*5+1])) & 0xFF;
                uint8_t d = (convert(value[i*5+2]) << 4 | convert(value[i*5+3])) & 0xFF;
                entry[i*2] = c;
                entry[i*2+1] = d;
            }
            #endif
            mask = generateMask(width, prelen);
        } else {
            // literal
            this->entry = parseNumber(value, width);
            this->mask = generateMask(width, -1);
        }
    }
}

std::vector<uint8_t> IpsaTableUpdateKey::parseNumber(std::string value, int width) {
    std::vector<uint8_t> result;
    int size = (width + 7) / 8;
    result.resize(size, 0);
    if (value.size() >= 2 && value.substr(0, 2) == "0x") {
        for (int i = value.size() - 1, j = size-1; i >= 2; i -= 2, j--) {
            uint8_t c = convert(value[i]);
            if (i > 2) {
                c |= convert(value[i-1]) << 4;
            }
            result[j] = c;
        }
    } else {
        int x = std::stoi(value);
        for (int j = size-1; j >= 0 && x > 0; x >>= 8, j--) {
            result[j] = x & 0xFF;
        }
    }
    return std::move(result);
}

std::vector<uint8_t> IpsaTableUpdateKey::generateMask(int width, int prelen) {
    std::vector<uint8_t> result;
    int size = (width + 7) / 8;
    result.resize(size, 0xFF);
    if (uint8_t x = width % 8; x > 0) {
        result[0] = (~(-1 << x)) & 0xFF;
    }
    if (prelen >= 0) {
        for (int z = width - prelen, i = size-1; z > 0; z -= 8, i--) {
            result[i] &= (-1 << std::max(z, 8));
        }
    }
    return std::move(result);
}