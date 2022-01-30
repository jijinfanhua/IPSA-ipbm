#pragma once
#include "rp4_ast.h"
#include <cstdio>
#include <cstring>

std::unique_ptr<Rp4Ast> tree;

std::string get_offset(int width) {
    return std::string(width, ' ');
}

void print_node(const Rp4TreeNode* node, int tab) {
    auto offset = get_offset(tab);
    std::cout << offset << node->toString() << std::endl;
    tab += 2;
    auto x = node->children();
    for (auto c : node->children()) {
        print_node(c, tab);
    }
}

void print_tree(const Rp4Ast* ast) {
    print_node(ast, 0);
}

int yyparse();
extern FILE* yyin;

class Rp4Compiler {
public:
    std::unique_ptr<Rp4Ast> ast;

    int parse(const char* filename) {
        FILE* fp = fopen(filename, "r");
        yyin = fp;
        int ret = yyparse();
        ast = std::move(tree);
        fclose(fp);
        return ret;
    }

    void print(const char* filename) {
        if (strlen(filename) > 0) {
            FILE* fp = freopen(filename, "w", stdout);
            print_tree(ast.get());
            fclose(fp);
        } else {
            print_tree(ast.get());
        }
    }
};