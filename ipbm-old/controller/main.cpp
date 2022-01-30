
#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <algorithm>

#include "include/parser.h"
#include "include/matcher.h"
#include "include/executer_config.h"
#include "include/mem_config.h"
#include "include/metadata.h"

void handleArgs(int argc, char *argv[], Parser* parser, MemConfig*mem_config, MatcherConfig* matcher,
                ExecuterConfig* executer_config, Processor *processor, Metadata* metadata) {
    std::string config_file_path;

    int opt;
    int digit_optind = 0;
    int option_index = 0;
    const char *optstring = "p:";

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
            case 1:{
                mem_config->initSRAMFromJson(config_file_path + optarg);
                break;
            }
            case 2:{
                mem_config->initTCAMFromJson(config_file_path + optarg);
                break;
            }
            case 3:{
                mem_config->initFlowTableFromJson(config_file_path + optarg);
                break;
            }
            case 4:{
                parser->initializeFromJson(config_file_path + optarg);
                break;
            }
            case 5:{
                matcher->initFromJson(config_file_path + optarg, parser, mem_config, processor);
                break;
            }
            case 6:{
                executer_config->initExecuterFromJson(config_file_path + optarg, matcher, processor);
                executer_config->printActionsByExecuter();
                break;
            }
            case 7:{
                processor->setProcessorNum(std::atoi(optarg));
                break;
            }
            case 'p':{
                config_file_path = optarg;
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
std::vector<std::string> instructions = {
        "add_entry", "update_entry", "delete_entry", "add_table", "add_header",
        "add_matcher", "add_executer", "add_action", "add_metadata"
};

// add_entry ipv4_lpm 192.168.111.10 255.255.255.0 ipv4_forward 192.168.110.10 2
// add_action ipv4_lpm 192.168.111.10 24 ipv4_forward 1 2
void handleRuntimeConfig(char * input, Parser* parser, MemConfig *mem_config, MatcherConfig* matcher,
                         ExecuterConfig* executer_config, Processor* processor, Metadata* metadata){
    std::vector<std::string> tokens;
    split(input, tokens, " ");
    int a = std::find(instructions.begin(), instructions.end(), tokens[0]) - instructions.begin();
    switch(a) {
        case 1: {
            int flow_table_id = mem_config->getFlowTableIdByName(tokens[1]);


            break;
        } case 2: {
            break;
        } case 3: {
            break;
        } case 4: {
            break;
        } case 5: {
            break;
        } case 6: {
            break;
        } case 7: {
            break;
        } case 8: {
            break;
        } default:
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

    handleArgs(argc, argv, parser, mem_config, matcher, executer_config, processor, metadata);


    char input[255] = "";
    std::cout << "main> ";
    while (std::cin.getline(input, 254)) {
        handleRuntimeConfig(input, parser, mem_config, matcher, executer_config, processor, metadata);
        std::cout << "main> ";
    }
    return 0;

}