#include "ast.h"
#include <iostream>

namespace ast {

std::unique_ptr<ast::RP4AST> tree;

std::string get_offset(int width) {
    return std::string(width, ' ');
}

template<typename T>
void print_basic(const T &val, int tab) {
    auto offset = get_offset(tab);
    std::cout << offset << val << '\n';
}

void print_node(const TreeNode *node, int tab) {
    auto offset = get_offset(tab);
    tab += 2;
    if (node == nullptr) {
        std::cout << offset << "NULL\n";
    } else if (auto x = dynamic_cast<const RP4AST *>(node)) {
        std::cout << offset << "PROGRAM\n";
        // headers
        for (auto &hdr: x->headers_def) {
            print_node(&hdr, tab);
        }
        // structs
        for (auto &st: x->structs_def) {
            print_node(&st, tab);
        }
        // header sequence
        print_node(&x->header_seq, tab);
        // action
        for (auto &action: x->actions_def) {
            print_node(&action, tab);
        }
        // tables
        for (auto &table: x->tables_def) {
            print_node(&table, tab);
        }
        // controls
        for (auto &ctrl: x->controls_def) {
            print_node(&ctrl, tab);
        }
    } else if (auto x = dynamic_cast<const HeaderDef *> (node)) {
        std::cout << offset << "HEADER " << x->name << "\n";
        for (auto &field: x->fields) {
            print_node(&field, tab);
        }
        print_node(&x->next_table, tab);
    } else if (auto x = dynamic_cast<const FieldDef *> (node)) {
        std::cout << offset << "FIELD\n";
        print_node(x->type.get(), tab);
        print_basic(x->name, tab);
    } else if (auto x = dynamic_cast<const BitType *>(node)) {
        std::cout << offset << "BIT<" << x->len << ">\n";
    } else if (auto x = dynamic_cast<const RegisterType *>(node)) {
        std::cout << offset << "REGISTER\n";
        print_node(x->type.get(), tab);
        print_basic(x->size, tab);
    } else if (auto x = dynamic_cast<const NamedType *>(node)) {
        std::cout << offset << x->type_name << "\n";
    } else if (auto x = dynamic_cast<const NextTable *>(node)) {
        std::cout << offset << "NEXT_TABLE: " << x->tag_name << "\n";
        for (auto &next_entry: x->table) {
            print_node(&next_entry, tab);
        }
    } else if (auto x = dynamic_cast<const NextTableEntry *> (node)) {
        if (x->val.empty()) {
            std::cout << offset << "default: accept\n";
        } else {
            std::cout << offset << std::hex << x->key << std::dec << ": " << x->val << '\n';
        }
    } else if (auto x = dynamic_cast<const StructDef *> (node)) {
        std::cout << offset << "STRUCT " << x->type_name << " " << x->var_name << "\n";
        for (auto &field: x->fields) {
            print_node(&field, tab);
        }
    } else if (auto x = dynamic_cast<const HeaderSequence *> (node)) {
        std::cout << offset << "HEADER_SEQUENCE " << x->name << "\n";
        for (auto &field: x->fields) {
            print_node(&field, tab);
        }
    } else if (auto x = dynamic_cast<const ActionDef *> (node)) {
        std::cout << offset << "ACTION " << x->name << "\n";
        // args
        for (auto &arg: x->args) {
            print_node(&arg, tab);
        }
        // statements
        for (auto &stmt: x->stmts) {
            print_node(stmt.get(), tab);
        }
    } else if (auto x = dynamic_cast<const ExprStmt *>(node)) {
        std::cout << offset << "STATEMENT\n";
        print_node(x->expr.get(), tab);
    } else if (auto x = dynamic_cast<const GotoStmt *> (node)) {
        std::cout << offset << "GOTO " << x->tag << "\n";
    } else if (auto x = dynamic_cast<const AssignStmt *> (node)) {
        std::cout << offset << "ASSIGN\n";
        print_node(x->lhs.get(), tab);
        print_node(x->rhs.get(), tab);
    } else if (auto x = dynamic_cast<const IntLit *>(node)) {
        std::cout << offset << "INT_LIT " << x->val << "\n";
    } else if (auto x = dynamic_cast<const BinaryExpr *> (node)) {
        std::cout << offset << "BINARY OP " << to_string(x->op) << "\n";
        print_node(x->lhs.get(), tab);
        print_node(x->rhs.get(), tab);
    } else if (auto x = dynamic_cast<const MemberOf *> (node)) {
        std::cout << offset << "MEMBER_OF\n";
        print_node(x->recv.get(), tab);
        print_basic(x->name, tab);
    } else if (auto x = dynamic_cast<const CallExpr *> (node)) {
        std::cout << offset << "CALL\n";
        print_node(x->caller.get(), tab);
        for (auto &param: x->params) {
            print_node(param.get(), tab);
        }
    } else if (auto x = dynamic_cast<const TableDef *> (node)) {
        std::cout << offset << "TABLE " << x->name << " " << x->size << "\n";
        for (auto &key: x->match_keys) {
            print_node(&key, tab);
        }
    } else if (auto x = dynamic_cast< const MatchKey *> (node)) {
        std::cout << offset << "KEY\n";
        print_node(x->key.get(), tab);
        print_basic(to_string(x->match_type), tab);
    } else if (auto x = dynamic_cast<const ControlDef *> (node)) {
        std::cout << offset << "CONTROL " << x->name << "\n";
        for (auto &stage: x->stages) {
            print_node(&stage, tab);
        }
    } else if (auto x = dynamic_cast<const StageDef *> (node)) {
        std::cout << offset << "STAGE " << x->name << "\n";
        std::cout << offset << "  PARSER " << x->parser << "\n";
        print_node(x->matcher.get(), tab);
        std::cout << offset << "  EXECUTOR\n";
        for (auto &act: x->executor) {
            std::cout << offset << "    " << act.first << " " << act.second << "\n";
        }
    } else if (auto x = dynamic_cast<const MatcherDefTable *>(node)) {
        std::cout << offset << "MATCHER " << x->table_name << "\n";
    } else if (auto x = dynamic_cast<const MatcherDefCond *>(node)) {
        std::cout << offset << "MATCHER\n";
        for (auto &cond : x->conds) {
            print_node(cond.get(), tab);
        }
    } else {
        throw std::runtime_error("Unexpected type of AST node");
    }
}

void print_tree(const ast::RP4AST *p) {
    print_node(p, 0);
}

}