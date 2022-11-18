//
// Created by xilinx_0 on 1/2/22.
//

#ifndef RECONF_SWITCH_IPSA_EXECUTOR_H
#define RECONF_SWITCH_IPSA_EXECUTOR_H

#include "defs.h"
#include <cmath>
#include <utility>
#include <vector>

//enum class OpCode {
//    ADD, SUB, MUL, DIV, MOD,
//    SHIFT_LEFT, SHIFT_RIGHT, BIT_AND, BIT_OR, BIT_XOR, BIT_NEG,
//    SET_FIELD, COPY_FIELD,
//    SET_TTL, DECREMENT_TTL, COPY_TTL_OUTWARDS, COPY_TTL_INWARDS,
//    PUSH, POP, DECREMENT
//};
//
//enum class ParameterType {
//    CONSTANT, HEADER, FIELD
//};

//union Parameter {
//    int header_id;
//    // field dose not have id
//    struct {
//        uint8_t hdr_id;
//        uint16_t internal_offset;
//        uint16_t fd_len;
//    } field;
//    int data;
//};

//uint32_t get_op_val(Parameter para, PHV * phv, std::vector<ActionParam*> action_paras) {
//    uint32_t res = 0;
//    switch(para.type) {
//        case PT::CONSTANT : res = para.val.data; break;
//        case PT::FIELD : { // field len is less than 32
//            int fd_len = para.val.field.fd_len;
//            int internal_offset = para.val.field.internal_offset;
//            HeaderInfo * hi = phv->parsed_headers[para.val.field.hdr_id];
//
//            int start = hi->hdr_offset + internal_offset;
//            int end = start + fd_len;
//
//            int byte_start = start / 8;
//            int byte_end = ceil(end * 1.0 / 8);
//
//            int left_shift = start % 8;
//            int right_shift = 8 - end % 8;
//
//            phv->packet[byte_start] << left_shift >> left_shift;
//            phv->packet[byte_end] >> right_shift << right_shift;
//
//            uint32_t value = 0;
//
//            for(int i = byte_end - 1; i >= byte_start; i--) {
//                value += (uint32_t)phv->packet[i] << ((byte_end - 1 - i) * 8);
//            }
//            res = value;
//            break;
//        }
//        case PT::PARAM : { // if it is value-, just extract it out; if it is field-, should be larger than uint32_t
//            ActionParam * ap = action_paras[para.val.action_para_id];
//            if(ap->action_para_len > 32) {
//                res = -1;
//            }else {
//                int byte_len = ceil(ap->action_para_len * 1.0 / 8);
//                for(int i = byte_len - 1; i >= 0; i--) {
//                    res += (uint32_t)ap->val[i] << ((byte_len - 1 - i) * 8);
//                }
//            }
//            break;
//        }
//        default:
//            break;
//    }
//    return res;
//}

uint32_t get_value(Data data) {
    uint32_t sum = 0;

    int byte_len = ceil(data.data_len * 1.0 / 8);
    int max = byte_len >= 4 ? 4 : byte_len;
    for(int i = max - 1; i >= 0 ; i--) {
        sum += (uint32_t)data.val[i] << ((max - 1 - i) * 8);
    }

    return sum;
}

Data get_data (int len, uint32_t d) {
    Data data{};
    data.data_len = len;
    int byte_len = ceil(len * 1.0 / 8);
    data.val = new uint8_t[byte_len];
    for(int i = 0; i < byte_len; i++) {
        data.val[byte_len - 1 - i] = d >> (i * 8);
    }
    return data;
}

Data get_op_val_any(Parameter para, PHV * phv, std::vector<ActionParam*> action_paras) {
    Data res{};
    LOG(INFO) << "      param type(constant, header, field, param): " << (int)para.type;
    switch(para.type) {
        case PT::CONSTANT : {
            res = para.val.constant_data;
            LOG(INFO) << "          length: " << res.data_len;
            LOG(INFO) << "          value: ";
            for(int i = 0 ; i < ceil(res.data_len*1.0/8); i++) {
                LOG(INFO) << "               " << (int)res.val[i];
            }
            break;
        }
        case PT::FIELD : {
            int fd_len = para.val.field.fd_len;
            int internal_offset = para.val.field.internal_offset;
            HeaderInfo * hi = phv->parsed_headers[para.val.field.hdr_id];

            LOG(INFO) << "          obtained from:";
            LOG(INFO) << "          hdr_id: " << (int)para.val.field.hdr_id;
            LOG(INFO) << "          internal_offset: " << para.val.field.internal_offset;
            LOG(INFO) << "          field_length: " << para.val.field.fd_len;


            int start = hi->hdr_offset + internal_offset;
            int end = start + fd_len;

            int byte_start = start / 8;
            int byte_end = end / 8; //ceil(end * 1.0 / 8);
            int byte_len = byte_end - byte_start + 1;

            LOG(INFO) << "          byte_start: " << byte_start;
            LOG(INFO) << "          byte_end: " << byte_end;
            LOG(INFO) << "          original value: ";
            for(int i = byte_start ; i <= byte_end; i++) {
                LOG(INFO) << "               " << (int)phv->packet[i];
            }

            int left_shift = start % 8;
            int right_shift = 8 - end % 8;

//            res.if_sign = false;
            res.data_len = fd_len;
            res.val = new uint8_t[(int)ceil(fd_len * 1.0 / 8)];
            int fd_len_byte = ceil(fd_len * 1.0 / 8);

            auto fd_backup = (uint8_t*) malloc(sizeof(uint8_t) * byte_len);
            memset(fd_backup, 0, sizeof(uint8_t) * byte_len);
            memcpy(fd_backup, phv->packet + byte_start, byte_len);
            fd_backup[0] = fd_backup[0] << left_shift >> left_shift;
            fd_backup[byte_len - 1] = fd_backup[byte_len - 1] >> right_shift << right_shift;

            int idx = fd_len_byte - 1;
            for(int i = byte_len - 1; i > 0; i--) {
                res.val[idx] = (fd_backup[i] >> right_shift) + (fd_backup[i - 1] << (8 - right_shift));
                idx--;
//                std::cout << (int)res.val[idx+1] << std::endl;
            }
            LOG(INFO) << "          length: " << res.data_len;
            LOG(INFO) << "          value: ";
            for(int i = 0 ; i < ceil(res.data_len*1.0/8); i++) {
                LOG(INFO) << "               " << (int)res.val[i];
            }
            break;
        }
        case PT::PARAM : { // if it is value-, just extract it out; if it is field-, should be larger than uint32_t
            ActionParam * ap = action_paras[para.val.action_para_id];
            res.data_len = ap->action_para_len;
//            res.if_sign = ap->if_sign;
            res.val = ap->val;
            LOG(INFO) << "          length: " << res.data_len;
            LOG(INFO) << "          value: ";
            for(int i = 0 ; i < ceil(res.data_len*1.0/8); i++) {
                LOG(INFO) << "               " << (int)res.val[i];
            }
            break;
        }
        default:
            break;
    }
    return res;
}

Data bit_compute(int op, Data data1, Data data2) {
    switch (op) {
        case 1: { // <<
            int left_shift_num = get_value(data2);
            uint32_t operand = get_value(data1);
            uint32_t bitmap = 0xffffffff;
            for(int i = 0; i < 32 - data1.data_len; i++) {
                bitmap -= 1 << (31 - i);
            }
            operand = (operand << left_shift_num) & bitmap;
            return get_data(data1.data_len, operand);
        }
        case 2 : { // >>
            int right_shift_num = get_value(data2);
            uint32_t operand = get_value(data1);
            operand = (operand >> right_shift_num);
            return get_data(data1.data_len, operand);
        }
        case 3 : { // &
            uint32_t left_operand = get_value(data1);
            uint32_t right_operand = get_value(data2);
            uint32_t res = left_operand & right_operand;
            return get_data(data1.data_len, res);
        }
        case 4 : { // |
            uint32_t left_operand = get_value(data1);
            uint32_t right_operand = get_value(data2);
            uint32_t res = left_operand | right_operand;
            return get_data(data1.data_len, res);
        }
        case 5 : { // ^
            uint32_t left_operand = get_value(data1);
            uint32_t right_operand = get_value(data2);
            uint32_t res = left_operand ^ right_operand;
            return get_data(data1.data_len, res);
        }
        case 6 : { // ~
            uint32_t left_operand = get_value(data1);
            uint32_t res = 0;
            if(data1.data_len <= 8) {
                res = uint8_t(~left_operand);
            } else if(data1.data_len <= 16) {
                res = uint16_t(~left_operand);
            } else if(data1.data_len <= 32) {
                res = ~left_operand;
            }
//            uint32_t res = ~left_operand;
            return get_data(data1.data_len, res);
        }
        default:
            break;
    }
}

struct ExpTreeNode {
public:
    Parameter opt;
    struct ExpTreeNode * left = nullptr;
    struct ExpTreeNode * right = nullptr;
};

struct Primitive: public ExpTreeNode {
public:
    FieldInfo *lvalue;

    Primitive(struct ExpTreeNode * r = nullptr) : root(r){}

    void set_lvalue(FieldInfo * fdInfo) {
        lvalue = fdInfo;
    }

    ExpTreeNode* get_root() {
        return this->root;
    }

    void set_root(ExpTreeNode * r) {
        root = r;
    }

    void run(PHV * phv, const std::vector<ActionParam*>& action_paras) {
        LOG(INFO) << "  Primitive run...";
        Data res = execute(phv, root, action_paras, 2);

        auto *p = new Parameter();
        p->type = ParameterType::FIELD;
        p->val.field = {lvalue->hdr_id, lvalue->internal_offset,
                        lvalue->fd_len, lvalue->fd_type};
        LOG(INFO) << "      modify_field: ";
        LOG(INFO) << "          header_id: " << (int)p->val.field.hdr_id;
        LOG(INFO) << "          header_offset: " << (int)phv->parsed_headers[p->val.field.hdr_id]->hdr_offset;
        LOG(INFO) << "          internal_offset: " << (int)p->val.field.internal_offset;
        LOG(INFO) << "          field_length: " << (int)p->val.field.fd_len;
        LOG(INFO) << "          field_type: " << (int)p->val.field.fd_type;

        modify_field_with_data(*p, res, phv);

        LOG(INFO) << "      modified with:";
        LOG(INFO) << "          length: " << res.data_len;
        LOG(INFO) << "          value: " << res.data_len;
        for(int i = 0; i < ceil(res.data_len*1.0/8);i++) {
            LOG(INFO) << "              " << (int)res.val[i];
        }
        LOG(INFO) << "      Mod result: ";

        int begin_bit = phv->parsed_headers[p->val.field.hdr_id]->hdr_offset + p->val.field.internal_offset;
        int end_bit = begin_bit + p->val.field.fd_len - 1;

        int begin_byte = begin_bit / 8;
        int end_byte = end_bit / 8;

        for(int i = begin_byte; i <= end_byte; i++) {
            LOG(INFO) << "          " << (int) phv->packet[i];
        }


    }

    void build_para_tree() {
        root->opt.type = ParameterType::OPERATOR;
        root->opt.val.op = OpCode::BIT_AND;

        root->left->opt.type = ParameterType::CONSTANT;
        uint8_t data0[] = "\xff";
        root->left->opt.val.constant_data = Data{16, data0};

        root->left->opt.type = ParameterType::OPERATOR;
        root->left->opt.val.op = OpCode::ADD;
    }

    Data execute(PHV * phv, ExpTreeNode * etn, const std::vector<ActionParam*>& action_paras, int of) {
        std::string off = "";
        for(int i = 0 ; i < of; i++) off+= "\t";
        LOG(INFO) << off << "node.type(constant, header, field, param, operator):  " <<  (int)etn->opt.type;
        if(etn->opt.type == ParameterType::OPERATOR) {
            LOG(INFO) << off << "   operator.op(+ -):  " <<  (int)etn->opt.val.op;
            // ony set_field and copy_field can exceed 32
            switch (etn->opt.val.op) {
                case OpCode::ADD : {
                    uint32_t res = get_value(execute(phv, etn->left, action_paras, of+1))
                            + get_value(execute(phv, etn->right, action_paras, of+1));
                    LOG(INFO) << off << "   value: " << res;
                    return get_data(32, res);
                }
                case OpCode::SUB : {
                    uint32_t res = get_value(execute(phv, etn->left, action_paras, of+1))
                                   - get_value(execute(phv, etn->right, action_paras, of+1));
                    LOG(INFO) << off << "   value: " << res;
                    return get_data(32, res);
                }
                case OpCode::SHIFT_LEFT : {
                    Data data = execute(phv, etn->left, action_paras, of+1);
                    Data shift = execute(phv, etn->right, action_paras, of+1);
                    Data res = bit_compute(1, data, shift);
                    return res;
                }
                case OpCode::SHIFT_RIGHT : {
                    Data data = execute(phv, etn->left, action_paras, of+1);
                    Data shift = execute(phv, etn->right, action_paras, of+1);
                    Data res = bit_compute(2, data, shift);
                    return res;
                }
                case OpCode::BIT_AND : {
                    Data data1 = execute(phv, etn->left, action_paras, of+1);
                    Data data2 = execute(phv, etn->right, action_paras, of+1);
                    Data res = bit_compute(3, data1, data2);
                    return res;
                }
                case OpCode::BIT_OR : {
                    Data data1 = execute(phv, etn->left, action_paras, of+1);
                    Data data2 = execute(phv, etn->right, action_paras, of+1);
                    Data res = bit_compute(4, data1, data2);
                    return res;
                }
                case OpCode::BIT_XOR : {
                    Data data1 = execute(phv, etn->left, action_paras, of+1);
                    Data data2 = execute(phv, etn->right, action_paras, of+1);
                    Data res = bit_compute(5, data1, data2);
                    return res;
                }
                case OpCode::BIT_NEG : {
                    Data data1 = execute(phv, etn->left, action_paras, of+1);
                    Data res = bit_compute(6, data1, Data{});
                    return res;
                }
//                case OpCode::SET_FIELD : {
//                    Data res = execute(phv, etn->left, action_paras);
//                    auto *p = new Parameter();
//                    p->type = ParameterType::FIELD;
//                    p->val.field = {lvalue->hdr_id, lvalue->internal_offset,
//                                   lvalue->fd_len, lvalue->fd_type};
//                    modify_field_with_data(*p, res, phv);
//                    break;
//                }
//                case OpCode::COPY_FIELD : {
//                    Data res = execute(phv, etn->left, action_paras);
//                    auto *p = new Parameter();
//                    p->type = ParameterType::FIELD;
//                    p->val.field = {lvalue->hdr_id, lvalue->internal_offset,
//                                    lvalue->fd_len, lvalue->fd_type};
//                    modify_field_with_data(*p, res, phv);
//                    break;
//                }
                default:
                    break;
            }
        } else {
            LOG(INFO) << off << "   get operand: ";
            return get_op_val_any(etn->opt, phv, action_paras);
        }
    }

    static void modify_field_with_data(Parameter para, Data data, PHV * phv) {
        int internal_offset = para.val.field.internal_offset; //48
        int fd_len = para.val.field.fd_len; //48
        auto hi = phv->parsed_headers[para.val.field.hdr_id];

        int begin_bit = hi->hdr_offset + internal_offset;//48
        int end_bit = begin_bit + fd_len - 1;//95

        int begin_byte = begin_bit / 8, end_byte = end_bit / 8;// 6 11

        int byte_len = end_byte - begin_byte + 1;//6
        auto span_byte = new uint8_t[byte_len]{0};

        auto new_data = new uint8_t[byte_len];
        memset(new_data, 0, byte_len * sizeof(uint8_t));
        int data_len = ceil(data.data_len * 1.0 / 8);
        if(data_len < byte_len) {
            memcpy(new_data+1, data.val, byte_len-1);
        } else if(data_len == byte_len) {
            memcpy(new_data, data.val, byte_len);
        } else{
            memcpy(new_data, data.val+data_len-byte_len, byte_len);
        }

        int right_empty = (end_bit / 8 + 1) * 8 - end_bit - 1;

        for(int i = byte_len - 1; i >= 1; i--) {
            span_byte[i] += (new_data[i] << right_empty);
            span_byte[i-1] += (new_data[i] >> (8 - right_empty));
        }
        span_byte[0] += (new_data[0] << right_empty);

        auto bitmap = new uint8_t[byte_len]{0};
        int left_empty = begin_bit % 8;

        for(int i = 0; i < left_empty; i++) {
            bitmap[0] += ( 1 << (7 - i) );
        }

        for(int i = 0; i < right_empty; i++) {
            bitmap[byte_len-1] += (1 << i);
        }

        int idx = 0;
        for(int i = begin_byte; i <= end_byte; i++) {
            phv->packet[i] = (phv->packet[i] & bitmap[idx]) + span_byte[idx];
            idx++;
        }


//
//        int begin = hi->hdr_offset + internal_offset; //48
//        int end = begin + fd_len; // 96
//
//        int begin_offset = begin % 8; //0
//        int end_offset = end % 8; // 0
//
//        int byte_begin = begin / 8; // 6
//        int byte_end = end / 8 - (end_offset == 0 ? 1 : 0);//ceil(end * 1.0 / 8); //11
//        int byte_len = byte_end - byte_begin + 1;//6
//
//        int origin_len =  data.data_len / 8 + (data.data_len % 8 ? 1 : 0);//6
//        int len = 0;
//        if(end_offset == 0) {
//            len = data.data_len / 8 + (data.data_len % 8 ? 1 : 0); // 6
//        } else {
//            len = (data.data_len + (8 - end_offset)) / 8 + ((data.data_len + (8 - end_offset)) % 8 ? 1 : 0);
//        }
////        int len = (data.data_len + (8 - end_offset)) / 8 + (data.data_len + (8 - end_offset)) % 8 ? 1 : 0;
//
//        auto * res = new uint8_t[len];
//        memset(res, 0, len * sizeof(uint8_t));
//
//        int idx = len - 1; // 5    //00 00 00 00 01 50 -> 00 00 00 00 01 01 // 00 00 00 00 00 00 // 01 01
//        for(int i = origin_len - 1; i >= 0; i--) {
//            res[idx] += (data.val[i] << (8 - (end_offset == 0 ? 8 : end_offset)));
//            if(idx > 0){
//                res[idx - 1] += (data.val[i] >> end_offset);
//                idx--;
//            }
//        }
//
//        idx = len - 1;
//        for(int i = byte_end; i >= byte_begin; i--) {
//            int save_bit_num_right = 0;
//            int save_bit_num_left = 0;
//            if (i == byte_end && end_offset != 0){
//                save_bit_num_right = 8 - end_offset;
//            }
//            if (i == byte_begin && begin_offset != 0){
//                save_bit_num_left = begin_offset;
//            }
//            uint8_t sd = 0;
//            for(int j = 0; j < save_bit_num_left; j++) {
//                sd += (1 << (7 - j));
//            }
//            for(int j = 0; j < save_bit_num_right; j++) {
//                sd += (1 << j);
//            }
//            phv->packet[i] = (phv->packet[i] & sd) + res[idx--]; // here
//        }
    }

private:
    struct ExpTreeNode * root;
};

//struct Primitive {
//    OpCode op;
//    int parameter_num;
//    Parameter * params;
//
//    Primitive() = default;
//
//    Primitive(OpCode _op, int _para_num, Parameter _para[]) {
//        op = _op;
//        parameter_num = _para_num;
//        params = new Parameter[parameter_num];
//        for(int i = 0; i < parameter_num; i++) {
//            params[i].type = _para[i].type;
//            params[i].val = _para[i].val;
//        }
//    }
//
//    void set_para(Parameter _para[]) {
//        params = new Parameter[parameter_num];
//        for(int i = 0; i < parameter_num; i++) {
//            params[i].type = _para[i].type;
//            params[i].val = _para[i].val;
//        }
//    }
//
//    static void modify_field_with_uint(Parameter para, uint32_t res, PHV * phv) {
//        int internal_offset = para.val.field.internal_offset;
//        int fd_len = para.val.field.fd_len;
//        auto hi = phv->parsed_headers[para.val.field.hdr_id];
//
//        int begin = hi->hdr_offset + internal_offset;
//        int end = begin + fd_len;
//
//        int begin_offset = begin % 8;
//        int end_offset = end % 8;
//
//
//        if(end_offset != 0){
//            res = res << (8 - end_offset);
//        }
//
//        int byte_begin = begin / 8;
//        int byte_end = end / 8 - (end_offset == 0 ? 1 : 0);
//
//        for(int i = byte_end; i >= byte_begin; i--) {
//            uint8_t by = res << (3 - (byte_end - i)) * 8 >> 3 * 8;
//            int save_bit_num_right = 0;
//            int save_bit_num_left = 0;
//            if (i == byte_end && end_offset != 0){
//                save_bit_num_right = 8 - end_offset;
//            }
//            if (i == byte_begin && begin_offset != 0){
//                save_bit_num_left = begin_offset;
//            }
//
//            uint8_t sd = 0;
//            for(int j = 0; j < save_bit_num_left; j++) {
//                sd += (1 << (7 - j));
//            }
//            for(int j = 0; j < save_bit_num_right; j++) {
//                sd += (1 << j);
//            }
//            phv->packet[i] = (phv->packet[i] & sd) + by; // here
//        }
//    }
//
//    static void modify_field_with_data(Parameter para, Data data, PHV * phv) {
//        int internal_offset = para.val.field.internal_offset;
//        int fd_len = para.val.field.fd_len;
//        auto hi = phv->parsed_headers[para.val.field.hdr_id];
//
//        int begin = hi->hdr_offset + internal_offset;
//        int end = begin + fd_len;
//
//        int begin_offset = begin % 8;
//        int end_offset = end % 8;
//
//        int byte_begin = begin / 8;
//        int byte_end = end / 8 - (end_offset == 0 ? 1 : 0);//ceil(end * 1.0 / 8);
//        int byte_len = byte_end - byte_begin + 1;
//
//        int origin_len =  data.data_len / 8 + (data.data_len % 8 ? 1 : 0);
//        int len = 0;
//        if(end_offset == 0) {
//            len = data.data_len / 8 + (data.data_len % 8 ? 1 : 0);
//        } else {
//            len = (data.data_len + (8 - end_offset)) / 8 + ((data.data_len + (8 - end_offset)) % 8 ? 1 : 0);
//        }
////        int len = (data.data_len + (8 - end_offset)) / 8 + (data.data_len + (8 - end_offset)) % 8 ? 1 : 0;
//
//        auto * res = new uint8_t[len];
//        memset(res, 0, len * sizeof(uint8_t));
//
//        int idx = len - 1;
//        for(int i = origin_len - 1; i >= 0; i--) {
//            res[idx] += (data.val[i] << (8 - (end_offset == 0 ? 8 : end_offset)));
//            if(idx > 0){
//                res[idx - 1] += (data.val[i] >> end_offset);
//                idx--;
//            }
//        }
//
//        idx = len - 1;
//        for(int i = byte_end; i >= byte_begin; i--) {
//            int save_bit_num_right = 0;
//            int save_bit_num_left = 0;
//            if (i == byte_end && end_offset != 0){
//                save_bit_num_right = 8 - end_offset;
//            }
//            if (i == byte_begin && begin_offset != 0){
//                save_bit_num_left = begin_offset;
//            }
//            uint8_t sd = 0;
//            for(int j = 0; j < save_bit_num_left; j++) {
//                sd += (1 << (7 - j));
//            }
//            for(int j = 0; j < save_bit_num_right; j++) {
//                sd += (1 << j);
//            }
//            phv->packet[i] = (phv->packet[i] & sd) + res[idx--]; // here
//        }
//    }
//
//    void execute(PHV * phv, const std::vector<ActionParam*>& action_paras) const {
//        switch(op) {
//            case OpCode::ADD: { // the first param must be field
//                // get second and third params
//                uint32_t res = get_value(get_op_val_any(params[1], phv, action_paras)) + get_value(get_op_val_any(params[2], phv, action_paras));
////                uint32_t res = get_op_val(params[1], phv, action_paras) + get_op_val(params[2], phv, action_paras);
//                modify_field_with_uint(params[0], res, phv);
//                break;
//            }
//            case OpCode::SUB : { // only care about + and -
//                uint32_t res = get_value(get_op_val_any(params[1], phv, action_paras)) - get_value(get_op_val_any(params[2], phv, action_paras)); // wrap around
//                modify_field_with_uint(params[0], res, phv);
//                break;
//            }
////            case OpCode::SHIFT_LEFT: {
////                uint32_t res = get_op_val(params[1], phv, action_paras) << get_op_val(params[2], phv, action_paras);
////                modify_field(params[0], res, phv);
////                break;
////            }
////            case OpCode::SHIFT_RIGHT: {
////                uint32_t res = get_op_val(params[1], phv, action_paras) >> get_op_val(params[2], phv, action_paras);
////                modify_field(params[0], res, phv);
////                break;
////            } // BIT_AND, BIT_OR, BIT_XOR, BIT_NEG,
////            case OpCode::BIT_AND : {
////                uint32_t res = get_op_val(params[1], phv, action_paras) & get_op_val(params[2], phv, action_paras);
////                modify_field(params[0], res, phv);
////                break;
////            }
////
////            case OpCode::BIT_OR : {
////                uint32_t res = get_op_val(params[1], phv, action_paras) | get_op_val(params[2], phv, action_paras);
////                modify_field(params[0], res, phv);
////                break;
////            }
////            case OpCode::BIT_XOR : {
////                uint32_t res = get_op_val(params[1], phv, action_paras) ^ get_op_val(params[2], phv, action_paras);
////                modify_field(params[0], res, phv);
////                break;
////            }
////            case OpCode::BIT_NEG : {
////                uint32_t res = ~get_op_val(params[1], phv, action_paras);
////                modify_field(params[0], res, phv);
////                break;
////            }
//            case OpCode::SET_FIELD : { // can be larger than 32
//                Data res = get_op_val_any(params[1], phv, action_paras);
//                modify_field_with_data(params[0], res, phv);
//                break;
//            }
//            case OpCode::COPY_FIELD : { // can be larger than 32
//                Data res = get_op_val_any(params[1], phv, action_paras);
//                modify_field_with_data(params[0], res, phv);
//                break;
//            }
////            case OpCode::DECREMENT : {
////                uint32_t res = get_op_val(params[0], phv, action_paras) - 1;
////                modify_field(params[0], res, phv);
////                break;
////            }
//            default:
//                break;
//        }
//    }
//};

// set action as array
struct Action {
    std::vector<Primitive*> prims;
    int primitive_num;

    std::vector<Data> primitive_res;

//    std::vector<Primitive> prims;
    int para_num = 0;
//    std::vector<ActionParam> action_paras;
    std::vector<ActionParam*> action_paras;
    std::vector<int> action_para_lens;
//    int next_proc_id;

//    Action(int _para_num) {
//        para_num = _para_num;
//    }
    Action() = default;

    void set_action_para_lens(std::vector<int> para_lens) {
        action_para_lens = std::move(para_lens);
    }

    void set_action_paras(uint8_t * value) {
        action_paras.clear();
        LOG(INFO) << "      action param num is: " << para_num;
        int cur_offset = 16;
        LOG(INFO) << "      param: ";
        for(int i = 0; i < para_num; i++) {
            int para_start = cur_offset;
            int para_end = cur_offset + action_para_lens[i];
            LOG(INFO) << "          length: " << action_para_lens[i];
            LOG(INFO) << "          start idx: " << para_start;
            LOG(INFO) << "          end idx: " << para_end;

            int left_offset = para_start % 8;
            int right_offset = para_end % 8;

            int byte_start = cur_offset / 8;
            int byte_end = (para_end / 8) - (right_offset == 0 ? 1 : 0);

            int new_len = byte_end - byte_start + 1;
            auto * value_new = new uint8_t[byte_end - byte_start + 1];
            memcpy(value_new, value + byte_start, byte_end - byte_start + 1);
//            std::cout << "f: " << (int)value_new[0] << std::endl;
            value_new[0] = (uint8_t(value_new[0] << left_offset) >> left_offset);
//            std::cout << "y: " << (int)value_new[0] << std::endl;
            value_new[new_len - 1] = right_offset == 0 ? value_new[new_len - 1] :
                    (value_new[new_len - 1] >> (8 - right_offset) << (8-right_offset));

            LOG(INFO) << "          value: ";
            for(int k = 0; k < new_len; k++) {
                LOG(INFO) << "              " <<(int)value_new[k];
            }

            auto data = new ActionParam();
            data->action_para_len = action_para_lens[i];
            int byte_len = (int)ceil(data->action_para_len * 1.0 / 8);
            data->val = new uint8_t[byte_len];
            memset(data->val, 0, sizeof(uint8_t) * byte_len);

            int idx = new_len - 1;

            for(int j = byte_len - 1; j >= 0; j--) {
                if(right_offset == 0) {
                    data->val[j] += value_new[idx];
                }else{
                    data->val[j] += value_new[idx] >> (8 - right_offset);
                    if(idx > 0){
                        data->val[j] += uint8_t(value_new[idx-1] << right_offset);
                    }
                }
//                std::cout << (int)data->val[j] << std::endl;
                idx--;
            }
            LOG(INFO) << "          action para value: ";
            for(int k = 0; k < byte_len; k++) {
                LOG(INFO) << "              " <<(int)data->val[k];
            }
            action_paras.push_back(data);
            cur_offset = para_end;
        }
        LOG(INFO) << "      set action paras end! ";
    }

    void set_primitive(const std::vector<Primitive*>& _prims) {
        for(auto it : _prims) {
            prims.push_back(it);
        }
    }

    void del() {
        auto p_len = prims.size();
        for(int i = 0; i < p_len; i++) {
//            delete prims[i]->params;
            delete prims[i];
        }

        auto a_len = action_paras.size();
        for(int i = 0; i < a_len; i++) {
            delete action_paras[i]->val;
            delete action_paras[i];
        }
    }

    void set_action_para(const std::vector<ActionParam*>& _action_paras) {
        for(auto it : _action_paras) {
            action_paras.push_back(it);
        }
    }

    void execute(PHV * phv) {
        LOG(INFO) << "Primitive num: " << prims.size();
        for(auto it : prims) {
            LOG(INFO)<< "The type of lvalue of primitive: " << (int)it->lvalue->fd_type << std::endl;
            it->run(phv, action_paras);
        }
        LOG(INFO) << "Primitive execute end!";
//        phv->next_proc_id = this->next_proc_id;
    }
};

//struct Gateway {
//
//};

class Executor {
public:
//    int action_num = 0;

    Action * _actions[EXECUTOR_ACTION_NUM];

//    std::vector<Action*> actions;
//    Action * default_action;

    Executor() {

    }

    void insert_action(Action *ac, int action_id) {
//        std::cout << "in insert action" << std::endl;
        _actions[action_id] = ac;

//        actions.push_back(ac);
//        action_num += 1;
    }

//    void insert_default_action(Action * ac) {
//        default_action = ac;
//    }

    // there is no parameter for default action
    void execute_miss_action(int action_id, PHV * phv) {
        Action * action = _actions[action_id];
        action->execute(phv);
    }

    void execute(PHV * phv) {
        LOG(INFO) << "  Executor...";
        int32_t action_id = phv->next_action_id;//phv->match_value[1] + ((uint32_t)phv->match_value[0] << 8);
        LOG(INFO) << "      action_id: " << action_id;
        auto action = _actions[action_id];
        action->set_action_paras(phv->match_value);
        action->execute(phv);
    }

    void execute_action(int action_id, PHV * phv) {
        Action * action = _actions[action_id];
//        action->set_action_paras(phv->match_value);
        action->execute(phv);
    }

    void set_action_para_value(uint8_t * value) {
        int action_id = uint32_t(value[1]) + ((uint32_t)value[0] << 8);
        auto action = _actions[action_id];
        action->set_action_paras(value);
    }

    void del_action(int action_id) {
        _actions[action_id]->del();
        delete _actions[action_id];
    }

    void clear_action() {
        for(int i = 0; i < EXECUTOR_ACTION_NUM; i++) {
            if(_actions[i] != nullptr) {
                _actions[i]->del();
                delete _actions[i];
            }
        }
    }
};

#endif //RECONF_SWITCH_IPSA_EXECUTOR_H
