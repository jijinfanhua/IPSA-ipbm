//
// Created by fengyong on 2021/2/9.
//

#include "../include/header.h"

Header::Header(uint32_t header_id, int32_t header_length, std::string& header_name) {
    this->header_id = header_id;
    this->header_length = header_length;
    this->header_name = std::move(header_name);
}

void Header::addField(Field field) {
    this->fields.push_back(field);
}

std::vector<Field>* Header::getField(){
    return &(this->fields);
}

void Header::addNextHeader(const NextHeader& next_header) {
    this->next_headers.push_back(next_header);
}

std::vector<NextHeader>* Header::getNextHeader() {
    return &(this->next_headers);
}

uint32_t Header::getHeaderId() const {
    return this->header_id;
}

void Header::setHeaderId(uint32_t headerId) {
    this->header_id = headerId;
}

int32_t Header::getHeaderLength() const {
    return this->header_length;
}

void Header::setHeaderLength(int32_t headerLength) {
    this->header_length = headerLength;
}

const std::string &Header::getHeaderName() const {
    return this->header_name;
}

void Header::setHeaderName(const std::string &headerName) {
    this->header_name = headerName;
}
