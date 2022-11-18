//
// Created by fengyong on 2021/2/14.
//

#include <fstream>
#include <iostream>
#include <cstring>

#include "../include/parser.h"

int main() {

//    auto p = new Parser();
//    p->initializeFromPlainFile("../config/header.txt");
//    p->fillFatherHeaderInfo();
//    p->printHeaderInfo();

    auto parser = new Parser();
    parser->initializeFromJson("../config/header.json");
    parser->printHeaderInfo();

//    auto p = new Parser();
//    p->initializeFromFile1("../config/header.txt");
//    p->fillFatherHeaderInfo();
//    p->printHeaderInfo();


    auto mpls = new Header();
    mpls->header_name = "mpls";
    mpls->header_id = 9;
    mpls->header_length = 32;
    mpls->header_tag = "0x8847";
    mpls->father_header_ids = {1,2};

    std::vector<int> field_ids;
    auto f1 = new Field();
    f1->field_id = parser->getMaxFieldNo() + 1;
    f1->field_length = 20;
    f1->field_name = "label";
    f1->field_internal_offset = 0;
    parser->addField(f1);
    field_ids.push_back(f1->field_id);

    auto f2 = new Field();
    f2->field_id = parser->getMaxFieldNo() + 1;
    f2->field_length = 3;
    f2->field_name = "exp";
    f2->field_internal_offset = 20;
    parser->addField(f2);
    field_ids.push_back(f2->field_id);

    auto f3 = new Field();
    f3->field_id = parser->getMaxFieldNo() + 1;
    f3->field_length = 1;
    f3->field_name = "s";
    f3->field_internal_offset = 23;
    parser->addField(f3);
    field_ids.push_back(f3->field_id);

    auto f4 = new Field();
    f4->field_id = parser->getMaxFieldNo() + 1;
    f4->field_length = 8;
    f4->field_name = "ttl";
    f4->field_internal_offset = 24;
    parser->addField(f4);
    field_ids.push_back(f4->field_id);

    mpls->fields = field_ids;
    parser->addHeaderAtRuntime(mpls);
    std::cout << "------------------------------" << std::endl;
    parser->printHeaderInfo();

    return 0;

#ifdef READ_FILE
    Header ethernet = {1, "ethernet", std::vector<int>{},
                       "", 112, 96, 16,
                       std::vector<int>{1,2,3}, std::vector<int>{2,3,4,5}};

    Header vlan = {2, "vlan", std::vector<int>{1, 2},
                   "0x8100", 32, 16, 16,
                   std::vector<int>{4,5,6,7}, std::vector<int>{2,3,4,5}};

    Header ipv4 = {3, "ipv4", std::vector<int>{1,2},
                   "0x0800", -9, 72, 8,
                   std::vector<int>{8,9,10,11,12,13,14,15,16,17,18,19,20}, std::vector<int>{6,7}};

    Header ipv6 = {4, "ipv6", std::vector<int>{1,2},
                   "0x86DD", 320, 48, 8,
                   std::vector<int>{21,22,23,24,25,26,27,28}, std::vector<int>{6,7}};

    Header arp = {5, "arp", std::vector<int>{1,2},
                  "0x0806", 224, 0, 0,
                  std::vector<int>{29,30,31,32,33,34,35,36,37}, std::vector<int>{}};

    Header rarp = {6, "rarp", std::vector<int>{1,2},
                  "0x8035", 224, 0, 0,
                  std::vector<int>{38,39,40,41,42,43,44,45,46}, std::vector<int>{}};


    Header tcp = {7, "tcp", std::vector<int>{3,4},
                  "6", 160, 0, 0,
                  std::vector<int>{47,48,49,50,51,52,53,54,55,56,57,58,59,60,61}, std::vector<int>{}};

    Header udp = {8, "udp", std::vector<int>{3,4},
                  "17", 64, 0, 0,
                  std::vector<int>{62,63,64,65}, std::vector<int>{}};

    std::vector<Header> headers {ethernet, vlan, ipv4, ipv6, arp, rarp, tcp, udp};
    p->setHeaders(headers);


    auto p = new Parser();

    std::vector<Field*> fields;
    std::vector<Header*> headers;
    std::vector<NextHeader*> next_headers;

    std::ifstream in("../config/header.txt");
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

    std::unordered_map<int, Header*> header_map;
    std::unordered_map<int, Field*> field_map;
    std::unordered_map<int, NextHeader*> next_header_map;

    while (in.getline(s, 255)) {
        if (s[0] == '\0')
            continue;

        if (s[0] == 'h') {
            sscanf(s, "h %d %d %d %s", &header_id, &field_num, &header_length, header_name);
            auto h = new Header();
            h->setHeaderId(header_id);
            h->setHeaderName(header_name);
            h->setHeaderLength(header_length);
            headers.push_back(h);
            header_map.insert(std::make_pair(header_id, h));
        } else if (s[0] == 'f'){
            sscanf(s, "f %d.%d %d %d %s", &header_id, &field_id, &field_length, &field_internal_offset, field_name);
            auto f = new Field();
            f->field_length = field_length;
            f->field_id = field_id;
            f->field_internal_offset = field_internal_offset;
            std::strcpy(f->field_name, field_name);
            fields.push_back(f);
            field_map.insert(std::make_pair(field_id, f));
            header_map[header_id]->fields.push_back(field_id);
        } else if(s[0] == 'n') {
            sscanf(s, "n.%d %d %d", &header_id, &next_header_type_internal_offset, &next_header_type_length);
            if (next_header_type_length == 0)
                continue;
            header_map[header_id]->next_header_type_internal_offset = next_header_type_internal_offset;
            header_map[header_id]->next_header_type_length = next_header_type_length;
        } else if(s[0] == 'm') {
            sscanf(s, "m.%d %s %d %s", &header_id, next_header_tag, &next_header_id, next_header_name);
            //std::cout << next_header_tag << std::endl;
            if(std::string(next_header_tag) == "0")
                continue;
            header_map[header_id]->next_header_ids.push_back(next_header_id);
            auto nh = new NextHeader();
            nh->next_header_id = next_header_id;
            //nh->header_id = header_id;
            std::strcpy(nh->next_header_tag, next_header_tag);
            std::strcpy(nh->next_header_name, next_header_name);
            next_header_map.insert(std::make_pair(next_header_id, nh));
            next_headers.push_back(nh);
        }else {
            std::cout << s << std::endl;
        }
    }
    p->setFields(fields);
    p->setHeaders(headers);
    p->setNextHeaders(next_headers);
    p->setHeaderMap(header_map);
    p->setFieldMap(field_map);
    p->setNextHeaderMap(next_header_map);
    p->fillFatherHeaderInfo();
    p->printHeaderInfo();


    auto mpls = new Header();
    strcpy(mpls->header_name, "mpls");
    mpls->header_id = 9;
    mpls->header_length = 32;
    strcpy(mpls->header_tag, "0x8847");
    mpls->father_header_ids = {1,2};

    std::vector<int> field_ids;
    auto f1 = new Field();
    f1->field_id = p->getMaxFieldNo() + 1;
    f1->field_length = 20;
    strcpy(f1->field_name, "label");
    f1->field_internal_offset = 0;
    p->addField(f1);
    field_ids.push_back(f1->field_id);

    auto f2 = new Field();
    f2->field_id = p->getMaxFieldNo() + 1;
    f2->field_length = 3;
    strcpy(f2->field_name, "exp");
    f2->field_internal_offset = 20;
    p->addField(f2);
    field_ids.push_back(f2->field_id);

    auto f3 = new Field();
    f3->field_id = p->getMaxFieldNo() + 1;
    f3->field_length = 1;
    strcpy(f3->field_name, "s");
    f3->field_internal_offset = 23;
    p->addField(f3);
    field_ids.push_back(f3->field_id);

    auto f4 = new Field();
    f4->field_id = p->getMaxFieldNo() + 1;
    f4->field_length = 8;
    strcpy(f4->field_name, "ttl");
    f4->field_internal_offset = 24;
    p->addField(f4);
    field_ids.push_back(f4->field_id);

    mpls->fields = field_ids;
    p->addHeader(mpls);

    //p->fillFatherHeaderInfo();
    p->printHeaderInfo();

    return 0;
    //initializeParserWithHeaderFile();
    //addHeaderInFSM();
#endif
}
