//
// Created by fengyong on 2021/2/27.
//

#ifndef GRPC_TEST_METADATA_H
#define GRPC_TEST_METADATA_H

#include <iostream>
#include <unordered_map>

struct Meta {
    std::string meta_name;
    int meta_id;
    int meta_length;
};

struct Metadata {
    std::unordered_map<int, Meta*> meta_map;
    int cur_meta_id;

    Metadata() : cur_meta_id(0) {};

    void insertMeta(Meta * meta) {
        meta_map.insert(std::make_pair(meta->meta_id, meta));
    }

    void insertRawMeta(const std::string & meta_name, int meta_length) {
        auto meta = new Meta();
        meta->meta_name = meta_name;
        meta->meta_id = cur_meta_id;
        cur_meta_id++;
        meta->meta_length = meta_length;
    }

    int getMetaIdByName(const std::string &meta_name) const{
        for(auto it : meta_map) {
            if(it.second->meta_name == meta_name) {
                return it.first;
            }
        }
        return -1;
    }

    void printMeta() {
        std::cout << "********** Matadata **********" << std::endl;
        for(auto it : meta_map) {
            auto meta = it.second;
            std::cout << meta->meta_name << ": " <<
                    meta->meta_length << "(length) " << meta->meta_id << "(id)" << std::endl;
        }
    }
};


#endif //GRPC_TEST_METADATA_H
