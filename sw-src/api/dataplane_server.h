//
//
// Created by xilinx_0 on 1/14/22.
//

#ifndef RECONF_SWITCH_IPSA_DATAPLANE_SERVER_H
#define RECONF_SWITCH_IPSA_DATAPLANE_SERVER_H

#include "../api_pb/controller_to_dataplane.grpc.pb.h"

#include "../dataplane/pipeline.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include <google/protobuf/text_format.h>

#include <iostream>

//using namespace grpc;
//using namespace rp4;

namespace api {

    Pipeline *ppl;

class CfgServiceImpl final : public rp4::CfgService::Service {

        template<class T>
        static void print_request(const T &request) {
            std::string req_str;
            google::protobuf::TextFormat::PrintToString(request, &req_str);
            std::cout << "Requesting \n" << req_str << std::endl;
        }

        grpc::Status setMetadata (
                grpc::ServerContext *context, const rp4::SetMetadataReq *request,
                rp4::SetMetadataResp * response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            std::vector<HeaderInfo*> metas;
            for(auto & hdr : request->headerinfos()) {
                auto meta = new HeaderInfo {
                    static_cast<uint8_t>(hdr.headerid()),
                    static_cast<uint16_t>(hdr.headerlength()),
                    static_cast<uint16_t>(hdr.headeroffset())
                };
                metas.push_back(meta);
            }
            ppl->set_metadata_header(metas);
            return grpc::Status::OK;
        }

        grpc::Status initParserLevel(
                grpc::ServerContext *context, const rp4::ParserLevelReq *request,
                rp4::ParserLevelResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

//            std::cout << request->procid() << ":"
//                << request->parserlevel() << std::endl;

//            print_request(*request);

            ppl->init_parser_level(request->procid(), request->parserlevel());
            return grpc::Status::OK;
        }

        // add entry, delete entry, modify entry
        grpc::Status modParserEntry(grpc::ServerContext *context, const rp4::ModParserEntryReq *request,
                                    rp4::ModParserEntryResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }
            std::vector<FieldInfo*> _trans_fds;
            for(auto & trans_fd : request->transfds()) {
                auto fi = new FieldInfo{static_cast<uint8_t>(trans_fd.hdrid()),
                                        static_cast<uint16_t>(trans_fd.internaloffset()),
                                        static_cast<uint16_t>(trans_fd.fieldlen()),
                                        get_field_type(trans_fd.fieldtype())};
                _trans_fds.push_back(fi);
            }
            ppl->modify_parser_entry_direct(request->procid(), request->parserlevel(), request->state(),
                                            request->key(), request->mask(), request->hdrid(), request->hdrlen(),
                                            request->nextstate(), request->transfieldnum(), _trans_fds, request->miss_act());

            return grpc::Status::OK;
        }

        grpc::Status clearParser(grpc::ServerContext *context, const rp4::ClearParserReq *request,
                                    rp4::ClearParserResp * response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            std::cout << request->procid() << std::endl;
            ppl->clear_parser(request->procid());
            return grpc::Status::OK;
        }

        grpc::Status insertRelationExp(grpc::ServerContext *context, const rp4::InsertRelationExpReq *request,
                                       rp4::InsertRelationExpResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            LOG(INFO) << "Gateway relation insert: ";
            LOG(INFO) << "  processor id: " << request->procid();

            auto * exp = new RelationExp();

            set_exp_param(request->param1(), exp->param1);
            set_exp_param(request->param2(), exp->param2);

            exp->relation = get_relation_code(request->relation());
            LOG(INFO) << "  relation: " << (int)exp->relation;

            ppl->insert_exp(request->procid(), exp);

            return grpc::Status::OK;
        }

        grpc::Status clearRelationExp(grpc::ServerContext *context, const rp4::ClearRelationExpReq *request,
                                       rp4::ClearRelationExpResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            ppl->clear_exp(request->procid());

            return grpc::Status::OK;
        }

        grpc::Status clearResMap(grpc::ServerContext *context, const rp4::ClearResMapReq *request,
                                 rp4::ClearResMapResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            ppl->clear_res_map(request->procid());
            return grpc::Status::OK;
        }

        grpc::Status modResMap(grpc::ServerContext *context, const rp4::ModResMapReq *request,
                               rp4::ModResMapResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            ppl->modify_res_map(request->procid(), request->key(),
                                get_gate_entry_type(request->entry().type()), request->entry().val());

            return grpc::Status::OK;
        }

        grpc::Status setDefaultGateEntry(grpc::ServerContext *context, const rp4::SetDefaultGateEntryReq *request,
                                         rp4::SetDefaultGateEntryResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            ppl->set_default_entry(request->proc_id(),
                                   get_gate_entry_type(request->default_entry().type()),
                                   request->default_entry().val());
//            LOG(INFO) << "  default entry: ";
//            LOG(INFO) << "      type (stage, matcher): " << request->default_entry().type();
//            LOG(INFO) << "      value: " << request->default_entry().val();
            return grpc::Status::OK;
        }

        grpc::Status setActionProc(grpc::ServerContext *context, const rp4::SetActionProcReq *request,
                                   rp4::SetActionProcResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            std::unordered_map<int, int> _action_proc;
            for(const auto& it : request->actionprocs()) {
                _action_proc[it.actionid()] = it.procid();
            }
            ppl->set_action_proc_map(request->procid(),
                                     request->matcherid(),
                                     _action_proc);

            return grpc::Status::OK;
        }

        grpc::Status setMemConfig(grpc::ServerContext *context, const rp4::SetMemConfigReq *request,
                                   rp4::SetMemConfigResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }
            LOG(INFO) << "Matcher set mem config: ";

            std::string key_s = request->keyconfig();
            uint8_t key_i[key_s.length()];
            // bytes to uint8*
//            LOG(INFO) << "  key config: ";
            for(int i = 0; i < key_s.length(); i++) {
                key_i[i] = (uint8_t)key_s[i];
//                LOG(INFO) << "      " << (int)key_i[i];
            }

            std::string value_s = request->valueconfig();
            uint8_t value_i[value_s.length()];
//            LOG(INFO) << "  value config: ";
            // bytes to uint8*
            for(int i = 0; i < value_s.length(); i++) {
                value_i[i] = (uint8_t)value_s[i];
//                LOG(INFO) << "      " << (int)value_i[i];
            }

            ppl->clear_old_config(request->procid(), request->matcherid());
            ppl->init_match_type(request->procid(), request->matcherid(),
                                 get_match_type(request->matchtype()));
            ppl->set_mem_config(request->procid(), request->matcherid(),
                                request->keywidth(), request->valuewidth(), request->depth(),
                                key_i, value_i);
            ppl->set_miss_act_id(request->procid(), request->matcherid(), request->missactid());
            return grpc::Status::OK;
        }

        grpc::Status setFieldInfo(grpc::ServerContext *context, const rp4::SetFieldInfoReq *request,
                                  rp4::SetFieldInfoResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            std::vector<FieldInfo*> _fdInfos;
            LOG(INFO) << "  in processor: " << request->procid();
            LOG(INFO) << "  matcher id is: " << request->matcherid();
            LOG(INFO) << "  matching field num: " << request->fdinfos().size();
            for(const auto& it : request->fdinfos()) {
                auto fd = new FieldInfo{static_cast<uint8_t>(it.hdrid()),
                                    static_cast<uint16_t>(it.internaloffset()),
                                    static_cast<uint16_t>(it.fieldlen()),
                                    get_field_type(it.fieldtype())};
                LOG(INFO) << "      field: ";
                LOG(INFO) << "          header id: " << (int)fd->hdr_id;
                LOG(INFO) << "          internal offset: " << (int)fd->internal_offset;
                LOG(INFO) << "          field length: " << (int)fd->fd_len;
                LOG(INFO) << "          field type: " << (int)fd->fd_type;
                _fdInfos.push_back(fd);
            }
            ppl->set_field_infos(request->procid(), request->matcherid(), _fdInfos);
            return grpc::Status::OK;
        }

//        grpc::Status setHitMissBitmap(grpc::ServerContext *context, const rp4::SetHitMissBitmapReq *request,
//                                      rp4::SetHitMissBitmapResp *response) override {
//            if(ppl == nullptr) {
//                return grpc::Status::CANCELLED;
//            }
//            ppl->set_hit_miss_bitmap(request->procid(), request->matcherid(), request->value());
//            return grpc::Status::OK;
//        }

        grpc::Status setNoTable(grpc::ServerContext *context, const rp4::SetNoTableReq *request,
                                     rp4::SetNoTableResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            ppl->set_no_table(request->procid(), request->matcherid(), request->notable());
            return grpc::Status::OK;
        }

        grpc::Status clearOldConfig(grpc::ServerContext *context, const rp4::ClearOldConfigReq *request,
                                      rp4::ClearOldConfigResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }
            ppl->clear_old_config(request->procid(), request->matcherid());
            return grpc::Status::OK;
        }

//        grpc::Status setHitActId(grpc::ServerContext *context, const rp4::SetHitActIdReq *request,
//                                rp4::SetHitActIdResp *response) override {
//            if(ppl == nullptr) {
//                return grpc::Status::CANCELLED;
//            }
//
//            ppl->set_hit_act_id(request->procid(), request->matcherid(), request->hit_act_id());
//            return grpc::Status::OK;
//        }

        grpc::Status setMissActId(grpc::ServerContext *context, const rp4::SetMissActIdReq *request,
                                rp4::SetMissActIdResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            ppl->set_miss_act_id(request->procid(), request->matcherid(), request->miss_act_id());
            return grpc::Status::OK;
        }


        grpc::Status insertSramEntry(grpc::ServerContext *context, const rp4::InsertSramEntryReq *request,
                                    rp4::InsertSramEntryResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            uint8_t key_i[request->key_byte_len()];
            uint8_t value_i[request->value_byte_len()];
            get_uint_from_bytes(request->key(), key_i);
            get_uint_from_bytes(request->value(), value_i);
            // right padding for key, value in controller.
            ppl->insert_sram_entry(request->procid(),
                                   request->matcherid(),
                                   key_i, value_i,
                                   request->key_byte_len(),
                                   request->value_byte_len());

            return grpc::Status::OK;
        }

        grpc::Status insertTcamEntry(grpc::ServerContext *context, const rp4::InsertTcamEntryReq *request,
                                     rp4::InsertTcamEntryResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            uint8_t key_i[request->key_byte_len()];
            uint8_t value_i[request->value_byte_len()];
            get_uint_from_bytes(request->key(), key_i);
            get_uint_from_bytes(request->value(), value_i);

            uint8_t mask_i[request->mask().length()];
            get_uint_from_bytes(request->mask(), mask_i);

            ppl->insert_tcam_entry(request->procid(),
                                   request->matcherid(),
                                   key_i, mask_i, value_i,
                                   request->key_byte_len(),
                                   request->value_byte_len());

            return grpc::Status::OK;
        }

        grpc::Status insertAction(grpc::ServerContext *context, const rp4::InsertActionReq *request,
                                  rp4::InsertActionResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            LOG(INFO) << "Executor insert action: " << request->actionid();

            std::vector<Primitive*> _prims;
            int a = 0;
            for(const auto & it : request->action().prims()) {
                auto prim = new Primitive();
                LOG(INFO) << "  primitive: " << a++;
                LOG(INFO) << "      lvalue: ";
                prim->lvalue = new FieldInfo{static_cast<uint8_t>(it.lvalue().hdrid()),
                                            static_cast<uint16_t>(it.lvalue().internaloffset()),
                                            static_cast<uint16_t>(it.lvalue().fieldlen()),
                                            get_field_type(it.lvalue().fieldtype())};
                LOG(INFO) << "          header_id: " << (int)prim->lvalue->hdr_id;
                LOG(INFO) << "          internal_offset: " << (int)prim->lvalue->internal_offset;
                LOG(INFO) << "          field_length: " << (int)prim->lvalue->fd_len;
                LOG(INFO) << "          field_type(field, valid, hit, miss): " << (int)prim->lvalue->fd_type;

                LOG(INFO) << "      rvalue: ";
                int offset = 3;
                auto root = new ExpTreeNode();
                build_exp_tree(root, it.root(), offset);
                prim->set_root(root);
                _prims.push_back(prim);
            }

            std::vector<int> _action_para_lens;
            for(auto it : request->action().actionparalens()) {
                _action_para_lens.push_back(it);
            }

            auto ac = new Action();
            ac->prims = _prims;
            ac->para_num = request->action().paranum();
            ac->action_para_lens = _action_para_lens;

            ppl->insert_action(request->procid(), ac, request->actionid());
            return grpc::Status::OK;
        }

        grpc::Status clearAction(grpc::ServerContext *context, const rp4::ClearActionReq *request,
                                  rp4::ClearActionResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            ppl->clear_action(request->procid());
            return grpc::Status::OK;
        }

        grpc::Status delAction(grpc::ServerContext *context, const rp4::DelActionReq *request,
                                  rp4::DelActionResp *response) override {
            if(ppl == nullptr) {
                return grpc::Status::CANCELLED;
            }

            ppl->del_action(request->procid(), request->actionid());

            return grpc::Status::OK;
        }

private:
        static void get_uint_from_bytes(const std::basic_string<char> & bytes, uint8_t bytes_i[]) {
            std::string bytes_s = bytes;
            // bytes to uint8*
            for(int i = 0; i < bytes_s.length(); i++) {
                bytes_i[i] = (uint8_t)bytes_s[i];
            }
        }

        static FieldType get_field_type(const rp4::FieldType & field_type) {
            switch(field_type) {
                case rp4::FieldType::VALID : return FieldType::VALID;
                case rp4::FieldType::FD : return FieldType::FIELD;
                case rp4::FieldType::HIT : return FieldType::HIT;
                case rp4::FieldType::MISS : return FieldType::MISS;
                default:
                    break;
            }
            return FieldType::FIELD;
        }

        static void set_exp_param(const rp4::GatewayParam & gp, GateParam & exp_gp) {
            LOG(INFO) << "  param: ";
            LOG(INFO) << "      param type (field 0 or constant 2): " << gp.paramtype();
            if(gp.paramtype() == rp4::ParameterType::FIELD) {
                switch(gp.field().fieldtype()) {
                    case rp4::FieldType::FD : {
                        exp_gp.type = ParameterType::FIELD;
                        exp_gp.field = {static_cast<uint8_t>(gp.field().hdrid()),
                                             static_cast<uint16_t>(gp.field().internaloffset()),
                                             static_cast<uint16_t>(gp.field().fieldlen()),
                                             FieldType::FIELD};
                        break;
                    }
                    case rp4::FieldType::VALID : {
                        exp_gp.type = ParameterType::FIELD;
                        exp_gp.field = {static_cast<uint8_t>(gp.field().hdrid()),
                                             0,
                                             1,
                                             FieldType::VALID};
                        break;
                    }
                    case rp4::FieldType::HIT : {
                        exp_gp.type = ParameterType::FIELD;
                        exp_gp.field = {0,
                                             0,
                                             1,
                                             FieldType::HIT};
                        break;
                    }
                    case rp4::FieldType::MISS : {
                        exp_gp.type = ParameterType::FIELD;
                        exp_gp.field = {0,
                                             0,
                                             1,
                                             FieldType::MISS};
                        break;
                    }
                    default:
                        break;
                }
                LOG(INFO) << "          header id: " << (int)exp_gp.field.hdr_id;
                LOG(INFO) << "          internal offset: " << (int)exp_gp.field.internal_offset;
                LOG(INFO) << "          field length: " << (int)exp_gp.field.fd_len;
                LOG(INFO) << "          field type (field, valid, hit, miss): " << gp.field().fieldtype();
            } else if(gp.paramtype() == rp4::ParameterType::CONSTANT) {
                exp_gp.type = ParameterType::CONSTANT;
                exp_gp.constant_data.data_len = gp.data().len();
                std::string value_s = gp.data().val();
                int byte_len = int(exp_gp.constant_data.data_len * 1.0 / 8);
                uint8_t value_i[byte_len];
                exp_gp.constant_data.val = new uint8_t[byte_len];
                // bytes to uint8*
                LOG(INFO) << "          constant data length: " << gp.data().len();
                for(int i = 0; i < ceil(gp.data().len()*1.0/8); i++) {
                    value_i[i] = (uint8_t)value_s[i];
                    exp_gp.constant_data.val[i] = value_i[i];
                    LOG(INFO) << "              " << (int)value_i[i];
                }

            } else {

            }
        }

        static void build_exp_tree(ExpTreeNode * root, const rp4::ExpTreeNode& rp4_root, int offset) {
            std::string log_str = "";
            for(int i = 0; i < offset; i++) log_str+="\t";
            LOG(INFO) << log_str << "node operation(const, header, field, param, operator): " << rp4_root.opt().type();
            switch (rp4_root.opt().type()) {
                case rp4::ParameterType::FIELD : {
                    root->opt.type = ParameterType::FIELD;
                    root->opt.val.field = {static_cast<uint8_t>(rp4_root.opt().field().hdrid()),
                                           static_cast<uint16_t>(rp4_root.opt().field().internaloffset()),
                                           static_cast<uint16_t>(rp4_root.opt().field().fieldlen()),
                                           get_field_type(rp4_root.opt().field().fieldtype())};
//                    root->opt = {
//                            ParameterType::FIELD,
//                            {.field={static_cast<uint8_t>(rp4_root.opt().field().hdrid()),
//                                     static_cast<uint16_t>(rp4_root.opt().field().internaloffset()),
//                                     static_cast<uint16_t>(rp4_root.opt().field().fieldlen()),
//                                     get_field_type(rp4_root.opt().field().fieldtype())}}
//                    };
                    LOG(INFO) << log_str << "value: ";
                    LOG(INFO) << log_str << "   header_id: " << (int)root->opt.val.field.hdr_id;
                    LOG(INFO) << log_str << "   internal_offset: " << (int)root->opt.val.field.internal_offset;
                    LOG(INFO) << log_str << "   field_len: " << (int)root->opt.val.field.fd_len;
                    LOG(INFO) << log_str << "   field_type(field, valid, hit, miss): " << (int)root->opt.val.field.fd_type;
                    break;
                }
                case rp4::ParameterType::CONSTANT : {
                    int byte_len = (int)ceil(rp4_root.opt().constantdata().len() * 1.0 / 8);
                    auto val = new uint8_t[byte_len];
                    get_uint_from_bytes(rp4_root.opt().constantdata().val(), val);
                    root->opt.type = ParameterType::CONSTANT;
                    root->opt.val.constant_data.data_len = rp4_root.opt().constantdata().len();
                    root->opt.val.constant_data.val = new uint8_t[byte_len];
                    for(int i = 0; i < byte_len; i++) {
                        root->opt.val.constant_data.val[i] = val[i];
                    }

//                    root->opt.val.constant_data = {rp4_root.opt().constantdata().len(), val};
//                    root->opt = {
//                            ParameterType::CONSTANT,
//                            {.constant_data = {rp4_root.opt().constantdata().len(), val}}
//                    };
                    LOG(INFO) << log_str << "value: ";
                    LOG(INFO) << log_str << "   data_len: " << (int)root->opt.val.constant_data.data_len;
                    LOG(INFO) << log_str << "   data: ";
                    for(int i = 0; i < rp4_root.opt().constantdata().len() * 1.0 / 8; i++) {
                        LOG(INFO) << log_str << "       " << (int)val[i];
                    }
                    break;
                }
                case rp4::ParameterType::PARAM : {
                    root->opt.type = ParameterType::PARAM;
                    root->opt.val.action_para_id = rp4_root.opt().actionparaid();
//                    root->opt = {
//                            ParameterType::PARAM,
//                            {.action_para_id = rp4_root.opt().actionparaid()}
//                    };
                    LOG(INFO) << log_str << "value: ";
                    LOG(INFO) << log_str << "   action_para_id: " << root->opt.val.action_para_id;
                    break;
                }
                case rp4::ParameterType::OPERATOR : {
                    root->opt.type = ParameterType::OPERATOR;
                    root->opt.val.op = get_op_code(rp4_root.opt().op());
//                    root->opt = {
//                            ParameterType::OPERATOR,
//                            {.op = get_op_code(rp4_root.opt().op())}
//                    };
                    LOG(INFO) << log_str << "value: ";
                    LOG(INFO) << log_str << "   op(+ - << >> & | ^ ~): " << (int)root->opt.val.op;
                    break;
                }
                default:
                    break;
            }

            if(rp4_root.has_left()) {
//                std::cout << "here fengyong" << std::endl;
                LOG(INFO) << log_str << "left: ";
                auto left_exp = new ExpTreeNode();
                root->left = left_exp;
                build_exp_tree(left_exp, rp4_root.left(), offset+1);
            }
            if(rp4_root.has_right()) {
                LOG(INFO) << log_str << "right: ";
                auto right_exp = new ExpTreeNode();
                root->right = right_exp;
                build_exp_tree(right_exp, rp4_root.right(), offset+1);
            }
        }

        static RelationCode get_relation_code(const rp4::RelationCode & rc){
            switch(rc) {
                case rp4::RelationCode::GT : return RelationCode::GT;
                case rp4::RelationCode::GTE : return RelationCode::GTE;
                case rp4::RelationCode::LT : return RelationCode::LT;
                case rp4::RelationCode::LTE : return RelationCode::LTE;
                case rp4::RelationCode::EQ : return RelationCode::EQ;
                case rp4::RelationCode::NEQ : return RelationCode::NEQ;
                default:
                    break;
            }
            return RelationCode::EQ;
        }

        static MatchType get_match_type(const rp4::MatchType & mt) {
            switch (mt) {
                case rp4::MatchType::EXACT : return MatchType::EXACT;
                case rp4::MatchType::LPM : return MatchType::LPM;
                case rp4::MatchType::TERNARY : return MatchType::TERNARY;
                default:
                    break;
            }
            return MatchType::EXACT;
        }

        static OpCode get_op_code(const rp4::OpCode & op) {
            switch (op) {
                case rp4::OpCode::ADD : return OpCode::ADD;
                case rp4::OpCode::SUB : return OpCode::SUB;
                case rp4::OpCode::SHIFT_LEFT : return OpCode::SHIFT_LEFT;
                case rp4::OpCode::SHIFT_RIGHT : return OpCode::SHIFT_RIGHT;
                case rp4::OpCode::BIT_AND : return OpCode::BIT_AND;
                case rp4::OpCode::BIT_OR : return OpCode::BIT_OR;
                case rp4::OpCode::BIT_XOR : return OpCode::BIT_XOR;
                case rp4::OpCode::BIT_NEG : return OpCode::BIT_NEG;
                case rp4::OpCode::SET_FIELD : return OpCode::SET_FIELD;
                case rp4::OpCode::COPY_FIELD : return OpCode::COPY_FIELD;
                default: break;
            }
            return OpCode::ADD;
        }

        static ParameterType get_para_type(const rp4::ParameterType & pt) {
            switch (pt) {
                case rp4::ParameterType::CONSTANT: return ParameterType::CONSTANT;
                case rp4::ParameterType::HEADER: return ParameterType::HEADER;
                case rp4::ParameterType::FIELD: return ParameterType::FIELD;
                case rp4::ParameterType::PARAM: return ParameterType::PARAM;
                default: break;
            }
            return ParameterType::CONSTANT;
        }

        static GateEntryType get_gate_entry_type (rp4::GateEntryType get) {
            switch (get) {
                case rp4::GateEntryType::TABLE : return GateEntryType::TABLE;
                case rp4::GateEntryType::STAGE : return GateEntryType::STAGE;
                default : break;
            }
        }

    };

    static inline void RunServer(Pipeline *pipe) {
        ppl = pipe;

        std::string server_address("0.0.0.0:50051");
        CfgServiceImpl service;

        grpc::EnableDefaultHealthCheckService(true);
        grpc::reflection::InitProtoReflectionServerBuilderPlugin();
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);

        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "Server listening on " << server_address << std::endl;

        server->Wait();
    }

}
#endif //RECONF_SWITCH_IPSA_DATAPLANE_SERVER_H
