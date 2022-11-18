//
// Created by fengyong on 2021/2/4.
//

#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <unistd.h>
#include <getopt.h>
#include <algorithm>
#include <utility>

#include "controller_dataplane_api.grpc.pb.h"

#include "include/mem_config.h"
#include "include/parser.h"
#include "include/matcher.h"
#include "include/executer_config.h"
#include "include/metadata.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using rp4::ModService;
using rp4_FlowTable = rp4::FlowTable;
using rp4::FlowEntry;
using rp4::FlowEntryModResponse;
using rp4::FlowEntryModRequest;
using rp4_MemType = rp4::MemType;
using rp4_MatchType = rp4::MatchType;
using rp4::FlowEntryModType;
using rp4::FlowEntryModRes;

using rp4::ModParserInfo;
using rp4::ModParserInfoResponse;
using rp4::ParserModRes;
using rp4_Header = rp4::Header;
using rp4_Field = rp4::Field;
using rp4_NextHeader = rp4::NextHeader;

using rp4::ModMatcherInfo;
using rp4::ModMatcherInfoResponse;
using rp4::MatcherModRes;
using rp4_MatchField = rp4::MatchField;

using rp4::AddActionInfo;
using rp4::AddActionInfoResponse;
using rp4_primitive = rp4::Primitive;
using rp4::ExecuterAddRes;

using rp4::AddFlowTableInfo;
using rp4::AddFlowTableInfoResponse;
using rp4::AddFlowTableRes;

using rp4::InitParserInfo;
using rp4::InitParserRes;
using rp4::InitParserInfoResponse;

using rp4::MemoryInfo;
using rp4::LogicalTableVector;
using rp4::InitMemRes;
using rp4::InitMemResponse;
using rp4_LogicalTable = rp4::LogicalTable;

using rp4_Action = rp4::Action;
using rp4_Parameter = rp4::Primitive::Parameter;

using rp4::ReturnCodeResponse;
using rp4_Primitive = rp4::Primitive;

using std::chrono::system_clock;

class ModFlowEntryClient {
public:
    explicit ModFlowEntryClient(std::shared_ptr<Channel> channel)
                                : stub_(ModService::NewStub(channel)){

    }

    void initParser(Parser *parser);

    void initMem(MemConfig * mem_config);

    void handleMatchTable(std::vector<std::string> tokens, FlowEntryModType mod_type, Parser* parser, MemConfig *mem_config, MatcherConfig* matcher,
                          ExecuterConfig* executer_config, Processor* processor, Metadata* metadata);
    void addParserInfo(std::vector<std::string> tokens, Parser* parser,
                       MemConfig *mem_config, MatcherConfig* matcher,
                       ExecuterConfig* executer_config, Processor* processor,
                       Metadata* metadata);
    void addFlowTableInfo(std::vector<std::string> tokens, Parser* parser, MemConfig *mem_config, MatcherConfig* matcher,
                          ExecuterConfig* executer_config, Processor* processor, Metadata* metadata);
    void addMatcher(std::vector<std::string> tokens, Parser* parser, MemConfig *mem_config, MatcherConfig* matcher,
                    ExecuterConfig* executer_config, Processor* processor, Metadata* metadata);
    void addAction(std::vector<std::string> tokens, Parser* parser, MemConfig *mem_config, MatcherConfig* matcher,
                   ExecuterConfig* executer_config, Processor* processor, Metadata* metadata) ;

private:
    std::unique_ptr<ModService::Stub> stub_;
    bool initParserInfo(InitParserInfo * parser_info);
    bool initMemInfo(MemoryInfo* mem_info);
    bool modifyEntry(FlowEntryModRequest *femr);
    bool addParserProtocol(ModParserInfo* mpi);
    bool addFlowTable(AddFlowTableInfo * afti);
    bool addMatcherInfo(ModMatcherInfo* mod_matcher_info);
    bool addActionInfo(AddActionInfo* add_action_info);
    void getMemoryInfo();
};

bool ModFlowEntryClient::initParserInfo(InitParserInfo * parser_info) {
    ClientContext context;
    ReturnCodeResponse init_parser_response;
    Status status = stub_->initParser(&context, *parser_info, &init_parser_response);
    if (!status.ok()) {
        std::cout << "init parser info rpc failed." << std::endl;
        return false;
    } else {
//        InitParserRes res = init_parser_response.init_parser_res();
        int res = init_parser_response.return_code();
        if (res == 1) {
            std::cout << "init parser info ok!" << std::endl;
        } else {
            std::cout << "init parser info failed!" << std::endl;
        }
        return true;
    }
    return true;
}

bool ModFlowEntryClient::initMemInfo(MemoryInfo* mem_info) {
    ClientContext context;
    ReturnCodeResponse init_mem_response;
    Status status = stub_->initSRAMandTCAM(&context, *mem_info, &init_mem_response);
    if (!status.ok()) {
        std::cout << "init mem info rpc failed." << std::endl;
        return false;
    } else {
//        InitMemRes res = init_mem_response.init_mem_res();
        int res = init_mem_response.return_code();
        if (res == 1) {
            std::cout << "init mem info ok!" << std::endl;
        } else {
            std::cout << "init mem info failed!" << std::endl;
        }
        return true;
    }
    return true;
}

bool ModFlowEntryClient::modifyEntry(FlowEntryModRequest *femr) {
    ClientContext context;
    ReturnCodeResponse fem_response;
    Status status = stub_->handleMatchTable(&context, *femr, &fem_response);

    if (!status.ok()) {
        std::cout << "handleMatchTable rpc failed." << std::endl;
        return false;
    } else {
//        FlowEntryModRes res = fem_response.res();
        int res = fem_response.return_code();
        if (res == 1)
            std::cout << "flow entry add ok!" << std::endl;
        else if (res == 2)
            std::cout << "flow entry modify ok!" << std::endl;
        else if (res == 3)
            std::cout << "flow entry delete ok!" << std::endl;
        else if (res == 4)
            std::cout << "no such entry." << std::endl;
        else if (res == 5)
            std::cout << "too many flow entries." << std::endl;
        else
            std::cout << "handleMatchTable rpc succeeded!" << std::endl;
        return true;
    }
    return false;
}

bool ModFlowEntryClient::addParserProtocol(ModParserInfo* mpi) {
    ClientContext context;
    ReturnCodeResponse mpi_response;
    Status status = stub_->addParserInfo(&context, *mpi, &mpi_response);

    if(!status.ok()) {
        std::cout << "addParserProtocol rpc failed." << std::endl;
        return false;
    } else {
//        ParserModRes res = mpi_response.parser_mod_res();
        int res = mpi_response.return_code();
        if (res == 1) {
            std::cout << "add parser protocol ok." << std::endl;
        } else{
            std::cout << "succeed!" << std::endl;
        }
        return true;
    }
    return true;
}

bool ModFlowEntryClient::addFlowTable(AddFlowTableInfo * afti) {
    ClientContext context;
    ReturnCodeResponse afti_response;
    Status status = stub_->addFlowTable(&context, *afti, &afti_response);
    if(!status.ok()) {
        std::cout << "addFlowTable rpc failed." << std::endl;
        return false;
    } else {
//        AddFlowTableRes res = afti_response.add_flow_table_res();
        int res = afti_response.return_code();
        if (res == 1) {
            std::cout << "add flow table ok." << std::endl;
        } else{
            std::cout << "failed!" << std::endl;
        }
        return true;
    }
    return true;
}

bool ModFlowEntryClient::addMatcherInfo(ModMatcherInfo* mod_matcher_info) {
    ClientContext context;
    ReturnCodeResponse mod_matcher_info_response;
    Status status = stub_->addMatcherInfo(&context, *mod_matcher_info, &mod_matcher_info_response);
    if(!status.ok()) {
        std::cout << "addMatcher rpc failed." << std::endl;
        return false;
    } else {
//        MatcherModRes res = mod_matcher_info_response.matcher_mod_res();
        int res = mod_matcher_info_response.return_code();
        if (res == 1) {
            std::cout << "add matcher ok." << std::endl;
        } else{
            std::cout << "failed!" << std::endl;
        }
        return true;
    }
    return true;
}

bool ModFlowEntryClient::addActionInfo(AddActionInfo* add_action_info){
    ClientContext context;
    ReturnCodeResponse add_action_info_response;
    Status status = stub_->addActionInfo(&context, *add_action_info, &add_action_info_response);
    if(!status.ok()) {
        std::cout << "addAction rpc failed." << std::endl;
        return false;
    } else {
//        MatcherModRes res = mod_matcher_info_response.matcher_mod_res();
        int res = add_action_info_response.return_code();
        if (res == 1) {
            std::cout << "add action ok." << std::endl;
        } else{
            std::cout << "failed!" << std::endl;
        }
        return true;
    }
    return true;
}

void ModFlowEntryClient::getMemoryInfo() {

}

/**
 * input:
 *      add_entry flow_table_name [192.168.111.10 24] action_name action_parameter
 *      add_entry ipv4_lpm 192.168.111.10 24 ipv4_forward 1 2
 *      add_entry mac_learning 00:00:00:00:00:00
 */
// add_entry ipv4_lpm 192.168.111.10 255.255.255.0 ipv4_forward 192.168.110.10 2
//              1       2               3           4
void ModFlowEntryClient::handleMatchTable(std::vector<std::string> tokens, FlowEntryModType mod_type, Parser* parser,
                                          MemConfig *mem_config, MatcherConfig* matcher,
                                          ExecuterConfig* executer_config, Processor* processor,
                                          Metadata* metadata) {

    auto femr = new FlowEntryModRequest(); // the object transferred to grpc server

    if(mod_type == FlowEntryModType::ADD) {
        uint32_t flow_table_id = mem_config->getFlowTableIdByName(tokens[1]);
        int action_id = executer_config->getActionIdByName(tokens[4]);

        auto srcFE = new FlowEntry();
        srcFE->set_tag(flow_table_id);
        srcFE->set_key(tokens[2]);
        srcFE->set_mask(tokens[3]);
        srcFE->set_action_id(action_id);

        for(int i = 5; i < tokens.size(); i++) {
            srcFE->add_action_data(tokens[i]);
        }

        femr->set_flow_table_id(flow_table_id);
        femr->set_allocated_srcflow(srcFE);
        femr->set_mod_type(mod_type);

        modifyEntry(femr); // call private function
    } else if (mod_type == FlowEntryModType::MODIFY) {
        uint32_t flow_table_id = mem_config->getFlowTableIdByName(tokens[1]);

        // update_entry ipv4_lpm 192.168.111.10 255.255.255.0 192.168.111.10 255.255.255.128 ipv4_forward 192.168.110.10 2
        //              1           2               3           4               5               6
        auto srcFE = new FlowEntry();
        srcFE->set_tag(flow_table_id);
        srcFE->set_key(tokens[2]);
        srcFE->set_mask(tokens[3]);

        auto modFE = new FlowEntry();
        modFE->set_key(tokens[4]);
        modFE->set_mask(tokens[5]);
        int action_id = executer_config->getActionIdByName(tokens[6]);
        modFE->set_action_id(action_id);

        for(int i = 7; i < tokens.size(); i++) {
            modFE->add_action_data(tokens[i]);
        }

        femr->set_flow_table_id(flow_table_id);
        femr->set_allocated_srcflow(srcFE);
        femr->set_mod_type(mod_type);
        femr->set_allocated_modflow(modFE);

        modifyEntry(femr); // call private function
    } else if (mod_type == FlowEntryModType::DELETE) {
        // delete_entry ipv4_lpm 192.168.111.10 255.255.255.0
        uint32_t flow_table_id = mem_config->getFlowTableIdByName(tokens[1]);

        auto srcFE = new FlowEntry();
        srcFE->set_tag(flow_table_id);
        srcFE->set_key(tokens[2]);
        srcFE->set_mask(tokens[3]);

        femr->set_flow_table_id(flow_table_id);
        femr->set_allocated_srcflow(srcFE);
        femr->set_mod_type(mod_type);

        modifyEntry(femr); // call private function
    }
}


rp4_MatchType getMatchType(MatchType match_type) {
    if (match_type == MatchType::EXACT)
        return rp4_MatchType::EXACT;
    else if (match_type == MatchType::TERNARY)
        return rp4_MatchType::TERNARY;
    else if (match_type == MatchType::LPM)
        return rp4_MatchType::LPM;
    else
        return rp4_MatchType::RANGE;
}

rp4_MemType getMemType(MemType mem_type) {
    if (mem_type == MemType::MEM_TCAM)
        return rp4_MemType::TCAM;
    else
        return rp4_MemType::SRAM;
}

void ModFlowEntryClient::addParserInfo(std::vector<std::string> tokens, Parser* parser,
                                       MemConfig *mem_config, MatcherConfig* matcher,
                                       ExecuterConfig* executer_config, Processor* processor,
                                       Metadata* metadata) {


    auto mpi = new ModParserInfo();

    std::string filename = tokens[1];
    FILE* fp = fopen(filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    auto c_header = new Header();
    c_header->header_name = d["header_name"].GetString();
    c_header->header_length = d["header_length"].GetInt();
    c_header->header_tag = d["header_tag"].GetString();
    c_header->next_header_type_internal_offset = d["next_header_type_internal_offset"].GetInt();
    c_header->next_header_type_length = d["next_header_type_length"].GetInt();

    auto previous_header_names = d["previous_header_names"].GetArray();
    int size = previous_header_names.Size();
    for(int i = 0; i < size; i++) {
        int header_id = parser->getHeaderIdByName(previous_header_names[i].GetString());
        c_header->father_header_ids.push_back(header_id);
    }

    std::vector<Field*> field_v;
    auto fields = d["fields"].GetArray();
    int field_size = fields.Size();
    for(int i = 0; i < field_size; i++) {
        auto field = new Field();
        field->field_name = fields[i]["field_name"].GetString();
        field->field_length = fields[i]["field_length"].GetInt();
        field->field_internal_offset = fields[i]["internal_offset"].GetInt();
        field_v.push_back(field);
    }

    std::vector<NextHeader*> next_header_v;
    auto next_headers = d["next_headers"].GetArray();
    int nh_size = next_headers.Size();
    for(int i = 0; i < nh_size; i++) {
        auto nh = new NextHeader();
        nh->next_header_name = next_headers[i]["next_header_name"].GetString();
        nh->next_header_tag = next_headers[i]["next_header_tag"].GetString();
        nh->next_header_id = parser->getHeaderIdByName(nh->next_header_name);
        next_header_v.push_back(nh);
    }

    parser->addHeaderAtRuntime(c_header, field_v, next_header_v);


    // there must be "new" or "mutable",
    //      or the local variable will be deleted on the return of function
    // TODO: get header_id by header_name, get field_id by field_name
    auto header = new rp4_Header();
    //auto header = mpi->mutable_header();
    header->set_header_id(c_header->header_id);
    for(int i : c_header->father_header_ids) {
        header->add_previous_header_id(2);
    }
    header->set_header_tag(c_header->header_tag);
    header->set_header_length(c_header->header_length);
    header->set_next_header_type_internal_offset(c_header->next_header_type_internal_offset);
    header->set_next_header_type_length(c_header->next_header_type_length);

    rp4_Field *field;
    for(auto f : field_v) {
        field = header->add_fields();
//        field->set_field_id(f->field_id);
        field->set_field_length(f->field_length);
        field->set_internal_offset(f->field_internal_offset);
    }

    rp4_NextHeader* next_header;
    for(auto nh : next_header_v) {
        next_header = header->add_next_headers();
        next_header->set_header_id(nh->next_header_id);
        next_header->set_tag(nh->next_header_tag);
    }

    mpi->set_allocated_header(header);
    addParserProtocol(mpi);
}

// add_table vlan_table exact 32 48 1 100
void ModFlowEntryClient::addFlowTableInfo(std::vector<std::string> tokens, Parser* parser,
                                          MemConfig *mem_config, MatcherConfig* matcher,
                                          ExecuterConfig* executer_config, Processor* processor,
                                          Metadata* metadata) {
    /*
    uint32 flow_table_id = 1;
    MatchType match_type = 2;
    MemType mem_type = 3;
    uint32 resided_logic_table_id = 4;
    uint32 key_length = 5;
    uint32 value_length = 6;
    uint32 action_data_num = 7;
     */
    int key_length = std::atoi(tokens[3].c_str());
    int value_length = std::atoi(tokens[4].c_str());
    int action_data_num = std::atoi(tokens[5].c_str());
    int possible_entry_num = std::stoi(tokens[6].c_str());
    std::string matchType = tokens[2];

    std::cout << key_length << " " << value_length << " " << action_data_num << " " << possible_entry_num << " " << std::endl;

    rp4_MatchType match_type;
    rp4_MemType mem_type = rp4_MemType::TCAM;
    if(matchType == "exact"){
        match_type = rp4_MatchType::EXACT;
        mem_type = rp4_MemType::SRAM;
    } else if(matchType == "lpm") {
        match_type = rp4_MatchType::LPM;
    } else if(matchType == "ternary") {
        match_type = rp4_MatchType::TERNARY;
    } else if(matchType == "range") {
        match_type = rp4_MatchType::RANGE;
    } else{
        match_type = rp4_MatchType::EXACT;
        mem_type = rp4_MemType::SRAM;
    }

    std::string flow_table_name = tokens[1];
    auto ft = mem_config->addPureFlowTable(tokens[1], tokens[2],
                                           key_length, value_length, action_data_num, possible_entry_num);

    auto afti = new AddFlowTableInfo();

    auto flow_table = new rp4_FlowTable();

    flow_table->set_flow_table_id(ft->getFlowTableId());
    flow_table->set_resided_logical_table_id(ft->getResidedLogicTableId());
//    flow_table->set_key_length(key_length);
    flow_table->set_value_length(value_length);
    flow_table->set_match_type(match_type);
    flow_table->set_mem_type(mem_type);

    afti->set_allocated_flow_table(flow_table);

    addFlowTable(afti);
}

MatchType getMatchTyeByName(const std::string& matchType) {
    if (matchType == "exact"){
        return MatchType::EXACT;
    } else if (matchType == "ternary") {
        return MatchType::TERNARY;
    } else if(matchType == "lpm") {
        return MatchType::LPM;
    } else if(matchType == "range") {
        return MatchType::RANGE;
    }
}

void ModFlowEntryClient::addMatcher(std::vector<std::string> tokens, Parser* parser, MemConfig *mem_config, MatcherConfig* matcher,
                                    ExecuterConfig* executer_config, Processor* processor, Metadata* metadata) {

    std::string filename = tokens[1];
    FILE* fp = fopen(filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    std::string processor_name = d["processor_name"].GetString();
    std::string flow_table_name = d["flow_table_name"].GetString();

    auto matcher_t = new Matcher();
    matcher_t->flow_table_name = flow_table_name;
    matcher_t->flow_table_id = mem_config->getFlowTableIdByName(flow_table_name);

    auto match_fields = d["match_fields"].GetArray();
    int size = match_fields.Size();
    int length_sum = 0;
    std::vector<MatchType> match_types;
    for(int i = 0; i < size; i++) {
        auto match_field = new MatchField();
        match_field->header_name = match_fields[i]["type"].GetString();
        match_field->field_name = match_fields[i]["value"].GetString();
        match_field->matchType = getMatchTyeByName(match_fields[i]["match_type"].GetString());
        match_types.push_back(match_field->matchType);

        match_field->field_length = match_fields[i]["length"].GetInt();
        length_sum += match_field->field_length;
        match_field->header_id = parser->getHeaderIdByName(match_field->header_name);
        match_field->field_id = parser->getFieldIdByName(match_field->field_name);

        matcher_t->match_fields.push_back(match_field);
    }
    matcher->addMatcherAtRuntime(processor_name, matcher_t, parser, mem_config, processor);

    MatchType match_type;
    int exact_count = count(match_types.begin(), match_types.end(), MatchType::EXACT);
    int lpm_count = count(match_types.begin(), match_types.end(), MatchType::LPM);
    int ternary_count = count(match_types.begin(), match_types.end(), MatchType::TERNARY);
    if(ternary_count != 0) {
        match_type = MatchType::TERNARY;
    } else if(lpm_count != 0) {
        match_type = MatchType::LPM;
    } else {
        match_type = MatchType::EXACT;
    }
    matcher_t->match_type = match_type;

    auto mod_matcher_info = new ModMatcherInfo();
    mod_matcher_info->set_processor_id(matcher_t->resided_processor_id);
    mod_matcher_info->set_match_length(length_sum + 8);// plus tag length
    mod_matcher_info->set_table_id(matcher_t->flow_table_id);
    mod_matcher_info->set_match_type(getMatchType(matcher_t->match_type));

    rp4_MatchField *mf;
    int mf_size = matcher_t->match_fields.size();
    for(int i = 0; i < mf_size; i++) {
        mf = mod_matcher_info->add_match_fields();
        mf->set_header_id(matcher_t->match_fields[i]->header_id);
        mf->set_field_id(matcher_t->match_fields[i]->field_id);
        mf->set_match_type(getMatchType(matcher_t->match_fields[i]->matchType));
    }
    addMatcherInfo(mod_matcher_info);
}

std::vector<OpCode> codes = {
        OpCode::ADD, OpCode::SUB, OpCode::MOD, OpCode::DIV, OpCode::MUL,
        OpCode::SHIFT_LEFT, OpCode::SHIFT_RIGHT, OpCode::EQ_DATA, OpCode::NEQ_DATA, OpCode::GT_DATA, OpCode::LT_DATA, OpCode::GET_DATA, OpCode::LET_DATA,
        OpCode::AND, OpCode::OR, OpCode::NOT, OpCode::BIT_AND, OpCode::BIT_OR, OpCode::BIT_XOR, OpCode::BIT_NEG,
        OpCode::TERNARY_OP, OpCode::COPY_FIELD, OpCode::SET_FIELD, OpCode::DECREMENT,
        OpCode::SET_TTL, OpCode::DECREMENT_TTL, OpCode::COPY_TTL_OUTWARDS, OpCode::COPY_TTL_INWARDS, OpCode::PUSH, OpCode::POP
};

int getParameterHeaderId(const std::string& header_name, Parser *parser) {
    if(header_name == "metadata") {
        return 100;
    } else if(header_name == "p") {
        return 101;
    } else {
        return parser->getHeaderIdByName(header_name);
    }
}

int getParamterFieldId(const std::string& field_name, Parser *parser) {
    int id = parser->getFieldIdByName(field_name);
    if(id != -1) {
        return id;
    }else{
        return std::atoi(field_name.c_str());
    }
}

void ModFlowEntryClient::addAction(std::vector<std::string> tokens, Parser* parser, MemConfig *mem_config, MatcherConfig* matcher,
                                   ExecuterConfig* executer_config, Processor* processor, Metadata* metadata) {
    auto action = executer_config->addActionByJson(tokens[1], processor);

    auto add_action_info = new AddActionInfo();
    add_action_info->set_processor_id(action->processor_id);
    auto ac = new rp4_Action();
    ac->set_action_id(action->action_id);
    ac->set_parameter_num(action->parameter_num);
    ac->set_action_name(action->action_name);

    for(int i = 0; i < action->parameter_lengths.size(); i++) {
        ac->add_parameter_lengths(action->parameter_lengths[i]);
    }

    rp4_Primitive* primitive;
    int size = action->primitives.size();
    for(int i = 0; i < size; i++) {
        primitive = ac->add_primitives();
        auto p = action->primitives[i];
        primitive->set_op_id(find(codes.begin(), codes.end(), p->op) - codes.begin());

        rp4_Parameter * rp4_p;
        int p_size = p->parameters.size();
        for(int j = 0; j < p_size; j++) {
            rp4_p = primitive->add_parameters();
            rp4_p->set_prefix_id(getParameterHeaderId(p->parameters[j].type, parser));
            rp4_p->set_prefix_id(getParamterFieldId(p->parameters[j].value, parser));
        }
    }

    add_action_info->set_allocated_action(ac);
    addActionInfo(add_action_info);
}


void ModFlowEntryClient::initParser(Parser *parser) {
    auto parser_info = new InitParserInfo();
    rp4_Header *h;

    auto header_map = parser->getHeaderMap();
    auto field_map = parser->getFieldMap();

    for(auto it : header_map) {
        auto header = it.second;

        h = parser_info->add_headers();
        h->set_header_id(header->header_id);
        for(auto i : header->father_header_ids) {
            h->add_previous_header_id(i);
        }
        h->set_header_tag(header->header_tag);
        h->set_header_length(header->header_length);
        h->set_next_header_type_internal_offset(header->next_header_type_internal_offset);
        h->set_next_header_type_length(header->next_header_type_length);

        rp4_Field * f;
        for(auto i : header->fields) {
            f = h->add_fields();
            auto field = field_map[i];
//            f->set_field_id(field->field_id);
            f->set_field_length(field->field_length);
            f->set_internal_offset(field->field_internal_offset);
        }

        rp4_NextHeader * nh;
        for(auto i : header->next_header_ids) {
            nh = h->add_next_headers();
            nh->set_header_id(i);
            nh->set_tag(header_map[i]->header_tag);
        }
    }
    initParserInfo(parser_info);
}

void ModFlowEntryClient::initMem(MemConfig * mem_config) {
    auto mem_info = new MemoryInfo();

    mem_info->set_sram_num(mem_config->getSramNum());
    mem_info->set_tcam_num(mem_config->getTcamNum());
    mem_info->set_bit_per_sram(mem_config->getBitPerSram());
    mem_info->set_bit_per_tcam(mem_config->getBitPerTcam());

    auto sram_map = mem_config->getSramMap();
    auto tcam_map = mem_config->getTcamMap();
    auto logical_table_map = mem_config->getLogicalTableMap();
    auto flow_table_map = mem_config->getFlowTableMap();

    for(auto it : sram_map) {
        int sram_id = it.first;
        auto sram = it.second;

        auto ltVec = new LogicalTableVector();
        auto sram_lts = sram->getLogicalTables();
        rp4_LogicalTable * rp4_logical_table;
        for(auto i : sram_lts) {
            auto logical_table = logical_table_map[i];
            rp4_logical_table = ltVec->add_logical_tables();

            rp4_logical_table->set_logical_table_id(logical_table->getLogicalTableId());
            rp4_logical_table->set_resided_hardware_id(logical_table->getResidedHardwareId());
            rp4_logical_table->set_supported_max_length(logical_table->getSupportedMaxLength());
            rp4_logical_table->set_supported_max_num(logical_table->getSupportedMaxNum());
            rp4_logical_table->set_mem_type(getMemType(logical_table->getMemType()));

            rp4_FlowTable *rp4_flow_t;
            auto flow_tables = logical_table->getInsideFlowTables();
            for(auto j : flow_tables) {
                auto flow_table = flow_table_map[j];

                rp4_flow_t = rp4_logical_table->add_inside_flow_tables();

                rp4_flow_t->set_flow_table_id(flow_table->getFlowTableId());
                rp4_flow_t->set_match_type(getMatchType(flow_table->getTableMatchType()));
                rp4_flow_t->set_mem_type(getMemType(flow_table->getMemType()));
                rp4_flow_t->set_resided_logical_table_id(flow_table->getResidedLogicTableId());
//                rp4_flow_t->set_key_length(flow_table->getKeyLength());
                rp4_flow_t->set_value_length(flow_table->getValueLength());
            }
        }
        (*mem_info->mutable_sram_memory_info())[sram_id] = *ltVec;
    }

    for(auto it : tcam_map) {
        int tcam_id = it.first;
        auto tcam = it.second;

        auto lt = logical_table_map[tcam->getLogicalTableId()];

        auto rp4_lt = new rp4_LogicalTable();
        rp4_lt->set_logical_table_id(lt->getLogicalTableId());
        rp4_lt->set_resided_hardware_id(lt->getResidedHardwareId());
        rp4_lt->set_supported_max_length(lt->getSupportedMaxLength());
        rp4_lt->set_supported_max_num(lt->getSupportedMaxNum());
        rp4_lt->set_mem_type(getMemType(lt->getMemType()));

        rp4_FlowTable *rp4_flow_t;
        auto flow_tables = lt->getInsideFlowTables();
        for(auto j : flow_tables) {
            auto flow_table = flow_table_map[j];

            rp4_flow_t = rp4_lt->add_inside_flow_tables();

            rp4_flow_t->set_flow_table_id(flow_table->getFlowTableId());
            rp4_flow_t->set_match_type(getMatchType(flow_table->getTableMatchType()));
            rp4_flow_t->set_mem_type(getMemType(flow_table->getMemType()));
            rp4_flow_t->set_resided_logical_table_id(flow_table->getResidedLogicTableId());
//            rp4_flow_t->set_key_length(flow_table->getKeyLength());
            rp4_flow_t->set_value_length(flow_table->getValueLength());
        }
        (*mem_info->mutable_tcam_memory_info())[tcam_id] = *rp4_lt;
    }
    initMemInfo(mem_info);
}

void handleArgs(int argc, char *argv[], ModFlowEntryClient & mod, Parser* parser, MemConfig*mem_config,
                MatcherConfig* matcher, ExecuterConfig* executer_config, Processor *processor, Metadata* metadata) {
    std::string config_file_path;

    int opt;
    int digit_optind = 0;
    int option_index = 0;
    const char *optstring = "p:s";

    static struct option long_options[] = {
            {"sram", required_argument, NULL, 1},
            {"tcam", required_argument, NULL, 2},
            {"mem_config_ft", required_argument, NULL, 3},
            {"header", required_argument, NULL, 4},
            {"matcher", required_argument, NULL, 5},
            {"executer", required_argument, NULL, 6},
            {"processor_num", required_argument, NULL, 7},
            {0,0,0,0}
    };

    while((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1){
        switch(opt){
            case 1: {
                mem_config->initSRAMFromJson(config_file_path + optarg);
                break;
            }
            case 2: {
                mem_config->initTCAMFromJson(config_file_path + optarg);
                break;
            }
            case 3: {
                mem_config->initFlowTableFromJson(config_file_path + optarg);
                break;
            }
            case 4: {
                parser->initializeFromJson(config_file_path + optarg);
                mod.initParser(parser);
                break;
            }
            case 5: {
                matcher->initFromJson(config_file_path + optarg, parser, mem_config, processor);
                break;
            }
            case 6:{
                executer_config->initExecuterFromJson(config_file_path + optarg, matcher, processor);
//                executer_config->printActionsByExecuter();
                break;
            }
            case 7: {
                processor->setProcessorNum(std::atoi(optarg));
                break;
            }
            case 'p': {
                config_file_path = optarg;
                break;
            }
            case 's': {
                mod.initMem(mem_config);
                break;
            }
            default:
                break;
        }
    }
}

void split(const std::string& s, std::vector<std::string>& tokens, const std::string& delimiters = " "){
    std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    std::string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
}

const std::vector<std::string> instructions = {
        "add_entry", "update_entry", "delete_entry", "add_table", "add_header",
        "add_matcher", "add_executer", "add_action", "print_parser"
};

// add_entry ipv4_lpm 192.168.111.10 255.255.255.0 ipv4_forward 192.168.110.10 2
// add_action ipv4_lpm 192.168.111.10 24 ipv4_forward 1 2
void handleRuntimeConfig(ModFlowEntryClient & mod, char * input, Parser* parser, MemConfig *mem_config, MatcherConfig* matcher,
                         ExecuterConfig* executer_config, Processor* processor, Metadata* metadata){
    std::vector<std::string> tokens;
    split(input, tokens, " ");
    int a = std::find(instructions.begin(), instructions.end(), tokens[0]) - instructions.begin();
    switch(a) {
        case 0: {
            mod.handleMatchTable(tokens, FlowEntryModType::ADD,
                                 parser, mem_config, matcher, executer_config, processor, metadata);

            break;
        } case 1: {
            mod.handleMatchTable(tokens, FlowEntryModType::MODIFY,
                                 parser, mem_config, matcher, executer_config, processor, metadata);
            break;
        } case 2: {
            mod.handleMatchTable(tokens, FlowEntryModType::DELETE,
                                 parser, mem_config, matcher, executer_config, processor, metadata);
            break;
        } case 3: {
            mod.addFlowTableInfo(tokens,
                                 parser, mem_config, matcher, executer_config, processor, metadata);
            break;
        } case 4: {
            mod.addParserInfo(tokens,
                              parser, mem_config, matcher, executer_config, processor, metadata);
            break;
        } case 5: {
            mod.addMatcher(tokens,
                           parser, mem_config, matcher, executer_config, processor, metadata);
            break;
        } case 6: {
            break;
        } case 7: {
            mod.addAction(tokens,
                          parser, mem_config, matcher, executer_config, processor, metadata);
            break;
        } case 8:{
             parser->printHeaderInfo();
             break;
        }
        default:
            break;

    }
}

int main(int argc, char** argv) {
    auto processor = new Processor();
    auto parser = new Parser();
    auto mem_config = new MemConfig();
    auto matcher = new MatcherConfig();
    auto executer_config = new ExecuterConfig(8);
    auto metadata = new Metadata();

    ModFlowEntryClient mod(grpc::CreateChannel("127.0.0.1:50051",
                                               grpc::InsecureChannelCredentials()));

    handleArgs(argc, argv, mod, parser, mem_config, matcher, executer_config, processor, metadata);

    char input[255] = "";
    std::cout << "main> ";
    while (std::cin.getline(input, 254)) {
        handleRuntimeConfig(mod, input, parser, mem_config, matcher, executer_config, processor, metadata);
        std::cout << "main> ";
    }

    return 0;
}