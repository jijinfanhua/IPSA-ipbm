#include "rp4_runtime.h"
#include <sstream>

Rp4Runtime runtime;

inline bool is_command(std::string cmd) {
    return cmd == "help" || cmd == "compile" ||
           cmd == "table_add" ||
           cmd == "table_mod" ||
           cmd == "table_del";
}

int main(int argc, char **argv) {
    std::string line, temp;
    while (true) {
        std::cout << ">>> " << std::flush;
        std::getline(std::cin, line);
        std::vector<std::string> params;
        std::istringstream is(line);
        while (std::getline(is, temp, ' ')) {
            params.push_back(temp);
        }
        std::string cmd = params[0];
        if (cmd == "help") {
            std::cout << "help" << std::endl;
            std::cout << "compile <rp4 file> <json file>" << std::endl;
            std::cout << "table_add/mod/del <table_name> <action_name> <key> => <action_para>" << std::endl;
        } else if (cmd == "compile") {
            std::string rp4_filename = params[1];
            std::string json_filename = params[2];
            runtime.emitTask(rp4_filename, json_filename);
        } else if (is_command(cmd)) {
            runtime.emitUpdate(std::move(params));
        }
    }
    return 0;
}