#pragma once

#include "api.h"
#include "api_pb/controller_dataplane_api.grpc.pb.h"

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <google/protobuf/text_format.h>

namespace api {

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class ModClient {
public:

    ModClient(std::shared_ptr<Channel> channel)
            : stub_(rp4::ModService::NewStub(channel)) {}

    template<class T>
    static void print_request(const T &request) {
        std::string req_str;
        google::protobuf::TextFormat::PrintToString(request, &req_str);
        std::cout << "Requesting \n" << req_str << std::endl;
    }

    RC add_flow_entry(int flow_table_id, const FlowEntry &flow) {
        // Data we are sending to the server.
        rp4::FlowEntryModRequest request;
        request.set_flow_table_id(flow_table_id);
        request.set_mod_type(dump_mod_type(MOD_INSERT));
        request.set_allocated_srcflow(dump_flow_entry(flow));

        print_request(request);

        // Container for the data we expect from the server.
        rp4::ReturnCodeResponse response;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->handleMatchTable(&context, request, &response);

        // Act upon its status.
        if (status.ok()) {
            return response.return_code();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return RPC_FAIL;
        }
    }

    RC del_flow_entry(int flow_table_id, const FlowEntry &flow) {
        // Data we are sending to the server.
        rp4::FlowEntryModRequest request;
        request.set_flow_table_id(flow_table_id);
        request.set_mod_type(dump_mod_type(MOD_DELETE));
        request.set_allocated_srcflow(dump_flow_entry(flow));

        print_request(request);

        // Container for the data we expect from the server.
        rp4::ReturnCodeResponse response;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->handleMatchTable(&context, request, &response);

        // Act upon its status.
        if (status.ok()) {
            return response.return_code();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return RPC_FAIL;
        }
    }

    RC mod_flow_entry(int flow_table_id, const FlowEntry &src_flow, const FlowEntry &dst_flow) {
        // Data we are sending to the server.
        rp4::FlowEntryModRequest request;
        request.set_flow_table_id(flow_table_id);
        request.set_mod_type(dump_mod_type(MOD_UPDATE));
        request.set_allocated_srcflow(dump_flow_entry(src_flow));
        request.set_allocated_modflow(dump_flow_entry(dst_flow));

        print_request(request);

        // Container for the data we expect from the server.
        rp4::ReturnCodeResponse response;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->handleMatchTable(&context, request, &response);

        // Act upon its status.
        if (status.ok()) {
            return response.return_code();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return RPC_FAIL;
        }
    }

    std::string get_flow_entry_value(int table_id, const std::string &key) {
        rp4::FlowEntryRequest request;
        FlowEntry flow;
        flow.key = key;
        request.set_flow_table_id(table_id);
        request.set_allocated_flow_entry(dump_flow_entry(flow));

        print_request(request);

        rp4::FlowEntry response;

        ClientContext context;
        Status status = stub_->getFlowEntry(&context, request, &response);

        if (status.ok()) {
            return response.action_data(0);
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return "";
        }
    }

    RC add_parser_header(const std::vector<int> &prev_hdr_ids, const std::string &hdr_tag,
                         const ParserHeader &hdr) {
        rp4::ModParserInfo request;
        auto pb_hdr = new rp4::Header;
        pb_hdr->set_header_id(hdr.hdr_id);
        for (auto &prev_hdr_id: prev_hdr_ids) {
            pb_hdr->add_previous_header_id(prev_hdr_id);
        }
        pb_hdr->set_header_tag(hdr_tag);
        pb_hdr->set_header_length(hdr.hdr_len);
        pb_hdr->set_next_header_type_internal_offset(hdr.next_hdr_type_start);
        pb_hdr->set_next_header_type_length(hdr.next_hdr_type_len);
        // TODO: pb_hdr->fields
        for (auto &next_hdr: hdr.next_table) {
            auto pb_next_hdr = pb_hdr->add_next_headers();
            pb_next_hdr->set_tag(next_hdr.tag);
            pb_next_hdr->set_header_id(next_hdr.hdr_id);
        }
        request.set_allocated_header(pb_hdr);

        print_request(request);

        rp4::ReturnCodeResponse response;

        ClientContext context;

        Status status = stub_->addParserInfo(&context, request, &response);

        if (status.ok()) {
            return response.return_code();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return RPC_FAIL;
        }
    }

    RC set_action(int action_id, const Action &action) {
        rp4::AddActionInfo request;
        auto pb_action = new rp4::Action;
        pb_action->set_action_id(action_id);
        for (auto &prim: action.ops) {
            auto pb_prim = pb_action->add_primitives();
            RC rc = dump_primitive(prim, *pb_prim);
            if (rc) { return rc; }
        }
        request.set_allocated_action(pb_action);

        print_request(request);

        rp4::ReturnCodeResponse response;

        ClientContext context;

        Status status = stub_->addActionInfo(&context, request, &response);

        if (status.ok()) {
            return response.return_code();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return RPC_FAIL;
        }
    }

    RC set_executor(int proc_id, int hit_action, int hit_next_proc,
                    int miss_action, int miss_next_proc, const std::vector<RuntimeArgSpec> &runtime_args_spec) {
        rp4::AddExecuterInfo request;
        request.set_processor_id(proc_id);
        request.set_hit_action(hit_action);
        request.set_hit_next_proc(hit_next_proc);
        request.set_miss_action(miss_action);
        request.set_miss_next_proc(miss_next_proc);
        for (auto &arg_spec: runtime_args_spec) {
            auto pb_arg_spec = request.add_args_spec();
            pb_arg_spec->set_internal_offset(arg_spec.val_offset);
            pb_arg_spec->set_field_length(arg_spec.len);
        }

        print_request(request);

        rp4::ReturnCodeResponse response;

        ClientContext context;

        Status status = stub_->addExecuterInfo(&context, request, &response);

        if (status.ok()) {
            return response.return_code();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return RPC_FAIL;
        }
    }

    RC set_flow_table(int flow_table_id, int logic_table_id, MemType mem_type, MatchType match_type,
                      const std::vector<FieldSpec> &key_fields, int val_len, bool is_counter) {
        rp4::AddFlowTableInfo request;
        auto pb_flow_table = new rp4::FlowTable;
        pb_flow_table->set_flow_table_id(flow_table_id);
        pb_flow_table->set_match_type(dump_match_type(match_type));
        pb_flow_table->set_mem_type(dump_mem_type(mem_type));
        pb_flow_table->set_resided_logical_table_id(logic_table_id);
        for (auto &key_field: key_fields) {
            auto pb_field = pb_flow_table->add_key_fields();
            *pb_field = dump_field(key_field);
        }
        pb_flow_table->set_value_length(val_len);
        pb_flow_table->set_is_counter(is_counter);
        request.set_allocated_flow_table(pb_flow_table);

        print_request(request);

        rp4::ReturnCodeResponse response;

        ClientContext context;

        Status status = stub_->addFlowTable(&context, request, &response);

        if (status.ok()) {
            return response.return_code();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return RPC_FAIL;
        }
    }

    RC set_processor(int proc_id, const Condition &cond, const NextStage &true_next, const NextStage &false_next) {
        rp4::AddProcessorRequest request;
        request.set_proc_id(proc_id);
        request.set_valid_hdr_id(cond.args.is_valid.hdr_id);
        request.set_allocated_true_next(dump_next_stage(true_next));
        request.set_allocated_false_next(dump_next_stage(false_next));

        print_request(request);

        rp4::ReturnCodeResponse response;

        ClientContext context;

        Status status = stub_->addProcessor(&context, request, &response);

        if (status.ok()) {
            return response.return_code();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            std::cout << "RPC failed" << std::endl;
            return RPC_FAIL;
        }
    }

private:
    std::unique_ptr<rp4::ModService::Stub> stub_;

private:
    static rp4::NextStage *dump_next_stage(const NextStage &next_stage) {
        auto pb_next_stage = new rp4::NextStage();
        pb_next_stage->set_is_table(next_stage.is_table);
        if (next_stage.is_table) {
            pb_next_stage->set_next_id(next_stage.table_id);
        } else {
            pb_next_stage->set_next_id(next_stage.proc_id);
        }
        return pb_next_stage;
    }

    static rp4::FlowEntryModType dump_mod_type(ModType mod_type) {
        static std::map<ModType, rp4::FlowEntryModType> m = {
                {MOD_INSERT, rp4::ADD},
                {MOD_DELETE, rp4::DELETE},
                {MOD_UPDATE, rp4::MODIFY},
        };
        return m.at(mod_type);
    }

    static rp4::FlowEntry *dump_flow_entry(const FlowEntry &flow_entry) {
        auto flow_entry_pb = new rp4::FlowEntry();
        flow_entry_pb->set_key(flow_entry.key);
//        flow_entry_pb.set_mask(flow_entry)    // TODO
        flow_entry_pb->set_action_id(flow_entry.action_id);
        for (auto &action_data: flow_entry.action_data) {
            flow_entry_pb->add_action_data(action_data);
        }
        return flow_entry_pb;
    }

    static rp4::Field dump_field(const FieldSpec &field) {
        rp4::Field pb_field;
        pb_field.set_header_id(field.hdr_id);
        pb_field.set_field_length(field.len);
        pb_field.set_internal_offset(field.start);
        return pb_field;
    }

    static rp4::MatchType dump_match_type(MatchType match_type) {
        static std::map<MatchType, rp4::MatchType> m = {
                {MATCH_EXACT,   rp4::EXACT},
                {MATCH_LPM,     rp4::LPM},
                {MATCH_TERNARY, rp4::TERNARY},
        };
        return m.at(match_type);
    }

    static rp4::MemType dump_mem_type(MemType mem_type) {
        static std::map<MemType, rp4::MemType> m = {
                {MEM_SRAM, rp4::SRAM},
                {MEM_TCAM, rp4::TCAM},
        };
        return m.at(mem_type);
    }

    static rp4::Primitive::Parameter dump_param_field(const FieldSpec &field) {
        rp4::Primitive::Parameter param;
        param.set_prefix_id(field.hdr_id);
        param.set_suffix_id(field.start);
        param.set_val(field.len);
        return param;
    }

    static rp4::Primitive::Parameter dump_param_int(int val) {
        rp4::Primitive::Parameter param;
        param.set_val(val);
        return param;
    }

    static RC dump_primitive(const Primitive &prim, rp4::Primitive &pb_prim) {
        pb_prim.set_op_id(prim.type);
        if (prim.type == OP_CKSUM) {
            auto pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_field(prim.args_cksum.cksum_field);
            pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_field(prim.args_cksum.dst_field);
        } else if (prim.type == OP_COPY_FIELD) {
            auto pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_field(prim.args_copy_field.dst);
            pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_field(prim.args_copy_field.src);
        } else if (prim.type == OP_ADD) {
            auto pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_field(prim.args_add.field);
            pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_int(prim.args_add.delta);
        } else if (prim.type == OP_SET_PORT) {
            auto pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_int(prim.args_set_port.arg_idx);
        } else if (prim.type == OP_SET_FIELD) {
            auto pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_field(prim.args_set_field.dst);
            pb_param = pb_prim.add_parameters();
            *pb_param = dump_param_int(prim.args_set_field.arg_src);
        } else if (prim.type == OP_RECIRCULATE) {
            // no arg, pass
        } else if (prim.type == OP_SET_MULTICAST) {
            // no arg, pass
        } else {
            return OUT_OF_RANGE_ERROR;
        }
        return 0;
    }
};

}
