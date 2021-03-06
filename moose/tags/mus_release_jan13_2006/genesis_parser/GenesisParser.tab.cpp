/* A Bison parser, made from GenesisParser.ypp
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	OR	257
# define	AND	258
# define	LT	259
# define	LE	260
# define	GT	261
# define	GE	262
# define	EQ	263
# define	NE	264
# define	POW	265
# define	UMINUS	266
# define	WHILE	267
# define	IF	268
# define	ELSE	269
# define	ELIF	270
# define	FOR	271
# define	FOREACH	272
# define	END	273
# define	INCLUDE	274
# define	ENDSCRIPT	275
# define	BREAK	276
# define	INT	277
# define	FLOAT	278
# define	STR	279
# define	RETURN	280
# define	WHITESPACE	281
# define	FUNCTION	282
# define	INTCONST	283
# define	DOLLARARG	284
# define	FLOATCONST	285
# define	STRCONST	286
# define	LITERAL	287
# define	IDENT	288
# define	VARREF	289
# define	FUNCREF	290
# define	EXTERN	291
# define	SL	292
# define	COMMAND	293
# define	EXPRCALL	294
# define	ARGUMENT	295
# define	ARGLIST	296
# define	LOCREF	297
# define	ICAST	298
# define	FCAST	299
# define	SCAST	300

#line 1 "GenesisParser.ypp"

// SLI parser
#include <iostream>
#include <string>
#include <vector>
#include <map>


// Upi Bhalla, 24 May 2004:
// I did a little checking up about portability of the setjmp construct
// in the parser code. It looks like it ought to work even in VC++,
// but I'll have to actually compile it to be sure. The consensus
// seems to be that this language construct should probably be
// avoided. Since I want to keep the changes to the SLI parser code
// to a minimum, I'll leave it in.
#include <setjmp.h>

using namespace std;

#include "../basecode/Shell.h"
#include <FlexLexer.h>
#include "GenesisParser.tab.h"
#include "script.h"
#include "GenesisParser.h"
#include "func_externs.h"

/*
** Parser routines which return something other than int.
*/

extern char *TokenStr(int token);

#line 70 "GenesisParser.ypp"

#include "GenesisParser.yy.cpp"
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		217
#define	YYFLAG		-32768
#define	YYNTBASE	66

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 300 ? yytranslate[x] : 124)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      58,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    11,     2,     2,     2,    20,    14,     2,
      60,    61,    18,    12,    65,    13,     2,    19,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    59,
       2,    62,     2,     2,    17,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    16,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    63,    15,    64,    21,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     3,     4,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      39,    41,    43,    45,    46,    54,    55,    67,    68,    69,
      81,    82,    91,    92,    95,    96,   104,   108,   109,   113,
     119,   123,   124,   126,   127,   131,   132,   136,   137,   142,
     144,   145,   151,   152,   156,   158,   162,   163,   165,   167,
     170,   171,   173,   175,   178,   180,   182,   184,   185,   186,
     192,   194,   196,   198,   201,   204,   207,   210,   211,   212,
     219,   220,   224,   226,   230,   232,   234,   236,   239,   241,
     242,   247,   248,   252,   253,   256,   258,   261,   263,   264,
     268,   272,   276,   279,   283,   287,   291,   295,   299,   303,
     307,   310,   314,   318,   321,   325,   329,   333,   337,   341,
     345,   347,   349,   351,   353,   355,   359,   363,   367
};
static const short yyrhs[] =
{
      67,     0,     0,     0,    67,    68,    70,    69,     0,    58,
       0,    59,     0,    72,     0,    74,     0,    76,     0,    79,
       0,    83,     0,    86,     0,    71,     0,    91,     0,    94,
       0,   108,     0,   106,     0,   114,     0,   120,     0,   121,
       0,   122,     0,    32,     0,     0,    24,    60,   123,    61,
      73,    67,    30,     0,     0,    28,    60,    83,    59,   123,
      59,    83,    61,    75,    67,    30,     0,     0,     0,    29,
      46,    60,    77,    98,    97,    98,    78,    61,    67,    30,
       0,     0,    25,    60,   123,    61,    80,    67,    81,    30,
       0,     0,    26,    67,     0,     0,    27,    60,   123,    61,
      82,    67,    81,     0,    46,    62,   123,     0,     0,    31,
      85,   100,     0,    84,    44,    38,    97,   100,     0,    84,
      44,   100,     0,     0,   101,     0,     0,    45,    89,    87,
       0,     0,    46,    90,   101,     0,     0,    88,    92,    96,
     100,     0,    47,     0,     0,    93,    87,    95,    96,   100,
       0,     0,    96,    99,   101,     0,   101,     0,    97,    99,
     101,     0,     0,    99,     0,    38,     0,    99,    38,     0,
       0,    38,     0,   102,     0,   101,   102,     0,    44,     0,
      43,     0,    41,     0,     0,     0,    63,   103,   105,   104,
      64,     0,    94,     0,    91,     0,   123,     0,    48,    45,
       0,    48,    47,     0,    39,    45,     0,    39,    47,     0,
       0,     0,   107,   109,   111,   110,    67,    30,     0,     0,
      60,   112,    61,     0,    45,     0,   112,    65,    45,     0,
      34,     0,    35,     0,    36,     0,   113,   115,     0,   117,
       0,     0,   115,    65,   116,   117,     0,     0,    45,   118,
     119,     0,     0,    62,   123,     0,    33,     0,    37,   123,
       0,    37,     0,     0,   123,    15,   123,     0,   123,    14,
     123,     0,   123,    16,   123,     0,    21,   123,     0,   123,
      17,   123,     0,   123,    12,   123,     0,   123,    13,   123,
       0,   123,    18,   123,     0,   123,    19,   123,     0,   123,
      20,   123,     0,   123,    22,   123,     0,    13,   123,     0,
     123,     3,   123,     0,   123,     4,   123,     0,    11,   123,
       0,   123,     5,   123,     0,   123,     6,   123,     0,   123,
       7,   123,     0,   123,     8,   123,     0,   123,     9,   123,
       0,   123,    10,   123,     0,    46,     0,    42,     0,    40,
       0,    43,     0,    41,     0,    63,   123,    64,     0,    63,
      91,    64,     0,    63,    94,    64,     0,    60,   123,    61,
       0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    78,    81,    85,    85,   121,   122,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   142,   162,   162,   179,   179,   201,   201,   201,
     225,   225,   242,   246,   248,   248,   265,   280,   280,   290,
     329,   365,   369,   382,   382,   396,   396,   427,   427,   471,
     478,   478,   499,   503,   509,   513,   519,   520,   523,   524,
     527,   528,   531,   537,   545,   552,   559,   566,   566,   566,
     580,   584,   588,   597,   616,   622,   653,   690,   690,   690,
     710,   712,   716,   727,   740,   746,   752,   760,   766,   767,
     767,   780,   780,   791,   793,   797,   809,   815,   823,   827,
     829,   831,   833,   836,   839,   841,   843,   845,   847,   849,
     851,   854,   856,   858,   861,   863,   865,   867,   869,   871,
     874,   894,   901,   908,   916,   924,   928,   932,   936
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "OR", "AND", "LT", "LE", "GT", "GE", "EQ", 
  "NE", "'!'", "'+'", "'-'", "'&'", "'|'", "'^'", "'@'", "'*'", "'/'", 
  "'%'", "'~'", "POW", "UMINUS", "WHILE", "IF", "ELSE", "ELIF", "FOR", 
  "FOREACH", "END", "INCLUDE", "ENDSCRIPT", "BREAK", "INT", "FLOAT", 
  "STR", "RETURN", "WHITESPACE", "FUNCTION", "INTCONST", "DOLLARARG", 
  "FLOATCONST", "STRCONST", "LITERAL", "IDENT", "VARREF", "FUNCREF", 
  "EXTERN", "SL", "COMMAND", "EXPRCALL", "ARGUMENT", "ARGLIST", "LOCREF", 
  "ICAST", "FCAST", "SCAST", "'\\n'", "';'", "'('", "')'", "'='", "'{'", 
  "'}'", "','", "script", "statement_list", "@1", "stmnt_terminator", 
  "statement", "endscript_marker", "while_stmnt", "@2", "for_stmnt", "@3", 
  "foreach_stmnt", "@4", "@5", "if_stmnt", "@6", "else_clause", "@7", 
  "assign_stmnt", "include_hdr", "@8", "include_stmnt", "opt_node", 
  "cmd_name", "@9", "@10", "cmd_stmnt", "@11", "funcref", "func_call", 
  "@12", "opt_arg_list", "arg_list", "optwslist", "wslist", "ws", 
  "arg_component_list", "arg_component", "@13", "@14", "ac_func_cmd_expr", 
  "ext_func", "func_hdr", "func_def", "@15", "@16", "func_args", 
  "func_arg_list", "decl_type", "decl_stmnt", "decl_list", "@17", 
  "decl_ident", "@18", "init", "break_stmnt", "return_stmnt", 
  "null_stmnt", "expr", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    66,    67,    68,    67,    69,    69,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    71,    73,    72,    75,    74,    77,    78,    76,
      80,    79,    81,    81,    82,    81,    83,    85,    84,    86,
      86,    87,    87,    89,    88,    90,    88,    92,    91,    93,
      95,    94,    96,    96,    97,    97,    98,    98,    99,    99,
     100,   100,   101,   101,   102,   102,   102,   103,   104,   102,
     105,   105,   105,   106,   106,   107,   107,   109,   110,   108,
     111,   111,   112,   112,   113,   113,   113,   114,   115,   116,
     115,   118,   117,   119,   119,   120,   121,   121,   122,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     0,     0,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     7,     0,    11,     0,     0,    11,
       0,     8,     0,     2,     0,     7,     3,     0,     3,     5,
       3,     0,     1,     0,     3,     0,     3,     0,     4,     1,
       0,     5,     0,     3,     1,     3,     0,     1,     1,     2,
       0,     1,     1,     2,     1,     1,     1,     0,     0,     5,
       1,     1,     1,     2,     2,     2,     2,     0,     0,     6,
       0,     3,     1,     3,     1,     1,     1,     2,     1,     0,
       4,     0,     3,     0,     2,     1,     2,     1,     0,     3,
       3,     3,     2,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     3,     3,     3,     3
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       2,     3,    98,     0,     0,     0,     0,    37,    22,    95,
      84,    85,    86,    97,     0,    43,    45,    49,     0,     0,
      13,     7,     8,     9,    10,    11,     0,    12,    47,    14,
      41,    15,    17,    77,    16,     0,    18,    19,    20,    21,
       0,     0,     0,     0,    60,     0,     0,     0,   122,   124,
     121,   123,   120,     0,     0,    96,    75,    76,    41,     0,
       0,    73,    74,     5,     6,     4,    60,    52,    66,    65,
      64,    67,    50,    42,    62,    80,    91,    87,    88,     0,
       0,     0,     0,    27,    61,    38,   113,   110,   102,     0,
     120,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    44,    36,    46,    61,    40,    60,     0,    52,
      63,     0,    78,    93,    89,    23,    30,     0,    56,   128,
     126,   127,   125,   111,   112,   114,   115,   116,   117,   118,
     119,   104,   105,   100,    99,   101,   103,   106,   107,   108,
     109,    60,    54,    58,     0,    48,    71,    70,    68,    72,
      60,    82,     0,     2,     0,    92,     0,     2,     2,     0,
      58,     0,    57,     0,    39,    59,    53,     0,    51,    81,
       0,     3,    94,    90,     3,     3,     0,    56,    55,    69,
      83,    79,    24,     2,     0,     0,     0,    28,    57,     3,
       0,    31,    25,     0,     0,     2,     2,    34,     3,     3,
       2,    26,    29,     3,    35,     0,     0,     0
};

static const short yydefgoto[] =
{
     215,     1,     2,    65,    19,    20,    21,   167,    22,   205,
      23,   128,   203,    24,   168,   195,   210,    25,    26,    44,
      27,    72,    28,    58,    60,    29,    67,    30,    31,   119,
     117,   151,   171,   154,    85,    73,    74,   118,   177,   158,
      32,    33,    34,    75,   163,   122,   162,    35,    36,    77,
     166,    78,   123,   165,    37,    38,    39,    55
};

static const short yypact[] =
{
  -32768,    11,   239,   -29,   -13,    14,   -26,-32768,-32768,-32768,
  -32768,-32768,-32768,    91,    68,-32768,   -41,-32768,    94,    -7,
  -32768,-32768,-32768,-32768,-32768,-32768,    31,-32768,-32768,-32768,
      51,-32768,-32768,-32768,-32768,    34,-32768,-32768,-32768,-32768,
      91,    91,    36,    25,    52,    91,    91,    91,-32768,-32768,
  -32768,-32768,-32768,    91,    80,   306,-32768,-32768,    51,    91,
      51,-32768,-32768,-32768,-32768,-32768,    59,-32768,-32768,-32768,
  -32768,-32768,-32768,    51,-32768,    40,-32768,    38,-32768,   158,
     178,   -41,    47,-32768,-32768,-32768,   353,-32768,    87,   198,
      55,    46,    65,    20,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,-32768,   306,    51,    51,-32768,    81,    80,-32768,
  -32768,    90,-32768,    74,-32768,-32768,-32768,    91,   100,-32768,
  -32768,-32768,-32768,   324,   324,   342,   342,   342,   342,   342,
     342,    26,    26,    26,    26,    26,    26,    87,    87,    87,
  -32768,    81,    51,   -49,    45,-32768,-32768,-32768,-32768,   306,
      81,-32768,   -57,-32768,    91,-32768,    34,-32768,-32768,   238,
  -32768,    51,   104,    45,-32768,-32768,    51,    82,-32768,-32768,
      99,   115,   306,-32768,   118,    23,    36,   100,    51,-32768,
  -32768,-32768,-32768,-32768,    89,   120,    92,-32768,    45,   122,
      91,-32768,-32768,    95,   218,-32768,-32768,-32768,   127,   128,
  -32768,-32768,-32768,    23,-32768,   159,   169,-32768
};

static const short yypgoto[] =
{
  -32768,    98,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,   -34,-32768,   -39,-32768,-32768,
  -32768,   131,-32768,-32768,-32768,   -42,-32768,-32768,   -38,-32768,
     110,    28,    22,  -110,   -44,   -43,   -71,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,    83,-32768,-32768,-32768,-32768,-32768,   -40
};


#define	YYLAST		375


static const short yytable[] =
{
      79,    80,   120,    82,   179,    86,    87,    88,   180,   -61,
     -61,    -1,    91,    89,    93,   -61,    92,   114,   172,   113,
      43,    59,   116,    94,    95,    96,    97,    98,    99,   100,
     101,    40,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   173,   111,   120,   108,   109,   110,    41,   111,   193,
     194,    63,    64,   -32,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   152,   155,    42,    66,   156,   198,   159,    76,
     157,   120,    81,   175,   132,    83,    68,   169,    69,    70,
      84,    45,    68,    46,    69,    70,   -45,   115,   -45,   -45,
     121,    47,    45,   124,    46,   120,   127,   174,    71,   111,
     130,   176,    47,    56,    71,    57,   178,   120,   -45,   153,
      48,    49,    50,    51,   182,    15,    90,    17,   152,   131,
     188,    48,    49,    50,    51,   161,   164,    52,   170,    61,
      53,    62,   175,    54,   190,   191,   189,   196,   192,   200,
     201,    53,   -33,   202,    54,   188,   206,   211,   212,   216,
     204,    94,    95,    96,    97,    98,    99,   100,   101,   217,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   214,
     111,    94,    95,    96,    97,    98,    99,   100,   101,   112,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   187,
     111,    94,    95,    96,    97,    98,    99,   100,   101,   197,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   125,
     111,    94,    95,    96,    97,    98,    99,   100,   101,   160,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   126,
     111,    94,    95,    96,    97,    98,    99,   100,   101,   183,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   129,
     111,   181,     0,     3,     4,   184,   185,     5,     6,     0,
       7,     8,     9,    10,    11,    12,    13,     0,    14,   207,
       0,     0,     0,     0,    15,    16,    17,    18,     0,     0,
       0,   199,     0,     0,     0,     0,     0,   186,     0,     0,
       0,     0,     0,   208,   209,     0,     0,     0,   213,    94,
      95,    96,    97,    98,    99,   100,   101,     0,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,    96,
      97,    98,    99,   100,   101,     0,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     0,   111,-32768,-32768,-32768,
  -32768,-32768,-32768,     0,   102,   103,   104,   105,   106,   107,
     108,   109,   110,     0,   111,   102,   103,   104,   105,   106,
     107,   108,   109,   110,     0,   111
};

static const short yycheck[] =
{
      40,    41,    73,    42,    61,    45,    46,    47,    65,    58,
      59,     0,    54,    53,    54,    64,    54,    60,   128,    59,
      46,    62,    66,     3,     4,     5,     6,     7,     8,     9,
      10,    60,    12,    13,    14,    15,    16,    17,    18,    19,
      20,   151,    22,   114,    18,    19,    20,    60,    22,    26,
      27,    58,    59,    30,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   115,   117,    60,    44,   118,   187,   118,    45,
     118,   152,    46,    38,    64,    60,    41,   127,    43,    44,
      38,    11,    41,    13,    43,    44,    41,    38,    43,    44,
      60,    21,    11,    65,    13,   176,    59,   151,    63,    22,
      64,   154,    21,    45,    63,    47,   160,   188,    63,    38,
      40,    41,    42,    43,   164,    45,    46,    47,   171,    64,
     173,    40,    41,    42,    43,    45,    62,    46,    38,    45,
      60,    47,    38,    63,    45,    30,    64,   186,    30,    60,
      30,    60,    30,    61,    63,   198,    61,    30,    30,     0,
     200,     3,     4,     5,     6,     7,     8,     9,    10,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    20,   213,
      22,     3,     4,     5,     6,     7,     8,     9,    10,    58,
      12,    13,    14,    15,    16,    17,    18,    19,    20,   171,
      22,     3,     4,     5,     6,     7,     8,     9,    10,   187,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    61,
      22,     3,     4,     5,     6,     7,     8,     9,    10,   119,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    61,
      22,     3,     4,     5,     6,     7,     8,     9,    10,   166,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    61,
      22,   163,    -1,    24,    25,   167,   168,    28,    29,    -1,
      31,    32,    33,    34,    35,    36,    37,    -1,    39,    61,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    -1,    -1,
      -1,   193,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      -1,    -1,    -1,   205,   206,    -1,    -1,    -1,   210,     3,
       4,     5,     6,     7,     8,     9,    10,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,     5,
       6,     7,     8,     9,    10,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,     5,     6,     7,
       8,     9,    10,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    22,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    22
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
// Shifted YY_DECL_VARIABLES to inside myFlexLexer class
#endif  /* !YYPURE */

int
myFlexLexer::yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 2:
#line 82 "GenesisParser.ypp"
{ 
		    yyval.pn = NULL;
 		  ;
    break;}
case 3:
#line 86 "GenesisParser.ypp"
{
		    yyval.str = (char *) MakeScriptInfo();
		    SetLine((ScriptInfo *) yyval.str);
		  ;
    break;}
case 4:
#line 91 "GenesisParser.ypp"
{
		    ResultValue	v;

		    if (InFunctionDefinition || Compiling)
			if (yyvsp[-1].pn != NULL)
			  {
			    v.r_str = yyvsp[-2].str;
			    yyval.pn = PTNew(SL, v, yyvsp[-3].pn, yyvsp[-1].pn);
			  }
			else
			  {
			    FreeScriptInfo((ScriptInfo *)yyvsp[-2].str);
			    yyval.pn = yyvsp[-3].pn;
			  }
		    else
		      {
		        /* execute statement */
		        if (setjmp(BreakBuf) == 0) {
			    if (setjmp(ReturnBuf) == 0)
				PTCall(yyvsp[-1].pn);
			    else
				EndScript();
		        }
			FreeScriptInfo((ScriptInfo *)yyvsp[-2].str);
			FreePTValues(yyvsp[-1].pn);
			PTFree(yyvsp[-1].pn);
		      }
		  ;
    break;}
case 22:
#line 143 "GenesisParser.ypp"
{
		    /*
		    ** When the end of a script is encountered, the simulator
		    ** sgets routine returns NULL.  The nextchar routine in the
		    ** lexer returns a special character '\200' which is lexed
		    ** as ENDSCRIPT.  We need this when we include a script
		    ** in a function or control structure so that the script
		    ** local variable storage is allocated and deallocated.
		    */

		    if (Compiling || InFunctionDefinition)
		      {
			yyval.pn = PTNew(ENDSCRIPT, RV, NULL, NULL);
		      }
		    else
			yyval.pn = NULL;
		  ;
    break;}
case 23:
#line 163 "GenesisParser.ypp"
{
		    Compiling++;
		    BreakAllowed++;
		    yyval.str = (char *) MakeScriptInfo();
		  ;
    break;}
case 24:
#line 169 "GenesisParser.ypp"
{
		    ResultValue	v;

		    v.r_str = yyvsp[-2].str;
		    yyval.pn = PTNew(WHILE, v, yyvsp[-4].pn, yyvsp[-1].pn);
		    Compiling--;
		    BreakAllowed--;
		  ;
    break;}
case 25:
#line 180 "GenesisParser.ypp"
{
		      Compiling++;
		      BreakAllowed++;
		      yyval.str = (char *) MakeScriptInfo();
		    ;
    break;}
case 26:
#line 187 "GenesisParser.ypp"
{
		    ResultValue	v;
		    ParseNode	*forbody, *whilepart;

		    v.r_str = (char *) MakeScriptInfo();
		    forbody = PTNew(SL, v, yyvsp[-1].pn, yyvsp[-4].pn);
		    v.r_str = yyvsp[-2].str;
		    whilepart = PTNew(FOR, v, yyvsp[-6].pn, forbody);
		    yyval.pn = PTNew(SL, v, yyvsp[-8].pn, whilepart);
		    Compiling--;
		    BreakAllowed--;
		  ;
    break;}
case 27:
#line 202 "GenesisParser.ypp"
{
			BEGIN FUNCLIT;
			Compiling++;
			BreakAllowed++;
		    ;
    break;}
case 28:
#line 208 "GenesisParser.ypp"
{
			BEGIN 0;
		    ;
    break;}
case 29:
#line 212 "GenesisParser.ypp"
{
		    Result	*rp;
		    ResultValue	v;
		    // char        buf[100];

		    rp = (Result *) yyvsp[-9].str;
		    v.r_str = (char *) rp;
		    yyval.pn = PTNew(FOREACH, v, yyvsp[-5].pn, yyvsp[-1].pn);
		    Compiling--;
		    BreakAllowed--;
		  ;
    break;}
case 30:
#line 226 "GenesisParser.ypp"
{
		    Compiling++;
		    yyval.str = (char *) MakeScriptInfo();
		  ;
    break;}
case 31:
#line 231 "GenesisParser.ypp"
{
		    ResultValue	v;
		    ParseNode	*stmntlists;

		    stmntlists = PTNew(0, v, yyvsp[-2].pn, yyvsp[-1].pn);
		    v.r_str = yyvsp[-3].str;
		    yyval.pn = PTNew(IF, v, yyvsp[-5].pn, stmntlists);
		    Compiling--;
		  ;
    break;}
case 32:
#line 243 "GenesisParser.ypp"
{
 		    yyval.pn = NULL;
 		  ;
    break;}
case 33:
#line 247 "GenesisParser.ypp"
{ yyval.pn = yyvsp[0].pn; ;
    break;}
case 34:
#line 249 "GenesisParser.ypp"
{
		    Compiling++;
		    yyval.str = (char *) MakeScriptInfo();
		  ;
    break;}
case 35:
#line 254 "GenesisParser.ypp"
{
		    ResultValue	v;
		    ParseNode	*stmntlists;

		    stmntlists = PTNew(0, v, yyvsp[-1].pn, yyvsp[0].pn);
		    v.r_str = yyvsp[-2].str;
		    yyval.pn = PTNew(IF, v, yyvsp[-4].pn, stmntlists);
		    Compiling--;
		  ;
    break;}
case 36:
#line 266 "GenesisParser.ypp"
{
		    ResultValue	v;
		    Result	*rp;
		    // char        buf[100];

		    yyval.pn = NULL;
		    rp = (Result *) yyvsp[-2].str;
			  {
			    v.r_str = (char *) rp;
		            yyval.pn = PTNew('=', v, yyvsp[0].pn, NULL);
			  }
		  ;
    break;}
case 37:
#line 281 "GenesisParser.ypp"
{
		    Pushyybgin(LIT);
		  ;
    break;}
case 38:
#line 285 "GenesisParser.ypp"
{
		    yyval.str = NULL;
		  ;
    break;}
case 39:
#line 291 "GenesisParser.ypp"
{
		    ResultValue	v;
		    // Result	*rp;
		    int		argc;
		    char	*argv[100];
		    char	argbuf[1000];
		    // jmp_buf	save;
		    // Result	r;

		    Popyybgin();
		    sprintf(argbuf, "%s", yyvsp[-3].str);
		    argc = 1;
		    argv[0] = argbuf;
		    do_cmd_args(yyvsp[-1].pn, &argc, argv);
		    argv[argc] = NULL;

		    if (!IncludeScript(argc, argv))
		      {
			sprintf(argbuf, "Script '%s' not found", yyvsp[-3].str);
			FreePTValues(yyvsp[-1].pn);
			PTFree(yyvsp[-1].pn);
			free(yyvsp[-3].str);
			yyerror(argbuf);
		      }

		    if (Compiling || InFunctionDefinition)
		      {
			v.r_str = yyvsp[-3].str;
			yyval.pn = PTNew(INCLUDE, v, yyvsp[-1].pn, NULL);
		      }
		    else
		      {
			FreePTValues(yyvsp[-1].pn);
			PTFree(yyvsp[-1].pn);
			free(yyvsp[-3].str);
			yyval.pn = NULL;
		      }
		  ;
    break;}
case 40:
#line 330 "GenesisParser.ypp"
{
		    ResultValue	v;
		    // Result	*rp;
		    int		argc;
		    char	*argv[100];
		    char	argbuf[1000];
		    // jmp_buf	save;
		    // Result	r;

		    Popyybgin();
		    sprintf(argbuf, "%s", yyvsp[-1].str);
		    argc = 1;
		    argv[0] = argbuf;
		    argv[argc] = NULL;

		    if (!IncludeScript(argc, argv))
		      {
			sprintf(argbuf, "Script '%s' not found", yyvsp[-1].str);
			free(yyvsp[-1].str);
			yyerror(argbuf);
		      }

		    if (Compiling || InFunctionDefinition)
		      {
			v.r_str = yyvsp[-1].str;
			yyval.pn = PTNew(INCLUDE, v, NULL, NULL);
		      }
		    else
		      {
			free(yyvsp[-1].str);
			yyval.pn = NULL;
		      }
		  ;
    break;}
case 41:
#line 366 "GenesisParser.ypp"
{
		    yyval.pn = (ParseNode*) NULL;
		  ;
    break;}
case 42:
#line 370 "GenesisParser.ypp"
{
		    yyval.pn = yyvsp[0].pn;
		  ;
    break;}
case 43:
#line 383 "GenesisParser.ypp"
{
		    Pushyybgin(LIT);
		  ;
    break;}
case 44:
#line 387 "GenesisParser.ypp"
{
		    ResultValue	v;

		    v.r_str = yyvsp[-2].str;
		    if (yyvsp[0].pn == NULL)
			yyval.pn = PTNew(COMMAND, v, NULL, NULL);
		    else
			yyval.pn = PTNew(FUNCTION, v, NULL, yyvsp[0].pn);
		  ;
    break;}
case 45:
#line 397 "GenesisParser.ypp"
{
		    Pushyybgin(LIT);
		  ;
    break;}
case 46:
#line 401 "GenesisParser.ypp"
{
		    ResultValue	v;
		    char*	varname;

		    varname = NULL;
		    if (LocalSymbols != NULL)
			varname = SymtabKey(LocalSymbols, (Result *)yyvsp[-2].str);
		    if (varname == NULL)
			varname = SymtabKey(&GlobalSymbols, (Result *)yyvsp[-2].str);
		    v.r_str = (char*) strsave(varname);

		    yyval.pn = PTNew(FUNCTION, v, NULL, yyvsp[0].pn);
		  ;
    break;}
case 47:
#line 428 "GenesisParser.ypp"
{
		    BEGIN LIT;
		  ;
    break;}
case 48:
#line 432 "GenesisParser.ypp"
{
		    // ResultValue	v;
		    // Result	*rp;
		    int		argc;
		    char	*argv[100];
		    char	argbuf[1000];
		    // jmp_buf	save;
		    // Result	r;

		    yyval.pn = yyvsp[-3].pn;
		    yyvsp[-3].pn->pn_left = yyvsp[-1].pn;
		    Popyybgin();
		    if (yyvsp[-3].pn->pn_val.r_type != EXPRCALL && yyvsp[-3].pn->pn_right == NULL &&
				!IsCommand(yyvsp[-3].pn->pn_val.r.r_str))
		      {
			if (IsInclude(yyvsp[-3].pn->pn_val.r.r_str))
			  {
			    sprintf(argbuf, "%s", yyvsp[-3].pn->pn_val.r.r_str);
			    argc = 1;
			    argv[0] = argbuf;
			    do_cmd_args(yyvsp[-1].pn, &argc, argv);
			    argv[argc] = NULL;
			    IncludeScript(argc, argv);

			    if (Compiling || InFunctionDefinition)
			      {
				yyvsp[-3].pn->pn_val.r_type = INCLUDE;
			      }
			    else
			      {
				FreePTValues(yyvsp[-3].pn);
				PTFree(yyvsp[-3].pn);
				yyval.pn = NULL;
			      }
			  }
		      }
		  ;
    break;}
case 49:
#line 472 "GenesisParser.ypp"
{
		    Pushyybgin(LIT);
		    yyval.str = yyvsp[0].str;
		  ;
    break;}
case 50:
#line 479 "GenesisParser.ypp"
{
		    BEGIN LIT;
		  ;
    break;}
case 51:
#line 483 "GenesisParser.ypp"
{
		    ResultValue	v;
		    Result	*rp;

		    Popyybgin();
		    rp = (Result *) yyvsp[-4].str;
		    if (yyvsp[-3].pn == NULL)
			yyval.pn = PTNew(FUNCTION, rp->r, yyvsp[-1].pn, NULL);
		    else
		      {
			v.r_str = (char*) strsave(SymtabKey(&GlobalSymbols, rp));
			yyval.pn = PTNew(FUNCTION, v, yyvsp[-1].pn, yyvsp[-3].pn);
		      }
		  ;
    break;}
case 52:
#line 500 "GenesisParser.ypp"
{
		    yyval.pn = NULL;
		  ;
    break;}
case 53:
#line 504 "GenesisParser.ypp"
{
		    yyval.pn = PTNew(ARGLIST, RV, yyvsp[-2].pn, yyvsp[0].pn);
		  ;
    break;}
case 54:
#line 510 "GenesisParser.ypp"
{
		    yyval.pn = PTNew(ARGLIST, RV, NULL, yyvsp[0].pn);
		  ;
    break;}
case 55:
#line 514 "GenesisParser.ypp"
{
		    yyval.pn = PTNew(ARGLIST, RV, yyvsp[-2].pn, yyvsp[0].pn);
		  ;
    break;}
case 62:
#line 532 "GenesisParser.ypp"
{
			    ResultValue	v;

			    yyval.pn = PTNew(ARGUMENT, v, NULL, yyvsp[0].pn);
			  ;
    break;}
case 63:
#line 538 "GenesisParser.ypp"
{
			    ResultValue	v;

			    yyval.pn = PTNew(ARGUMENT, v, yyvsp[-1].pn, yyvsp[0].pn);
			  ;
    break;}
case 64:
#line 546 "GenesisParser.ypp"
{
		    ResultValue	v;

		    v.r_str = yyvsp[0].str;
		    yyval.pn = PTNew(LITERAL, v, NULL, NULL);
		  ;
    break;}
case 65:
#line 553 "GenesisParser.ypp"
{
		    ResultValue	v;

		    v.r_str = yyvsp[0].str;
		    yyval.pn = PTNew(LITERAL, v, NULL, NULL);
		  ;
    break;}
case 66:
#line 560 "GenesisParser.ypp"
{
		    ResultValue	v;

		    v.r_int = yyvsp[0].iconst;
		    yyval.pn = PTNew(DOLLARARG, v, NULL, NULL);
		  ;
    break;}
case 67:
#line 567 "GenesisParser.ypp"
{
		    Pushyybgin(0);
		  ;
    break;}
case 68:
#line 571 "GenesisParser.ypp"
{
		    Popyybgin();
		  ;
    break;}
case 69:
#line 575 "GenesisParser.ypp"
{
		    yyval.pn = yyvsp[-2].pn;
		  ;
    break;}
case 70:
#line 581 "GenesisParser.ypp"
{
		    yyval.pn = yyvsp[0].pn;
		  ;
    break;}
case 71:
#line 585 "GenesisParser.ypp"
{
		    yyval.pn = yyvsp[0].pn;
		  ;
    break;}
case 72:
#line 589 "GenesisParser.ypp"
{
		    if (yyvsp[0].pn->pn_val.r_type == STRCONST)
			yyvsp[0].pn->pn_val.r_type = LITERAL;

		    yyval.pn = yyvsp[0].pn;
		  ;
    break;}
case 73:
#line 598 "GenesisParser.ypp"
{
		    ParseNode	*funcpn;
		    ResultValue	v;
		    Result	*rp;

		    rp = SymtabNew(&GlobalSymbols, yyvsp[0].str);
		    if (rp->r_type != 0 && rp->r_type != FUNCTION)
			fprintf(stderr, "WARNING: function name '%s' is redefining a variable!\n", yyvsp[0].str);

		    rp->r_type = FUNCTION;

		    v.r_str = (char *) NULL;
		    funcpn = PTNew(SL, v, NULL, NULL);
		    rp->r.r_str = (char *) funcpn;

		    free(yyvsp[0].str);
		    yyval.pn = NULL;
		  ;
    break;}
case 74:
#line 617 "GenesisParser.ypp"
{
		    yyval.pn = NULL;
		  ;
    break;}
case 75:
#line 623 "GenesisParser.ypp"
{
		    ParseNode	*funcpn;
		    ResultValue	v;
		    Result	*rp;
		    // char	*script;

		    if (InFunctionDefinition)
		      {
			fprintf(stderr, "Function definition within another function or\n");
			fprintf(stderr, "within a control structure (FUNCTION %s).\n", yyvsp[0].str);
			yyerror("");
			/* No Return */
		      }

		    InFunctionDefinition++;
		    NextLocal = 0;
		    rp = SymtabNew(&GlobalSymbols, yyvsp[0].str);
		    if (rp->r_type != 0 && rp->r_type != FUNCTION)
			fprintf(stderr, "WARNING: function name '%s' is redefining a variable!\n", yyvsp[0].str);

		    rp->r_type = FUNCTION;

		    LocalSymbols = SymtabCreate();
		    v.r_str = (char *) LocalSymbols;
		    funcpn = PTNew(SL, v, NULL, NULL);
		    rp->r.r_str = (char *) funcpn;

		    free(yyvsp[0].str);
		    yyval.pn = funcpn;
		  ;
    break;}
case 76:
#line 654 "GenesisParser.ypp"
{
		    ParseNode	*funcpn;
		    // ResultValue	v;
		    Result	*rp;
		    // char	*script;

		    rp = (Result *) yyvsp[0].str;
		    if (InFunctionDefinition)
		      {
			fprintf(stderr, "Function definition within another function or\n");
			fprintf(stderr, "within a control structure (FUNCTION %s).\n", yyvsp[0].str);
			yyerror("");
			/* No Return */
		      }

		    /*
		    ** Free old function parse tree and symtab
		    */

		    funcpn = (ParseNode *) rp->r.r_str;
		    if (funcpn->pn_val.r.r_str != NULL)
			SymtabDestroy((Symtab *)(funcpn->pn_val.r.r_str));
		    FreePTValues(funcpn->pn_left);
		    PTFree(funcpn->pn_left);
		    FreePTValues(funcpn->pn_right);
		    PTFree(funcpn->pn_right);

		    InFunctionDefinition++;
		    NextLocal = 0;
		    LocalSymbols = SymtabCreate();
		    funcpn->pn_val.r.r_str = (char *) LocalSymbols;

		    yyval.pn = funcpn;
		  ;
    break;}
case 77:
#line 691 "GenesisParser.ypp"
{
		    ReturnIdents = 1;
		  ;
    break;}
case 78:
#line 695 "GenesisParser.ypp"
{
		    ReturnIdents = 0;
		  ;
    break;}
case 79:
#line 699 "GenesisParser.ypp"
{
		    InFunctionDefinition--;

		    yyvsp[-5].pn->pn_left = yyvsp[-3].pn;
		    yyvsp[-5].pn->pn_right = yyvsp[-1].pn;

		    LocalSymbols = NULL;
		    yyval.pn = NULL;
		  ;
    break;}
case 80:
#line 711 "GenesisParser.ypp"
{ yyval.pn = NULL; ;
    break;}
case 81:
#line 713 "GenesisParser.ypp"
{ yyval.pn = yyvsp[-1].pn; ;
    break;}
case 82:
#line 717 "GenesisParser.ypp"
{
		    ResultValue	v;
		    ParseNode	*init;

		    ArgMatch = 1;
		    v.r_int = ArgMatch++;
		    init = PTNew(DOLLARARG, v, NULL, NULL);
		    yyval.pn = vardef(yyvsp[0].str, STR, SCAST, init);
		    free(yyvsp[0].str);
		  ;
    break;}
case 83:
#line 728 "GenesisParser.ypp"
{
		    ResultValue	v;
		    ParseNode	*init;

		    v.r_int = ArgMatch++;
		    init = PTNew(DOLLARARG, v, NULL, NULL);
		    v.r_str = (char *) MakeScriptInfo();
		    yyval.pn = PTNew(SL, v, yyvsp[-2].pn, vardef(yyvsp[0].str, STR, SCAST, init));
		    free(yyvsp[0].str);
		  ;
    break;}
case 84:
#line 741 "GenesisParser.ypp"
{
		    ReturnIdents = 1;
		    DefType = INT;
		    DefCast = ICAST;
		  ;
    break;}
case 85:
#line 747 "GenesisParser.ypp"
{
		    ReturnIdents = 1;
		    DefType = FLOAT;
		    DefCast = FCAST;
		  ;
    break;}
case 86:
#line 753 "GenesisParser.ypp"
{
		    ReturnIdents = 1;
		    DefType = STR;
		    DefCast = SCAST;
		  ;
    break;}
case 87:
#line 761 "GenesisParser.ypp"
{
		    yyval.pn = yyvsp[0].pn;
		  ;
    break;}
case 89:
#line 768 "GenesisParser.ypp"
{
		    ReturnIdents = 1;
		  ;
    break;}
case 90:
#line 772 "GenesisParser.ypp"
{
		    ResultValue	v;

		    v.r_str = (char *) MakeScriptInfo();
		    yyval.pn = PTNew(SL, v, yyvsp[-3].pn, yyvsp[0].pn);
		  ;
    break;}
case 91:
#line 781 "GenesisParser.ypp"
{
		    ReturnIdents = 0;
		  ;
    break;}
case 92:
#line 785 "GenesisParser.ypp"
{
		    yyval.pn = vardef(yyvsp[-2].str, DefType, DefCast, yyvsp[0].pn);
		    free(yyvsp[-2].str);
		  ;
    break;}
case 93:
#line 792 "GenesisParser.ypp"
{ yyval.pn = NULL; ;
    break;}
case 94:
#line 794 "GenesisParser.ypp"
{ yyval.pn = yyvsp[0].pn; ;
    break;}
case 95:
#line 798 "GenesisParser.ypp"
{
		    ResultValue	v;

		    if (BreakAllowed)
			yyval.pn = PTNew(BREAK, v, NULL, NULL);
		    else
			yyerror("BREAK found outside of a loop");
			/* No Return */
		  ;
    break;}
case 96:
#line 810 "GenesisParser.ypp"
{
		    ResultValue	v;

		    yyval.pn = PTNew(RETURN, v, yyvsp[0].pn, NULL);
		  ;
    break;}
case 97:
#line 816 "GenesisParser.ypp"
{
		    ResultValue	v;

		    yyval.pn = PTNew(RETURN, v, NULL, NULL);
		  ;
    break;}
case 98:
#line 824 "GenesisParser.ypp"
{ yyval.pn = NULL; ;
    break;}
case 99:
#line 828 "GenesisParser.ypp"
{ yyval.pn = PTNew('|', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 100:
#line 830 "GenesisParser.ypp"
{ yyval.pn = PTNew('&', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 101:
#line 832 "GenesisParser.ypp"
{ yyval.pn = PTNew('^', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 102:
#line 834 "GenesisParser.ypp"
{ yyval.pn = PTNew('~', RV, yyvsp[0].pn, NULL); ;
    break;}
case 103:
#line 837 "GenesisParser.ypp"
{ yyval.pn = PTNew('@', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 104:
#line 840 "GenesisParser.ypp"
{ yyval.pn = PTNew('+', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 105:
#line 842 "GenesisParser.ypp"
{ yyval.pn = PTNew('-', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 106:
#line 844 "GenesisParser.ypp"
{ yyval.pn = PTNew('*', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 107:
#line 846 "GenesisParser.ypp"
{ yyval.pn = PTNew('/', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 108:
#line 848 "GenesisParser.ypp"
{ yyval.pn = PTNew('%', RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 109:
#line 850 "GenesisParser.ypp"
{ yyval.pn = PTNew(POW, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 110:
#line 852 "GenesisParser.ypp"
{ yyval.pn = PTNew(UMINUS, RV, yyvsp[0].pn, NULL); ;
    break;}
case 111:
#line 855 "GenesisParser.ypp"
{ yyval.pn = PTNew(OR, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 112:
#line 857 "GenesisParser.ypp"
{ yyval.pn = PTNew(AND, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 113:
#line 859 "GenesisParser.ypp"
{ yyval.pn = PTNew('!', RV, yyvsp[0].pn, NULL); ;
    break;}
case 114:
#line 862 "GenesisParser.ypp"
{ yyval.pn = PTNew(LT, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 115:
#line 864 "GenesisParser.ypp"
{ yyval.pn = PTNew(LE, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 116:
#line 866 "GenesisParser.ypp"
{ yyval.pn = PTNew(GT, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 117:
#line 868 "GenesisParser.ypp"
{ yyval.pn = PTNew(GE, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 118:
#line 870 "GenesisParser.ypp"
{ yyval.pn = PTNew(EQ, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 119:
#line 872 "GenesisParser.ypp"
{ yyval.pn = PTNew(NE, RV, yyvsp[-2].pn, yyvsp[0].pn); ;
    break;}
case 120:
#line 875 "GenesisParser.ypp"
{ 
		    Result	*rp;
		    ResultValue	v;

		    /*
		    ** Variable reference
		    */

		    rp = (Result *) yyvsp[0].str;
		      {
			if (rp->r_type == FUNCTION || rp->r_type == LOCREF)
			    v = rp->r;
			else /* Global Variable */
			    v.r_str = (char *) rp;

		        yyval.pn = PTNew(rp->r_type, v, NULL, NULL);
		      }
 		  ;
    break;}
case 121:
#line 895 "GenesisParser.ypp"
{ 
		    ResultValue	v;

		    v.r_float = yyvsp[0].fconst;
		    yyval.pn = PTNew(FLOATCONST, v, NULL, NULL);
 		  ;
    break;}
case 122:
#line 902 "GenesisParser.ypp"
{ 
		    ResultValue	v;

		    v.r_int = yyvsp[0].iconst;
		    yyval.pn = PTNew(INTCONST, v, NULL, NULL);
 		  ;
    break;}
case 123:
#line 909 "GenesisParser.ypp"
{ 
		    ResultValue	v;

		    v.r_str = yyvsp[0].str;
		    yyval.pn = PTNew(STRCONST, v, NULL, NULL);
 		  ;
    break;}
case 124:
#line 917 "GenesisParser.ypp"
{
		    ResultValue	v;

		    v.r_int = yyvsp[0].iconst;
		    yyval.pn = PTNew(DOLLARARG, v, NULL, NULL);
		  ;
    break;}
case 125:
#line 925 "GenesisParser.ypp"
{ yyval.pn = yyvsp[-1].pn; ;
    break;}
case 126:
#line 929 "GenesisParser.ypp"
{ yyval.pn = yyvsp[-1].pn; ;
    break;}
case 127:
#line 933 "GenesisParser.ypp"
{ yyval.pn = yyvsp[-1].pn; ;
    break;}
case 128:
#line 937 "GenesisParser.ypp"
{ yyval.pn = yyvsp[-1].pn; ;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 941 "GenesisParser.ypp"



/*
** TokenStr
**
**	Return the token string for the given token.
*/

char *TokenStr(int token)
{	/* TokenStr --- Return token string for token */

	static char	buf[100];

	switch (token)
	  {

	  case LT: return("<");
	  case LE: return("<=");
	  case GT: return(">");
	  case GE: return(">=");
	  case EQ: return("==");
	  case NE: return("!=");

	  case OR: return("||");
	  case AND: return("&&");

#define	RET(tok)	case tok: return("tok")

	  RET(UMINUS);

	  RET(WHILE);
	  RET(IF);
	  RET(ELSE);
	  RET(FOR);
	  RET(FOREACH);
	  RET(END);
	  RET(INCLUDE);
	  RET(BREAK);
	  RET(INT);
	  RET(FLOAT);
	  RET(STR);
	  RET(RETURN);
	  RET(WHITESPACE);
	  RET(FUNCTION);
	  RET(INTCONST);
	  RET(DOLLARARG);
	  RET(FLOATCONST);
	  RET(STRCONST);
	  RET(LITERAL);
	  RET(IDENT);
	  RET(VARREF);
	  RET(FUNCREF);
	  RET(SL);
	  RET(COMMAND);
	  RET(ARGUMENT);
	  RET(ARGLIST);
	  RET(LOCREF);
	  RET(ICAST);
	  RET(FCAST);
	  RET(SCAST);

	  }

	if (token < 128)
	    if (token < ' ')
		sprintf(buf, "^%c", token+'@');
	    else
		sprintf(buf, "%c", token);
	else
	    sprintf(buf, "%d", token);

	return(buf);

}	/* TokenStr */


ParseNode * myFlexLexer::vardef(char* ident, int type, int castop, ParseNode* init)
{	/* vardef --- Define a variable */

	ParseNode	*pn;
	Result		*rp;
	// Result		*r;
	ResultValue	v, slv;

	if (InFunctionDefinition && LocalSymbols != NULL)
	  {
	    rp = SymtabNew(LocalSymbols, ident);
	    if (rp->r_type == 0)
	      {
	        rp->r_type = LOCREF;
		rp->r.r_loc.l_type = type;
		rp->r.r_loc.l_offs = NextLocal++;
	      }

	    v.r_str = (char *) rp;
	    pn = PTNew(castop, v, NULL, NULL);
	    if (init)
	      {
		slv.r_str = (char *) MakeScriptInfo();
		pn = PTNew(SL, slv, pn, PTNew('=', v, init, NULL));
	      }
	  }
	else
	  {
	    rp = SymtabNew(&GlobalSymbols, ident);
	    switch(type)
	      {

	      case INT:
	        if (rp->r_type == 0)
	            rp->r.r_int = 0;
	        else
		    CastToInt(rp);
	        break;

	      case FLOAT:
	        if (rp->r_type == 0)
	            rp->r.r_float = 0.0;
	        else
		    CastToFloat(rp);
	        break;

	      case STR:
	        if (rp->r_type == 0)
	            rp->r.r_str = (char *) strsave("");
	        else
		    CastToStr(rp);
	        break;

	      }

	    rp->r_type = type;
	    v.r_str = (char *) rp;
	    if (init)
	        pn = PTNew('=', v, init, NULL);
	    else
	        pn = NULL;
	  }

	return(pn);

}	/* vardef */


void myFlexLexer::ParseInit()

{    /* ParseInit --- Initialize parser variables */

        InFunctionDefinition = 0;
	Compiling = 0;
	BreakAllowed = 0;
	LocalSymbols = NULL;
	nextchar(1);	/* Flush lexer input */
	PTInit();	/* Reinit parse tree evaluation */

}    /* ParseInit */


int myFlexLexer::NestedLevel()

{    /* NestedLevel --- Return TRUE if in func_def or control structure */

        return(InFunctionDefinition || Compiling);

}    /* NestedLevel */

