    /* we don't need yywrap() function */
%option noyywrap
    /* we don't need yyunput() function */
%option nounput
    /* we don't need input() function */
%option noinput
    /* enable location */
%option bison-bridge
%option bison-locations

%{

#include "rp4_ast.h"
#include "yacc.hpp"
#include <iostream>

// automatically update location
#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line; \
    yylloc->first_column = yylloc->last_column; \
    for (int i = 0; yytext[i] != '\0'; i++) { \
        if(yytext[i] == '\n') { \
            yylloc->last_line++; \
            yylloc->last_column = 1; \
        } else { \
            yylloc->last_column++; \
        } \
    }

%}

alpha [a-zA-Z]
digit [0-9]
white_space [ \t]+
new_line "\r"|"\n"|"\r\n"
sign "+"|"-"
identifier {alpha}(_|{alpha}|{digit})*
value_int {sign}?{digit}+
hex_int 0x[0-9a-fA-F]+
bin_int 0b[01]+
value_float {sign}?{digit}+\.({digit}+)?
value_string '[^']*'
single_op ";"|"("|")"|","|"*"|"="|">"|"<"|"."|":"|"#"|"["|"]"|"{"|"}"|"+"|"-"|"/"|"&"|"|"|"^"|"~"|"!"

%x comment

%%
    /* block comment */
"/*" { BEGIN(comment); }
<comment>"*/" { BEGIN(INITIAL); }
<comment>[^*] { /* ignore the text of the comment */ }
<comment>\* { /* ignore *'s that aren't part of */ }
    /* single line comment */
"//".* { /* ignore single line comment */ }
    /* white space and new line */
{white_space} { /* ignore white space */ }
{new_line} { /* ignore new line */ }
    /* keywords */
"bit" { return BIT; }
"struct" { return STRUCT; }
"true" { return TRUE; }
"false" { return FALSE; }
"header" { return HEADER; }
"headers" { return HEADERS; }
"parser" { return PARSER; }
"state" { return STATE; }
"transition" { return TRANSITION; }
"packet" { return PACKET; }
"packet_in" { return PACKET_IN; }
"select" { return SELECT; }
"default" { return DEFAULT; }
"accept" { return ACCEPT; }
"extract" { return EXTRACT; }
"actions" { return ACTIONS; }
"action" { return ACTION; }
"NoAction" { return NOACTION; }
"tables" { return TABLES; }
"table" { return TABLE; }
"key" { return KEY; }
"size" { return SIZE; }
"default_action" { return DEFAULT_ACTION; }
"entries" { return ENTRIES; }
"exact" { return EXACT; }
"ternary" { return TERNARY; }
"lpm" { return LPM; }
"control" { return CONTROL; }
"matcher" { return MATCHER; }
"executor" { return EXECUTOR; }
"switch" { return SWITCH; }
"__HIT__" { return HIT; }
"__MISS__" { return MISS; }
"isValid" { return IS_VALID; }
"None" { return NONE; }
"stage" { return STAGE; }
"value_width" { return VALUE_WIDTH; }
    /* operators */
">=" { return GEQ; }
"<=" { return LEQ; }
"!=" { return NEQ; }
"==" { return EQU; }
"&&&" { return ANDANDAND; }
"///" { return DIVDIVDIV; }
{single_op} { return yytext[0]; }
    /* id */
{identifier} {
    yylval->sv_str = yytext;
    return IDENTIFIER;
}
    /* literals */
{value_int} {
    yylval->sv_int = atoi(yytext);
    return VALUE_INT;
}
{hex_int} {
    yylval->sv_int = strtol(yytext + 2, nullptr, 16);
    return VALUE_INT;
}
{bin_int} {
    yylval->sv_int = strtol(yytext + 2, nullptr, 2);
    return VALUE_INT;
}
{value_string} {
    yylval->sv_str = std::string(yytext + 1, strlen(yytext) - 2);
    return VALUE_STRING;
}
    /* EOF */
<<EOF>> { return T_EOF; }
    /* unexpected char */
. { std::cerr << "Lexer Error: unexpected character " << yytext[0] << std::endl; }


%%
