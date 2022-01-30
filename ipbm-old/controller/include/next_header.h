//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_NEXT_HEADER_H
#define GRPC_TEST_NEXT_HEADER_H

#include <iostream>
#include <string>
#include <utility>

class NextHeader {
public:
    NextHeader(uint32_t headerId, std::string tag);
    uint32_t getHeaderId() const;
    void setHeaderId(uint32_t headerId);
    const std::string &getTag() const;
    void setTag(const std::string &header_tag);

private:
    uint32_t header_id;
    std::string tag;
};

#endif //GRPC_TEST_NEXT_HEADER_H
