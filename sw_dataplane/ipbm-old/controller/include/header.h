//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_HEADER_H
#define GRPC_TEST_HEADER_H

#include <iostream>
#include <vector>
#include <utility>

#include "field.h"
#include "next_header.h"

class Header {
public:
    Header(uint32_t header_id, int32_t header_length, std::string& header_name);
    void addField(Field field);
    std::vector<Field>* getField();
    void addNextHeader(const NextHeader& next_header);
    std::vector<NextHeader>* getNextHeader();
    uint32_t getHeaderId() const;
    void setHeaderId(uint32_t headerId);
    int32_t getHeaderLength() const;
    void setHeaderLength(int32_t headerLength);
    const std::string &getHeaderName() const;
    void setHeaderName(const std::string &headerName);

private:
    uint32_t header_id;
    int32_t header_length;
    std::string header_name;
    std::vector<Field> fields;
    std::vector<NextHeader> next_headers;
};

#endif //GRPC_TEST_HEADER_H
