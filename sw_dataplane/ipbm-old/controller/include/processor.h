//
// Created by fengyong on 2021/2/10.
//

#ifndef GRPC_TEST_PROCESSOR_H
#define GRPC_TEST_PROCESSOR_H

#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm>

//#include "parser.h"
//#include "matcher.h"
//#include "executer.h"

struct Proc {
    int processor_id;
    std::string processor_name;
    int matcher_id;
    int executer_id;
    bool if_use;
};

class Processor {
public:
    Processor(int processorNum);

    Processor() {};
    void setProcessorNum(int processorNum);

    int* getProcBitmap();
    void setProBitmapAndProc(int index, bool if_use);
    int allocMatcherToProcessor(std::string processor_name);
    int allocExecuterToProcessor(std::string processor_name);
private:
    std::unordered_map<int, Proc*> proc_map;
    int processor_num;
    int * proc_bitmap;
    std::vector<std::string> pro_names;
};

Processor::Processor(int processorNum) {
    this->processor_num = processorNum;
    for(int i = 0; i < processorNum; i++) {
        auto proc = new Proc();
        proc->processor_id = i;
        this->proc_map.insert(std::make_pair(i, proc));
    }
    proc_bitmap = (int*) malloc(sizeof(int) * processorNum);//new int(processorNum);
    memset(proc_bitmap, 0, processorNum * sizeof(int));
}

int* Processor::getProcBitmap() {
    return this->proc_bitmap;
}

void Processor::setProBitmapAndProc(int index, bool if_use) {
    proc_bitmap[index] = if_use ? 1 : 0;
    proc_map[index]->if_use = if_use;
}

int Processor::allocMatcherToProcessor(std::string processor_name) {
    for(auto it : proc_map) {
        if(it.second->processor_name == processor_name) {
            return it.first;
        }
    }
//    if(find(pro_names.begin(), pro_names.end(), processor_name)
//                                                                != pro_names.end()) {
//        return -2;
//    }

    for(int i = 0; i < processor_num; i++) {
        if (proc_bitmap[i] == 0) {
            proc_map[i]->if_use = true;
            proc_map[i]->processor_name = processor_name;

            proc_bitmap[i] = 1;
            pro_names.push_back(processor_name);
            std::cout << i << std::endl;
            return i;
        }
    }
    return -1;
}

int Processor::allocExecuterToProcessor(std::string processor_name) {
    for(auto it : proc_map) {
        if(it.second->processor_name == processor_name) {
            return it.first;
        }
    }

    for(int i = 0; i < processor_num; i++) {
        if (proc_bitmap[i] == 0) {
            proc_map[i]->if_use = true;
            proc_map[i]->processor_name = processor_name;

            proc_bitmap[i] = 1;
            pro_names.push_back(processor_name);
            return i;
        }
    }
    return -1;
}

void Processor::setProcessorNum(int processorNum) {
    this->processor_num = processorNum;
    for(int i = 0; i < processorNum; i++) {
        auto proc = new Proc();
        proc->processor_id = i;
        this->proc_map.insert(std::make_pair(i, proc));
    }
    proc_bitmap = (int*) malloc(sizeof(int) * processorNum);//new int(processorNum);
    memset(proc_bitmap, 0, processorNum * sizeof(int));
}

#endif //GRPC_TEST_PROCESSOR_H
