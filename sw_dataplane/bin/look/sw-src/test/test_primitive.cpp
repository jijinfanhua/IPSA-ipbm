//
// Created by xilinx_0 on 1/24/22.
//

#include "../dataplane/executor.h"

int main() {
    ExpTreeNode * root = new ExpTreeNode();
    root->opt.type = ParameterType::CONSTANT;
    int a3 = 255;
    uint8_t * data3 = get_uints(a3, 16);
    root->opt.val.constant_data = Data{16, data3};
//    ExpTreeNode * root = new ExpTreeNode();
//    root->opt.type = ParameterType::OPERATOR;
//    root->opt.val.op = OpCode::BIT_AND;
//
//    auto node2 = new ExpTreeNode();
//    node2->opt.type = ParameterType::OPERATOR;
//    node2->opt.val.op = OpCode::ADD;
//
//    auto node3 = new ExpTreeNode();
//    node3->opt.type = ParameterType::CONSTANT;
//    int a3 = 255;
//    uint8_t * data3 = get_uints(a3, 16);
//    node3->opt.val.constant_data = Data{16, data3};
//
////    auto node3 = new ExpTreeNode();
////    node3->opt.type = ParameterType::OPERATOR;
////    node3->opt.val.op = OpCode::BIT_NEG;
////
////    auto node6 = new ExpTreeNode();
////    node6->opt.type = ParameterType::CONSTANT;
////    int a6 = 237;
////    uint8_t * data6 = get_uints(a6, 8); // 18
////    node6->opt.val.constant_data = Data{8, data6};
////
////    node3->left = node6;
//
//    root->left = node2;
//    root->right = node3;
//
//    auto node4 = new ExpTreeNode();
//    node4->opt.type = ParameterType::CONSTANT;
//    int a4 = 5;
//    uint8_t * data4 = get_uints(a4, 16);
//    node4->opt.val.constant_data = Data{16, data4};
//
//    auto node5 = new ExpTreeNode();
//    node5->opt.type = ParameterType::CONSTANT;
//    int a5 = 255;
//    uint8_t * data5 = get_uints(a5, 16);
//    node5->opt.val.constant_data = Data{16, data5};
//
//    node2->left = node4;
//    node2->right = node5;

    auto prim = new Prim(root);

    auto phv = new PHV();
    std::vector<ActionParam*> action_paras;

    prim->run(phv, action_paras);

}