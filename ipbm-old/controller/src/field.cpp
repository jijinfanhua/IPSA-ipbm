
#include "../include/field.h"

//
// Created by fengyong on 2021/2/9.
//

Field::Field(uint32_t field_id, uint32_t field_length, uint32_t internel_offset){
    this->field_id = field_id;
    this->field_length = field_length;
    this->internel_offset = internel_offset;
}

uint32_t Field::getFieldId() const {
    return this->field_id;
}

void Field::setFieldId(uint32_t fieldId) {
    this->field_id = fieldId;
}

uint32_t Field::getFieldLength() const {
    return this->field_length;
}

void Field::setFieldLength(uint32_t fieldLength) {
    this->field_length = fieldLength;
}

uint32_t Field::getInternelOffset() const {
    return this->internel_offset;
}

void Field::setInternelOffset(uint32_t internelOffset) {
    this->internel_offset = internelOffset;
}
