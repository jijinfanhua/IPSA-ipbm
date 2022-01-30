#pragma once

#include "api.h"
#include "api_pb/controller_dataplane_api.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include <iostream>
#include <memory>
#include <string>

namespace api {

// Logic and data behind the server's behavior.
class ModServiceImpl final : public rp4::ModService::Service {
    grpc::Status handleMatchTable(
            grpc::ServerContext *context, const rp4::FlowEntryModRequest *request,
            rp4::ReturnCodeResponse *response) override {
        RC rc;
        if (request->mod_type() == rp4::ADD) {
            auto src_flow = load_flow_entry(request->srcflow());
            rc = add_flow_entry(MEM_SRAM, request->flow_table_id(), src_flow); // TODO use mem type
        } else if (request->mod_type() == rp4::DELETE) {
            auto src_flow = load_flow_entry(request->srcflow());
            rc = del_flow_entry(MEM_SRAM, request->flow_table_id(), src_flow);
        } else if (request->mod_type() == rp4::MODIFY) {
            auto src_flow = load_flow_entry(request->srcflow());
            auto dst_flow = load_flow_entry(request->modflow());
            rc = mod_flow_entry(MEM_SRAM, request->flow_table_id(), src_flow, dst_flow);
        } else {
            return grpc::Status::CANCELLED;
        }
        response->set_return_code(rc);
        return grpc::Status::OK;
    }

    grpc::Status
    getFlowEntry(grpc::ServerContext *context, const rp4::FlowEntryRequest *request,
                 rp4::FlowEntry *response) override {
        auto query_flow = load_flow_entry(request->flow_entry());
        auto val = get_flow_entry_action_data(request->flow_table_id(), query_flow.key);
        response->add_action_data(val);
        return grpc::Status::OK;
    }

    grpc::Status addParserInfo(
            grpc::ServerContext *context, const rp4::ModParserInfo *request,
            rp4::ReturnCodeResponse *response) override {
        auto &pb_hdr = request->header();
        std::vector<NextParserHeader> next_table;
        for (auto &pb_next_hdr: pb_hdr.next_headers()) {
            NextParserHeader next_hdr(pb_next_hdr.tag(), pb_next_hdr.header_id());
            next_table.push_back(next_hdr);
        }
        ParserHeader hdr(pb_hdr.header_id(), pb_hdr.header_length(), pb_hdr.next_header_type_internal_offset(),
                         pb_hdr.next_header_type_length(), next_table);
        std::vector<int> prev_hdr_ids;
        for (auto &pb_prev_hdr_id: pb_hdr.previous_header_id()) {
            prev_hdr_ids.push_back(pb_prev_hdr_id);
        }
        add_parser_header(prev_hdr_ids, pb_hdr.header_tag(), hdr);
        response->set_return_code(0);
        return grpc::Status::OK;
    }

    grpc::Status addMatcherInfo(
            grpc::ServerContext *context, const rp4::ModMatcherInfo *request,
            rp4::ReturnCodeResponse *response) override {
        response->set_return_code(0);
        return grpc::Status::OK;
    }

    grpc::Status addActionInfo(
            grpc::ServerContext *context, const rp4::AddActionInfo *request,
            rp4::ReturnCodeResponse *response) override {
        Action action;
        auto &pb_action = request->action();
        for (auto &pb_prim: pb_action.primitives()) {
            Primitive prim;
            RC rc = parse_op(pb_prim, prim);
            if (rc) {
                response->set_return_code(rc);
                return grpc::Status::OK;
            }
            action.ops.push_back(prim);
        }
        set_action(pb_action.action_id(), action);
        response->set_return_code(0);
        return grpc::Status::OK;
    }

    grpc::Status addExecuterInfo(
            grpc::ServerContext *context, const rp4::AddExecuterInfo *request,
            rp4::ReturnCodeResponse *response) override {
        std::vector<RuntimeArgSpec> args_spec;
        for (auto &pb_arg_spec: request->args_spec()) {
            RuntimeArgSpec arg_spec(pb_arg_spec.internal_offset(), pb_arg_spec.field_length());
            args_spec.push_back(arg_spec);
        }
        set_executor(request->processor_id(), request->hit_action(), request->hit_next_proc(),
                     request->miss_action(), request->miss_next_proc(), args_spec);
        response->set_return_code(0);
        return grpc::Status::OK;
    }

    grpc::Status addFlowTable(
            grpc::ServerContext *context, const rp4::AddFlowTableInfo *request,
            rp4::ReturnCodeResponse *response) override {
        auto &pb_flow_table = request->flow_table();
        std::vector<FieldSpec> key_fields;
        for (auto &pb_key_field: pb_flow_table.key_fields()) {
            FieldSpec key_field(pb_key_field.header_id(), pb_key_field.internal_offset(),
                                pb_key_field.field_length());
            key_fields.push_back(key_field);
        }
        RC rc = set_flow_table(pb_flow_table.flow_table_id(), pb_flow_table.resided_logical_table_id(),
                               load_mem_type(pb_flow_table.mem_type()), load_match_type(pb_flow_table.match_type()),
                               key_fields, pb_flow_table.value_length(), pb_flow_table.is_counter());
        response->set_return_code(rc);
        return grpc::Status::OK;
    }

    grpc::Status addProcessor(grpc::ServerContext *context, const rp4::AddProcessorRequest *request,
                              rp4::ReturnCodeResponse *response) override {
        auto true_next = load_next_stage(request->true_next());
        auto false_next = load_next_stage(request->false_next());
        set_processor(request->proc_id(), Condition::is_valid(request->valid_hdr_id()),
                      true_next, false_next);
        response->set_return_code(0);
        return grpc::Status::OK;
    }

    grpc::Status addMetaData(
            grpc::ServerContext *context, const rp4::AddMetaDataRequest *request,
            rp4::ReturnCodeResponse *response) override {
        response->set_return_code(0);
        return grpc::Status::OK;
    }

    grpc::Status requestMemoryInfo(
            grpc::ServerContext *context, const rp4::MemoryInfoRequest *request, rp4::MemoryInfo *response) override {

        return grpc::Status::OK;
    }

    grpc::Status initSRAMandTCAM(
            grpc::ServerContext *context, const rp4::MemoryInfo *request, rp4::ReturnCodeResponse *response) override {
        response->set_return_code(0);
        return grpc::Status::OK;
    }

    grpc::Status initParser(
            grpc::ServerContext *context, const rp4::InitParserInfo *request,
            rp4::ReturnCodeResponse *response) override {
        response->set_return_code(0);
        return grpc::Status::OK;
    }

private:
    static NextStage load_next_stage(const rp4::NextStage &pb_next_stage) {
        if (pb_next_stage.is_table()) {
            return NextStage::table(pb_next_stage.next_id());
        } else {
            return NextStage::proc(pb_next_stage.next_id());
        }
    }

    static FlowEntry load_flow_entry(const rp4::FlowEntry &pb_entry) {
        FlowEntry entry;
        entry.key = pb_entry.key();
        entry.action_id = pb_entry.action_id();
        for (auto &pb_action_data: pb_entry.action_data()) {
            entry.action_data.push_back(pb_action_data);
        }
        return entry;
    }

    static MemType load_mem_type(rp4::MemType pb_type) {
        static std::map<rp4::MemType, MemType> m = {
                {rp4::SRAM, MEM_SRAM},
                {rp4::TCAM, MEM_TCAM},
        };
        return m.at(pb_type);
    }

    static MatchType load_match_type(rp4::MatchType pb_match_type) {
        static std::map<rp4::MatchType, MatchType> m = {
                {rp4::EXACT,   MATCH_EXACT},
                {rp4::TERNARY, MATCH_TERNARY},
                {rp4::LPM,     MATCH_LPM},
        };
        return m.at(pb_match_type);
    }

    static FieldSpec parse_param_field(const rp4::Primitive::Parameter &pb_param) {
        return {pb_param.prefix_id(), pb_param.suffix_id(), pb_param.val()};
    }

    static int parse_param_int(const rp4::Primitive::Parameter &pb_param) {
        return pb_param.val();
    }

    static RC parse_op(const rp4::Primitive &pb_prim, Primitive &prim) {
        int pb_op = pb_prim.op_id();
        if (pb_op == OP_CKSUM) {
            if (pb_prim.parameters_size() != 2) { return ARGUMENT_ERROR; }
            auto cksum_field = parse_param_field(pb_prim.parameters(0));
            auto dst_field = parse_param_field(pb_prim.parameters(1));
            prim = Primitive::cksum(cksum_field, dst_field, HASH_CSUM16);
        } else if (pb_op == OP_COPY_FIELD) {
            if (pb_prim.parameters_size() != 2) { return ARGUMENT_ERROR; }
            auto dst_field = parse_param_field(pb_prim.parameters(0));
            auto src_field = parse_param_field(pb_prim.parameters(1));
            prim = Primitive::copy_field(dst_field, src_field);
        } else if (pb_op == OP_ADD) {
            if (pb_prim.parameters_size() != 2) { return ARGUMENT_ERROR; }
            auto field = parse_param_field(pb_prim.parameters(0));
            auto delta = parse_param_int(pb_prim.parameters(1));
            prim = Primitive::add(field, delta);
        } else if (pb_op == OP_SET_PORT) {
            if (pb_prim.parameters_size() != 1) { return ARGUMENT_ERROR; }
            int port = parse_param_int(pb_prim.parameters(0));
            prim = Primitive::set_port_from_arg(port);
        } else if (pb_op == OP_SET_FIELD) {
            if (pb_prim.parameters_size() != 2) { return ARGUMENT_ERROR; }
            auto dst = parse_param_field(pb_prim.parameters(0));
            int src_arg_idx = parse_param_int(pb_prim.parameters(1));
            prim = Primitive::set_field(dst, src_arg_idx);
        } else if (pb_op == OP_RECIRCULATE) {
            if (pb_prim.parameters_size() != 0) { return ARGUMENT_ERROR; }
            prim = Primitive::recirculate();
        } else if (pb_op == OP_SET_MULTICAST) {
            if (pb_prim.parameters_size() != 0) { return ARGUMENT_ERROR; }
            prim = Primitive::set_multicast();
        } else {
            return OUT_OF_RANGE_ERROR;
        }
        return 0;
    }
};

static inline void RunServer() {
    std::string server_address("0.0.0.0:50051");
    ModServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

}
