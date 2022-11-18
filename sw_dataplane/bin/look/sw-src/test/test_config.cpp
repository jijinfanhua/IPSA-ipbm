//
// Created by xilinx_0 on 1/7/22.
//

#include "../dataplane/global.h"

#include <iostream>

#include <gflags/gflags.h>
//#include <glog/logging.h>
#include <unistd.h>

#include <regex>

///home/xilinx_0/fengyong/reconf-switch-IPSA/sw-src/conf/switch.yml
DEFINE_string(ifconf, "../../sw-src/conf/switch.yml", "Interfaces");

std::string trim(std::string & str) {
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
    return str;
}

void convert_to_flow_entry(std::string s_flow_entry) {
    std::regex ws(" ");
    std::vector<std::string> v(std::sregex_token_iterator(s_flow_entry.begin(),
                                                          s_flow_entry.end(), ws, -1),
                               std::sregex_token_iterator());
    for(auto a : v) {
        std::cout << trim(a) << std::endl;
    }

}

int main(int argc, char *argv[]) {

    std::string a = "1 3 > 2 5";
    convert_to_flow_entry(a);


//    gflags::ParseCommandLineFlags(&argc, &argv, true);
//
//    char pwd[255];
//
//    std::cout << getcwd(pwd, 255) << std::endl;
//
//    config_init(FLAGS_ifconf);
    return 0;
}