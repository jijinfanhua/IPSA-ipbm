//
// Created by fengyong on 2021/2/4.
//

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "controller_dataplane_api.grpc.pb.h"
#include "controller_dataplane_api.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using rp4::ModService;
using rp4::FlowEntry;
using rp4::FlowEntryModResponse;
using rp4::FlowEntryModRequest;
using rp4::MemType;
using rp4::FlowEntryModType;
using rp4::FlowEntryModRes;
using rp4::FlowTable;

using rp4::ModParserInfo;
using rp4::ModParserInfoResponse;
using rp4::ParserModRes;
using rp4::Header;
using rp4::Field;
using rp4::NextHeader;

using rp4::ModMatcherInfo;
using rp4::ModMatcherInfoResponse;
using rp4::MatcherModRes;
using rp4::MatchField;

using rp4::AddActionInfo;
using rp4::AddActionInfoResponse;
using rp4::Primitive;
using rp4::ExecuterAddRes;

using rp4::AddFlowTableInfo;
using rp4::AddFlowTableInfoResponse;
using rp4::AddFlowTableRes;
using rp4::ActionAddRes;
using rp4::Action;

using rp4::AddExecuterInfo;
using rp4::AddExecuterInfoResponse;

using rp4::MemoryInfoRequest;
using rp4::MemoryInfo;

using rp4::InitMemRes;
using rp4::InitMemResponse;
using rp4::LogicalTableVector;
using rp4::LogicalTable;
using rp4::FlowTable;

using rp4::InitParserInfo;
using rp4::InitParserInfoResponse;
using rp4::InitParserRes;

using google::protobuf::RepeatedPtrField;
using google::protobuf::RepeatedField;
using google::protobuf::int32;

using rp4::MatchType;
using rp4::ReturnCodeResponse;

using std::chrono::system_clock;

int initParserInfo(const RepeatedPtrField<Header>& headers) {
//    std::cout << "****** header nums: " << headers.size() << " ******" << std::endl;
//    for(const auto& header: headers) {
//        std::cout << header.header_id() << ": ";
//        std::cout << header.header_length() << " ";
//        std::cout << header.header_tag() << std::endl;
//    }
    return 1;
}

int initMem(int sram_num, int tcam_num, int bit_per_sram, int32 bit_per_tcam,
                   const google::protobuf::Map<int, rp4::LogicalTableVector> sram_map,
                    const google::protobuf::Map<int, rp4::LogicalTable> tcam_map) {
//    std::cout << "*********** SRAM and TCAM ************" << std::endl;
//    std::cout << "SRAM num: " << sram_num << std::endl;
//    for(const auto& it : sram_map) {
//        auto lts = it.second;
//        std::cout << "+++++++++++++ SRAM " << it.first << " +++++++++++++" << std::endl;
//        for(const auto& ele : lts.logical_tables()) {
//            std::cout << "-------------- LogicalTable " << ele.logical_table_id() << " -----------" << std::endl;
//            std::cout << "resided_hardware_id: " << ele.resided_hardware_id() << std::endl;
//            std::cout << "supported_max_length: " << ele.supported_max_length() << std::endl;
//            std::cout << "supported_max_num: " << ele.supported_max_num() << std::endl;
//
//            auto fts = ele.inside_flow_tables();
//            for(const auto& ft : fts) {
//                std::cout << "\tflow_table: " << ft.flow_table_id() << std::endl;
//            }
//        }
//    }
//
//
//    std::cout << "TCAM num: " << tcam_num << std::endl;
//
//    std::cout << "SRAMs: " << sram_map.size() << std::endl;
//    std::cout << "TCAMs: " << tcam_map.size() << std::endl;
    return 1;
}

int modifyFlowEntry(/*MemType memType, */ uint32_t tableId, const FlowEntry* srcFlow,
                                FlowEntryModType modType, const FlowEntry* modFlow){

    if(modType == FlowEntryModType::ADD) {
        std::cout << "add entry: " << srcFlow->key() << std::endl;
        std::cout << "mask is: " << srcFlow->mask() << std::endl;
        std::cout << "action_id is: " << srcFlow->action_id() << std::endl;
        return 1;
    } else if(modType == FlowEntryModType::MODIFY) {
        std::cout << "src entry is: " << srcFlow->key() << "; mod entry is: " << modFlow->key() << std::endl;
        return 2;
    } else if(modType == FlowEntryModType::DELETE) {
        std::cout << "delete entry is: " << srcFlow->key() << std::endl;
        return 3;
    }
    // TODO: flow entry modification logic

    return 1;
}

int addParserProtocol(const Header& header){
    std::cout << "add ok!" << std::endl;
    // TODO: add parser protocol
//    std::cout << "header_id: " << header.header_id() << std::endl;
//    std::cout << "header_tag:" << header.header_tag() << std::endl;
//    std::cout << "previous_header_ids:" << std::endl;
//    for(auto it : header.previous_header_id()) {
//        std::cout << "\t" << it << std::endl;
//    }
//    std::cout << "fields:" << std::endl;
//    for(auto & it : header.fields()) {
//        std::cout << "\tfield_no: " << it.field_id() << std::endl;
//        std::cout << "\t\tfield_length: " << it.field_length() << std::endl;
//        std::cout << "\t\tinternal_offset: " << it.internal_offset() << std::endl;
//    }

//    std::cout << "field num: " << fields.size() << std::endl;
//    std::cout << "field0.id: " << fields.at(0).field_id() << std::endl;
    return 1;
}

int addMatcher(uint32_t processor_id,
                         const RepeatedPtrField<MatchField>& match_fields,
                         uint32_t table_id) {

    // TODO: add matcher info
    std::cout << "add ok!" << std::endl;
    return 1;
}

int addAction(uint32_t processor_id, const Action &action) {
    std::cout << "add ok!" << std::endl;
    return 1;
}

//(info->flow_table_id(), info->mem_type(), info->residing_pool_table_id(), info->key_length(),
//                            info->value_length(), info->action_data_num(), info->match_type(), info->mem_type());
int addNewFlowTable(const FlowTable& flow_table) {

    std::cout << "flow table id: " << flow_table.flow_table_id() << std::endl;
    std::cout << "resided logical table id: " << flow_table.resided_logical_table_id() << std::endl;
//    std::cout << "key length: " << flow_table.key_length() << std::endl;
    std::cout << "value_length:" << flow_table.value_length() << std::endl;

    return 1;
}

class ModServiceImpl final : public ModService::Service{
    /**
     * add/modify/delete flow entry
     * @return
     */
    Status handleMatchTable(ServerContext* context, const FlowEntryModRequest* request,
                            ReturnCodeResponse* response) override {
        request->srcflow();
        int res = modifyFlowEntry(/*request->mem_type(), */ request->flow_table_id(), &(request->srcflow()),
                                             request->mod_type(), &(request->modflow()));
        response->set_return_code(res);
        return Status::OK;
    }

    /**
     * add protocol in parser
     * @return
     */
    Status addParserInfo(ServerContext* context, const ModParserInfo* info,
                         ReturnCodeResponse* response) override {

        int res = addParserProtocol(info->header());
        response->set_return_code(res);
        return Status::OK;
    }

    /**
     * add matcher info
     * @return
     */
    Status addMatcherInfo(ServerContext* context, const ModMatcherInfo* info,
                          ReturnCodeResponse* response) override {

        int res = addMatcher(info->processor_id(), info->match_fields(),
                                            info->table_id());
        response->set_return_code(res);
        return Status::OK;
    }

    Status addActionInfo(ServerContext* context, const AddActionInfo* info,
                         ReturnCodeResponse* response) override {

        int res = addAction(info->processor_id(), info->action());
        response->set_return_code(res);
        return Status::OK;
    }

    Status addExecuterInfo(ServerContext* context, const AddExecuterInfo* info,
                           ReturnCodeResponse* response) override {


        return Status::OK;
    }

    Status addFlowTable(ServerContext* context, const AddFlowTableInfo* info,
                        ReturnCodeResponse* response) override {

        addNewFlowTable(info->flow_table());
        response->set_return_code(1);
        return Status::OK;
    }

    Status requestMemoryInfo(ServerContext* context, const MemoryInfoRequest* info,
                             MemoryInfo* response) override {

        return Status::OK;
    }

    Status initSRAMandTCAM(ServerContext* context, const MemoryInfo* info,
                           ReturnCodeResponse* response) override {
        initMem(info->sram_num(), info->tcam_num(), info->bit_per_sram(), info->bit_per_tcam(),
                info->sram_memory_info(), info->tcam_memory_info());
        return Status::OK;
    }

    Status initParser(ServerContext* context, const InitParserInfo* info,
                      ReturnCodeResponse* response) override {
        initParserInfo(info->headers());
        return Status::OK;
    }

};

void RunServer(){
    std::string server_address("127.0.0.1:50051");
    ModServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char ** argv) {
    RunServer();
    return 0;
}

