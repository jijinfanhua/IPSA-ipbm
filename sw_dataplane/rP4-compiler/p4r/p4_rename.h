#pragma once

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <functional>

template <typename T>
class P4Rename {
public:
    std::vector<std::string> original_names;
    std::vector<std::string> new_names;
    std::vector<bool> has_suffix;
    std::vector<bool> is_duplicated;

    std::string get_name(const std::string& original_name, int index);
    void add_names(const std::vector<T>& v, std::function<std::string(const T&)> f);
    void add_name(const std::string& name, bool suffix);
    P4Rename(const std::vector<T>& v, std::function<std::string(const T&)> f);
    P4Rename() {}
};

template <typename T>
void P4Rename<T>::add_names(const std::vector<T>& v, std::function<std::string(const T&)> f) {
    for (auto& item : v) {
        add_name(f(item), false);
    }
}

template <typename T>
void P4Rename<T>::add_name(const std::string& name, bool suffix) {
    if (auto x = std::find_if(std::begin(original_names), std::end(original_names), [&](auto& _name) {
        return name == _name;
    }); x != std::end(original_names)) {
        is_duplicated[x - std::begin(original_names)] = true;
    } else {
        original_names.push_back(name);
        new_names.push_back("");
        has_suffix.push_back(suffix);
        is_duplicated.push_back(false);
    }   
}

template <typename T>
P4Rename<T>::P4Rename(const std::vector<T>& v, std::function<std::string(const T&)> f) {
    add_names(v, f);
}

template <typename T>
std::string P4Rename<T>::get_name(const std::string& original_name, int index) {
    for (unsigned i = 0; i < original_names.size(); i++) {
        if (original_names[i] == original_name) {
            if (new_names[i].size() == 0) {
                unsigned int start = -1;
                for (int x = 0; x < 10; x++) { // omit infinite loop
                    auto next = original_name.find_first_of('.', start + 1);
                    if (next != std::string::npos) {
                        auto test_text = original_name.substr(next+1);
                        if (std::find_if(std::begin(original_names), std::end(original_names), [&](auto& name) {
                            return name != original_name && name.size() >= test_text.size() &&\
                                name.substr(name.size() - test_text.size()) == test_text;
                        }) != std::end(original_names)) {
                            break;
                        } else {
                            start = next;
                        }
                    } else {
                        break;
                    }
                }
                auto temp = original_name.substr(start+1);
                for (unsigned int i = 0; i < temp.size(); i++) {
                    if (temp[i] == '.') {
                        temp[i] = '_';
                    }
                }
                if (has_suffix[i]) {
                    new_names[i] = temp + "_s";
                } else {
                    new_names[i] = temp;
                }
            }
            if (is_duplicated[i]) {
                return new_names[i] + "_" + std::to_string(index);
            } else {
                return new_names[i];
            }
        }
    }
    return original_name;
}