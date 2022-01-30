%{
#include "ast.h"
#include "yacc.tab.h"
#include <iostream>
#include <memory>

int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}

using namespace ast;
%}

// request a pure (reentrant) parser
%define api.pure full
// enable location in error handler
%locations

// keywords
%token INCLUDE
    STREAM PACKET_SEG PACKET
    HEADERS HDR BIT IMPLICIT PARSER DEFAULT FUNC ACCEPT
    STRUCTS STRUCT REGISTER
    HEADER_SEQUENCE
    ACTIONS ACTION GOTO
    TABLES TABLE EXACT TERNARY LPM KEY SIZE
    CONTROL STAGE MATCHER EXECUTOR

// non-keywords
%token EQU NEQ LEQ GEQ T_EOF

// type-specific tokens
%token <sv_str> IDENTIFIER VALUE_STRING FILENAME
%token <sv_int> VALUE_INT
%token <sv_float> VALUE_FLOAT

// specify types for non-terminal symbol
// %type <sv_include_def> include_def
%type <sv_str> next_tag
%type <sv_headers_def> headers_def header_list
%type <sv_header_def> header_def
%type <sv_field_def> field_def
%type <sv_fields_def> fields_def param_list_opt param_list
%type <sv_next_table> next_table_def;
%type <sv_next_entry> next_table_entry
%type <sv_next_entries> next_table_entries
%type <sv_field_type> type_def
%type <sv_structs_def> structs_def struct_list
%type <sv_struct_def> struct_def
%type <sv_header_seq> header_seq
%type <sv_expr> expr literal receiver
%type <sv_lvalue> lvalue
%type <sv_exprs> expr_list_opt expr_list
%type <sv_action> action_def
%type <sv_actions> actions_def action_list
%type <sv_stmt> stmt
%type <sv_stmts> stmts
%type <sv_match_type> match_type
%type <sv_int> size_def action_map_key
%type <sv_table> table
%type <sv_tables> table_list tables_def
%type <sv_match_key> match_key
%type <sv_match_keys> match_key_list match_keys_def
%type <sv_control> control_def
%type <sv_controls> controls_def
%type <sv_stages> stages_def
%type <sv_stage> stage_def
%type <sv_matcher_def> matcher_def
%type <sv_action_map> action_map

%%
start:
        headers_def structs_def header_seq actions_def tables_def controls_def
    {
        tree = std::make_unique<RP4AST>($1, $2, $3, $4, $5, $6);
        YYACCEPT;
    }
    ;

// header
headers_def:
        HEADERS '{' header_list '}'
    {
        $$ = $3;
    }
    ;

header_list:
        /* epsilon */
    {
        $$ = {};
    }
    |   header_list header_def
    {
        $$.push_back($2);
    }
    ;

header_def:
        HDR IDENTIFIER '{' fields_def next_table_def '}'
    {
        $$ = HeaderDef($2, $4, $5);
    }
    ;

fields_def:
        /* epsilon */
    {
        $$ = {};
    }
    |   fields_def field_def ';'
    {
        $$.push_back($2);
    }
    ;

field_def:
        type_def IDENTIFIER
    {
        $$ = FieldDef($1, $2);
    }
    ;

type_def:
        BIT '<' VALUE_INT '>'
    {
        $$ = std::make_shared<BitType>($3);
    }
    |   REGISTER '<' type_def '>' '(' VALUE_INT ')'
    {
        $$ = std::make_shared<RegisterType>($3, $6);
    }
    |   IDENTIFIER
    {
        $$ = std::make_shared<NamedType>($1);
    }
    ;

next_table_def:
        IMPLICIT PARSER '(' next_tag ')' '{' next_table_entries '}'
    {
        $$ = NextTable($4, $7);
    }
    ;

next_tag:
        /* epsilon */
    {
        $$ = "";
    }
    |   IDENTIFIER
    ;

next_table_entries:
        /* epsilon */
    {
        $$ = {};
    }
    |   next_table_entries next_table_entry
    {
        $$.push_back($2);
    }
    ;

next_table_entry:
        VALUE_INT ':' IDENTIFIER ';'
    {
        $$ = NextTableEntry($1, $3);
    }
    |   DEFAULT ':' ACCEPT ';'
    {
        $$ = NextTableEntry(-1, "");
    }
    ;

// structs
structs_def:
        STRUCTS '{' struct_list '}'
    {
        $$ = $3;
    }
    ;

struct_list:
        /* epsilon */
    {
        $$ = {};
    }
    |   struct_list struct_def
    {
        $$.push_back($2);
    }
    ;

struct_def:
        STRUCT IDENTIFIER '{' fields_def '}' IDENTIFIER ';'
    {
        $$ = StructDef($2, $4, $6);
    }
    ;

// header sequence
header_seq:
        HEADER_SEQUENCE '{' fields_def '}' IDENTIFIER ';'
    {
        $$ = HeaderSequence($3, $5);
    }
    ;

actions_def:
        ACTIONS '{' action_list '}'
    {
        $$ = $3;
    }
    ;

action_list:
        /* epsilon */
    {
        $$ = {};
    }
    |   action_list action_def
    {
        $$.push_back($2);
    }
    ;

action_def:
        ACTION IDENTIFIER '(' param_list_opt ')' '{' stmts '}'
    {
        $$ = ActionDef($2, $4, $7);
    }
    ;

param_list_opt:
        /* epsilon */
    {
        $$ = {};
    }
    |   param_list
    ;

param_list:
        field_def
    {
        $$ = {$1};
    }
    |   param_list ',' field_def
    {
        $$.push_back($3);
    }
    ;

stmts:
        /* epsilon */
    {
        $$ = {};
    }
    |   stmts stmt ';'
    {
        $$.push_back($2);
    }
    ;

stmt:
        /* epsilon */
    {
        $$ = nullptr;
    }
    |   lvalue '=' expr
    {
        $$ = std::make_shared<AssignStmt>($1, $3);
    }
    |   expr
    {
        $$ = std::make_shared<ExprStmt>($1);
    }
    |   GOTO IDENTIFIER
    {
        $$ = std::make_shared<GotoStmt>($2);
    }
    ;

expr:
        literal
    |   lvalue
    {
        $$ = std::static_pointer_cast<Expr>($1);
    }
    |   expr '(' expr_list_opt ')'
    {
        $$ = std::make_shared<CallExpr>($1, $3);
    }
    |   expr EQU expr
    {
        $$ = std::make_shared<BinaryExpr>(OP_EQ, $1, $3);
    }
    |   expr NEQ expr
    {
        $$ = std::make_shared<BinaryExpr>(OP_NE, $1, $3);
    }
    |   expr '<' expr
    {
        $$ = std::make_shared<BinaryExpr>(OP_LT, $1, $3);
    }
    |   expr '>' expr
    {
        $$ = std::make_shared<BinaryExpr>(OP_GT, $1, $3);
    }
    |   expr LEQ expr
    {
        $$ = std::make_shared<BinaryExpr>(OP_LE, $1, $3);
    }
    |   expr GEQ expr
    {
        $$ = std::make_shared<BinaryExpr>(OP_GE, $1, $3);
    }
    ;

lvalue:
        receiver IDENTIFIER
    {
        $$ = std::make_shared<MemberOf>($1, $2);
    }
    ;

receiver:
        /* epsilon */
    {
        $$ = nullptr;
    }
    |   expr '.'
    {
        $$ = $1;
    }
    ;

expr_list_opt:
        /* epsilon */
    {
        $$ = {};
    }
    |   expr_list
    ;

expr_list:
        expr
    {
        $$ = {$1};
    }
    |   expr_list ',' expr
    {
        $$.push_back($3);
    }
    ;

literal:
        VALUE_INT
    {
        $$ = std::make_shared<IntLit>($1);
    }
    ;

// tables
tables_def:
        TABLES '{' table_list '}'
    {
        $$ = $3;
    }
    ;

table_list:
        /* epsilon */
    {
        $$ = {};
    }
    |   table_list table
    {
        $$.push_back($2);
    }
    ;

table:
        TABLE IDENTIFIER '{' match_keys_def size_def '}'
    {
        $$ = TableDef($2, $4, $5);
    }
    ;

match_keys_def:
        KEY '=' '{' match_key_list '}'
    {
        $$ = $4;
    }
    ;

match_key_list:
        /* epsilon */
    {
        $$ = {};
    }
    |   match_key_list match_key ';'
    {
        $$.push_back($2);
    }
    ;

match_key:
        lvalue ':' match_type
    {
        $$ = MatchKey($1, $3);
    }
    ;

match_type:
        EXACT
    {
        $$ = MATCH_EXACT;
    }
    |   TERNARY
    {
        $$ = MATCH_TERNARY;
    }
    |   LPM
    {
        $$ = MATCH_LPM;
    }
    ;

size_def:
        SIZE '=' VALUE_INT ';'
    {
        $$ = $3;
    }
    ;

// control
controls_def:
        /* epsilon */
    {
        $$ = {};
    }
    |   controls_def control_def
    {
        $$.push_back($2);
    }
    ;

control_def:
        CONTROL IDENTIFIER '{' stages_def '}'
    {
        $$ = ControlDef($2, $4);
    }
    ;

stages_def:
        /* epsilon */
    {
        $$ = {};
    }
    |   stages_def stage_def
    {
        $$.push_back($2);
    }
    ;

stage_def:
        STAGE IDENTIFIER '{'
            PARSER ':' IDENTIFIER ';'
            MATCHER ':' matcher_def ';'
            EXECUTOR '{' action_map '}'
        '}'
    {
        $$ = StageDef($2, $6, $10, $14);
    }
    ;

matcher_def:
        IDENTIFIER
    {
        $$ = std::make_shared<MatcherDefTable>($1);
    }
    |   '{' expr_list '}'
    {
        $$ = std::make_shared<MatcherDefCond>($2);
    }
    ;

action_map:
        /* epsilon */
    {
        $$ = {};
    }
    |   action_map action_map_key ':' IDENTIFIER ';'
    {
        $$.emplace_back($2, $4);
    }
    ;

action_map_key:
        VALUE_INT
    |   DEFAULT
    {
        $$ = -1;
    }
    ;

%%
