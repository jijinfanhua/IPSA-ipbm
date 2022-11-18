/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_YACC_HPP_INCLUDED
# define YY_YY_YACC_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    BIT = 258,
    STRUCT = 259,
    TRUE = 260,
    FALSE = 261,
    HEADER = 262,
    HEADERS = 263,
    PARSER = 264,
    STATE = 265,
    TRANSITION = 266,
    PACKET = 267,
    PACKET_IN = 268,
    DEFAULT = 269,
    ACCEPT = 270,
    EXTRACT = 271,
    SELECT = 272,
    ACTIONS = 273,
    ACTION = 274,
    NOACTION = 275,
    TABLES = 276,
    TABLE = 277,
    KEY = 278,
    SIZE = 279,
    DEFAULT_ACTION = 280,
    ENTRIES = 281,
    EXACT = 282,
    TERNARY = 283,
    LPM = 284,
    STAGE = 285,
    CONTROL = 286,
    MATCHER = 287,
    EXECUTOR = 288,
    SWITCH = 289,
    HIT = 290,
    MISS = 291,
    NONE = 292,
    IS_VALID = 293,
    VALUE_WIDTH = 294,
    EQU = 295,
    NEQ = 296,
    LEQ = 297,
    GEQ = 298,
    T_EOF = 299,
    ANDANDAND = 300,
    DIVDIVDIV = 301,
    IDENTIFIER = 302,
    VALUE_STRING = 303,
    VALUE_INT = 304,
    ADD = 305,
    DEL = 306,
    REBUILD = 307,
    IN = 308,
    EXPRESSION = 309,
    OPERATION = 310,
    ACTION_TO_STAGE = 311,
    NEXT_STAGE = 312
  };
#endif

/* Value type.  */

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (void);

#endif /* !YY_YY_YACC_HPP_INCLUDED  */
