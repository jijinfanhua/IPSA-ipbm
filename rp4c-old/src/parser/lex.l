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
#include "ast.h"
#include "yacc.tab.h"
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
single_op ";"|"("|")"|","|"*"|"="|">"|"<"|"."|":"|"#"|"["|"]"|"{"|"}"

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
"#include".* { /* ignore includes */ }
{white_space} { /* ignore white space */ }
{new_line} { /* ignore new line */ }
    /* keywords */
"include" { return INCLUDE; }
"stream" { return STREAM; }
"packet_seg" { return PACKET_SEG; }
"packet" { return PACKET; }
"headers" { return HEADERS; }
"hdr" { return HDR; }
"bit" { return BIT; }
"implicit" { return IMPLICIT; }
"parser" { return PARSER; }
"default" { return DEFAULT; }
"func" { return FUNC; }
"accept" { return ACCEPT; }
"structs" { return STRUCTS; }
"struct" { return STRUCT; }
"register" { return REGISTER; }
"header_sequence" { return HEADER_SEQUENCE; }
"actions" { return ACTIONS; }
"action" { return ACTION; }
"goto" { return GOTO; }
"tables" { return TABLES; }
"table" { return TABLE; }
"exact" { return EXACT; }
"ternary" { return TERNARY; }
"lpm" { return LPM; }
"key" { return KEY; }
"size" { return SIZE; }
"control" { return CONTROL; }
"stage" { return STAGE; }
"matcher" { return MATCHER; }
"executor" { return EXECUTOR; }
    /* operators */
">=" { return GEQ; }
"<=" { return LEQ; }
"!=" { return NEQ; }
"==" { return EQU; }
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
{value_float} {
    yylval->sv_float = atof(yytext);
    return VALUE_FLOAT;
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
