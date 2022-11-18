#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace ast {

enum MatchType {
    MATCH_EXACT, MATCH_TERNARY, MATCH_LPM
};

enum BinaryOp {
    OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE
};

static inline std::string to_string(MatchType type) {
    static const std::map<MatchType, std::string> m = {
            {MATCH_EXACT,   "EXACT"},
            {MATCH_TERNARY, "TERNARY"},
            {MATCH_LPM,     "LPM"}
    };
    return m.at(type);
}

static inline std::string to_string(BinaryOp op) {
    static const std::map<BinaryOp, std::string> m = {
            {OP_EQ, "=="},
            {OP_NE, "!="},
            {OP_LT, "<"},
            {OP_GT, ">"},
            {OP_LE, "<="},
            {OP_GE, ">="}
    };
    return m.at(op);
}

// Base class for tree nodes
struct TreeNode {
    virtual ~TreeNode() = default;  // enable polymorphism
};

struct FieldType : public TreeNode {
};

struct BitType : public FieldType {
    int len;

    BitType(int len_) : len(len_) {}
};

struct RegisterType : public FieldType {
    std::shared_ptr<FieldType> type;
    int size;

    RegisterType(std::shared_ptr<FieldType> type_, int size_) :
            type(std::move(type_)), size(size_) {}
};

struct NamedType : public FieldType {
    std::string type_name;

    NamedType(std::string type_name_) : type_name(std::move(type_name_)) {}
};

struct FieldDef : public TreeNode {
    std::shared_ptr<FieldType> type;
    std::string name;

    FieldDef() = default;

    FieldDef(std::shared_ptr<FieldType> type_, std::string name_) : type(std::move(type_)), name(std::move(name_)) {}
};

struct NextTableEntry : TreeNode {
    int key;
    std::string val;

    NextTableEntry() = default;

    NextTableEntry(int key_, std::string val_) : key(key_), val(std::move(val_)) {}
};

struct NextTable : public TreeNode {
    std::string tag_name;
    std::vector<NextTableEntry> table;

    NextTable() = default;

    NextTable(std::string tag_name_, std::vector<NextTableEntry> table_) :
            tag_name(std::move(tag_name_)), table(std::move(table_)) {}
};

struct HeaderDef : public TreeNode {
    std::string name;
    std::vector<FieldDef> fields;
    NextTable next_table;

    HeaderDef() = default;

    HeaderDef(std::string name_, std::vector<FieldDef> fields_, NextTable next_table_) :
            name(std::move(name_)), fields(std::move(fields_)), next_table(std::move(next_table_)) {}
};

struct StructDef : public TreeNode {
    std::string type_name;
    std::vector<FieldDef> fields;
    std::string var_name;

    StructDef() = default;

    StructDef(std::string type_name_, std::vector<FieldDef> fields_, std::string var_name_) : type_name(
            std::move(type_name_)), fields(std::move(fields_)), var_name(std::move(var_name_)) {}
};

struct HeaderSequence : public TreeNode {
    std::vector<FieldDef> fields;
    std::string name;

    HeaderSequence() = default;

    HeaderSequence(std::vector<FieldDef> fields_, std::string name_) : fields(std::move(fields_)),
                                                                       name(std::move(name_)) {}
};

struct Expr : public TreeNode {
};

struct IntLit : public Expr {
    int val;

    IntLit(int val_) : val(val_) {}
};

struct BinaryExpr : public Expr {
    BinaryOp op;
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;

    BinaryExpr(BinaryOp op_, std::shared_ptr<Expr> lhs_, std::shared_ptr<Expr> rhs_) : op(op_), lhs(std::move(lhs_)),
                                                                                       rhs(std::move(rhs_)) {}
};

struct LValue : public Expr {
};

struct MemberOf : public LValue {
    std::shared_ptr<Expr> recv;
    std::string name;

    MemberOf(std::shared_ptr<Expr> recv_, std::string name_) : recv(std::move(recv_)), name(std::move(name_)) {}
};

struct CallExpr : public Expr {
    std::shared_ptr<Expr> caller;
    std::vector<std::shared_ptr<Expr>> params;

    CallExpr(std::shared_ptr<Expr> caller_, std::vector<std::shared_ptr<Expr>> params_) : caller(std::move(caller_)),
                                                                                          params(std::move(params_)) {}
};

struct Stmt : public TreeNode {
};

struct ExprStmt : public Stmt {
    std::shared_ptr<Expr> expr;

    ExprStmt(std::shared_ptr<Expr> expr_) : expr(std::move(expr_)) {}
};

struct GotoStmt : public Stmt {
    std::string tag;

    GotoStmt(std::string tag_) : tag(std::move(tag_)) {}
};

struct AssignStmt : public Stmt {
    std::shared_ptr<LValue> lhs;
    std::shared_ptr<Expr> rhs;

    AssignStmt(std::shared_ptr<LValue> lhs_, std::shared_ptr<Expr> rhs_) : lhs(std::move(lhs_)), rhs(std::move(rhs_)) {}
};

struct ActionDef : public TreeNode {
    std::string name;
    std::vector<FieldDef> args;
    std::vector<std::shared_ptr<Stmt>> stmts;

    ActionDef() = default;

    ActionDef(std::string name_, std::vector<FieldDef> args_, std::vector<std::shared_ptr<Stmt>> stmts_) : name(
            std::move(name_)), args(std::move(args_)), stmts(std::move(stmts_)) {}
};

struct MatchKey : public TreeNode {
    std::shared_ptr<LValue> key;
    MatchType match_type;

    MatchKey() = default;

    MatchKey(std::shared_ptr<LValue> key_, MatchType match_type_) : key(std::move(key_)), match_type(match_type_) {}
};

struct TableDef : public TreeNode {
    std::string name;
    std::vector<MatchKey> match_keys;
    int size;

    TableDef() = default;

    TableDef(std::string name_, std::vector<MatchKey> match_keys_, int size_) :
            name(std::move(name_)), match_keys(std::move(match_keys_)), size(size_) {}
};

struct MatcherDef : public TreeNode {
};

struct MatcherDefTable : public MatcherDef {
    std::string table_name;

    MatcherDefTable(std::string table_name_) : table_name(std::move(table_name_)) {}
};

struct Condition : public TreeNode {
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;

    Condition() = default;

    Condition(std::shared_ptr<Expr> lhs_, std::shared_ptr<Expr> rhs_) :
            lhs(std::move(lhs_)), rhs(std::move(rhs_)) {}
};

struct MatcherDefCond : public MatcherDef {
    std::vector<std::shared_ptr<Expr>> conds;

    MatcherDefCond(std::vector<std::shared_ptr<Expr>> conds_) : conds(std::move(conds_)) {}
};

struct StageDef : public TreeNode {
    std::string name;
    std::string parser;
    std::shared_ptr<MatcherDef> matcher;
    std::vector<std::pair<int, std::string>> executor;

    StageDef() = default;

    StageDef(std::string name_, std::string parser_, std::shared_ptr<MatcherDef> matcher_,
             std::vector<std::pair<int, std::string>> executor_) : name(std::move(name_)), parser(std::move(parser_)),
                                                                   matcher(std::move(matcher_)),
                                                                   executor(std::move(executor_)) {}
};

struct ControlDef : public TreeNode {
    std::string name;
    std::vector<StageDef> stages;

    ControlDef() = default;

    ControlDef(std::string name_, std::vector<StageDef> stages_) : name(std::move(name_)), stages(std::move(stages_)) {}
};

struct RP4AST : public TreeNode {
    // std::shared_ptr<IncludeDef> include_def;
    std::vector<HeaderDef> headers_def;
    std::vector<StructDef> structs_def;
    HeaderSequence header_seq;
    std::vector<ActionDef> actions_def;
    std::vector<TableDef> tables_def;
    std::vector<ControlDef> controls_def;

    RP4AST(std::vector<HeaderDef> headers_def_, std::vector<StructDef> struct_def_, HeaderSequence header_seq_,
           std::vector<ActionDef> actions_def_, std::vector<TableDef> tables_def_,
           std::vector<ControlDef> controls_def_) :
            headers_def(std::move(headers_def_)), structs_def(std::move(struct_def_)),
            header_seq(std::move(header_seq_)),
            actions_def(std::move(actions_def_)),
            tables_def(std::move(tables_def_)), controls_def(std::move(controls_def_)) {}
};

// Semantic value
struct SemValue {
    int sv_int;
    float sv_float;
    std::string sv_str;

    HeaderDef sv_header_def;
    std::vector<HeaderDef> sv_headers_def;
    FieldDef sv_field_def;
    std::vector<FieldDef> sv_fields_def;
    NextTableEntry sv_next_entry;
    std::vector<NextTableEntry> sv_next_entries;
    NextTable sv_next_table;
    std::shared_ptr<FieldType> sv_field_type;
    StructDef sv_struct_def;
    std::vector<StructDef> sv_structs_def;
    HeaderSequence sv_header_seq;
    std::shared_ptr<Expr> sv_expr;
    std::vector<std::shared_ptr<Expr>> sv_exprs;
    ActionDef sv_action;
    std::vector<ActionDef> sv_actions;
    std::shared_ptr<Stmt> sv_stmt;
    std::vector<std::shared_ptr<Stmt>> sv_stmts;
    std::shared_ptr<LValue> sv_lvalue;
    MatchType sv_match_type;
    TableDef sv_table;
    std::vector<TableDef> sv_tables;
    MatchKey sv_match_key;
    std::vector<MatchKey> sv_match_keys;
    std::shared_ptr<MatcherDef> sv_matcher_def;
    ControlDef sv_control;
    std::vector<ControlDef> sv_controls;
    StageDef sv_stage;
    std::vector<StageDef> sv_stages;
    std::vector<std::pair<int, std::string>> sv_action_map;
};

extern std::unique_ptr<ast::RP4AST> tree;

void print_tree(const ast::RP4AST *tree);

}

#define YYSTYPE ast::SemValue
