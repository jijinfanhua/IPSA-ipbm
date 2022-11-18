//
// Created by fengyong on 2021/2/10.
//

#ifndef GRPC_TEST_PARSER_H
#define GRPC_TEST_PARSER_H

#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <errno.h>

//#include "../util/json/json/json.h"

#include "../util/json/rapidjson/document.h"
#include "../util/json/rapidjson/filereadstream.h"

/**
 * NextHeader: | neaxt_header_id | next_header_name | next_header_tag |
 * top level: next_header_name->next_header_id | next_header_tag->next_header_id
 */
struct NextHeader{
    int next_header_id;
    std::string next_header_name;
    std::string next_header_tag;
};

/**
 *
 */
struct Field {
    int field_id;
    std::string field_name;
    int field_length;
    int field_internal_offset;
};

struct Header {
    int header_id;
    std::string header_name;
    std::vector<int> father_header_ids;
    std::string header_tag;

    int header_length;
    int next_header_type_internal_offset;
    int next_header_type_length;

    std::vector<int> fields;
    std::vector<int> next_header_ids;
    std::vector<std::string> next_header_names;

    void setHeaderName(char * headerName) {
        header_name = headerName;
    }
};

class Parser {
public:
    Parser() : cur_header_idx(1), cur_field_idx(1) {};

    void setFields(const std::vector<Field*>& v_fields);// no use
    void addFields(const std::vector<Field*>& field_cluster); // no use
    void addField(Field* field);
    void setHeaders(const std::vector<Header*>& v_headers);

    void addHeaderBeforeRuntime(Header *header);
    void addHeaderAtRuntime(Header* header);

    void addNextHeader(NextHeader *nextHeader);
    void setNextHeaders(const std::vector<NextHeader*>& v_next_headers);
    void addNextHeaders(const std::vector<NextHeader*>& next_header_cluster);

    void setHeaderMap(std::unordered_map<int, Header*> headerMap);
    void setFieldMap(std::unordered_map<int, Field*> fieldMap);
    void setNextHeaderMap(std::unordered_map<int, NextHeader*> nextHeaderMap);

    int getMaxFieldNo();

    int getHeaderIdByName(std::string header_name);
    int getFieldIdByName(char * field_name);

    void fillFatherHeaderInfo();
    void printHeaderInfo();
    void printHeaderNameIDMap();

    void initializeFromJson(const std::string& json_filename);
    void initializeFromPlainFile(const std::string& filename);


private:
    std::unordered_map<int, Header*> header_map;
    std::unordered_map<int, Field*> field_map;
    std::unordered_map<int, NextHeader*> next_header_map;
    std::unordered_map<std::string, NextHeader*> next_header_name_map;

    std::unordered_map<std::string, int> header_name_id_map;
    std::unordered_map<std::string, int> field_name_id_map;

    int cur_header_idx;
    int cur_field_idx;
};

/**
 * store fields in 'field_name_id_map' and 'field_map', indexed by 'field_id'
 * it is used before runtime, actually initiation
 * @param v_fields
 */
void Parser::setFields(const std::vector<Field*>& v_fields) {
    for(auto it : v_fields) {
        field_name_id_map.insert(std::make_pair(it->field_name, it->field_id));
        field_map.insert(std::make_pair(it->field_id, it));
    }
}

/**
 * We support two ways for field insertion at runtime:
 * 1. cluster insertion
 * 2. single insertion
 * add to 'field_name_id_map' and 'field_map'
 * @param field_cluster
 */
void Parser::addFields(const std::vector<Field*>& field_cluster) {
    for(auto & it : field_cluster) {
        field_name_id_map.insert(std::make_pair(it->field_name, it->field_id));
        field_map.insert(std::make_pair(it->field_id, it));
    }
}

/**
 * before runtime, add to 'header_name_id_map' and 'header_map'
 * @param v_headers
 */
void Parser::setHeaders(const std::vector<Header*>& v_headers) {
    for(auto it : v_headers) {
        header_name_id_map.insert(std::make_pair(it->header_name, it->header_id));
        header_map.insert(std::make_pair(it->header_id, it));
    }
}

/**
 * add single header before runtime
 * @param header
 */
void Parser::addHeaderBeforeRuntime(Header *header) {
    header_name_id_map.insert(std::make_pair(header->header_name, header->header_id));
    header_map.insert(std::make_pair(header->header_id, header));
}

/**
 * add header at runtime
 * fill the 'next_header' info in its father protocols and
 * add next_header obj to next_header map.
 * @param header
 */
void Parser::addHeaderAtRuntime(Header* header) {
    header_name_id_map.insert(std::make_pair(header->header_name, header->header_id));
    header_map.insert(std::make_pair(header->header_id, header));

    /**** add relation between father protocol ****/
    std::vector<int> fatherIds = header->father_header_ids;
    for(int it : fatherIds){
        header_map[it]->next_header_ids.push_back(header->header_id);
    }
    /**** add end ****/

    /**** add to next_header_map ****/
    auto * nh = new NextHeader();
    nh->next_header_id = header->header_id;
    nh->next_header_tag = header->header_tag;
    nh->next_header_name = header->header_name;
    this->addNextHeader(nh);
    /**** add end ****/
}

void Parser::setNextHeaders(const std::vector<NextHeader*>& v_next_headers) {
    for(auto it : v_next_headers) {
        next_header_map.insert(std::make_pair(it->next_header_id, it));
    }
}

void Parser::addNextHeaders(const std::vector<NextHeader*>& next_header_cluster) {
    for (auto it : next_header_cluster) {
        next_header_map.insert(std::make_pair(it->next_header_id, it));
    }
}

void Parser::addNextHeader(NextHeader *nextHeader) {
    next_header_map.insert(std::make_pair(nextHeader->next_header_id, nextHeader));
}

int Parser::getHeaderIdByName(std::string header_name) {
    for(auto it : header_name_id_map) {
        if(it.first == header_name){
            return it.second;
        }
    }
    return 0;
}

int Parser::getFieldIdByName(char *field_name) {
    for(auto it : field_name_id_map) {
        if(it.first == field_name) {
            return it.second;
        }
    }
    return 0;
}

void Parser::printHeaderNameIDMap() {
    for(auto it : header_name_id_map) {
        std::cout << it.first << " : " << it.second << std::endl;
    }
}

void Parser::setHeaderMap(std::unordered_map<int, Header *> headerMap) {
    this->header_map = std::move(headerMap);
}

void Parser::setFieldMap(std::unordered_map<int, Field *> fieldMap) {
    this->field_map = std::move(fieldMap);
}

void Parser::setNextHeaderMap(std::unordered_map<int, NextHeader *> nextHeaderMap) {
    this->next_header_map = std::move(nextHeaderMap);
}

void Parser::printHeaderInfo() {
    for(auto it : header_map) {
        auto header = it.second;
        std::cout << "********* " << header->header_id << ":" << header->header_name << " *********" << std::endl;
        std::cout << "next_header_type_length: " << header->next_header_type_length << std::endl;
        std::cout << "next_header_type_internal_offset: " << header->next_header_type_internal_offset << std::endl;
        std::cout << "FATHERINFO: " << std::endl;
        std::cout << "\theader_tag: " << header->header_tag << std::endl;
        std::cout << "\tfather_header_id: ";
        for(auto ele : header->father_header_ids) {
            std::cout << ele << "; ";
        }
        std::cout << std::endl;
        std::cout << "FIELD: " << std::endl;
        for(auto ele : header->fields) {
            std::cout << "\t" << field_map[ele]->field_id << " " << field_map[ele]->field_name << " " << field_map[ele]->field_length << std::endl;
        }
        std::cout << "NEXTHEADER: " << std::endl;
        for(auto ele : header->next_header_ids) {
            std::cout << "\t" << next_header_map[ele]->next_header_id
                      << " " << next_header_map[ele]->next_header_name << " "
                      << next_header_map[ele]->next_header_tag << std::endl;
        }
    }
}

void Parser::fillFatherHeaderInfo() {
    for(auto it : next_header_map) {
        auto nh = it.second;
        header_map[nh->next_header_id]->header_tag = nh->next_header_tag;
    }

    for(auto it : header_map) {
        auto header = it.second;
        for(auto ele : header->next_header_ids) {
            header_map[ele]->father_header_ids.push_back(header->header_id);
        }
    }
}

/**
 * field should be added after header insertion
 * @param field
 */
void Parser::addField(Field *field) {
    this->field_map.insert(std::make_pair(field->field_id, field));
    field_name_id_map.insert(std::make_pair(field->field_name, field->field_id));
}

int Parser::getMaxFieldNo() {
    int max = 0;
    for(auto it : field_map) {
        if(it.first > max) {
            max = it.first;
        }
    }
    return max;
}

//void Parser::initializeFromFile(const std::string& filename) {
//    std::vector<Field*> fields;
//    std::vector<Header*> headers;
//    std::vector<NextHeader*> next_headers;
//
//    std::ifstream in("../config/header.txt");
//    //std::string s;
//    char s[255];
//
//    int header_id;
//    int field_num;
//    int header_length;
//    char header_name[30];
//
//    int field_id;
//    int field_length;
//    int field_internal_offset;
//    char field_name[24];
//
//    int next_header_type_internal_offset;
//    int next_header_type_length;
//
//    char next_header_tag[50];
//    int next_header_id;
//    char next_header_name[50];
//
//    std::unordered_map<int, Header*> header_map;
//    std::unordered_map<int, Field*> field_map;
//    std::unordered_map<int, NextHeader*> next_header_map;
//
//    while (in.getline(s, 255)) {
//        if (s[0] == '\0')
//            continue;
//
//        if (s[0] == 'h') {
//            sscanf(s, "h %d %d %d %s", &header_id, &field_num, &header_length, header_name);
//            auto h = new Header();
//            h->header_id = header_id;
//            h->setHeaderName(header_name);
//            h->header_length = header_length;
//            headers.push_back(h);
//            header_map.insert(std::make_pair(header_id, h));
//        } else if (s[0] == 'f'){
//            sscanf(s, "f %d.%d %d %d %s", &header_id, &field_id, &field_length, &field_internal_offset, field_name);
//            auto f = new Field();
//            f->field_length = field_length;
//            f->field_id = field_id;
//            f->field_internal_offset = field_internal_offset;
////            std::strcpy(f->field_name, field_name);
//            f->field_name = field_name;
//            fields.push_back(f);
//            field_map.insert(std::make_pair(field_id, f));
//            header_map[header_id]->fields.push_back(field_id);
//        } else if(s[0] == 'n') {
//            sscanf(s, "n.%d %d %d", &header_id, &next_header_type_internal_offset, &next_header_type_length);
//            if (next_header_type_length == 0)
//                continue;
//            header_map[header_id]->next_header_type_internal_offset = next_header_type_internal_offset;
//            header_map[header_id]->next_header_type_length = next_header_type_length;
//        } else if(s[0] == 'm') {
//            sscanf(s, "m.%d %s %d %s", &header_id, next_header_tag, &next_header_id, next_header_name);
//            //std::cout << next_header_tag << std::endl;
//            if(std::string(next_header_tag) == "0")
//                continue;
//            header_map[header_id]->next_header_ids.push_back(next_header_id);
//            auto nh = new NextHeader();
//            nh->next_header_id = next_header_id;
//            //nh->header_id = header_id;
////            std::strcpy(nh->next_header_tag, next_header_tag);
////            std::strcpy(nh->next_header_name, next_header_name);
//            nh->next_header_tag = next_header_tag;
//            nh->next_header_name = next_header_name;
//            next_header_map.insert(std::make_pair(next_header_id, nh));
//            next_headers.push_back(nh);
//        }else {
//            std::cout << s << std::endl;
//        }
//    }
//}


/**
 * with pre-defined header_id and field_id
 * @param filename
 */
void Parser::initializeFromPlainFile(const std::string& filename) {

    std::ifstream in(filename);
    //std::string s;
    char s[255];

    int header_id;
    int field_num;
    int header_length;
    char header_name[30];

    int field_id;
    int field_length;
    int field_internal_offset;
    char field_name[24];

    int next_header_type_internal_offset;
    int next_header_type_length;

    char next_header_tag[50];
    int next_header_id;
    char next_header_name[50];

    while (in.getline(s, 255)) {
        if (s[0] == '\0')
            continue;

        if (s[0] == 'h') {
            sscanf(s, "h %d %d %d %s", &header_id, &field_num, &header_length, header_name);
            auto h = new Header();
            h->header_id = header_id;
            h->setHeaderName(header_name);
            h->header_length = header_length;

            //this->addHeaderBeforeRuntime(h);

            header_name_id_map.insert(std::make_pair(h->header_name, h->header_id));
            header_map.insert(std::make_pair(h->header_id, h));
        } else if (s[0] == 'f'){
            sscanf(s, "f %d.%d %d %d %s", &header_id, &field_id, &field_length, &field_internal_offset, field_name);
            auto f = new Field();
            f->field_length = field_length;
            f->field_id = field_id;
            f->field_internal_offset = field_internal_offset;
            //std::strcpy(f->field_name, field_name);
            f->field_name = field_name;
            //addField(f);
            //std::cout << header_id << " : " << field_id << std::endl;

            this->field_map.insert(std::make_pair(f->field_id, f));
            field_name_id_map.insert(std::make_pair(f->field_name, f->field_id));

            header_map[header_id]->fields.push_back(field_id); //TODO: test
        } else if(s[0] == 'n') {
            sscanf(s, "n.%d %d %d", &header_id, &next_header_type_internal_offset, &next_header_type_length);
            if (next_header_type_length == 0)
                continue;
            header_map[header_id]->next_header_type_internal_offset = next_header_type_internal_offset;
            header_map[header_id]->next_header_type_length = next_header_type_length;
        } else if(s[0] == 'm') {
            sscanf(s, "m.%d %s %d %s", &header_id, next_header_tag, &next_header_id, next_header_name);
            if(std::string(next_header_tag) == "0")
                continue;
            header_map[header_id]->next_header_ids.push_back(next_header_id);

            auto nh = new NextHeader();
            nh->next_header_id = next_header_id;
            nh->next_header_tag = next_header_tag;
            nh->next_header_name = next_header_name;
            next_header_map.insert(std::make_pair(next_header_id, nh));
        }else {
            std::cout << s << std::endl;
        }
    }
}

void Parser::initializeFromJson(const std::string &json_filename) {
    FILE* fp = fopen(json_filename.c_str(), "rb");
    char readBufer[65536];
    rapidjson::FileReadStream is(fp, readBufer, sizeof(readBufer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    for(auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
        auto h = new Header();
        h->header_id = cur_header_idx++;
        h->header_name = it->value["header_name"].GetString();
        h->header_length = it->value["header_length"].GetInt();
        h->next_header_type_internal_offset = it->value["next_header_type_internal_offset"].GetInt();
        h->next_header_type_length = it->value["next_header_type_length"].GetInt();

        this->header_map.insert(std::make_pair(h->header_id, h));
        this->header_name_id_map.insert(std::make_pair(h->header_name, h->header_id));

        for(auto ele = it->value["fields"].MemberBegin(); ele != it->value["fields"].MemberEnd(); ++ele){
            auto f = new Field();
            f->field_name = ele->value["field_name"].GetString();
            f->field_id = cur_field_idx++;
            f->field_length = ele->value["field_length"].GetInt();
            f->field_internal_offset = ele->value["field_internal_offset"].GetInt();

            this->field_map.insert(std::make_pair(f->field_id, f));
            field_name_id_map.insert(std::make_pair(f->field_name, f->field_id));
            h->fields.push_back(f->field_id);
        }

        //TODO:
        // 1. add header_tag/father_ids
        // 2. initiate next_header_id using header name
        for(auto ele = it->value["next_headers"].MemberBegin(); ele != it->value["next_headers"].MemberEnd(); ++ele) {
            auto nh = new NextHeader();
            nh->next_header_name = ele->value["header_name"].GetString();
            nh->next_header_tag = ele->value["header_tag"].GetString();

            next_header_name_map.insert(std::make_pair(nh->next_header_name, nh));
            h->next_header_names.push_back(nh->next_header_name);// TODO: transfer name to id
        }
    }

    // initiate next_header_id using header name
    for(auto it : next_header_name_map) {
        auto next_header_id = this->getHeaderIdByName(it.first);
        it.second->next_header_id = next_header_id;
        this->next_header_map.insert(std::make_pair(next_header_id, it.second));
    }

    // add header tag
    for(auto it : next_header_map) {
        auto nh = it.second;
        header_map[it.first]->header_tag = nh->next_header_tag;
    }

    // add header father_id
    for(auto it : header_map) {
        auto header = it.second;
        for(auto ele : header->next_header_names) {
            auto next_header_id = this->getHeaderIdByName(ele); // transfer name to ID
            header->next_header_ids.push_back(next_header_id);
            header_map[next_header_id]->father_header_ids.push_back(header->header_id);
        }
    }
}

//void Parser::initializeFromJson(const std::string &json_filename) {
//    Json::Reader reader;
//    Json::Value root;
//    std::ifstream ifs("../config/header.json");
//
//    if(!reader.parse(ifs, root, false)){
//        std::cout << "reader parse error: " << errno << std::endl;
//    }
//
//    int size = root.size();
//    std::cout << size << std::endl;
//
//    for(auto it : root){
//        auto h = new Header();
//        h->header_id = cur_header_idx++;
//        h->header_name = it["header_name"].asString();
//        std::cout << h->header_name << std::endl;
//        h->header_length = it["header_length"].asInt();
//        h->next_header_type_internal_offset = it["next_header_type_internal_offset"].asInt();
//        h->next_header_type_length = it["next_header_type_length"].asInt();
//
//        this->header_map.insert(std::make_pair(h->header_id, h));
//        this->header_name_id_map.insert(std::make_pair(h->header_name, h->header_id));
//
//        for(auto field : it["fields"]) {
//            auto f = new Field();
//            f->field_name = field["field_name"].asString();
//            f->field_id = cur_field_idx++;
//            f->field_length = field["field_length"].asInt();
//            f->field_internal_offset = field["field_internal_offset"].asInt();
//
//            this->field_map.insert(std::make_pair(f->field_id, f));
//            field_name_id_map.insert(std::make_pair(f->field_name, f->field_id));
//            h->fields.push_back(f->field_id);
//        }
//
//        //TODO:
//        // 1. add header_tag/father_ids
//        // 2. initiate next_header_id using header name
//        for(auto next_header : it["next_headers"]) {
//            auto nh = new NextHeader();
//            nh->next_header_name = next_header["header_name"].asString();
//            nh->next_header_tag = next_header["header_tag"].asString();
//
//            next_header_name_map.insert(std::make_pair(nh->next_header_name, nh));
//            h->next_header_names.push_back(nh->next_header_name);// TODO: transfer name to id
//        }
//    }
//
//    // initiate next_header_id using header name
//    for(auto it : next_header_name_map) {
//        auto next_header_id = this->getHeaderIdByName(it.first);
//        it.second->next_header_id = next_header_id;
//        this->next_header_map.insert(std::make_pair(next_header_id, it.second));
////        std::cout << it.first << " : " << next_header_id << std::endl;
//    }
//
//    // add header tag
//    for(auto it : next_header_map) {
////        std::cout << it.first << " : " << it.second->next_header_name << std::endl;
//        auto nh = it.second;
//        header_map[it.first]->header_tag = nh->next_header_tag;
//    }
//
//    // add header father_id
//    for(auto it : header_map) {
//        auto header = it.second;
//        for(auto ele : header->next_header_names) {
//            auto next_header_id = this->getHeaderIdByName(ele);
//            header->next_header_ids.push_back(next_header_id);
//            header_map[next_header_id]->father_header_ids.push_back(header->header_id);
//        }
//
////        for(auto ele : header->next_header_ids) {
////            header_map[ele]->father_header_ids.push_back(header->header_id);
////        }
//    }
//}


#endif //GRPC_TEST_PARSER_H
