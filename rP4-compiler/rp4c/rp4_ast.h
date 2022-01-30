#pragma once

#include "rp4_header.h"
#include "rp4_struct.h"
#include "rp4_parser.h"
#include "rp4_action.h"
#include "rp4_table.h"
#include "rp4_pipeline.h"

class Rp4SemValue {
public:
    int sv_int;
    std::string sv_str;

    Rp4HeaderDef sv_header_def;
    std::vector<Rp4HeaderDef> sv_header_defs;
    Rp4FieldDef sv_field_def;
    std::vector<Rp4FieldDef> sv_field_defs;
    std::shared_ptr<Rp4Type> sv_type;
    Rp4StructDef sv_struct_def;
    std::vector<Rp4StructDef> sv_struct_defs;
    Rp4ParserDef sv_parser_def;
    Rp4StateDef sv_state_def;
    std::vector<Rp4StateDef> sv_state_defs;
    Rp4Extract sv_extract_stmt;
    std::shared_ptr<Rp4Transition> sv_transition_stmt;
    Rp4Member sv_member;
    Rp4Field sv_field;
    std::vector<Rp4Field> sv_fields;
    std::shared_ptr<Rp4Key> sv_key;
    std::vector<std::shared_ptr<Rp4Key>> sv_keys;
    Rp4TransitionEntry sv_transition_entry;
    std::vector<Rp4TransitionEntry> sv_transition_entries;
    Rp4ActionsDef sv_actions_def;
    Rp4ActionDef sv_action_def;
    std::vector<Rp4ActionDef> sv_action_defs;
    Rp4Expression sv_expression;
    std::vector<Rp4Expression> sv_expressions;
    std::shared_ptr<Rp4LValue> sv_lvalue;
    std::shared_ptr<Rp4Operation> sv_operation;
    std::vector<std::shared_ptr<Rp4Operation>> sv_operations;
    std::shared_ptr<Rp4Operand> sv_operand;
    Rp4Operator sv_op;
    Rp4TablesDef sv_tables_def;
    Rp4TableDef sv_table_def;
    std::vector<Rp4TableDef> sv_table_defs;
    Rp4TableKeyDef sv_table_key_def;
    Rp4KeyEntry sv_key_entry;
    std::vector<Rp4KeyEntry> sv_key_entries;
    Rp4MatchType sv_match_type;
    std::shared_ptr<Rp4TableOptionalStmt> sv_table_optional_stmt;
    std::vector<std::shared_ptr<Rp4TableOptionalStmt>> sv_table_optional_stmts;
    Rp4MatchEntry sv_match_entry;
    std::vector<Rp4MatchEntry> sv_match_entries;
    Rp4MatchKey sv_match_key;
    Rp4Pipeline sv_pipeline;
    Rp4StageDef sv_stage_def;
    std::vector<Rp4StageDef> sv_stage_defs;
    Rp4StageParser sv_stage_parser;
    Rp4StageParserHeader sv_stage_parser_header;
    std::vector<Rp4StageParserHeader> sv_stage_parser_headers;
    Rp4StageMatcher sv_stage_matcher;
    Rp4SwitchEntry sv_switch_entry;
    std::vector<Rp4SwitchEntry> sv_switch_entries;
    std::shared_ptr<Rp4SwitchValue> sv_switch_value;
    Rp4StageExecutor sv_stage_executor;
    Rp4StageExecutorEntry sv_stage_executor_entry;
    std::vector<Rp4StageExecutorEntry> sv_stage_executor_entries;
};

class Rp4Ast : public Rp4TreeNode {
public:
    std::vector<Rp4HeaderDef> header_defs;
    std::vector<Rp4StructDef> struct_defs;
    Rp4ParserDef parser_def;
    Rp4ActionsDef actions_def;
    Rp4TablesDef tables_def;
    Rp4Pipeline ingress_def;
    Rp4Pipeline egress_def;
    Rp4Ast(
        std::vector<Rp4HeaderDef> _header_defs,
        std::vector<Rp4StructDef> _struct_defs,
        Rp4ParserDef _parser_def,
        Rp4ActionsDef _actions_def,
        Rp4TablesDef _tables_def,
        Rp4Pipeline _ingress_def,
        Rp4Pipeline _egress_def
        ) : 
        header_defs(std::move(_header_defs)), 
        struct_defs(std::move(_struct_defs)),
        parser_def(std::move(_parser_def)),
        actions_def(std::move(_actions_def)),
        tables_def(std::move(_tables_def)),
        ingress_def(std::move(_ingress_def)),
        egress_def(std::move(_egress_def))
        {}
    
    virtual std::vector<const Rp4TreeNode*> children() const {
        auto dst = mapped(header_defs);
        add(dst, struct_defs);
        add(dst, parser_def);
        add(dst, actions_def);
        add(dst, tables_def);
        add(dst, ingress_def);
        add(dst, egress_def);
        return std::move(dst);
    }
    virtual std::string toString() const {
        return "Rp4Ast";
    }
};

extern std::unique_ptr<Rp4Ast> tree;

#ifndef YYSTYPE
#define YYSTYPE Rp4SemValue
#endif