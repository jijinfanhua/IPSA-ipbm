//
// Created by xilinx_0 on 1/14/22.
//

#ifndef RECONF_SWITCH_IPSA_CONTROLLER_CLIENT_H
#define RECONF_SWITCH_IPSA_CONTROLLER_CLIENT_H

#include "api_pb/controller_to_dataplane.grpc.pb.h"
//#include "api_pb/controller_to_dataplane.pb.h"

#include "../dataplane/pipeline.h"

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <google/protobuf/text_format.h>

typedef int RC;

namespace api {

    using grpc::Channel;
    using grpc::ClientContext;
    using grpc::Status;

    class CfgClient {
    public:

        CfgClient(std::shared_ptr<Channel> channel)
                : stub_(rp4::CfgService::NewStub(channel)) {};

        template<class T>
        static void print_request(const T &request) {
            std::string req_str;
            google::protobuf::TextFormat::PrintToString(request, &req_str);
            std::cout << "Requesting \n" << req_str << std::endl;
        }

    private:
        std::unique_ptr<rp4::CfgService::Stub> stub_;

    public:

        RC set_metadata (std::vector<HeaderInfo*> metas) {
            rp4::SetMetadataReq request;
            for(auto it : metas) {
                auto meta = request.add_headerinfos();
                meta->set_headerid(it->hdr_id);
                meta->set_headeroffset(it->hdr_offset);
                meta->set_headerlength(it->hdr_len);
            }

            rp4::SetMetadataResp response;
            ClientContext context;

            Status status = stub_->setMetadata(&context, request, &response);

            if(status.ok()) {
                return 1;
            } else {
                return 0;
            }
        }

        RC init_parser_level(int proc_id, int parser_level) {
            rp4::ParserLevelReq request;
            request.set_procid(proc_id);
            request.set_parserlevel(parser_level);

//            print_request(request);

            rp4::ParserLevelResp response;
            ClientContext context;

            Status status = stub_->initParserLevel(&context, request, &response);
            if(status.ok()) {
                return 1;
            } else {
                return 0;
            }

            return 0;
        }

        RC mod_parser_entry(int proc_id, int parser_level, int state,
                            uint32_t entry, uint32_t mask,
                            int hdr_id, int hdr_len, int next_state,
                            int trans_fd_num, std::vector<FieldInfo*> trans_fds,
                            uint8_t miss_act) {
            rp4::ModParserEntryReq request;

            request.set_procid(proc_id);
            request.set_parserlevel(parser_level);
            request.set_state(state);
            request.set_key(entry);
            request.set_mask(mask);
            request.set_hdrid(hdr_id);
            request.set_hdrlen(hdr_len);
            request.set_nextstate(next_state);
            request.set_transfieldnum(trans_fd_num);
            request.set_miss_act(miss_act);

            for(auto it : trans_fds) {
                rp4::FieldInfo *field = request.add_transfds();
                field->set_hdrid(it->hdr_id);
                field->set_internaloffset(it->internal_offset);
                field->set_fieldlen(it->fd_len);
                field->set_fieldtype(get_field_type(it->fd_type));
            }

            rp4::ModParserEntryResp response;
            ClientContext context;
            Status status = stub_->modParserEntry(&context, request, &response);

            if(status.ok()) {
                return 1;
            } else {
                return 0;
            }
        }

        RC clear_parser(int proc_id) {
            rp4::ClearParserReq request;
            request.set_procid(proc_id);

            rp4::ClearParserResp response;
            ClientContext context;
            Status status = stub_->clearParser(&context, request, &response);
            if(status.ok()) return 1;
            else return 0;
        }

        RC insert_relation_exp(int proc_id, RelationExp * exp) {
            // valid, hit, miss, field, constant
            rp4::InsertRelationExpReq request;
            request.set_procid(proc_id);

            rp4::GatewayParam *gp1 = request.mutable_param1();
            rp4::GatewayParam *gp2 = request.mutable_param2();

            set_gateway_param(gp1, exp->param1);
            set_gateway_param(gp2, exp->param2);

            request.set_relation(get_relation_code(exp->relation));

            rp4::InsertRelationExpResp response;
            ClientContext context;
            Status status = stub_->insertRelationExp(&context, request, &response);
            if(status.ok()) return 1;
            else return 0;
        }

        RC clear_relation_exp(int proc_id) {
            rp4::ClearRelationExpReq request;
            request.set_procid(proc_id);

            rp4::ClearRelationExpResp response;
            ClientContext context;

            Status status = stub_->clearRelationExp(&context, request, &response);
            if(status.ok()) return 1;
            else return 0;
        }

        RC clear_res_map(int proc_id) {
            rp4::ClearResMapReq request;
            request.set_procid(proc_id);

            rp4::ClearResMapResp response;
            ClientContext context;

            Status status = stub_->clearResMap(&context, request, &response);
            if(status.ok()) return 1;
            else return 0;
        }

        static rp4::GateEntryType get_gate_entry_type (GateEntryType get) {
            switch (get) {
                case GateEntryType::TABLE : return rp4::GateEntryType::TABLE;
                case GateEntryType::STAGE : return rp4::GateEntryType::STAGE;
                default : break;
            }
        }

        RC mod_res_map(int proc_id, int key, GateEntryType type, int value) {
            rp4::ModResMapReq request;
            request.set_procid(proc_id);
            request.set_key(key);
            auto entry = request.mutable_entry();
            entry->set_type(get_gate_entry_type(type));
            entry->set_val(value);

            rp4::ModResMapResp response;
            ClientContext context;

            Status status = stub_->modResMap(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        RC set_default_gate_entry(int proc_id, GateEntryType type, int value) {
            rp4::SetDefaultGateEntryReq request;
            request.set_proc_id(proc_id);
            auto entry = request.mutable_default_entry();
            entry->set_type(get_gate_entry_type(type));
            entry->set_val(value);

            rp4::SetDefaultGateEntryResp response;
            ClientContext context;

            Status status = stub_->setDefaultGateEntry(&context, request, &response);
            if(status.ok()) return 1;
            else return 0;
        }

        RC set_no_table(int proc_id, int matcher_id, bool no_table) {
            rp4::SetNoTableReq request;
            request.set_procid(proc_id);
            request.set_matcherid(matcher_id);
            request.set_notable(no_table);

            rp4::SetNoTableResp response;
            ClientContext context;

            Status status = stub_->setNoTable(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        RC set_action_proc(int proc_id, int matcher_id, std::unordered_map<int, int> action_proc_map) {
            rp4::SetActionProcReq request;
            request.set_procid(proc_id);
            request.set_matcherid(matcher_id);

            for(auto it : action_proc_map) {
                rp4::ActionProc * action_proc = request.add_actionprocs();
                action_proc->set_actionid(it.first);
                action_proc->set_procid(it.second);
            }

            rp4::SetActionProcResp response;
            ClientContext context;

            Status status = stub_->setActionProc(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        static rp4::MatchType get_match_type(MatchType mt) {
            switch (mt) {
                case MatchType::EXACT : return rp4::MatchType::EXACT;
                case MatchType::LPM : return rp4::MatchType::LPM;
                case MatchType::TERNARY : return rp4::MatchType::TERNARY;
                default:
                    break;
            }
        }

        RC set_mem_config(int proc_id, int matcher_id, int key_width, int value_width,
                          uint8_t * keyConfig, uint8_t * valueConfig, MatchType match_type,
                          int depth, int miss_act_id) {
            rp4::SetMemConfigReq request;
            request.set_procid(proc_id);
            request.set_matcherid(matcher_id);
            request.set_keywidth(key_width);
            request.set_valuewidth(value_width);
            request.set_depth(depth);
            request.set_matchtype(get_match_type(match_type));

            request.set_keyconfig(get_string(keyConfig, key_width * depth));
            request.set_valueconfig(get_string(valueConfig, value_width * depth));

            request.set_missactid(miss_act_id);

            rp4::SetMemConfigResp response;
            ClientContext context;

            Status status = stub_->setMemConfig(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        RC set_field_info(int proc_id, int matcher_id, std::vector<FieldInfo*> _fdInfos) {
            rp4::SetFieldInfoReq request;
            request.set_procid(proc_id);
            request.set_matcherid(matcher_id);

            for(auto it : _fdInfos){
                auto fd = request.add_fdinfos();
                fd->set_hdrid(it->hdr_id);
                fd->set_internaloffset(it->internal_offset);
                fd->set_fieldlen(it->fd_len);
                fd->set_fieldtype(get_field_type(it->fd_type));
            }

            rp4::SetFieldInfoResp response;
            ClientContext context;

            Status status = stub_->setFieldInfo(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

//        RC set_hit_miss_bitmap(int proc_id, int matcher_id, int value) {
//            rp4::SetHitMissBitmapReq request;
//            request.set_procid(proc_id);
//            request.set_matcherid(matcher_id);
//            request.set_value(value);
//
//            rp4::SetHitMissBitmapResp response;
//            ClientContext context;
//
//            Status status = stub_->setHitMissBitmap(&context, request, &response);
//
//            if(status.ok()) return 1;
//            else return 0;
//        }

        RC clear_old_config(int proc_id, int matcher_id) {
            rp4::ClearOldConfigReq request;
            request.set_procid(proc_id);
            request.set_matcherid(matcher_id);

            rp4::ClearOldConfigResp response;
            ClientContext context;

            Status status = stub_->clearOldConfig(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        // no
//        RC clear_field_info() {
//            return 0;
//        }

//        RC set_hit_act_id (int proc_id, int matcher_id, int hit_act_id) {
//            rp4::SetHitActIdReq request;
//            request.set_procid(proc_id);
//            request.set_matcherid(matcher_id);
//            request.set_hit_act_id(hit_act_id);
//
//            rp4::SetHitActIdResp response;
//            ClientContext context;
//
//            Status status = stub_->setHitActId(&context, request, &response);
//
//            if(status.ok()) return 1;
//            else return 0;
//        }

        RC set_miss_act_id (int proc_id, int matcher_id, int miss_act_id) {
            rp4::SetMissActIdReq request;
            request.set_procid(proc_id);
            request.set_matcherid(matcher_id);
            request.set_miss_act_id(miss_act_id);

            rp4::SetMissActIdResp response;
            ClientContext context;

            Status status = stub_->setMissActId(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        RC insert_sram_entry(int proc_id, int matcher_id, uint8_t * key,
                             uint8_t * value, int key_byte_len, int value_byte_len) {
            rp4::InsertSramEntryReq request;
            request.set_procid(proc_id);
            request.set_matcherid(matcher_id);
            request.set_key_byte_len(key_byte_len);
            request.set_value_byte_len(value_byte_len);

            request.set_key(get_string(key, key_byte_len));
            request.set_value(get_string(value, value_byte_len));

            rp4::InsertSramEntryResp response;
            ClientContext context;

            Status status = stub_->insertSramEntry(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        RC insert_tcam_entry(int proc_id, int matcher_id, uint8_t * key, uint8_t * mask,
                             uint8_t * value, int key_byte_len, int value_byte_len) {
            rp4::InsertTcamEntryReq request;
            request.set_procid(proc_id);
            request.set_matcherid(matcher_id);
            request.set_key_byte_len(key_byte_len);
            request.set_value_byte_len(value_byte_len);

            request.set_key(get_string(key, key_byte_len));
            request.set_mask(get_string(mask, key_byte_len));
            request.set_value(get_string(value, value_byte_len));

            rp4::InsertTcamEntryResp response;
            ClientContext context;

            Status status = stub_->insertTcamEntry(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        RC insert_action(int proc_id, int action_id, int para_num, std::vector<Primitive*> primitives,
                         std::vector<int> action_para_lens) {
//            std::cout << "in cfg insert_action" << std::endl;
            rp4::InsertActionReq request;
            request.set_procid(proc_id);
            request.set_actionid(action_id);
            auto action = request.mutable_action();
            action->set_paranum(para_num);

            for(auto val : action_para_lens) {
                action->add_actionparalens(val);
            }

            for(auto it : primitives) {
                auto prim = action->add_prims();

                auto lvalue = prim->mutable_lvalue();
                lvalue->set_hdrid(it->lvalue->hdr_id);
                lvalue->set_internaloffset(it->lvalue->internal_offset);
                lvalue->set_fieldlen(it->lvalue->fd_len);
                lvalue->set_fieldtype(get_field_type(it->lvalue->fd_type));

                auto rp4_root = prim->mutable_root();
                auto p_root = it->get_root();
                build_exp_tree(rp4_root, p_root);

            }
//            std::cout << "out cfg insert_action 1" << std::endl;

            rp4::InsertActionResp response;
            ClientContext context;

            Status status = stub_->insertAction(&context, request, &response);

//            std::cout << "out cfg insert_action 2" << std::endl;

            if(status.ok()) return 1;
            else return 0;

            return 0;
        }

        RC clear_action(int proc_id) {
            rp4::ClearActionReq request;
            request.set_procid(proc_id);

            rp4::ClearActionResp response;
            ClientContext context;

            Status status = stub_->clearAction(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
        }

        RC del_action(int proc_id, int action_id) {
            rp4::DelActionReq request;
            request.set_procid(proc_id);
            request.set_actionid(action_id);

            rp4::DelActionResp response;
            ClientContext context;

            Status status = stub_->delAction(&context, request, &response);

            if(status.ok()) return 1;
            else return 0;
            return 0;
        }

    private:
        static rp4::FieldType get_field_type(const FieldType & field_type) {
            switch(field_type) {
                case FieldType::VALID : return rp4::FieldType::VALID;
                case FieldType::FIELD : return rp4::FieldType::FD;
                case FieldType::HIT : return rp4::FieldType::HIT;
                case FieldType::MISS : return rp4::FieldType::MISS;
                default:
                    break;
            }
            return rp4::FieldType::FD;
        }

        static void set_gateway_param(rp4::GatewayParam * gp, GateParam & exp_gp) {
            if(exp_gp.type == ParameterType::FIELD) {
                gp->set_paramtype(rp4::ParameterType::FIELD);
                auto field = gp->mutable_field();
                field->set_fieldtype(get_field_type(exp_gp.field.fd_type));
                switch (exp_gp.field.fd_type) {
                    case FieldType::FIELD : {
                        field->set_hdrid(exp_gp.field.hdr_id);
                        field->set_internaloffset(exp_gp.field.internal_offset);
                        field->set_fieldlen(exp_gp.field.fd_len);
                        break;
                    }
                    case FieldType::VALID : {
                        field->set_hdrid(exp_gp.field.hdr_id);
                        break;
                    }
                    case FieldType::HIT : {
                        break;
                    }
                    case FieldType::MISS : {
                        break;
                    }
                    default:
                        break;
                }
            } else if (exp_gp.type == ParameterType::CONSTANT) {
                gp->set_paramtype(rp4::ParameterType::CONSTANT);
                rp4::Data * d = gp->mutable_data();
                d->set_len(exp_gp.constant_data.data_len);
                int len = ceil(exp_gp.constant_data.data_len * 1.0 / 8);
                char tmp[len];
                for(int i = 0; i < len; i++) {
                    tmp[i] = exp_gp.constant_data.val[i];
                }
                d->set_val(std::string(tmp, len));
            }
        }

        std::string get_string(uint8_t * key, int byte_len) {
//            LOG(INFO) << "byte_len: " << byte_len;
            char tmp[byte_len];
            for(int i = 0; i < byte_len; i++) {
                tmp[i] = key[i];
            }
            return std::string(tmp, byte_len);
        }

        static rp4::RelationCode get_relation_code (RelationCode rc) {
            switch (rc) {
                case RelationCode::GT : return rp4::RelationCode::GT;
                case RelationCode::GTE : return rp4::RelationCode::GTE;
                case RelationCode::LT : return rp4::RelationCode::LT;
                case RelationCode::LTE : return rp4::RelationCode::LTE;
                case RelationCode::EQ : return rp4::RelationCode::EQ;
                case RelationCode::NEQ : return rp4::RelationCode::NEQ;
                default:
                    break;
            }
        }

        rp4::ParameterType get_parameter_type (ParameterType pt) {
            switch (pt) {
                case ParameterType::FIELD : return rp4::ParameterType::FIELD;
                case ParameterType::CONSTANT : return rp4::ParameterType::CONSTANT;
                case ParameterType::PARAM : return rp4::ParameterType::PARAM;
                case ParameterType::OPERATOR : return rp4::ParameterType::OPERATOR;
                default:
                    break;
            }
        }

        static rp4::OpCode get_op_code(const OpCode & op) {
            switch (op) {
                case OpCode::ADD : return rp4::OpCode::ADD;
                case OpCode::SUB : return rp4::OpCode::SUB;
                case OpCode::SHIFT_LEFT : return rp4::OpCode::SHIFT_LEFT;
                case OpCode::SHIFT_RIGHT : return rp4::OpCode::SHIFT_RIGHT;
                case OpCode::BIT_AND : return rp4::OpCode::BIT_AND;
                case OpCode::BIT_OR : return rp4::OpCode::BIT_OR;
                case OpCode::BIT_XOR : return rp4::OpCode::BIT_XOR;
                case OpCode::BIT_NEG : return rp4::OpCode::BIT_NEG;
                case OpCode::SET_FIELD : return rp4::OpCode::SET_FIELD;
                case OpCode::COPY_FIELD : return rp4::OpCode::COPY_FIELD;
                default: break;
            }
        }

        void build_exp_tree (rp4::ExpTreeNode * rp4_node, ExpTreeNode * p_node) {
            auto rp4_opt = rp4_node->mutable_opt();
            rp4_opt->set_type(get_parameter_type(p_node->opt.type));
            switch (p_node->opt.type) {
                case ParameterType::FIELD : {
                    auto fd = rp4_opt->mutable_field();
                    fd->set_hdrid(p_node->opt.val.field.hdr_id);
                    fd->set_internaloffset(p_node->opt.val.field.internal_offset);
                    fd->set_fieldlen(p_node->opt.val.field.fd_len);
                    fd->set_fieldtype(get_field_type(p_node->opt.val.field.fd_type));

                    break;
                }
                case ParameterType::CONSTANT : {
                    int s_len = ceil(p_node->opt.val.constant_data.data_len * 1.0 / 8);
                    auto data = rp4_opt->mutable_constantdata();
                    data->set_len(p_node->opt.val.constant_data.data_len);
                    data->set_val(get_string(p_node->opt.val.constant_data.val, s_len));
                    break;
                }
                case ParameterType::PARAM : {
                    rp4_opt->set_actionparaid(p_node->opt.val.action_para_id);
                    break;
                }
                case ParameterType::OPERATOR : {
                    rp4_opt->set_op(get_op_code(p_node->opt.val.op));
                    break;
                }
                default:
                    break;
            }

            if(p_node->left != nullptr) {
//                std::cout << "here" << std::endl;
                auto left = rp4_node->mutable_left();
                build_exp_tree(left, p_node->left);
            }

            if(p_node->right != nullptr) {
                auto right = rp4_node->mutable_right();
                build_exp_tree(right, p_node->right);
            }
        }
    };
}

#endif //RECONF_SWITCH_IPSA_CONTROLLER_CLIENT_H
