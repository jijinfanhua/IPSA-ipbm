//
// Created by fengyong on 2021/2/9.
//

#ifndef GRPC_TEST_FIELD_H
#define GRPC_TEST_FIELD_H

#include <iostream>

class Field {
public:
    Field(uint32_t field_id, uint32_t field_length, uint32_t internel_offset);
    uint32_t getFieldId() const;
    void setFieldId(uint32_t fieldId);
    uint32_t getFieldLength() const;
    void setFieldLength(uint32_t fieldLength);
    uint32_t getInternelOffset() const;
    void setInternelOffset(uint32_t internelOffset);

private:
    uint32_t field_id;
    uint32_t field_length;
    uint32_t internel_offset;
};

#endif //GRPC_TEST_FIELD_H
