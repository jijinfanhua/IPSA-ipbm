#pragma once

#include <memory>

class ByteArray {
public:
    ByteArray() : data_(nullptr), size_(0) {}

    ByteArray(size_t size) {
        data_ = new uint8_t[size];
        size_ = size;
    }

    ByteArray(const ByteArray &other) = delete;

    ByteArray(ByteArray &&other) noexcept: ByteArray() {
        *this = std::move(other);
    }

    ByteArray &operator=(const ByteArray &other) = delete;

    ByteArray &operator=(ByteArray &&other) noexcept {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    ~ByteArray() {
        delete[] data_;
    }

    size_t size() const { return size_; }

    bool empty() const { return size_ == 0; }

    const uint8_t *data() const { return data_; }

    uint8_t *data() { return data_; }

private:
    uint8_t *data_;
    size_t size_;
};