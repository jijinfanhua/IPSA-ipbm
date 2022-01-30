#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <any>

// Base class for tree nodes
class Rp4TreeNode {
public:
    Rp4TreeNode() {}
    virtual std::vector<const Rp4TreeNode*> children() const {
        return {};
    }
    virtual std::string toString() const = 0;
}; 

template <class T, std::enable_if_t<std::is_base_of_v<Rp4TreeNode, T>, bool> = true>
void add(std::vector<const Rp4TreeNode*>& dst, const T& v) {
    dst.push_back(dynamic_cast<const Rp4TreeNode*>(&v));
}

template <class T, std::enable_if_t<std::is_base_of_v<Rp4TreeNode, T>, bool> = true>
void add(std::vector<const Rp4TreeNode*>& dst, const std::shared_ptr<T>& v) {
    dst.push_back(dynamic_cast<const Rp4TreeNode*>(v.get()));
}

template <class T, std::enable_if_t<std::is_base_of_v<Rp4TreeNode, T>, bool> = true>
void add(std::vector<const Rp4TreeNode*>& dst, const std::vector<T>& v) {
    for (auto& x : v) {
        add(dst, x);
    }
}

template <class T, std::enable_if_t<std::is_base_of_v<Rp4TreeNode, T>, bool> = true>
void add(std::vector<const Rp4TreeNode*>& dst, const std::vector<std::shared_ptr<T>>& v) {
    for (auto& x : v) {
        add(dst, x);
    }
}

template <class T, std::enable_if_t<std::is_base_of_v<Rp4TreeNode, T>, bool> = true>
std::vector<const Rp4TreeNode*> mapped(const std::vector<T>& v) {
    std::vector<const Rp4TreeNode*> dst;
    add(dst, v);
    return std::move(dst);
}

template <class T, std::enable_if_t<std::is_base_of_v<Rp4TreeNode, T>, bool> = true>
std::vector<const Rp4TreeNode*> mapped(const std::vector<std::shared_ptr<T>>& v) {
    std::vector<const Rp4TreeNode*> dst;
    add(dst, v);
    return std::move(dst);
}