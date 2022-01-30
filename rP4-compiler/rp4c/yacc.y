%{
#include "rp4_ast.h"
#include "yacc.hpp"
#include <iostream>
#include <memory>

int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}

%}

// request a pure (reentrant) parser
%define api.pure full
// enable location in error handler
%locations

// keywords
%token BIT STRUCT
%token TRUE FALSE
%token HEADER HEADERS
%token PARSER STATE TRANSITION PACKET PACKET_IN
%token DEFAULT ACCEPT EXTRACT SELECT
%token ACTIONS ACTION NOACTION
%token TABLES TABLE KEY SIZE DEFAULT_ACTION ENTRIES
%token EXACT TERNARY LPM STAGE
%token CONTROL MATCHER EXECUTOR SWITCH HIT MISS NONE
%token IS_VALID VALUE_WIDTH


// non-keywords
%token EQU NEQ LEQ GEQ T_EOF
%token ANDANDAND DIVDIVDIV

// type-specific tokens
%token <sv_str> IDENTIFIER VALUE_STRING
%token <sv_int> VALUE_INT

%type <sv_str> stage_executor_entry_left stage_name
%type <sv_header_defs> header_defs
%type <sv_header_def> header_def
%type <sv_field_defs> field_defs parameters
%type <sv_field_def> field_def
%type <sv_type> type_def
%type <sv_struct_defs> struct_defs
%type <sv_struct_def> struct_def
%type <sv_parser_def> parser_def
%type <sv_state_def> state_def
%type <sv_state_defs> state_defs
%type <sv_extract_stmt> extract_stmt
%type <sv_transition_stmt> transition_stmt
%type <sv_member> member
%type <sv_field> field
%type <sv_fields> fields
%type <sv_key> key_def
%type <sv_transition_entry> transition_entry direct_entry
%type <sv_transition_entries> transition_entries
%type <sv_actions_def> actions_def
%type <sv_action_defs> action_defs
%type <sv_action_def> action_def
%type <sv_expression> expression
%type <sv_expressions> expressions
%type <sv_lvalue> lvalue
%type <sv_operation> operation
%type <sv_operations> operations
%type <sv_operand> operand
%type <sv_op> op
%type <sv_tables_def> tables_def
%type <sv_table_def> table_def
%type <sv_table_defs> table_defs
%type <sv_table_key_def> table_key_def
%type <sv_key_entry> key_entry
%type <sv_key_entries> key_entries
%type <sv_match_type> match_type
%type <sv_keys> key_defs
%type <sv_table_optional_stmt> table_optional_stmt
%type <sv_table_optional_stmts> table_optional_stmts
%type <sv_match_entry> match_entry
%type <sv_match_entries> match_entries
%type <sv_match_key> match_key
%type <sv_pipeline> pipeline
%type <sv_stage_def> stage_def
%type <sv_stage_defs> stage_defs
%type <sv_stage_parser> stage_parser
%type <sv_stage_parser_header> stage_parser_header
%type <sv_stage_parser_headers> stage_parser_headers
%type <sv_stage_matcher> stage_matcher
%type <sv_switch_entry> switch_entry
%type <sv_switch_entries> switch_entries
%type <sv_switch_value> switch_value
%type <sv_stage_executor> stage_executor
%type <sv_stage_executor_entry> stage_executor_entry
%type <sv_stage_executor_entries> stage_executor_entries

%%

start:  
        header_defs struct_defs parser_def actions_def tables_def pipeline pipeline
    {
        tree = std::make_unique<Rp4Ast>($1, $2, $3, $4, $5, $6, $7);
        YYACCEPT;
    }
    ;

// headers
header_defs:    
        /* epsilon */
    {
        $$ = {};
    }
    |   header_defs header_def
    {
        $$.push_back($2);
    }
    ;

header_def:
        HEADER IDENTIFIER '{' field_defs '}'
    {
        $$ = Rp4HeaderDef($2, $4);
    }
    ;

field_defs:
        /* epsilon */
    {
        $$ = {};
    }
    |   field_defs field_def ';'
    {
        $$.push_back($2);
    }
    ;

field_def:
        type_def IDENTIFIER
    {
        $$ = Rp4FieldDef($1, $2);
    }
    ;

type_def:
        BIT '<' VALUE_INT '>'
    {
        $$ = std::make_shared<Rp4BitType>($3);
    }
    |   IDENTIFIER
    {
        $$ = std::make_shared<Rp4NamedType>($1);
    }
    ;

struct_defs:
        /* epsilon */
    {
        $$ = {};
    }
    |   struct_defs struct_def
    {
        $$.push_back($2);
    }
    ;

struct_def:
        STRUCT IDENTIFIER '{' field_defs '}' IDENTIFIER ';'
    {
        $$ = Rp4StructDef($2, $4, $6);
    }
    |   STRUCT HEADERS '{' field_defs '}' IDENTIFIER ';'
    {
        $$ = Rp4StructDef("headers", $4, $6, true);
    }
    ;

parser_def:
        PARSER IDENTIFIER '(' PACKET_IN PACKET ')' '{' state_defs '}'
    {
        $$ = Rp4ParserDef($2, $8);
    }
    ;

state_defs:
        /* epsilon */
    {
        $$ = {};
    }
    |   state_defs state_def
    {
        $$.push_back($2);
    }
    ;

state_def:
    STATE IDENTIFIER '{' extract_stmt transition_stmt '}'
    {
        $$ = Rp4StateDef($2, $4, $5);
    }
    ;

extract_stmt:
    PACKET '.' EXTRACT '(' member ')' ';'
    {
        $$ = Rp4Extract($5);
    }
    ;

member:
    IDENTIFIER '.' IDENTIFIER
    {
        $$ = Rp4Member($1, $3);
    }
    ;

transition_stmt:
    TRANSITION SELECT '(' fields ')' '{' transition_entries '}'
    {
        $$ = std::make_shared<Rp4SelectTransition>($4, $7);
    }
    |   TRANSITION direct_entry ';'
    {
        $$ = std::make_shared<Rp4DirectTransition>($2);
    }
    ;

fields:
        /* epsilon */
    {
        $$ = {};
    }
    |   field
    {
        $$ = { $1 };
    }
    |   fields ',' field
    {
        $$.push_back($3);
    }
    ;

direct_entry:
    IDENTIFIER
    {
        $$ = Rp4TransitionEntry(std::make_shared<Rp4DefaultKey>(), $1);
    }
    |   ACCEPT
    {
        $$ = Rp4TransitionEntry(std::make_shared<Rp4DefaultKey>(), "accept", 0);
    }
    ;

field:
    IDENTIFIER '.' IDENTIFIER '.' IDENTIFIER
    {
        $$ = Rp4Field($1, $3, $5);
    }
    |   IDENTIFIER '.' IDENTIFIER '.' IS_VALID '(' ')'
    {
        $$ = Rp4Field($1, $3, "isValid");
    }
    ;

transition_entries:
        /* epsilon */
    {
        $$ = {};
    }
    |   transition_entries transition_entry
    {
        $$.push_back($2);
    }
    ;

transition_entry:
        key_def ':' IDENTIFIER ';'
    {
        $$ = Rp4TransitionEntry($1, $3);
    }
    |   key_def ':' ACCEPT ';'
    {
        $$ = Rp4TransitionEntry($1, "accept", 0);
    }
    ;

key_def:
        VALUE_INT
    {
        $$ = std::make_shared<Rp4ExactKey>($1);
    }
    |   TRUE
    {
        $$ = std::make_shared<Rp4ExactKey>(1);
    }
    |   FALSE
    {
        $$ = std::make_shared<Rp4ExactKey>(0);
    }
    |   VALUE_INT ANDANDAND VALUE_INT
    {
        $$ = std::make_shared<Rp4TernaryKey>($1, $3);
    }
    |   VALUE_INT DIVDIVDIV VALUE_INT
    {
        $$ = std::make_shared<Rp4LpmKey>($1, $3);
    }
    |   DEFAULT
    {
        $$ = std::make_shared<Rp4DefaultKey>();
    }
    |   '*'
    {
        $$ = std::make_shared<Rp4DefaultKey>();
    }
    ;

actions_def:
    ACTIONS '{' action_defs '}'
    {
        $$ = Rp4ActionsDef($3);
    }
    ;

action_defs:
        /* epsilon */
    {
        $$ = {};
    }
    |   action_defs action_def
    {
        $$.push_back($2);
    }
    ;

action_def:
    ACTION IDENTIFIER '(' parameters ')' '{' expressions '}'
    {
        $$ = Rp4ActionDef($2, $4, $7);
    }
    |   ACTION NOACTION '(' ')' '{' '}'
    {
        $$ = Rp4ActionDef("NoAction", {}, {}, true);
    }
    ;

parameters:
        /* epsilon */
    {
        $$ = {};
    }
    |   field_def
    {
        $$ = { $1 };
    }
    |   parameters ',' field_def
    {
        $$.push_back($3);
    }
    ;

expressions:
        /* epsilon */
    {
        $$ = {};
    }
    |   expressions expression
    {
        $$.push_back($2);
    }
    ;

expression:
    lvalue '=' operation ';'
    {
        $$ = Rp4Expression($1, $3);
    }
    ;

lvalue:
        member
    {
        $$ = std::make_shared<Rp4MetaLValue>($1);
    }
    |   field
    {
        $$ = std::make_shared<Rp4HeaderLValue>($1);
    }
    ;

operation:
        operand
    {
        $$ = std::move($1);
    }
    |   '(' operation op operation ')'
    {
        $$ = std::make_shared<Rp4Binary>($3, $2, $4);
    }
    ;

op:
        '+'
    {
        $$ = OP_PLUS;
    }
    |   '-'
    {
        $$ = OP_MINUS;
    }
    |   '*'
    {
        $$ = OP_MUL;
    }
    |   '/'
    {
        $$ = OP_DIV;
    }
    |   '&'
    {
        $$ = OP_AND;
    }
    |   '|'
    {
        $$ = OP_OR;
    }
    |   '^'
    {
        $$ = OP_XOR;
    }
    |   '~'
    {
        $$ = OP_NOT;
    }
    |   '!'
    {
        $$ = OP_NOTL;
    }
    |   EQU
    {
        $$ = OP_EQ;
    }
    |   NEQ
    {
        $$ = OP_NE;
    }
    |   '<'
    {
        $$ = OP_LT;
    }
    |   '>'
    {
        $$ = OP_GT;
    }
    |   LEQ
    {
        $$ = OP_LE;
    }
    |   GEQ
    {
        $$ = OP_GE;
    }
    ;

operand:
        lvalue
    {
        $$ = std::move($1);
    }
    |   VALUE_INT
    {
        $$ = std::make_shared<Rp4Literal>($1);
    }
    |   IDENTIFIER
    {
        $$ = std::make_shared<Rp4Parameter>($1);
    }
    |   TRUE
    {
        $$ = std::make_shared<Rp4Literal>(1);
    }
    |   FALSE
    {
        $$ = std::make_shared<Rp4Literal>(0);
    }
    ;

tables_def:
    TABLES '{' table_defs '}'
    {
        $$ = Rp4TablesDef($3);
    }
    ;

table_defs:
        /* epsilon */
    {
        $$ = {};
    }
    |   table_defs table_def
    {
        $$.push_back($2);
    }
    ;

table_def:
    TABLE IDENTIFIER '{' table_key_def table_optional_stmts '}'
    {
        $$ = Rp4TableDef($2, $4, $5);
    }
    ;

table_key_def:
    KEY '=' '{' key_entries '}'
    {
        $$ = Rp4TableKeyDef($4);
    }
    ;

key_entries:
        /* epsilon */
    {
        $$ = {};
    }
    |   key_entries key_entry
    {
        $$.push_back($2);
    }
    ;

key_entry:
    lvalue ':' match_type ';'
    {
        $$ = Rp4KeyEntry($1, $3);
    }
    ;

match_type:
        EXACT
    {
        $$ = MT_EXACT;
    }
    |   TERNARY
    {
        $$ = MT_TERNARY;
    }
    |   LPM
    {
        $$ = MT_LPM;
    }
    ;

table_optional_stmts:
        /* epsilon */
    {
        $$ = {};
    }
    |   table_optional_stmts table_optional_stmt
    {
        $$.push_back($2);
    }
    ;

table_optional_stmt:
        SIZE '=' VALUE_INT ';'
    {
        $$ = std::make_shared<Rp4TableSizeStmt>($3);
    }
    |   ENTRIES '=' '{' match_entries '}'
    {
        $$ = std::make_shared<Rp4TableEntriesStmt>($4);
    }
    |   DEFAULT_ACTION '=' IDENTIFIER ';'
    {
        $$ = std::make_shared<Rp4DefaultActionStmt>($3);
    }
    |   DEFAULT_ACTION '=' NOACTION ';'
    {
        $$ = std::make_shared<Rp4DefaultActionStmt>("NoAction");
    }
    |   VALUE_WIDTH '=' VALUE_INT ';'
    {
        $$ = std::make_shared<Rp4TableValueWidthStmt>($3);
    }
    ;

match_entries:
        /* epsilon */
    {
        $$ = {};
    }
    |   match_entries match_entry
    {
        $$.push_back($2);
    }
    ;

match_entry:
    match_key ':' IDENTIFIER ';'
    {
        $$ = Rp4MatchEntry($1, $3);
    }
    |   match_key ':' NOACTION ';'
    {
        $$ = Rp4MatchEntry($1, "NoAction");
    }
    ;

match_key:
    '(' key_defs ')'
    {
        $$ = Rp4MatchKey($2);
    }
    ;

key_defs:
        /* epsilon */
    {
        $$ = {};
    }
    |   key_def
    {
        $$ = { $1 };
    }
    |   key_defs ',' key_def
    {
        $$.push_back($3);
    }
    ;

pipeline:
    CONTROL IDENTIFIER '{' stage_defs '}'
    {
        $$ = Rp4Pipeline($2, $4);
    }
    ;

stage_defs:
        /* epsilon */
    {
        $$ = {};
    }
    |   stage_defs stage_def
    {
        $$.push_back($2);
    }
    ;

stage_def:
    STAGE IDENTIFIER '{' stage_parser stage_matcher stage_executor '}'
    {
        $$ = Rp4StageDef($2, $4, $5, $6);
    }
    ;

stage_parser:
    PARSER '{' stage_parser_headers '}' ';'
    {
        $$ = Rp4StageParser($3);
    }
    ;

stage_parser_headers:
        /* epsilon */
    {
        $$ = {};
    }
    |   stage_parser_headers stage_parser_header
    {
        $$.push_back($2);
    }
    ;

stage_parser_header:
    IDENTIFIER ';'
    {
        $$ = Rp4StageParserHeader($1);
    }
    ;

stage_matcher:
    MATCHER '{' SWITCH '(' operations ')' '{' switch_entries '}' '}' ';'
    {
        $$ = Rp4StageMatcher($5, $8);
    }
    ;

operations:
        /* epsilon */
    {
        $$ = {};
    }
    |   operation
    {
        $$ = { $1 };
    }
    |   operations ',' operation
    {
        $$.push_back($3);
    }
    ;

switch_entries:
        /* epsilon */
    {
        $$ = {};
    }
    |   switch_entries switch_entry
    {
        $$.push_back($2);
    }
    ;

switch_entry:
    key_def ':' switch_value ';'
    {
        $$ = Rp4SwitchEntry($1, $3);
    }
    ;

switch_value:
        TABLE '(' IDENTIFIER ')'
    {
        $$ = std::make_shared<Rp4SwitchTableStmt>($3);
    }
    |   STAGE '(' IDENTIFIER ')'
    {
        $$ = std::make_shared<Rp4SwitchStageStmt>($3);
    }
    |   ACTION '(' IDENTIFIER ')'
    {
        $$ = std::make_shared<Rp4SwitchActionStmt>($3);
    }
    |   ACTION '(' NOACTION ')'
    {
        $$ = std::make_shared<Rp4SwitchActionStmt>("NoAction");
    }
    |   NONE
    {
        $$ = std::make_shared<Rp4SwitchStageStmt>("None", true);
    }
    ;

stage_executor:
    EXECUTOR '{' stage_executor_entries '}' ';'
    {
        $$ = Rp4StageExecutor($3);
    }
    ;

stage_executor_entries:
        /* epsilon */
    {
        $$ = {};
    }
    |   stage_executor_entries stage_executor_entry
    {
        $$.push_back($2);
    }
    ;

stage_executor_entry:
    stage_executor_entry_left ':' stage_name ';'
    {
        $$ = Rp4StageExecutorEntry($1, $3);
    }
    ;

stage_executor_entry_left:
        IDENTIFIER
    |   HIT
    {
        $$ = "__HIT__";
    }
    |   MISS
    {
        $$ = "__MISS__";
    }
    |   NOACTION 
    {
        $$ = "NoAction";
    }
    ;

stage_name:
        IDENTIFIER
    |   NONE
    {
        $$ = "None";
    }
    ;

%%