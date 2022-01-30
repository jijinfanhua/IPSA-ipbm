//
// Created by fengyong on 2021/2/9.
//

#include "../include/header.h"

NextHeader::NextHeader(uint32_t headerId, std::string tag) {
    this->header_id = headerId;
    this->tag = std::move(tag);
}

uint32_t NextHeader::getHeaderId() const {
    return header_id;
}

void NextHeader::setHeaderId(uint32_t headerId) {
    header_id = headerId;
}

const std::string &NextHeader::getTag() const {
    return tag;
}

void NextHeader::setTag(const std::string &header_tag) {
    this->tag = header_tag;
}
