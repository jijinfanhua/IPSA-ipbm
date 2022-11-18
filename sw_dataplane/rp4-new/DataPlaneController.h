//
// Created by xilinx_0 on 3/9/22.
//

#ifndef RP4_DATAPLANECONTROLLER2_H
#define RP4_DATAPLANECONTROLLER2_H

#include "../sw-src/dataplane/defs.h"
#include "../sw-src/dataplane/util/BitArray.h"
#include "json.hpp"
#include "../sw-src/api_pb/controller_to_dataplane.pb.h"
#include "../sw-src/api_pb/controller_to_dataplane.grpc.pb.h"
#include <string>
#include <fstream>
#include <grpcpp/security/credentials.h>
#include <grpcpp/create_channel.h>

using nlohmann::json;
using std::string;

class DataPlaneController {

public:

    std::unique_ptr<rp4::CfgService::Stub> stub;

//    ModTableController modTableController;

    void ExtractMetadata(const json& j) {
        rp4::SetMetadataReq setMetadataReq;
        for (const json& hdrInfo : j) {
            rp4::HeaderInfo* headerInfo = setMetadataReq.add_headerinfos();
            headerInfo->set_headerid(hdrInfo.at("id").get<int>());
            headerInfo->set_headerlength(hdrInfo.at("length").get<int>());
            headerInfo->set_headeroffset(hdrInfo.at("offset").get<int>());
        }
        grpc::ClientContext context;
        rp4::SetMetadataResponse response;
        stub->SetMetadata(&context, setMetadataReq, &response);
    }

    void SetNextProcessor(int procId, int nextProcId) {
        rp4::SetNextProcessorReq req;
        req.set_procid(procId);
        req.set_nextprocid(nextProcId);
        grpc::ClientContext context;
        rp4::SetNextProcessorResponse response;
        stub->SetNextProcessor(&context, req, &response);
    }



    std::unordered_map<string, rp4::FieldType> fieldTypeMap = {
            {"FIELD" , rp4::FieldType::FD},
            {"VALID" , rp4::FieldType::VALID},
            {"HIT"   , rp4::FieldType::HIT},
            {"MISS"  , rp4::FieldType::MISS}
    };

    std::unordered_map<string, rp4::RelationCode> relationCodeMap = {
            {"==" , rp4::RelationCode::EQ},
            {"!=" , rp4::RelationCode::NEQ},
            {">"  , rp4::RelationCode::GT},
            {">=" , rp4::RelationCode::GTE},
            {"<"  , rp4::RelationCode::LT},
            {"<=" , rp4::RelationCode::LTE}
    };

    std::unordered_map<string, rp4::GateEntryType> gateEntryTypeMap = {
            {"TABLE", rp4::GateEntryType::TABLE},
            {"STAGE", rp4::GateEntryType::STAGE}
    };
    std::unordered_map<string, rp4::ParameterType> parameterTypeMap = {
            {"CONST", rp4::ParameterType::CONSTANT},
            {"FIELD", rp4::ParameterType::FIELD},
            {"PARAM", rp4::ParameterType::PARAM},
            {"EXPR" , rp4::ParameterType::OPERATOR},
            {"HEADER", rp4::ParameterType::HEADER}
    };

    std::unordered_map<string, rp4::MatchType> matchTypeMap = {
            {"LPM"    , rp4::MatchType::LPM},
            {"TERNARY", rp4::MatchType::TERNARY},
            {"EXACT"  , rp4::MatchType::EXACT}
    };

    std::unordered_map<string, rp4::OpCode> opCodeMap = {
            {"+" , rp4::OpCode::ADD},
            {"-" , rp4::OpCode::SUB},
            {"&" , rp4::OpCode::BIT_AND},
            {"|" , rp4::OpCode::BIT_OR},
            {"~" , rp4::OpCode::BIT_NEG},
            {"^" , rp4::OpCode::BIT_XOR},
            {">>", rp4::OpCode::SHIFT_RIGHT},
            {"<<", rp4::OpCode::SHIFT_LEFT},
            {"PUSH", rp4::OpCode::PUSH},
            {"POP" , rp4::OpCode::POP}
    };


    rp4::Parameter* GetParameter(const json& j, rp4::Parameter* parameter) {
        rp4::ParameterType type = parameterTypeMap[j.at("type").get<string>()];
        parameter->set_type(type);
        auto& value = j.at("value");
        switch (type) {
            case rp4::ParameterType::CONSTANT:
            case rp4::ParameterType::HEADER:
            case rp4::ParameterType::PARAM: {
                parameter->set_content(value.get<int>());
                break;
            }
            case rp4::ParameterType::FIELD: {
                auto fieldInfo = parameter->mutable_field();
                fieldInfo->set_internaloffset ((uint8_t)       value.at("field_internal_offset").get<int>());
                fieldInfo->set_hdrid          ((uint8_t)       value.at("header_id")            .get<int>());
                fieldInfo->set_fieldlen       ((uint16_t)   value.at("field_length")         .get<int>());
                fieldInfo->set_fieldtype      (fieldTypeMap[value.at("type")                 .get<string>()]);
                break;
            }
            case rp4::ParameterType::OPERATOR: {
                auto Left = parameter->mutable_left();
                if (!value.at("left").is_null()) {
                    GetParameter(value.at("left"), Left);
                }

                auto Right = parameter->mutable_right();
                if (!value.at("right").is_null()) {
                    GetParameter(value.at("right"), Right);
                }
                parameter->set_op(opCodeMap[value.at("op").get<string>()]);
                break;
            }
        }
    }


    void ParserClear(int procId, int stageId) {
        rp4::ParserClearReq parserClearReq;
        parserClearReq.set_procid(procId);
        parserClearReq.set_stageid(stageId);
        grpc::ClientContext context;
        rp4::ParserClearResponse response;
        stub->ParserClear(&context, parserClearReq, &response);
    }

    void ExtractParser(int procId, int stageId, const json& j) {

        ParserClear(procId, stageId);
        rp4::ParserInitLevelReq parserInitLevelReq;

        parserInitLevelReq.set_procid(procId);
        parserInitLevelReq.set_stageid(stageId);
        parserInitLevelReq.set_level(j.at("parser_level").get<int>());

        grpc::ClientContext context1;
        rp4::ParserInitLevelResponse response1;
        stub->ParserInitLevel(&context1, parserInitLevelReq, &response1);

        int level_id = 0;
        for (auto &level_json: j.at("levels")) {
            for (auto &parser_entry_json: level_json) {

                rp4::ParserModifyEntryReq parserModifyEntryReq;
                parserModifyEntryReq.set_procid ( procId);
                parserModifyEntryReq.set_stageid( stageId);
                parserModifyEntryReq.set_level  (level_id);

                const auto& parserTcamEntry = parser_entry_json.at("parser_tcam_entry");
                const auto& parserSramEntry = parser_entry_json.at("parser_sram_entry");

                parserModifyEntryReq.set_state(parserTcamEntry.at("current_state").get<int>());
                parserModifyEntryReq.set_entry(parserTcamEntry.at("entry")        .get<int>());
                parserModifyEntryReq.set_mask (parserTcamEntry.at("mask")         .get<int>());

                parserModifyEntryReq.set_hdrid     (parserSramEntry.at("header_id")           .get<int>());
                parserModifyEntryReq.set_hdr_len   (parserSramEntry.at("header_length")       .get<int>());
                parserModifyEntryReq.set_next_state(parserSramEntry.at("next_state")          .get<int>());
                parserModifyEntryReq.set_field_num (parserSramEntry.at("transition_field_num").get<int>());
                parserModifyEntryReq.set_missaction(parserSramEntry.at("accept_drop")         .get<int>());

                for(const json & fdJson : parserSramEntry.at("transition_fields")) {
                    auto fieldInfo = parserModifyEntryReq.add_fields();
                    fieldInfo->set_internaloffset ((uint8_t)       fdJson.at("field_internal_offset").get<int>());
                    fieldInfo->set_hdrid          ((uint8_t)       fdJson.at("header_id")            .get<int>());
                    fieldInfo->set_fieldlen       ((uint16_t)   fdJson.at("field_length")         .get<int>());
                    fieldInfo->set_fieldtype      (fieldTypeMap[fdJson.at("type")                 .get<string>()]);
                }
                grpc::ClientContext context2;
                rp4::ParserModifyEntryResponse response2;
                stub->ParserModifyEntry(&context2, parserModifyEntryReq, &response2);
            }
            level_id++;
        }
    }

    void GatewayClear(int procId, int stageId) {
        rp4::GatewayClearResultMapReq req1;
        rp4::GatewayClearRelationExpressionReq req2;
        req1.set_procid(procId);
        req1.set_stageid(stageId);
        req2.set_procid(procId);
        req2.set_stageid(stageId);
        grpc::ClientContext context1;
        rp4::GatewayClearResultMapResponse response1;
        stub->GatewayClearResultMap(&context1, req1, &response1);
        grpc::ClientContext context2;
        rp4::GatewayClearRelationExpressionResponse response2;
        stub->GatewayClearRelationExpression(&context2, req2, &response2);
    }

    void ExtractGateway(int procId, int stageId, const json& j) {

        for (const auto &expression: j.at("expressions")) {
            const auto& e = expression.at("value");
//            rp4::RelationExpression *relationExpression = GetRelationExpression(expression.at("value"));
            rp4::GatewayInsertRelationExpressionReq gatewayInsertRelationExpressionReq;
            gatewayInsertRelationExpressionReq.set_relation(relationCodeMap[e.at("op")]);
            gatewayInsertRelationExpressionReq.set_procid(procId);
            gatewayInsertRelationExpressionReq.set_stageid(stageId);
            auto param1 = gatewayInsertRelationExpressionReq.mutable_param1();
            auto param2 = gatewayInsertRelationExpressionReq.mutable_param2();
            GetParameter(e.at("left") , param1);
            GetParameter(e.at("right"), param2);

            grpc::ClientContext context2;
            rp4::GatewayInsertRelationExpressionResponse response1;
            stub->GatewayInsertRelationExpression(&context2, gatewayInsertRelationExpressionReq, &response1);
        }
        rp4::GatewaySetDefaultGateEntryReq gatewaySetDefaultGateEntryReq;
        auto nextTable = j.at("next_table");
        auto defaultEntryJson = nextTable.at("default");

        gatewaySetDefaultGateEntryReq.set_type(gateEntryTypeMap[defaultEntryJson.at("type").get<std::string>()]);
        gatewaySetDefaultGateEntryReq.set_val(defaultEntryJson.at("value").get<int>());
        gatewaySetDefaultGateEntryReq.set_procid(procId);
        gatewaySetDefaultGateEntryReq.set_stageid(stageId);

        grpc::ClientContext context3;
        rp4::GatewaySetDefaultGateEntryResponse response2;
        stub->GatewaySetDefaultGateEntry(&context3, gatewaySetDefaultGateEntryReq, &response2);

        for (auto &entry: nextTable.at("entries")) {
            int bitmap = entry.at("bitmap").get<int>();
            auto value = entry.at("value");

            rp4::GatewayModifyResultMapReq gatewayModifyResultMapReq;
            gatewayModifyResultMapReq.set_procid(procId);
            gatewayModifyResultMapReq.set_stageid(stageId);
            gatewayModifyResultMapReq.set_key(bitmap);
            gatewayModifyResultMapReq.set_type(gateEntryTypeMap[value.at("type").get<string>()]);
            gatewayModifyResultMapReq.set_val(value.at("value").get<int>());

            grpc::ClientContext context4;
            rp4::GatewayModifyResultMapResponse response1;
            stub->GatewayModifyResultMap(&context4, gatewayModifyResultMapReq, &response1);
        }
    }


    string get_string(const bytes& b) {
        string res;
        for (auto x : b) {
            res.push_back((char)x);
        }
        return res;
    }

    void ExtractMatcher(int procId, int stageId, const json& j) {

        for (auto &mt: j) {

            int Id = mt.at("id").get<int>();
            rp4::MatcherClearReq matcherClearReq;
            matcherClearReq.set_procid(procId);
            matcherClearReq.set_stageid(stageId);
            matcherClearReq.set_matcherid(Id);

            grpc::ClientContext context0;
            rp4::MatcherClearResponse response0;
            stub->MatcherClear(&context0, matcherClearReq, &response0);

            int keyWidth   = mt.at("key_memory")  .at("width").get<int>();
            int valueWidth = mt.at("value_memory").at("width").get<int>();
            int depth      = mt.at("key_memory")  .at("depth").get<int>();
            auto type      = matchTypeMap[mt.at("match_type").get<string>()];
            bytes keyConfig(depth * keyWidth, 0);
            bytes valueConfig(depth * valueWidth, 0);
            int id = 0;
            for (auto& config : mt.at("key_memory").at("config")) {
                keyConfig[id] = config.get<int>();
                id++;
            }
            id = 0;
            for (auto& config : mt.at("value_memory").at("config")) {
                valueConfig[id] = config.get<int>();
                id++;
            }

            rp4::MatcherSetMemoryConfigReq matcherSetMemoryConfigReq;
            matcherSetMemoryConfigReq.set_stageid(stageId);
            matcherSetMemoryConfigReq.set_procid(procId);
            matcherSetMemoryConfigReq.set_matcherid(Id);
            matcherSetMemoryConfigReq.set_depth(depth);
            matcherSetMemoryConfigReq.set_keywidth(keyWidth);
            matcherSetMemoryConfigReq.set_valuewidth(valueWidth);
            matcherSetMemoryConfigReq.set_keyconfig(get_string(keyConfig));
            matcherSetMemoryConfigReq.set_valueconfig(get_string(valueConfig));
            matcherSetMemoryConfigReq.set_matchtype(type);
            grpc::ClientContext context;
            rp4::MatcherSetMemoryConfigResponse response1;
            stub->MatcherSetMemoryConfig(&context, matcherSetMemoryConfigReq, &response1);

            rp4::MatcherSetMissActionIdReq matcherSetMissActionIdReq;
            matcherSetMissActionIdReq.set_stageid(stageId);
            matcherSetMissActionIdReq.set_procid(procId);
            matcherSetMissActionIdReq.set_matcherid(Id);
            matcherSetMissActionIdReq.set_missactionid(mt.at("miss_action_id").get<int>());

            grpc::ClientContext context1;
            rp4::MatcherSetMissActionResponse response2;
            stub->MatcherSetMissActionId(&context1, matcherSetMissActionIdReq, &response2);

            rp4::MatcherSetNoTableReq matcherSetNoTableReq;
            matcherSetNoTableReq.set_stageid(stageId);
            matcherSetNoTableReq.set_procid(procId);
            matcherSetNoTableReq.set_matcherid(Id);
            matcherSetNoTableReq.set_notable(mt.at("no_table").get<int>());
            grpc::ClientContext context2;
            rp4::MatcherSetNoTableResponse response3;
            stub->MatcherSetNoTable(&context2, matcherSetNoTableReq, &response3);

            rp4::MatcherSetActionToStageMapReq matcherSetActionToStageMapReq;
            matcherSetActionToStageMapReq.set_stageid(stageId);
            matcherSetActionToStageMapReq.set_procid(procId);
            matcherSetActionToStageMapReq.set_matcherid(Id);
            for(const auto& ap : mt.at("action_to_proc")) {
                auto actionProcessor = matcherSetActionToStageMapReq.add_actiontostagemap();
                actionProcessor->set_actionid(ap.at("action_id").get<int>());
                actionProcessor->set_stageid(ap.at("stage_id").get<int>());
            }
            grpc::ClientContext context3;
            rp4::MatcherSetActionToStageMapResponse response4;
            stub->MatcherSetActionToStageMap(&context3, matcherSetActionToStageMapReq, &response4);

            rp4::MatcherSetFieldInfoReq matcherSetFieldInfoReq;
            matcherSetFieldInfoReq.set_procid(procId);
            matcherSetFieldInfoReq.set_stageid(stageId);
            matcherSetFieldInfoReq.set_matcherid(Id);

            for (const auto& fdInfo : mt.at("field_infos")) {
                auto fieldInfo = matcherSetFieldInfoReq.add_fields();
                fieldInfo->set_internaloffset ((uint8_t)       fdInfo.at("field_internal_offset").get<int>());
                fieldInfo->set_hdrid          ((uint8_t)       fdInfo.at("header_id")            .get<int>());
                fieldInfo->set_fieldlen       ((uint16_t)   fdInfo.at("field_length")         .get<int>());
                fieldInfo->set_fieldtype      (fieldTypeMap[fdInfo.at("type")                 .get<string>()]);
            }
            grpc::ClientContext context4;
            rp4::MatcherSetFieldInfoResponse response5;
            stub->MatcherSetFieldInfo(&context4, matcherSetFieldInfoReq, &response5);
        }

    }

    void ExecutorClear(int procId, int stageId) {
        rp4::ExecutorClearActionReq req;
        req.set_procid(procId);
        req.set_stageid(stageId);
        grpc::ClientContext context;
        rp4::ExecutorClearActionResponse response1;
        stub->ExecutorClearAction(&context, req, &response1);
    }


    void ExtractExecutor(int procId, int stageId, const json& j) {

        for (auto &ac: j) {
            rp4::ExecutorInsertActionReq executorInsertActionReq;


            for (auto &it: ac.at("action_parameters_lengths")) {
                executorInsertActionReq.add_actionparalens(it.get<int>());
            }
            for (auto &p: ac.at("primitives")) {
                auto primitive = executorInsertActionReq.add_primitives();
                auto lvalue = primitive->mutable_lvalue();
                auto rvalue = primitive->mutable_root();
                lvalue->set_internaloffset ((uint8_t)       p.at("lvalue").at("field_internal_offset").get<int>());
                lvalue->set_hdrid          ((uint8_t)       p.at("lvalue").at("header_id")            .get<int>());
                lvalue->set_fieldlen       ((uint16_t)   p.at("lvalue").at("field_length")         .get<int>());
                lvalue->set_fieldtype      (fieldTypeMap[p.at("lvalue").at("type")                 .get<string>()]);
                GetParameter(p.at("rvalue"), rvalue);
            }

            executorInsertActionReq.set_actionid(ac.at("id").get<int>());
            executorInsertActionReq.set_procid(procId);
            executorInsertActionReq.set_stageid(stageId);
            grpc::ClientContext context;
            rp4::ExecutorInsertActionResponse response1;
            stub->ExecutorInsertAction(&context, executorInsertActionReq, &response1);
        }

    }

    void InsertEntry(int procId, int stageId, int matcher_id, const bytes& in_key, const bytes& in_mask, const bytes& in_value) {
        rp4::MatcherInsertEntryReq req;
        req.set_matcherid(matcher_id);
        req.set_stageid(stageId);
        req.set_procid(procId);
        req.set_match_key(get_string(in_key));
        req.set_value(get_string(in_value));
        req.set_mask(get_string(in_mask));
        grpc::ClientContext context;
        rp4::MatcherInsertEntryResponse response;
        stub->MatcherInsertEntry(&context, req, &response);

    }

public:
    explicit DataPlaneController(int port) {
        auto insecureChannelCredentials = grpc::InsecureChannelCredentials();
        auto channel = grpc::CreateChannel("0.0.0.0:" + std::to_string(port), insecureChannelCredentials);
        stub = rp4::CfgService::NewStub(channel);
    }

    void TestInsertEntry(const string& entry_config_path) {

        json j;
        std::ifstream in(entry_config_path);
        in >> j;
        for (const auto& mt : j.at("entry")) {
            int procId = mt.at("proc_id").get<int>();
            int stageId = mt.at("stage_id").get<int>();
            int matcher_id = mt.at("matcher_id").get<int>();
            for (const auto& entry : mt.at("table")) {
                BitArray key, mask, value;
                for (const auto& key_j : entry.at("key")) {
                    BitArray tmp = ToBitArray(key_j);
                    tmp.Combine(key);
                    key = tmp;
//                    key.Print(2);
                }
                for (const auto& value_j : entry.at("value")) {
                    BitArray tmp = ToBitArray(value_j);
                    tmp.Combine(value);
                    value = tmp;
//                    value.Print(2);
                }
                if (entry.find("mask") != entry.end()) {
                    for (const auto& mask_j : entry.at("mask")) {
                        BitArray tmp = ToBitArray(mask_j);
                        tmp.Combine(mask);
                        mask = tmp;
//                        mask.Print(2);
                    }
                }
                key.Print(); mask.Print(); value.Print();
                InsertEntry(procId, stageId, matcher_id, key.Bytes(), mask.Bytes(), value.Bytes());
            }
        }
    }

    BitArray ToBitArray(const json& j) {
        vector<uint8_t> value;
        for (const auto& x : j.at("val")) {
            value.push_back(x.get<uint8_t>());
        } return BitArray((byte_ptr)&value[0], j.at("len").get<int>());
    }



    void Extract(const json& j) {
        ExtractMetadata(j.at("metadata"));
        rp4::SetStartStageReq req;
        req.set_processor(j.at("initial_processor").get<int>());
        req.set_stage(j.at("initial_stage").get<int>());
        grpc::ClientContext context;
        rp4::SetStartStageResponse response;
        stub->SetStartStage(&context, req, &response);
        for (int i = 0; i < 16; i++) {
            if (j.find("processor_" + std::to_string(i)) == j.end()) continue;
            const auto& processor = j.at("processor_" + std::to_string(i));
            if (processor.is_null()) continue;
            SetNextProcessor(i, processor.at("next_processor").get<int>());
            for (const auto& stage : processor.at("stage_list")) {
                const auto &parser = stage.at("parser");
                const auto &gateway = stage.at("gateway");
                const auto &matcher = stage.at("matcher");
                const auto &executor = stage.at("executor");
                int stageId = stage.at("id").get<int>(); // todo: need confirm.
                ParserClear    (i, stageId);
                ExtractParser  (i, stageId, parser);
                GatewayClear   (i, stageId);
                ExtractGateway (i, stageId, gateway);
                ExtractMatcher (i, stageId, matcher);
                ExecutorClear  (i, stageId);
                ExtractExecutor(i, stageId, executor);
            }
        }

    }
    void Execute() {
        rp4::ExecuteReq req;
        grpc::ClientContext context;
        rp4::ExecuteResponse response1;
        stub->Execute(&context, req, &response1);
    }

};

void insertConfig(int port, std::string input_filename);

void insertEntry(int port, std::string entry_path);

void InitializeController();
#endif //RP4_DATAPLANECONTROLLER2_H
