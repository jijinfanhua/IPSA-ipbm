#pragma once

#include "ast.h"
#include <cstdio>

int yyparse();

extern FILE *yyin;

class Parser {
public:
    std::unique_ptr<ast::RP4AST> tree;

    int parse(const char *filename) {
        FILE *fp = fopen(filename, "r");
        yyin = fp;
        int ret = yyparse();
        tree = std::move(ast::tree);
        fclose(fp);
        return ret;
    }

    void print() const {
        print_tree(tree.get());
    }
};
