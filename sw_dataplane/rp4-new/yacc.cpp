/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "yacc.y" /* yacc.c:339  */

#include "yacc.hpp"
#include "ipsa_base.h"
#include "rp4_ast.h"
#include <iostream>
#include <algorithm>
#include <memory>
#line 10 "yacc.y" /* yacc.c:339  */

extern IpsaBase base[30];
extern int num;
extern std::unique_ptr<Rp4Ast> ast[30];
int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}


#line 86 "yacc.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "yacc.hpp".  */
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

/* Copy the second part of user declarations.  */

#line 203 "yacc.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   393

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  78
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  67
/* YYNRULES -- Number of rules.  */
#define YYNRULES  157
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  385

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   312

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    77,     2,     2,     2,     2,    73,     2,
      63,    64,    68,    70,    66,    71,    65,    72,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    60,
      61,    69,    62,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    75,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    58,    74,    59,    76,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   110,   110,   115,   122,   136,   139,   146,   150,   163,
     176,   180,   184,   192,   195,   202,   206,   211,   216,   227,
     230,   237,   241,   245,   249,   253,   257,   261,   279,   282,
     289,   297,   300,   307,   314,   318,   326,   329,   336,   340,
     348,   351,   359,   362,   369,   376,   383,   390,   394,   402,
     405,   409,   416,   420,   427,   431,   439,   442,   449,   453,
     460,   464,   468,   472,   476,   480,   484,   491,   499,   502,
     509,   513,   521,   524,   528,   536,   539,   546,   553,   557,
     564,   568,   575,   579,   583,   587,   591,   595,   599,   603,
     607,   611,   615,   619,   623,   627,   631,   638,   642,   646,
     650,   654,   661,   669,   672,   679,   686,   694,   697,   704,
     711,   715,   719,   727,   730,   737,   741,   745,   749,   753,
     761,   764,   771,   775,   782,   790,   793,   797,   804,   812,
     815,   822,   829,   837,   840,   847,   854,   862,   865,   869,
     877,   880,   887,   894,   898,   902,   906,   910,   917,   925,
     928,   935,   942,   943,   947,   951,   958,   959
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BIT", "STRUCT", "TRUE", "FALSE",
  "HEADER", "HEADERS", "PARSER", "STATE", "TRANSITION", "PACKET",
  "PACKET_IN", "DEFAULT", "ACCEPT", "EXTRACT", "SELECT", "ACTIONS",
  "ACTION", "NOACTION", "TABLES", "TABLE", "KEY", "SIZE", "DEFAULT_ACTION",
  "ENTRIES", "EXACT", "TERNARY", "LPM", "STAGE", "CONTROL", "MATCHER",
  "EXECUTOR", "SWITCH", "HIT", "MISS", "NONE", "IS_VALID", "VALUE_WIDTH",
  "EQU", "NEQ", "LEQ", "GEQ", "T_EOF", "ANDANDAND", "DIVDIVDIV",
  "IDENTIFIER", "VALUE_STRING", "VALUE_INT", "ADD", "DEL", "REBUILD", "IN",
  "EXPRESSION", "OPERATION", "ACTION_TO_STAGE", "NEXT_STAGE", "'{'", "'}'",
  "';'", "'<'", "'>'", "'('", "')'", "'.'", "','", "':'", "'*'", "'='",
  "'+'", "'-'", "'/'", "'&'", "'|'", "'^'", "'~'", "'!'", "$accept",
  "start", "header_del", "dels", "del", "rebuilds", "rebuild", "adds",
  "add", "header_defs", "header_def", "field_defs", "field_def",
  "type_def", "struct_defs", "struct_def", "parser_def", "state_defs",
  "state_def", "extract_stmt", "member", "transition_stmt", "fields",
  "direct_entry", "field", "transition_entries", "transition_entry",
  "key_def", "actions_def", "action_defs", "action_def", "parameters",
  "expressions", "expression", "lvalue", "operation", "op", "operand",
  "tables_def", "table_defs", "table_def", "table_key_def", "key_entries",
  "key_entry", "match_type", "table_optional_stmts", "table_optional_stmt",
  "match_entries", "match_entry", "match_key", "key_defs", "pipeline",
  "stage_defs", "stage_def", "stage_parser", "stage_parser_headers",
  "stage_parser_header", "stage_matcher", "operations", "switch_entries",
  "switch_entry", "switch_value", "stage_executor",
  "stage_executor_entries", "stage_executor_entry",
  "stage_executor_entry_left", "stage_name", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   123,   125,
      59,    60,    62,    40,    41,    46,    44,    58,    42,    61,
      43,    45,    47,    38,   124,    94,   126,    33
};
# endif

#define YYPACT_NINF -328

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-328)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -328,    45,  -328,    80,  -328,    40,    44,  -328,    42,   119,
      50,  -328,    63,    28,    56,  -328,   117,    92,   121,   114,
     110,   135,   113,   115,    24,  -328,  -328,   109,   111,   107,
     116,   120,   151,   122,    61,  -328,   126,  -328,   154,   130,
    -328,   148,   149,   133,   129,   131,   134,     7,  -328,  -328,
     170,  -328,   128,  -328,   127,   155,   157,   125,   132,   136,
     142,   138,   144,   145,   140,   178,   168,   146,   139,  -328,
    -328,   141,   156,     9,    15,   190,     5,   158,   159,   160,
     161,   162,   163,  -328,   164,   155,   165,   153,     8,   181,
     166,   199,  -328,  -328,   188,   171,   173,   183,   172,  -328,
    -328,   175,   176,   167,  -328,  -328,   174,   177,   179,   193,
    -328,   194,     0,   180,  -328,  -328,   182,  -328,    82,   184,
    -328,   195,   185,   196,     3,    87,   186,   189,    14,   187,
     192,   169,   191,   197,   205,   206,   207,   198,   200,  -328,
    -328,  -328,   201,   203,     8,   204,    70,   202,  -328,   208,
     211,  -328,  -328,  -328,   -14,  -328,   209,  -328,  -328,  -328,
    -328,  -328,  -328,   210,   212,   216,  -328,  -328,   213,  -328,
      14,  -328,  -328,  -328,  -328,  -328,   214,   215,  -328,  -328,
    -328,  -328,   217,   220,   221,   218,     3,    23,  -328,  -328,
    -328,  -328,   219,   222,   223,   224,  -328,  -328,   118,   -26,
     225,   226,   227,   230,   232,     4,    -8,   228,   235,   236,
      43,    14,   238,     6,  -328,  -328,  -328,   233,  -328,  -328,
    -328,   -19,   -12,   240,    77,   229,   241,  -328,   234,   237,
    -328,   239,  -328,  -328,  -328,  -328,   242,   243,   244,  -328,
     248,  -328,  -328,   249,   251,  -328,   231,  -328,  -328,  -328,
    -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,
    -328,  -328,    14,  -328,  -328,   252,  -328,  -328,   253,   213,
    -328,  -328,   245,  -328,   246,  -328,   250,   255,   256,  -328,
     257,  -328,  -328,    14,    53,    78,   254,   264,  -328,  -328,
    -328,   265,    54,   258,   260,  -328,    14,   103,  -328,  -328,
    -328,    47,  -328,    89,   259,   261,   262,   263,   266,   268,
    -328,  -328,   283,   269,  -328,  -328,  -328,   272,  -328,     3,
    -328,   267,   270,  -328,  -328,  -328,  -328,  -328,   271,   273,
     292,  -328,  -328,  -328,    95,    79,  -328,  -328,   288,    10,
     274,  -328,     3,   276,   277,    -1,   278,  -328,   279,  -328,
     280,  -328,  -328,  -328,  -328,   284,   297,   298,  -328,   286,
     282,   275,   285,    98,  -328,  -328,   301,   289,   304,   294,
     298,  -328,  -328,   231,  -328,  -328,     1,  -328,  -328,   287,
      29,   293,   295,  -328,  -328
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      28,     0,    19,    36,     1,    13,     0,    29,    40,     0,
       3,    20,     0,     0,     0,    37,     5,     0,     0,     0,
       0,     0,     0,     0,     0,    14,    31,     0,     0,     0,
       0,     4,     0,     0,     0,    23,     0,    22,     0,     0,
      21,     0,     0,     0,     0,     0,     0,     0,    31,    31,
       0,    68,     0,     6,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    35,
      30,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   103,     0,     0,     0,     0,    72,     0,
       0,     0,   140,   149,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,    67,    69,     0,     0,     0,     0,
       7,     0,     0,     0,     2,    25,     0,    73,     0,     0,
     113,     0,     0,     0,    26,    27,     0,     0,   137,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   102,
     104,   129,     0,     0,     0,     0,     0,     0,   133,     0,
       0,    61,    62,    65,    60,    66,     0,   141,   155,   153,
     154,   152,   150,     0,     0,     0,   100,   101,    99,    98,
       0,    78,    79,    97,   138,    80,    17,     0,    34,    39,
      38,    42,     0,     0,     0,     0,     0,     0,    71,    75,
      74,   107,     0,     0,     0,     0,   105,   114,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    11,     9,     8,     0,    12,   128,
     130,     0,     0,     0,     0,     0,     0,    24,     0,     0,
     134,     0,   149,   131,    63,    64,     0,     0,     0,   147,
       0,   157,   156,     0,     0,    75,    46,    91,    92,    95,
      96,    93,    94,    84,    82,    83,    85,    86,    87,    88,
      89,    90,     0,   139,    18,     0,    41,    43,     0,     0,
      70,    76,     0,   106,     0,   108,     0,     0,     0,   120,
       0,   135,   132,   137,     0,     0,     0,     0,   142,   151,
      15,    16,     0,     0,     0,    10,     0,     0,   115,   118,
     117,     0,   119,     0,     0,     0,     0,     0,     0,     0,
      54,    81,     0,     0,   110,   111,   112,     0,   116,   125,
     121,     0,     0,   148,   146,   145,   143,   144,     0,     0,
       0,    77,   109,   126,     0,     0,   140,    55,     0,     0,
       0,   124,     0,     0,     0,     0,     0,    53,     0,    52,
       0,    44,   127,   123,   122,     0,     0,    49,    48,     0,
       0,     0,     0,     0,    50,   136,     0,     0,     0,     0,
       0,    46,    45,     0,    56,    51,     0,    47,    57,     0,
       0,     0,     0,    59,    58
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,
    -328,   -11,   -74,  -328,  -328,  -328,  -328,  -328,  -328,  -328,
    -130,  -328,  -328,  -328,  -327,  -328,  -328,  -186,  -328,  -328,
     247,  -328,    -6,  -328,  -219,  -169,  -328,  -328,  -328,  -328,
     281,  -328,  -328,  -328,  -328,  -328,    52,  -328,  -328,  -328,
    -328,   290,  -328,    65,  -328,  -328,  -328,  -328,   -42,   -88,
    -328,  -328,  -328,    25,  -328,  -328,  -328
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    31,    53,    10,    25,     5,    11,     3,
       7,    47,    71,    72,     8,    15,    16,   213,   267,   330,
     171,   340,   363,   350,   172,   376,   378,   156,    32,    76,
      35,   118,   221,   271,   173,   174,   262,   175,    55,   112,
      37,   120,   222,   275,   317,   146,   197,   301,   320,   321,
     334,    85,   187,    40,   123,   199,   230,   150,   176,   124,
     157,   240,   202,   125,   162,   163,   243
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
     218,   210,   272,   274,   151,   152,   151,   152,   151,   152,
      68,    68,    68,   153,   117,   153,   265,   153,    68,   166,
     167,   228,    36,   236,    34,   347,   237,   348,   269,   241,
     364,   203,   204,   229,   238,   269,    27,    73,    74,   242,
     270,   239,   263,   375,   381,     4,    13,   273,   154,    43,
     154,    14,   154,    39,    69,    69,    69,   349,   355,   139,
     377,   168,    69,   169,   104,   266,    70,   155,   101,   155,
     190,   155,   272,   158,   102,    28,   382,   170,    44,    45,
      46,    57,   219,   247,   248,   249,   250,     6,   159,   160,
       9,    12,   309,   293,   192,   193,   194,   277,   305,   343,
     161,   310,    24,    29,   251,   252,   318,   158,    58,   195,
     319,   253,   304,   254,   255,   256,   257,   258,   259,   260,
     261,    26,   159,   160,   278,   306,   344,   313,    17,   196,
     314,   315,   316,   333,   161,    30,    36,    77,    18,    33,
      34,    19,   192,   193,   194,    20,   143,    78,   144,    21,
      79,    22,    23,   322,    80,   211,   352,   195,    81,   341,
      82,   342,   369,    38,   370,    39,    41,    48,    42,    49,
      50,    52,    54,    59,    51,    56,    60,    61,    62,    63,
      64,    67,    65,    75,    66,    83,    84,    86,    87,    90,
     379,    92,    93,    94,    89,    88,    91,    95,    96,    97,
      98,    99,   103,   100,   119,   106,   107,   108,   122,   110,
     126,   113,   115,   129,   109,   137,   111,   116,   127,   121,
     128,   130,   131,   132,   138,   147,   361,   134,   149,   179,
     135,   133,   136,   164,   177,   182,   183,   184,   141,   291,
     142,   303,   165,   148,   201,   185,   208,   186,   345,   198,
     227,   180,   220,   145,   178,   181,     0,   284,   244,   231,
     188,   189,   191,   268,   214,   207,   200,   215,   216,     0,
       0,   217,   212,     0,     0,     0,   205,   206,   209,   234,
     211,   235,   245,   246,   232,   264,   233,   279,   223,   276,
     280,   224,   225,   226,   281,   329,   292,   282,   290,   294,
     295,   307,   283,   339,   346,   285,   286,   287,   288,   289,
     298,   308,   269,   297,   296,   299,   300,   302,   312,   323,
       0,     0,   311,   105,     0,   324,   325,   326,   336,   331,
     327,   328,   332,   351,   335,   337,   353,   354,   338,   367,
     358,   356,   357,   359,   360,   362,   365,   366,   371,   372,
     368,   373,   374,   383,   380,   384,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   114,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   140
};

static const yytype_int16 yycheck[] =
{
     186,   170,   221,   222,     5,     6,     5,     6,     5,     6,
       3,     3,     3,    14,    88,    14,    10,    14,     3,     5,
       6,    47,    22,    19,    19,    15,    22,    17,    47,    37,
     357,    45,    46,    59,    30,    47,     8,    48,    49,    47,
      59,    37,   211,   370,    15,     0,     4,    59,    49,    25,
      49,     9,    49,    30,    47,    47,    47,    47,    59,    59,
      59,    47,    47,    49,    59,    59,    59,    68,    59,    68,
     144,    68,   291,    20,    59,    47,    47,    63,    54,    55,
      56,    20,    59,    40,    41,    42,    43,     7,    35,    36,
      50,    47,    38,   262,    24,    25,    26,    20,    20,    20,
      47,    47,    52,    47,    61,    62,    59,    20,    47,    39,
      63,    68,    59,    70,    71,    72,    73,    74,    75,    76,
      77,    58,    35,    36,    47,    47,    47,   296,     9,    59,
      27,    28,    29,   319,    47,    18,    22,     9,    19,    47,
      19,    22,    24,    25,    26,    26,    64,    19,    66,    30,
      22,    32,    33,    64,    26,    66,   342,    39,    30,    64,
      32,    66,    64,    53,    66,    30,    53,    58,    53,    58,
      63,    51,    21,    47,    58,    53,    22,    47,    30,    30,
      47,    47,    53,    13,    53,    58,    31,    30,    63,    47,
     376,    47,    47,    53,    58,    63,    58,    19,    30,    53,
      61,    60,    12,    47,    23,    47,    47,    47,     9,    47,
      22,    47,    47,    30,    53,    22,    53,    64,    47,    53,
      47,    49,    47,    47,    30,    30,   356,    53,    32,    60,
      53,    64,    53,    47,    47,    30,    30,    30,    58,   245,
      58,   283,    53,    58,    33,    47,    30,    47,   336,    47,
     198,    60,   187,    69,    62,    58,    -1,   232,    30,    34,
      59,    58,    58,    30,    47,    53,    58,    47,    47,    -1,
      -1,    53,    57,    -1,    -1,    -1,    67,    67,    65,    49,
      66,    49,    47,    47,    58,    47,    59,    58,    69,    49,
      49,    69,    69,    69,    60,    12,    65,    60,    47,    47,
      47,    47,    63,    11,    16,    63,    63,    63,    60,    60,
      60,    47,    47,    67,    69,    60,    60,    60,    58,    60,
      -1,    -1,    64,    76,    -1,    64,    64,    64,    58,    60,
      64,    63,    60,    59,    67,    64,    60,    60,    65,    64,
      60,    63,    63,    59,    47,    47,    60,    65,    47,    60,
      65,    47,    58,    60,    67,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    79,    80,    87,     0,    85,     7,    88,    92,    50,
      83,    86,    47,     4,     9,    93,    94,     9,    19,    22,
      26,    30,    32,    33,    52,    84,    58,     8,    47,    47,
      18,    81,   106,    47,    19,   108,    22,   118,    53,    30,
     131,    53,    53,    25,    54,    55,    56,    89,    58,    58,
      63,    58,    51,    82,    21,   116,    53,    20,    47,    47,
      22,    47,    30,    30,    47,    53,    53,    47,     3,    47,
      59,    90,    91,    89,    89,    13,   107,     9,    19,    22,
      26,    30,    32,    58,    31,   129,    30,    63,    63,    58,
      47,    58,    47,    47,    53,    19,    30,    53,    61,    60,
      47,    59,    59,    12,    59,   108,    47,    47,    47,    53,
      47,    53,   117,    47,   129,    47,    64,    90,   109,    23,
     119,    53,     9,   132,   137,   141,    22,    47,    47,    30,
      49,    47,    47,    64,    53,    53,    53,    22,    30,    59,
     118,    58,    58,    64,    66,    69,   123,    30,    58,    32,
     135,     5,     6,    14,    49,    68,   105,   138,    20,    35,
      36,    47,   142,   143,    47,    53,     5,     6,    47,    49,
      63,    98,   102,   112,   113,   115,   136,    47,    62,    60,
      60,    58,    30,    30,    30,    47,    47,   130,    59,    58,
      90,    58,    24,    25,    26,    39,    59,   124,    47,   133,
      58,    33,   140,    45,    46,    67,    67,    53,    30,    65,
     113,    66,    57,    95,    47,    47,    47,    53,   105,    59,
     131,   110,   120,    69,    69,    69,    69,   124,    47,    59,
     134,    34,    58,    59,    49,    49,    19,    22,    30,    37,
     139,    37,    47,   144,    30,    47,    47,    40,    41,    42,
      43,    61,    62,    68,    70,    71,    72,    73,    74,    75,
      76,    77,   114,   113,    47,    10,    59,    96,    30,    47,
      59,   111,   112,    59,   112,   121,    49,    20,    47,    58,
      49,    60,    60,    63,   141,    63,    63,    63,    60,    60,
      47,   110,    65,   113,    47,    47,    69,    67,    60,    60,
      60,   125,    60,   136,    59,    20,    47,    47,    47,    38,
      47,    64,    58,   113,    27,    28,    29,   122,    59,    63,
     126,   127,    64,    60,    64,    64,    64,    64,    63,    12,
      97,    60,    60,   105,   128,    67,    58,    64,    65,    11,
      99,    64,    66,    20,    47,   137,    16,    15,    17,    47,
     101,    59,   105,    60,    60,    59,    63,    63,    60,    59,
      47,    98,    47,   100,   102,    60,    65,    64,    65,    64,
      66,    47,    60,    47,    58,   102,   103,    59,   104,   105,
      67,    15,    47,    60,    60
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    78,    79,    79,    80,    81,    81,    82,    82,    82,
      82,    82,    82,    83,    83,    84,    84,    84,    84,    85,
      85,    86,    86,    86,    86,    86,    86,    86,    87,    87,
      88,    89,    89,    90,    91,    91,    92,    92,    93,    93,
      94,    94,    95,    95,    96,    97,    98,    99,    99,   100,
     100,   100,   101,   101,   102,   102,   103,   103,   104,   104,
     105,   105,   105,   105,   105,   105,   105,   106,   107,   107,
     108,   108,   109,   109,   109,   110,   110,   111,   112,   112,
     113,   113,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   115,   115,   115,
     115,   115,   116,   117,   117,   118,   119,   120,   120,   121,
     122,   122,   122,   123,   123,   124,   124,   124,   124,   124,
     125,   125,   126,   126,   127,   128,   128,   128,   129,   130,
     130,   131,   132,   133,   133,   134,   135,   136,   136,   136,
     137,   137,   138,   139,   139,   139,   139,   139,   140,   141,
     141,   142,   143,   143,   143,   143,   144,   144
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     7,     3,     4,     0,     2,     3,     6,     6,
       8,     6,     6,     0,     2,     9,     9,     6,     8,     0,
       2,     3,     3,     3,     9,     6,     6,     6,     0,     2,
       5,     0,     3,     2,     4,     1,     0,     2,     7,     7,
       0,     9,     0,     2,     6,     7,     3,     8,     3,     0,
       1,     3,     1,     1,     5,     7,     0,     2,     4,     4,
       1,     1,     1,     3,     3,     1,     1,     4,     0,     2,
       8,     6,     0,     1,     3,     0,     2,     4,     1,     1,
       1,     5,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     0,     2,     6,     5,     0,     2,     4,
       1,     1,     1,     0,     2,     4,     5,     4,     4,     4,
       0,     2,     4,     4,     3,     0,     1,     3,     5,     0,
       2,     7,     5,     0,     2,     2,    11,     0,     1,     3,
       0,     2,     4,     4,     4,     4,     4,     1,     5,     0,
       2,     4,     1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 111 "yacc.y" /* yacc.c:1646  */
    {
        tree = std::make_unique<Rp4Ast>((yyvsp[-6].sv_header_defs), (yyvsp[-5].sv_struct_defs), (yyvsp[-4].sv_parser_def), (yyvsp[-3].sv_actions_def), (yyvsp[-2].sv_tables_def), (yyvsp[-1].sv_pipeline), (yyvsp[0].sv_pipeline));
        YYACCEPT;
    }
#line 1658 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 116 "yacc.y" /* yacc.c:1646  */
    {
        YYACCEPT;
    }
#line 1666 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 123 "yacc.y" /* yacc.c:1646  */
    {
        if ((yyvsp[-3].sv_header_defs).size()) {
            ast[num]->header_defs = std::move((yyvsp[-3].sv_header_defs));
            ast[num]->struct_defs = std::move((yyvsp[-2].sv_struct_defs));
            ast[num]->parser_def = std::move((yyvsp[-1].sv_parser_def));
            base[num].header_manager.load(ast[num]->struct_defs, ast[num]->header_defs);
            base[num].level_manager.load(ast[num]->parser_def, &base[num].header_manager);
            base[num].action_manager.reloadPrimitives(&base[num].header_manager);
            base[num].table_manager.reloadHeader(&base[num].header_manager);
        }
    }
#line 1682 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 136 "yacc.y" /* yacc.c:1646  */
    {
        
    }
#line 1690 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 140 "yacc.y" /* yacc.c:1646  */
    {

    }
#line 1698 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 147 "yacc.y" /* yacc.c:1646  */
    {
        base[num].stage_manager.logical_stages.erase((yyvsp[0].sv_str));
    }
#line 1706 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 151 "yacc.y" /* yacc.c:1646  */
    {
        base[num].table_manager.tables.erase((yyvsp[-3].sv_str));
        auto& entries = base[num].stage_manager.lookup((yyvsp[0].sv_str))->def.matcher.switch_entries;
        auto itr = entries.begin();
        while (itr != entries.end()) {
            if ((*itr).value->isTableStmt() && std::static_pointer_cast<Rp4SwitchTableStmt>((*itr).value)->name == (yyvsp[-3].sv_str)) {
                entries.erase(itr);
                break;
            }
            itr++;
        }
    }
#line 1723 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 164 "yacc.y" /* yacc.c:1646  */
    {
        base[num].action_manager.actions.erase((yyvsp[-3].sv_str));
        auto& entries = base[num].stage_manager.lookup((yyvsp[0].sv_str))->def.executor.entries;
        auto itr = entries.begin();
        while (itr != entries.end()) {
            if ((*itr).action_name == (yyvsp[-3].sv_str)) {
                entries.erase(itr);
                break;
            }
            itr++;
        }
    }
#line 1740 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 177 "yacc.y" /* yacc.c:1646  */
    {
        //ban
    }
#line 1748 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 181 "yacc.y" /* yacc.c:1646  */
    {
        base[num].stage_manager.lookup((yyvsp[0].sv_str))->def.parser.remove((yyvsp[-3].sv_str));
    }
#line 1756 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 185 "yacc.y" /* yacc.c:1646  */
    {
        //ban
    }
#line 1764 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 192 "yacc.y" /* yacc.c:1646  */
    {

    }
#line 1772 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 196 "yacc.y" /* yacc.c:1646  */
    {

    }
#line 1780 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 203 "yacc.y" /* yacc.c:1646  */
    {
        base[num].table_manager.lookup((yyvsp[-3].sv_str))->miss_action_id = base[num].action_manager.lookup((yyvsp[-6].sv_str))->id;
    }
#line 1788 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 207 "yacc.y" /* yacc.c:1646  */
    {
        base[num].action_manager.lookup((yyvsp[-4].sv_str))->rebuildExpressions((yyvsp[0].sv_expressions));
        base[num].action_manager.reloadPrimitives(&base[num].header_manager);
    }
#line 1797 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 212 "yacc.y" /* yacc.c:1646  */
    {
        base[num].stage_manager.lookup((yyvsp[-1].sv_str))->def.matcher.rebuildOps((yyvsp[0].sv_operations));

    }
#line 1806 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 217 "yacc.y" /* yacc.c:1646  */
    {
        for (auto& entry : base[num].stage_manager.lookup((yyvsp[-2].sv_str))->def.executor.entries) {
            if (entry.action_name == (yyvsp[-5].sv_str))
                entry.stage_name = (yyvsp[0].sv_str);
        }
    }
#line 1817 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 227 "yacc.y" /* yacc.c:1646  */
    {

    }
#line 1825 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 231 "yacc.y" /* yacc.c:1646  */
    {

    }
#line 1833 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 238 "yacc.y" /* yacc.c:1646  */
    {
        base[num].stage_manager.addStage(&((yyvsp[0].sv_stage_def)), true);
    }
#line 1841 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 242 "yacc.y" /* yacc.c:1646  */
    {
        base[num].table_manager.addTable((yyvsp[0].sv_table_def), &base[num].header_manager, &base[num].action_manager);
    }
#line 1849 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 246 "yacc.y" /* yacc.c:1646  */
    {
        base[num].action_manager.addAction((yyvsp[0].sv_action_def), &base[num].header_manager);
    }
#line 1857 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 250 "yacc.y" /* yacc.c:1646  */
    {
        //ban
    }
#line 1865 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 254 "yacc.y" /* yacc.c:1646  */
    {
        base[num].stage_manager.lookup((yyvsp[0].sv_str))->def.parser.add((yyvsp[-3].sv_str));
    }
#line 1873 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 258 "yacc.y" /* yacc.c:1646  */
    {
        base[num].stage_manager.lookup((yyvsp[-1].sv_str))->def.matcher.addEntries((yyvsp[0].sv_switch_entries));
    }
#line 1881 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 262 "yacc.y" /* yacc.c:1646  */
    {
        base[num].stage_manager.lookup((yyvsp[-1].sv_str))->def.executor.addExecutor((yyvsp[0].sv_stage_executor_entries));
    }
#line 1889 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 279 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_header_defs) = {};
    }
#line 1897 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 283 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_header_defs).push_back((yyvsp[0].sv_header_def));
    }
#line 1905 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 290 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_header_def) = Rp4HeaderDef((yyvsp[-3].sv_str), (yyvsp[-1].sv_field_defs));
    }
#line 1913 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 297 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_field_defs) = {};
    }
#line 1921 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 301 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_field_defs).push_back((yyvsp[-1].sv_field_def));
    }
#line 1929 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 308 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_field_def) = Rp4FieldDef((yyvsp[-1].sv_type), (yyvsp[0].sv_str));
    }
#line 1937 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 315 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_type) = std::make_shared<Rp4BitType>((yyvsp[-1].sv_int));
    }
#line 1945 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 319 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_type) = std::make_shared<Rp4NamedType>((yyvsp[0].sv_str));
    }
#line 1953 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 326 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_struct_defs) = {};
    }
#line 1961 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 330 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_struct_defs).push_back((yyvsp[0].sv_struct_def));
    }
#line 1969 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 337 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_struct_def) = Rp4StructDef((yyvsp[-5].sv_str), (yyvsp[-3].sv_field_defs), (yyvsp[-1].sv_str));
    }
#line 1977 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 341 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_struct_def) = Rp4StructDef("headers", (yyvsp[-3].sv_field_defs), (yyvsp[-1].sv_str), true);
    }
#line 1985 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 348 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_parser_def) = {};
    }
#line 1993 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 352 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_parser_def) = Rp4ParserDef((yyvsp[-7].sv_str), (yyvsp[-1].sv_state_defs));
    }
#line 2001 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 359 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_state_defs) = {};
    }
#line 2009 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 363 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_state_defs).push_back((yyvsp[0].sv_state_def));
    }
#line 2017 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 370 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_state_def) = Rp4StateDef((yyvsp[-4].sv_str), (yyvsp[-2].sv_extract_stmt), (yyvsp[-1].sv_transition_stmt));
    }
#line 2025 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 377 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_extract_stmt) = Rp4Extract((yyvsp[-2].sv_member));
    }
#line 2033 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 384 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_member) = Rp4Member((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2041 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 391 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_transition_stmt) = std::make_shared<Rp4SelectTransition>((yyvsp[-4].sv_fields), (yyvsp[-1].sv_transition_entries));
    }
#line 2049 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 395 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_transition_stmt) = std::make_shared<Rp4DirectTransition>((yyvsp[-1].sv_transition_entry));
    }
#line 2057 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 402 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_fields) = {};
    }
#line 2065 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 406 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_fields) = { (yyvsp[0].sv_field) };
    }
#line 2073 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 410 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_fields).push_back((yyvsp[0].sv_field));
    }
#line 2081 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 417 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_transition_entry) = Rp4TransitionEntry(std::make_shared<Rp4DefaultKey>(), (yyvsp[0].sv_str));
    }
#line 2089 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 421 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_transition_entry) = Rp4TransitionEntry(std::make_shared<Rp4DefaultKey>(), "accept", 0);
    }
#line 2097 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 428 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_field) = Rp4Field((yyvsp[-4].sv_str), (yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2105 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 432 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_field) = Rp4Field((yyvsp[-6].sv_str), (yyvsp[-4].sv_str), "isValid");
    }
#line 2113 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 439 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_transition_entries) = {};
    }
#line 2121 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 443 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_transition_entries).push_back((yyvsp[0].sv_transition_entry));
    }
#line 2129 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 450 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_transition_entry) = Rp4TransitionEntry((yyvsp[-3].sv_key), (yyvsp[-1].sv_str));
    }
#line 2137 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 454 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_transition_entry) = Rp4TransitionEntry((yyvsp[-3].sv_key), "accept", 0);
    }
#line 2145 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 461 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key) = std::make_shared<Rp4ExactKey>((yyvsp[0].sv_int));
    }
#line 2153 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 465 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key) = std::make_shared<Rp4ExactKey>(1);
    }
#line 2161 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 469 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key) = std::make_shared<Rp4ExactKey>(0);
    }
#line 2169 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 473 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key) = std::make_shared<Rp4TernaryKey>((yyvsp[-2].sv_int), (yyvsp[0].sv_int));
    }
#line 2177 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 477 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key) = std::make_shared<Rp4LpmKey>((yyvsp[-2].sv_int), (yyvsp[0].sv_int));
    }
#line 2185 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 481 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key) = std::make_shared<Rp4DefaultKey>();
    }
#line 2193 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 485 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key) = std::make_shared<Rp4DefaultKey>();
    }
#line 2201 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 492 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_actions_def) = Rp4ActionsDef((yyvsp[-1].sv_action_defs));
    }
#line 2209 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 499 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_action_defs) = {};
    }
#line 2217 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 503 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_action_defs).push_back((yyvsp[0].sv_action_def));
    }
#line 2225 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 510 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_action_def) = Rp4ActionDef((yyvsp[-6].sv_str), (yyvsp[-4].sv_field_defs), (yyvsp[-1].sv_expressions));
    }
#line 2233 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 514 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_action_def) = Rp4ActionDef("NoAction", {}, {}, true);
    }
#line 2241 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 521 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_field_defs) = {};
    }
#line 2249 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 525 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_field_defs) = { (yyvsp[0].sv_field_def) };
    }
#line 2257 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 529 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_field_defs).push_back((yyvsp[0].sv_field_def));
    }
#line 2265 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 536 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_expressions) = {};
    }
#line 2273 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 540 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_expressions).push_back((yyvsp[0].sv_expression));
    }
#line 2281 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 547 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_expression) = Rp4Expression((yyvsp[-3].sv_lvalue), (yyvsp[-1].sv_operation));
    }
#line 2289 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 554 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_lvalue) = std::make_shared<Rp4MetaLValue>((yyvsp[0].sv_member));
    }
#line 2297 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 558 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_lvalue) = std::make_shared<Rp4HeaderLValue>((yyvsp[0].sv_field));
    }
#line 2305 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 565 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operation) = std::move((yyvsp[0].sv_operand));
    }
#line 2313 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 569 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operation) = std::make_shared<Rp4Binary>((yyvsp[-2].sv_op), (yyvsp[-3].sv_operation), (yyvsp[-1].sv_operation));
    }
#line 2321 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 576 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_PLUS;
    }
#line 2329 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 580 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_MINUS;
    }
#line 2337 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 584 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_MUL;
    }
#line 2345 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 588 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_DIV;
    }
#line 2353 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 592 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_AND;
    }
#line 2361 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 596 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_OR;
    }
#line 2369 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 600 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_XOR;
    }
#line 2377 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 604 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_NOT;
    }
#line 2385 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 608 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_NOTL;
    }
#line 2393 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 612 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_EQ;
    }
#line 2401 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 616 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_NE;
    }
#line 2409 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 620 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_LT;
    }
#line 2417 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 624 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_GT;
    }
#line 2425 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 628 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_LE;
    }
#line 2433 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 632 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_op) = OP_GE;
    }
#line 2441 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 639 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operand) = std::move((yyvsp[0].sv_lvalue));
    }
#line 2449 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 643 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operand) = std::make_shared<Rp4Literal>((yyvsp[0].sv_int));
    }
#line 2457 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 647 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operand) = std::make_shared<Rp4Parameter>((yyvsp[0].sv_str));
    }
#line 2465 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 651 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operand) = std::make_shared<Rp4Literal>(1);
    }
#line 2473 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 655 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operand) = std::make_shared<Rp4Literal>(0);
    }
#line 2481 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 662 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_tables_def) = Rp4TablesDef((yyvsp[-1].sv_table_defs));
    }
#line 2489 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 669 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_defs) = {};
    }
#line 2497 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 673 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_defs).push_back((yyvsp[0].sv_table_def));
    }
#line 2505 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 680 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_def) = Rp4TableDef((yyvsp[-4].sv_str), (yyvsp[-2].sv_table_key_def), (yyvsp[-1].sv_table_optional_stmts));
    }
#line 2513 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 687 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_key_def) = Rp4TableKeyDef((yyvsp[-1].sv_key_entries));
    }
#line 2521 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 694 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key_entries) = {};
    }
#line 2529 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 698 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key_entries).push_back((yyvsp[0].sv_key_entry));
    }
#line 2537 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 705 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_key_entry) = Rp4KeyEntry((yyvsp[-3].sv_lvalue), (yyvsp[-1].sv_match_type));
    }
#line 2545 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 712 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_match_type) = MT_EXACT;
    }
#line 2553 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 111:
#line 716 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_match_type) = MT_TERNARY;
    }
#line 2561 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 720 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_match_type) = MT_LPM;
    }
#line 2569 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 727 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_optional_stmts) = {};
    }
#line 2577 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 114:
#line 731 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_optional_stmts).push_back((yyvsp[0].sv_table_optional_stmt));
    }
#line 2585 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 738 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_optional_stmt) = std::make_shared<Rp4TableSizeStmt>((yyvsp[-1].sv_int));
    }
#line 2593 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 116:
#line 742 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_optional_stmt) = std::make_shared<Rp4TableEntriesStmt>((yyvsp[-1].sv_match_entries));
    }
#line 2601 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 117:
#line 746 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_optional_stmt) = std::make_shared<Rp4DefaultActionStmt>((yyvsp[-1].sv_str));
    }
#line 2609 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 118:
#line 750 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_optional_stmt) = std::make_shared<Rp4DefaultActionStmt>("NoAction");
    }
#line 2617 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 119:
#line 754 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_table_optional_stmt) = std::make_shared<Rp4TableValueWidthStmt>((yyvsp[-1].sv_int));
    }
#line 2625 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 120:
#line 761 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_match_entries) = {};
    }
#line 2633 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 121:
#line 765 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_match_entries).push_back((yyvsp[0].sv_match_entry));
    }
#line 2641 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 122:
#line 772 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_match_entry) = Rp4MatchEntry((yyvsp[-3].sv_match_key), (yyvsp[-1].sv_str));
    }
#line 2649 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 123:
#line 776 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_match_entry) = Rp4MatchEntry((yyvsp[-3].sv_match_key), "NoAction");
    }
#line 2657 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 124:
#line 783 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_match_key) = Rp4MatchKey((yyvsp[-1].sv_keys));
    }
#line 2665 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 125:
#line 790 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_keys) = {};
    }
#line 2673 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 126:
#line 794 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_keys) = { (yyvsp[0].sv_key) };
    }
#line 2681 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 127:
#line 798 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_keys).push_back((yyvsp[0].sv_key));
    }
#line 2689 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 128:
#line 805 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_pipeline) = Rp4Pipeline((yyvsp[-3].sv_str), (yyvsp[-1].sv_stage_defs));
    }
#line 2697 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 129:
#line 812 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_defs) = {};
    }
#line 2705 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 130:
#line 816 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_defs).push_back((yyvsp[0].sv_stage_def));
    }
#line 2713 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 131:
#line 823 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_def) = Rp4StageDef((yyvsp[-5].sv_str), (yyvsp[-3].sv_stage_parser), (yyvsp[-2].sv_stage_matcher), (yyvsp[-1].sv_stage_executor));
    }
#line 2721 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 132:
#line 830 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_parser) = Rp4StageParser((yyvsp[-2].sv_stage_parser_headers));
    }
#line 2729 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 133:
#line 837 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_parser_headers) = {};
    }
#line 2737 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 134:
#line 841 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_parser_headers).push_back((yyvsp[0].sv_stage_parser_header));
    }
#line 2745 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 135:
#line 848 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_parser_header) = Rp4StageParserHeader((yyvsp[-1].sv_str));
    }
#line 2753 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 136:
#line 855 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_matcher) = Rp4StageMatcher((yyvsp[-6].sv_operations), (yyvsp[-3].sv_switch_entries));
    }
#line 2761 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 137:
#line 862 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operations) = {};
    }
#line 2769 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 138:
#line 866 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operations) = { (yyvsp[0].sv_operation) };
    }
#line 2777 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 139:
#line 870 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_operations).push_back((yyvsp[0].sv_operation));
    }
#line 2785 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 140:
#line 877 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_switch_entries) = {};
    }
#line 2793 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 141:
#line 881 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_switch_entries).push_back((yyvsp[0].sv_switch_entry));
    }
#line 2801 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 142:
#line 888 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_switch_entry) = Rp4SwitchEntry((yyvsp[-3].sv_key), (yyvsp[-1].sv_switch_value));
    }
#line 2809 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 143:
#line 895 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_switch_value) = std::make_shared<Rp4SwitchTableStmt>((yyvsp[-1].sv_str));
    }
#line 2817 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 144:
#line 899 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_switch_value) = std::make_shared<Rp4SwitchStageStmt>((yyvsp[-1].sv_str));
    }
#line 2825 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 145:
#line 903 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_switch_value) = std::make_shared<Rp4SwitchActionStmt>((yyvsp[-1].sv_str));
    }
#line 2833 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 146:
#line 907 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_switch_value) = std::make_shared<Rp4SwitchActionStmt>("NoAction");
    }
#line 2841 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 147:
#line 911 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_switch_value) = std::make_shared<Rp4SwitchStageStmt>("None", true);
    }
#line 2849 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 148:
#line 918 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_executor) = Rp4StageExecutor((yyvsp[-2].sv_stage_executor_entries));
    }
#line 2857 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 149:
#line 925 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_executor_entries) = {};
    }
#line 2865 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 150:
#line 929 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_executor_entries).push_back((yyvsp[0].sv_stage_executor_entry));
    }
#line 2873 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 151:
#line 936 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_stage_executor_entry) = Rp4StageExecutorEntry((yyvsp[-3].sv_str), (yyvsp[-1].sv_str));
    }
#line 2881 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 153:
#line 944 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_str) = "__HIT__";
    }
#line 2889 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 154:
#line 948 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_str) = "__MISS__";
    }
#line 2897 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 155:
#line 952 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_str) = "NoAction";
    }
#line 2905 "yacc.cpp" /* yacc.c:1646  */
    break;

  case 157:
#line 960 "yacc.y" /* yacc.c:1646  */
    {
        (yyval.sv_str) = "None";
    }
#line 2913 "yacc.cpp" /* yacc.c:1646  */
    break;


#line 2917 "yacc.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 965 "yacc.y" /* yacc.c:1906  */
