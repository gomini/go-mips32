




#define YYBISON 1

#define YYBISON_VERSION "3.0.2"

#define YYSKELETON_NAME "yacc.c"

#define YYPURE 0

#define YYPUSH 0

#define YYPULL 1




#line 5 "a.y" /* yacc.c:339  */

#include <u.h>
#include <stdio.h>
#include <libc.h>
#include "a.h"
#include "../../runtime/funcdata.h"

#line 74 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LTYPE1 = 258,
    LTYPE2 = 259,
    LTYPE3 = 260,
    LTYPE4 = 261,
    LTYPE5 = 262,
    LTYPE6 = 263,
    LTYPE7 = 264,
    LTYPE8 = 265,
    LTYPE9 = 266,
    LTYPEA = 267,
    LTYPEB = 268,
    LTYPEC = 269,
    LTYPED = 270,
    LTYPEE = 271,
    LTYPEF = 272,
    LTYPEG = 273,
    LTYPEH = 274,
    LTYPEI = 275,
    LTYPEJ = 276,
    LMUL32 = 277,
    LMOVN = 278,
    LBITOP = 279,
    LPREF = 280,
    LLL = 281,
    LSC = 282,
    LBRK = 283,
    LCONST = 284,
    LSP = 285,
    LSB = 286,
    LFP = 287,
    LPC = 288,
    LHI = 289,
    LLO = 290,
    LMREG = 291,
    LPCDAT = 292,
    LFUNCDAT = 293,
    LREG = 294,
    LFREG = 295,
    LFCREG = 296,
    LR = 297,
    LM = 298,
    LF = 299,
    LFCR = 300,
    LSCHED = 301,
    LFCONST = 302,
    LSCONST = 303,
    LNAME = 304,
    LLAB = 305,
    LVAR = 306
  };
#endif
#define LTYPE1 258
#define LTYPE2 259
#define LTYPE3 260
#define LTYPE4 261
#define LTYPE5 262
#define LTYPE6 263
#define LTYPE7 264
#define LTYPE8 265
#define LTYPE9 266
#define LTYPEA 267
#define LTYPEB 268
#define LTYPEC 269
#define LTYPED 270
#define LTYPEE 271
#define LTYPEF 272
#define LTYPEG 273
#define LTYPEH 274
#define LTYPEI 275
#define LTYPEJ 276
#define LMUL32 277
#define LMOVN 278
#define LBITOP 279
#define LPREF 280
#define LLL 281
#define LSC 282
#define LBRK 283
#define LCONST 284
#define LSP 285
#define LSB 286
#define LFP 287
#define LPC 288
#define LHI 289
#define LLO 290
#define LMREG 291
#define LPCDAT 292
#define LFUNCDAT 293
#define LREG 294
#define LFREG 295
#define LFCREG 296
#define LR 297
#define LM 298
#define LF 299
#define LFCR 300
#define LSCHED 301
#define LFCONST 302
#define LSCONST 303
#define LNAME 304
#define LLAB 305
#define LVAR 306

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 13 "a.y" /* yacc.c:355  */

	Sym	*sym;
	int32	lval;
	double	dval;
	char	sval[8];
	Addr	addr;

#line 224 "y.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */


#line 239 "y.tab.c" /* yacc.c:358  */

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

#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
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
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
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
void *malloc (YYSIZE_T);
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *);
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

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

#define YYFINAL  2
#define YYLAST   668

#define YYNTOKENS  70
#define YYNNTS  28
#define YYNRULES  126
#define YYNSTATES  300

#define YYUNDEFTOK  2
#define YYMAXUTOK   306

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    68,    12,     5,     2,
      66,    67,    10,     8,    65,     9,     2,    11,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    62,    64,
       6,    63,     7,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     4,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     2,    69,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61
};

#if YYDEBUG
static const yytype_uint16 yyrline[] =
{
       0,    42,    42,    43,    47,    46,    55,    60,    66,    70,
      71,    72,    78,    82,    89,    93,   100,   111,   118,   129,
     133,   140,   144,   148,   152,   156,   163,   171,   182,   193,
     201,   212,   225,   232,   236,   240,   244,   251,   258,   265,
     274,   285,   289,   293,   300,   304,   311,   318,   325,   332,
     339,   346,   353,   358,   359,   362,   368,   379,   380,   381,
     382,   383,   388,   395,   396,   397,   398,   399,   404,   411,
     412,   415,   418,   424,   432,   438,   446,   452,   459,   465,
     470,   475,   481,   487,   495,   496,   502,   510,   519,   520,
     526,   529,   530,   536,   543,   552,   553,   555,   563,   571,
     572,   580,   588,   596,   606,   609,   613,   619,   620,   621,
     624,   625,   629,   633,   637,   641,   647,   648,   652,   656,
     660,   664,   668,   672,   676,   680,   684
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "'|'", "'^'", "'&'", "'<'", "'>'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "LTYPE1", "LTYPE2", "LTYPE3", "LTYPE4",
  "LTYPE5", "LTYPE6", "LTYPE7", "LTYPE8", "LTYPE9", "LTYPEA", "LTYPEB",
  "LTYPEC", "LTYPED", "LTYPEE", "LTYPEF", "LTYPEG", "LTYPEH", "LTYPEI",
  "LTYPEJ", "LMUL32", "LMOVN", "LBITOP", "LPREF", "LLL", "LSC", "LBRK",
  "LCONST", "LSP", "LSB", "LFP", "LPC", "LHI", "LLO", "LMREG", "LPCDAT",
  "LFUNCDAT", "LREG", "LFREG", "LFCREG", "LR", "LM", "LF", "LFCR",
  "LSCHED", "LFCONST", "LSCONST", "LNAME", "LLAB", "LVAR", "':'", "'='",
  "';'", "','", "'('", "')'", "'$'", "'~'", "$accept", "prog", "line",
  "$@1", "inst", "comma", "rel", "vlgen", "vgen", "lgen", "fgen", "mreg",
  "fcreg", "freg", "ximm", "nireg", "ireg", "gen", "oreg", "imr", "imm",
  "reg", "sreg", "name", "offset", "pointer", "con", "expr", YY_NULLPTR
};
#endif

# ifdef YYPRINT
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   124,    94,    38,    60,    62,    43,    45,
      42,    47,    37,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,    58,    61,    59,    44,    40,    41,    36,   126
};
# endif

#define YYPACT_NINF -144

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-144)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

static const yytype_int16 yypact[] =
{
    -144,   293,  -144,   -60,    59,    59,   512,   -46,   376,   -20,
     -46,    66,   518,   518,   205,   205,   -32,   -32,   -32,   -46,
     -46,   402,   430,   -20,   -20,   -20,   -38,   489,   -20,   -38,
     -38,   -38,   -24,    20,   -29,  -144,  -144,   -18,  -144,  -144,
     -15,   599,   -13,  -144,  -144,  -144,     1,   599,   599,  -144,
     137,  -144,   347,    48,   599,     3,  -144,  -144,  -144,  -144,
       4,     6,  -144,  -144,  -144,  -144,  -144,  -144,  -144,    11,
      22,    44,    21,  -144,  -144,  -144,  -144,  -144,    47,   493,
     493,    51,    54,    79,   599,    84,    62,   143,    99,   103,
     105,   560,   119,  -144,  -144,   464,  -144,   -46,  -144,  -144,
    -144,  -144,  -144,   135,   139,   141,   145,   146,   147,     6,
     152,  -144,   156,   157,  -144,  -144,   599,   599,  -144,   599,
    -144,   -20,    59,  -144,  -144,   194,   599,   599,   142,   148,
    -144,    90,   566,   177,  -144,  -144,  -144,     6,  -144,   518,
     -20,    83,   599,   599,   599,   464,   -20,   137,   347,  -144,
    -144,  -144,  -144,   181,  -144,  -144,   583,   537,   560,   585,
      19,   599,   -32,   -30,   166,   150,  -144,   182,  -144,  -144,
    -144,   464,   -20,   -20,   -20,   489,   -20,   489,   -38,   489,
     345,   231,   531,   184,  -144,   187,  -144,   188,   150,  -144,
    -144,    19,  -144,   599,   599,   599,   243,   247,   599,   599,
     599,   599,   599,  -144,  -144,   489,  -144,   189,  -144,  -144,
    -144,   192,   195,   196,   198,   200,  -144,   190,   142,   201,
     136,  -144,  -144,   204,  -144,   207,  -144,  -144,   210,   212,
    -144,   214,  -144,   -46,  -144,   218,  -144,   215,   219,  -144,
    -144,  -144,  -144,  -144,  -144,  -144,  -144,  -144,  -144,   -20,
      59,   217,   223,   605,   327,   185,   599,   599,   151,   151,
    -144,  -144,  -144,  -144,  -144,  -144,  -144,  -144,  -144,  -144,
     -20,  -144,  -144,   225,    83,   560,   -38,   119,   -32,  -144,
     -20,   -20,  -144,  -144,   244,  -144,   220,   220,  -144,  -144,
    -144,   277,  -144,  -144,  -144,  -144,   229,   599,  -144,  -144
};

static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,    53,     0,     0,
      53,    53,     0,     0,     0,     0,     0,     0,     0,    53,
      53,    53,    53,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     9,     3,     0,    11,    99,
       0,     0,     0,    96,    95,    98,     0,     0,     0,   110,
     104,   111,     0,     0,     0,     0,    70,    69,    90,    88,
      91,    89,    54,    17,    61,    62,    72,    76,    74,     0,
       0,     0,     0,    57,    58,    59,    60,    71,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    67,    68,    54,    41,    53,    64,    65,
      66,    63,    44,     0,     0,     0,     0,     0,     0,     0,
       0,    52,     0,     0,     8,     4,     0,     0,    10,     0,
      97,     0,     0,   113,   112,     0,     0,     0,     0,     0,
     116,     0,     0,     0,    82,    81,    79,    78,   114,     0,
       0,     0,     0,     0,     0,     0,     0,   104,     0,    21,
      22,    84,    86,     0,    23,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   104,    37,     0,    38,    42,
      43,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    13,    98,    15,    98,   104,   105,
     106,     0,    93,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   115,    83,     0,    16,     0,   108,   107,
     109,     0,     0,     0,     0,     0,    18,    53,    56,     0,
       0,    85,    25,     0,    26,     0,    28,    29,     0,     0,
      33,    76,    34,    53,    56,     0,    45,     0,     0,    48,
      49,    50,    51,    39,    40,     5,     6,     7,   100,     0,
       0,     0,     0,   126,   125,   124,     0,     0,   117,   118,
     119,   120,   121,    80,    92,    94,   101,    73,    77,    75,
      54,    19,    87,     0,     0,     0,     0,     0,     0,    36,
       0,     0,    12,    14,     0,   102,   122,   123,    20,    55,
      27,    30,    32,    35,    46,    47,     0,     0,   103,    31
};

static const yytype_int16 yypgoto[] =
{
    -144,  -144,   107,  -144,  -144,    43,   -66,  -144,   -11,   289,
     290,   291,   294,    27,   -91,   -77,  -143,    63,    28,     0,
      -3,     8,    26,    24,   -98,   110,    -6,    25
};

static const yytype_int16 yydefgoto[] =
{
      -1,     1,    36,   180,    37,    63,   149,    72,    97,    55,
      98,    99,   100,    77,    56,   150,   151,   101,    58,    42,
      43,    59,    45,    60,   128,   212,   130,   131
};

static const yytype_uint16 yytable[] =
{
      61,   168,    61,   155,    38,    46,    61,    61,    86,    86,
     221,   103,    44,    44,   154,    61,    61,    78,    67,    62,
     231,   109,    70,   107,    70,   166,   111,   112,   113,    39,
      41,   104,    40,   106,   117,   120,   110,    81,    85,    87,
     114,   123,   124,    88,    89,    90,   118,   137,   138,   218,
     105,   119,   121,    79,    80,   108,    47,   132,   133,   208,
     209,   210,    91,    92,    96,   102,   122,   234,   139,    57,
     140,    57,   141,   153,   153,    82,    83,   142,   129,   222,
     221,   136,   115,   116,   169,   167,   145,    49,   143,    61,
     251,   224,   226,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   152,   152,   134,   135,    50,    39,    51,
     144,    40,   146,   183,    52,    39,   156,    54,    40,   157,
     189,   190,   186,   208,   209,   210,   124,    41,   160,   184,
      44,    62,    39,    61,   216,    40,   213,   214,   215,    61,
     170,   181,   182,   125,   158,   126,   127,   185,   187,   159,
     223,   167,   167,   228,   161,   229,   227,   203,   126,   127,
     236,   200,   201,   202,   162,    61,   207,   211,   163,   109,
     164,   109,   217,   109,   219,   243,   208,   209,   210,   273,
     152,   238,   239,   225,   241,    39,   292,    53,    40,   230,
     232,   196,   197,   198,   199,   200,   201,   202,   237,   109,
     171,   188,   206,   240,   172,   242,   173,   244,   191,   290,
     174,   175,   176,    47,    48,   192,   233,   177,   253,   254,
     255,   178,   179,   258,   259,   260,   261,   262,   198,   199,
     200,   201,   202,   263,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,    49,   205,   219,   220,   235,   256,
     283,   248,   249,   250,   257,   270,   264,   282,    44,   265,
     271,   273,   266,   267,    50,   268,    51,   269,   272,   167,
     274,    84,   275,   291,    54,   276,   279,   277,   288,   278,
     280,   286,   287,   284,   281,   296,   297,   245,   294,   295,
     285,   299,   289,     2,     3,   246,   298,    73,    74,    75,
     219,   252,    76,     0,     0,   293,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,   195,   196,   197,   198,   199,   200,   201,   202,
      30,    31,     0,     0,     0,     0,     3,     0,     0,    32,
       0,     0,    33,     0,    34,    47,    48,    35,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    47,    48,    49,     0,     0,     0,
       0,     0,    30,    31,     0,     0,    39,     0,     0,    40,
       0,    32,     0,     0,    33,     0,    34,     0,    51,    35,
      47,    48,     0,    84,     0,    49,    54,     0,     0,     0,
      64,    65,    66,     0,     0,    39,    67,    68,    40,    69,
      70,    71,     0,     0,     0,    50,     0,    51,    47,    48,
       0,    49,    52,     0,    53,    54,    93,    94,    66,     0,
       0,    39,    67,    68,    40,    69,    70,    71,     0,     0,
       0,    50,     0,    51,     0,     0,     0,    95,    52,    49,
       0,    54,    47,    48,    93,    94,    66,     0,     0,    39,
      67,    68,    40,    69,    70,    71,     0,     0,     0,    50,
       0,    51,     0,     0,     0,    62,    52,    47,    48,    54,
       0,    47,    48,    49,     0,     0,     0,     0,    93,    94,
      66,     0,     0,    39,    67,    68,    40,    69,    70,    71,
      47,    48,     0,    50,     0,    51,    47,    48,    49,     0,
      52,     0,    49,    54,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,     0,    47,    48,     0,    50,     0,
      51,    49,   147,     0,    51,    52,     0,    49,    54,   148,
       0,    39,    54,     0,    40,     0,     0,    39,    47,    48,
      40,    50,     0,    51,    47,    48,    49,    50,    52,    51,
      53,    54,     0,     0,    52,     0,    39,    54,     0,    40,
       0,    47,    48,    47,    48,   247,   165,     0,    51,    49,
       0,     0,     0,    84,     0,    49,    54,    47,    48,   194,
     195,   196,   197,   198,   199,   200,   201,   202,     0,   165,
       0,    51,    49,   204,    49,     0,    84,    51,     0,    54,
       0,     0,    84,     0,     0,    54,     0,     0,    49,     0,
       0,     0,    50,     0,    51,     0,    51,     0,     0,   148,
       0,    84,    54,    41,    54,     0,     0,     0,     0,     0,
      51,     0,     0,     0,     0,    84,     0,     0,    54
};

static const yytype_int16 yycheck[] =
{
       6,    92,     8,    80,    64,     5,    12,    13,    14,    15,
     153,    22,     4,     5,    80,    21,    22,     9,    50,    65,
      50,    27,    54,    26,    54,    91,    29,    30,    31,    49,
      68,    23,    52,    25,    63,    41,    28,    11,    14,    15,
      64,    47,    48,    16,    17,    18,    64,    53,    54,   147,
      24,    66,    65,    10,    11,    27,     8,     9,    10,    40,
      41,    42,    19,    20,    21,    22,    65,   165,    65,     6,
      66,     8,    66,    79,    80,    12,    13,    66,    52,   156,
     223,    53,    62,    63,    95,    91,    65,    39,    66,    95,
     188,   157,   158,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    79,    80,    57,    58,    59,    49,    61,
      66,    52,    65,   119,    66,    49,    65,    69,    52,    65,
     126,   127,   122,    40,    41,    42,   132,    68,    66,   121,
     122,    65,    49,   139,   145,    52,   142,   143,   144,   145,
      97,   116,   117,     6,    65,     8,     9,   121,   122,    65,
     156,   157,   158,   159,    11,   161,   159,    67,     8,     9,
     171,    10,    11,    12,    65,   171,   140,   141,    65,   175,
      65,   177,   146,   179,   148,   178,    40,    41,    42,    43,
     156,   173,   174,   157,   176,    49,   277,    68,    52,   162,
     163,     6,     7,     8,     9,    10,    11,    12,   172,   205,
      65,     7,   139,   175,    65,   177,    65,   179,    66,   275,
      65,    65,    65,     8,     9,    67,    50,    65,   193,   194,
     195,    65,    65,   198,   199,   200,   201,   202,     8,     9,
      10,    11,    12,   205,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    39,    68,   220,    66,    66,     6,
     250,    67,    65,    65,     7,    65,    67,   249,   250,    67,
     217,    43,    67,    67,    59,    67,    61,    67,    67,   275,
      66,    66,    65,   276,    69,    65,   233,    65,   270,    65,
      65,   256,   257,    66,    65,    41,     9,   180,   280,   281,
      67,   297,    67,     0,     1,    64,    67,     8,     8,     8,
     274,   191,     8,    -1,    -1,   278,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,     5,     6,     7,     8,     9,    10,    11,    12,
      47,    48,    -1,    -1,    -1,    -1,     1,    -1,    -1,    56,
      -1,    -1,    59,    -1,    61,     8,     9,    64,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,     8,     9,    39,    -1,    -1,    -1,
      -1,    -1,    47,    48,    -1,    -1,    49,    -1,    -1,    52,
      -1,    56,    -1,    -1,    59,    -1,    61,    -1,    61,    64,
       8,     9,    -1,    66,    -1,    39,    69,    -1,    -1,    -1,
      44,    45,    46,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    -1,    -1,    -1,    59,    -1,    61,     8,     9,
      -1,    39,    66,    -1,    68,    69,    44,    45,    46,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    59,    -1,    61,    -1,    -1,    -1,    65,    66,    39,
      -1,    69,     8,     9,    44,    45,    46,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    -1,    -1,    -1,    59,
      -1,    61,    -1,    -1,    -1,    65,    66,     8,     9,    69,
      -1,     8,     9,    39,    -1,    -1,    -1,    -1,    44,    45,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
       8,     9,    -1,    59,    -1,    61,     8,     9,    39,    -1,
      66,    -1,    39,    69,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    -1,     8,     9,    -1,    59,    -1,
      61,    39,    59,    -1,    61,    66,    -1,    39,    69,    66,
      -1,    49,    69,    -1,    52,    -1,    -1,    49,     8,     9,
      52,    59,    -1,    61,     8,     9,    39,    59,    66,    61,
      68,    69,    -1,    -1,    66,    -1,    49,    69,    -1,    52,
      -1,     8,     9,     8,     9,    64,    59,    -1,    61,    39,
      -1,    -1,    -1,    66,    -1,    39,    69,     8,     9,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    -1,    59,
      -1,    61,    39,    57,    39,    -1,    66,    61,    -1,    69,
      -1,    -1,    66,    -1,    -1,    69,    -1,    -1,    39,    -1,
      -1,    -1,    59,    -1,    61,    -1,    61,    -1,    -1,    66,
      -1,    66,    69,    68,    69,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    -1,    -1,    -1,    66,    -1,    -1,    69
};

static const yytype_uint8 yystos[] =
{
       0,    71,     0,     1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      47,    48,    56,    59,    61,    64,    72,    74,    64,    49,
      52,    68,    89,    90,    91,    92,    89,     8,     9,    39,
      59,    61,    66,    68,    69,    79,    84,    87,    88,    91,
      93,    96,    65,    75,    44,    45,    46,    50,    51,    53,
      54,    55,    77,    79,    80,    81,    82,    83,    91,    75,
      75,    92,    87,    87,    66,    93,    96,    93,    83,    83,
      83,    75,    75,    44,    45,    65,    75,    78,    80,    81,
      82,    87,    75,    78,    91,    92,    91,    90,    88,    96,
      91,    90,    90,    90,    64,    62,    63,    63,    64,    66,
      96,    65,    65,    96,    96,     6,     8,     9,    94,    92,
      96,    97,     9,    10,    57,    58,    88,    96,    96,    65,
      66,    66,    66,    66,    66,    65,    65,    59,    66,    76,
      85,    86,    93,    96,    76,    85,    65,    65,    65,    65,
      66,    11,    65,    65,    65,    59,    76,    96,    84,    78,
      75,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      73,    97,    97,    96,    91,    92,    89,    92,     7,    96,
      96,    66,    67,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    67,    57,    68,    87,    92,    40,    41,
      42,    92,    95,    96,    96,    96,    78,    92,    94,    92,
      66,    86,    85,    96,    76,    92,    76,    90,    96,    96,
      83,    50,    83,    50,    94,    66,    78,    92,    91,    91,
      88,    91,    88,    90,    88,    72,    64,    64,    67,    65,
      65,    94,    95,    97,    97,    97,     6,     7,    97,    97,
      97,    97,    97,    88,    67,    67,    67,    67,    67,    67,
      65,    75,    67,    43,    66,    65,    65,    65,    65,    75,
      65,    65,    91,    89,    66,    67,    97,    97,    91,    67,
      76,    90,    84,    83,    91,    91,    41,     9,    67,    96
};

static const yytype_uint8 yyr1[] =
{
       0,    70,    71,    71,    73,    72,    72,    72,    72,    72,
      72,    72,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    75,    75,    76,    76,    77,    77,    77,
      77,    77,    77,    78,    78,    78,    78,    78,    78,    79,
      79,    80,    81,    81,    82,    82,    83,    83,    84,    84,
      84,    84,    84,    84,    85,    85,    85,    86,    87,    87,
      87,    88,    88,    88,    88,    89,    89,    90,    91,    92,
      92,    93,    93,    93,    94,    94,    94,    95,    95,    95,
      96,    96,    96,    96,    96,    96,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97
};

static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     0,     4,     4,     4,     2,     1,
       2,     2,     6,     4,     6,     4,     4,     2,     4,     5,
       6,     3,     3,     3,     3,     4,     4,     6,     4,     4,
       6,     8,     6,     4,     4,     6,     5,     3,     3,     4,
       4,     2,     3,     3,     2,     4,     6,     6,     4,     4,
       4,     4,     2,     0,     1,     4,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     1,     4,     1,     4,     2,     2,
       4,     2,     2,     3,     1,     2,     1,     3,     1,     1,
       1,     1,     4,     3,     4,     1,     1,     2,     1,     1,
       4,     4,     5,     7,     0,     2,     2,     1,     1,     1,
       1,     1,     2,     2,     2,     3,     1,     3,     3,     3,
       3,     3,     4,     4,     3,     3,     3
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
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

#define YYTERROR        1
#define YYERRCODE       256



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

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)



static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}



static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}


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



static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif


#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
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

static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  const char *yyformat = YY_NULLPTR;
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  int yycount = 0;

  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          int yyxbegin = yyn < 0 ? -yyn : 0;
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


static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




int yychar;

YYSTYPE yylval;
int yynerrs;



int
yyparse (void)
{
    int yystate;
    int yyerrstatus;


    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  int yytoken = 0;
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;
  goto yysetstate;

 yynewstate:
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

yybackup:


  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;


  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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

  if (yyerrstatus)
    yyerrstatus--;

  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


yyreduce:
  yylen = yyr2[yyn];

  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 47 "a.y" /* yacc.c:1646  */
    {
		(yyvsp[-1].sym) = labellookup((yyvsp[-1].sym));
		if((yyvsp[-1].sym)->type == LLAB && (yyvsp[-1].sym)->value != pc)
			yyerror("redeclaration of %s", (yyvsp[-1].sym)->labelname);
		(yyvsp[-1].sym)->type = LLAB;
		(yyvsp[-1].sym)->value = pc;
	}
#line 1590 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 56 "a.y" /* yacc.c:1646  */
    {
		(yyvsp[-3].sym)->type = LVAR;
		(yyvsp[-3].sym)->value = (yyvsp[-1].lval);
	}
#line 1599 "y.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 61 "a.y" /* yacc.c:1646  */
    {
		if((yyvsp[-3].sym)->value != (yyvsp[-1].lval))
			yyerror("redeclaration of %s", (yyvsp[-3].sym)->name);
		(yyvsp[-3].sym)->value = (yyvsp[-1].lval);
	}
#line 1609 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 67 "a.y" /* yacc.c:1646  */
    {
		nosched = (yyvsp[-1].lval);
	}
#line 1617 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 79 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-5].lval), &(yyvsp[-4].addr), (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1625 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 83 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1633 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 90 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-5].lval), &(yyvsp[-4].addr), (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1641 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 94 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1649 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 101 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1661 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 112 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-1].lval), &nullgen, NREG, &nullgen);
	}
#line 1669 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 119 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1681 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 130 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-4].lval), &(yyvsp[-3].addr), (yyvsp[-1].lval), &nullgen);
	}
#line 1689 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 134 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-5].lval), &(yyvsp[-4].addr), (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1697 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 141 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-2].lval), &nullgen, NREG, &(yyvsp[0].addr));
	}
#line 1705 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 145 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-2].lval), &nullgen, NREG, &(yyvsp[0].addr));
	}
#line 1713 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 149 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-2].lval), &nullgen, NREG, &(yyvsp[0].addr));
	}
#line 1721 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 153 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-2].lval), &nullgen, NREG, &(yyvsp[0].addr));
	}
#line 1729 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 157 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &nullgen, (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1737 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 164 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1749 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 172 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-5].lval), &(yyvsp[-4].addr), (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1761 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 183 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1773 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 194 "a.y" /* yacc.c:1646  */
    {
		settext((yyvsp[-2].addr).sym);
		(yyvsp[0].addr).type = D_CONST2;
		(yyvsp[0].addr).offset2 = ArgsSizeUnknown;
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1785 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 202 "a.y" /* yacc.c:1646  */
    {
		settext((yyvsp[-4].addr).sym);
		(yyvsp[0].addr).type = D_CONST2;
		(yyvsp[0].addr).offset2 = ArgsSizeUnknown;
		outcode((yyvsp[-5].lval), &(yyvsp[-4].addr), (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1800 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 213 "a.y" /* yacc.c:1646  */
    {
		settext((yyvsp[-6].addr).sym);
		(yyvsp[-2].addr).type = D_CONST2;
		(yyvsp[-2].addr).offset2 = (yyvsp[0].lval);
		outcode((yyvsp[-7].lval), &(yyvsp[-6].addr), (yyvsp[-4].lval), &(yyvsp[-2].addr));
	}
#line 1814 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 226 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-5].lval), &(yyvsp[-4].addr), (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1822 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 233 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1830 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 237 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1838 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 241 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-5].lval), &(yyvsp[-4].addr), (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1846 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 245 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-4].lval), &(yyvsp[-3].addr), (yyvsp[-1].lval), &nullgen);
	}
#line 1854 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 252 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-2].lval), &nullgen, NREG, &(yyvsp[0].addr));
	}
#line 1862 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 259 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-2].lval), &nullgen, NREG, &(yyvsp[0].addr));
	}
#line 1870 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 266 "a.y" /* yacc.c:1646  */
    {
		if((yyvsp[-2].addr).type != D_CONST && (yyvsp[0].addr).type != D_CONST)
			yyerror("arguments to PCDATA must be integer const");
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1880 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 275 "a.y" /* yacc.c:1646  */
    {
		if((yyvsp[-2].addr).type != D_CONST)
			yyerror("index for FUNCDATA must be integer const");
		if((yyvsp[0].addr).type != D_EXTERN && (yyvsp[0].addr).type != D_STATIC && (yyvsp[0].addr).type != D_OREG)
			yyerror("value for FUNCDATA must be symbol ref");
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1892 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 286 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-1].lval), &nullgen, NREG, &nullgen);
	}
#line 1900 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 290 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-2].lval), &nullgen, NREG, &(yyvsp[0].addr));
	}
#line 1908 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 294 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-2].lval), &(yyvsp[-1].addr), NREG, &nullgen);
	}
#line 1916 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 301 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-1].lval), &nullgen, NREG, &nullgen);
	}
#line 1924 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 305 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1932 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 312 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-5].lval), &(yyvsp[-4].addr), (yyvsp[-2].lval), &(yyvsp[0].addr));
	}
#line 1940 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 319 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-5].lval), &(yyvsp[-2].addr), (yyvsp[-4].lval), &(yyvsp[0].addr));
	}
#line 1948 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 326 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1956 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 333 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1964 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 340 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1972 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 347 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-3].lval), &(yyvsp[-2].addr), NREG, &(yyvsp[0].addr));
	}
#line 1980 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 354 "a.y" /* yacc.c:1646  */
    {
		outcode((yyvsp[-1].lval), &(yyvsp[0].addr), NREG, &nullgen);
	}
#line 1988 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 363 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_BRANCH;
		(yyval.addr).offset = (yyvsp[-3].lval) + pc;
	}
#line 1998 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 369 "a.y" /* yacc.c:1646  */
    {
		(yyvsp[-1].sym) = labellookup((yyvsp[-1].sym));
		(yyval.addr) = nullgen;
		if(pass == 2 && (yyvsp[-1].sym)->type != LLAB)
			yyerror("undefined label: %s", (yyvsp[-1].sym)->labelname);
		(yyval.addr).type = D_BRANCH;
		(yyval.addr).offset = (yyvsp[-1].sym)->value + (yyvsp[0].lval);
	}
#line 2011 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 384 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_HI;
	}
#line 2020 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 389 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_LO;
	}
#line 2029 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 400 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_HI;
	}
#line 2038 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 405 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_LO;
	}
#line 2047 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 419 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_MREG;
		(yyval.addr).reg = (yyvsp[0].lval);
	}
#line 2057 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 425 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_MREG;
		(yyval.addr).reg = (yyvsp[-1].lval);
	}
#line 2067 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 433 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_FCREG;
		(yyval.addr).reg = (yyvsp[0].lval);
	}
#line 2077 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 439 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_FCREG;
		(yyval.addr).reg = (yyvsp[-1].lval);
	}
#line 2087 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 447 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_FREG;
		(yyval.addr).reg = (yyvsp[0].lval);
	}
#line 2097 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 453 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_FREG;
		(yyval.addr).reg = (yyvsp[-1].lval);
	}
#line 2107 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 460 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_CONST;
		(yyval.addr).offset = (yyvsp[0].lval);
	}
#line 2117 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 466 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = (yyvsp[0].addr);
		(yyval.addr).type = D_CONST;
	}
#line 2126 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 471 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = (yyvsp[0].addr);
		(yyval.addr).type = D_OCONST;
	}
#line 2135 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 476 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_SCONST;
		memcpy((yyval.addr).u.sval, (yyvsp[0].sval), sizeof((yyval.addr).u.sval));
	}
#line 2145 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 482 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_FCONST;
		(yyval.addr).u.dval = (yyvsp[0].dval);
	}
#line 2155 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 488 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_FCONST;
		(yyval.addr).u.dval = -(yyvsp[0].dval);
	}
#line 2165 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 497 "a.y" /* yacc.c:1646  */
    {
		if((yyvsp[-1].lval) != 0)
			yyerror("offset must be zero");
		(yyval.addr) = (yyvsp[0].addr);
	}
#line 2175 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 503 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = (yyvsp[0].addr);
		if((yyvsp[0].addr).name != D_EXTERN && (yyvsp[0].addr).name != D_STATIC) {
		}
	}
#line 2185 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 511 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_OREG;
		(yyval.addr).reg = (yyvsp[-1].lval);
		(yyval.addr).offset = 0;
	}
#line 2196 "y.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 521 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_OREG;
		(yyval.addr).offset = (yyvsp[0].lval);
	}
#line 2206 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 531 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = (yyvsp[-3].addr);
		(yyval.addr).type = D_OREG;
		(yyval.addr).reg = (yyvsp[-1].lval);
	}
#line 2216 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 537 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_OREG;
		(yyval.addr).reg = (yyvsp[-1].lval);
		(yyval.addr).offset = 0;
	}
#line 2227 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 544 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_OREG;
		(yyval.addr).reg = (yyvsp[-1].lval);
		(yyval.addr).offset = (yyvsp[-3].lval);
	}
#line 2238 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 556 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_CONST;
		(yyval.addr).offset = (yyvsp[0].lval);
	}
#line 2248 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 564 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_REG;
		(yyval.addr).reg = (yyvsp[0].lval);
	}
#line 2258 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 573 "a.y" /* yacc.c:1646  */
    {
		if((yyval.lval) < 0 || (yyval.lval) >= NREG)
			print("register value out of range\n");
		(yyval.lval) = (yyvsp[-1].lval);
	}
#line 2268 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 581 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_OREG;
		(yyval.addr).name = (yyvsp[-1].lval);
		(yyval.addr).sym = nil;
		(yyval.addr).offset = (yyvsp[-3].lval);
	}
#line 2280 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 589 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_OREG;
		(yyval.addr).name = (yyvsp[-1].lval);
		(yyval.addr).sym = linklookup(ctxt, (yyvsp[-4].sym)->name, 0);
		(yyval.addr).offset = (yyvsp[-3].lval);
	}
#line 2292 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 597 "a.y" /* yacc.c:1646  */
    {
		(yyval.addr) = nullgen;
		(yyval.addr).type = D_OREG;
		(yyval.addr).name = D_STATIC;
		(yyval.addr).sym = linklookup(ctxt, (yyvsp[-6].sym)->name, 1);
		(yyval.addr).offset = (yyvsp[-3].lval);
	}
#line 2304 "y.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 606 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = 0;
	}
#line 2312 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 610 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[0].lval);
	}
#line 2320 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 614 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = -(yyvsp[0].lval);
	}
#line 2328 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 626 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[0].sym)->value;
	}
#line 2336 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 630 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = -(yyvsp[0].lval);
	}
#line 2344 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 634 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[0].lval);
	}
#line 2352 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 638 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = ~(yyvsp[0].lval);
	}
#line 2360 "y.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 642 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-1].lval);
	}
#line 2368 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 649 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-2].lval) + (yyvsp[0].lval);
	}
#line 2376 "y.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 653 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-2].lval) - (yyvsp[0].lval);
	}
#line 2384 "y.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 657 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-2].lval) * (yyvsp[0].lval);
	}
#line 2392 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 661 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-2].lval) / (yyvsp[0].lval);
	}
#line 2400 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 665 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-2].lval) % (yyvsp[0].lval);
	}
#line 2408 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 669 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-3].lval) << (yyvsp[0].lval);
	}
#line 2416 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 673 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-3].lval) >> (yyvsp[0].lval);
	}
#line 2424 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 677 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-2].lval) & (yyvsp[0].lval);
	}
#line 2432 "y.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 681 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-2].lval) ^ (yyvsp[0].lval);
	}
#line 2440 "y.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 685 "a.y" /* yacc.c:1646  */
    {
		(yyval.lval) = (yyvsp[-2].lval) | (yyvsp[0].lval);
	}
#line 2448 "y.tab.c" /* yacc.c:1646  */
    break;


#line 2452 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


yyerrlab:
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {

      if (yychar <= YYEOF)
        {
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  goto yyerrlab1;


yyerrorlab:

  if ( 0)
     goto yyerrorlab;

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


yyerrlab1:
  yyerrstatus = 3;

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

      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


yyacceptlab:
  yyresult = 0;
  goto yyreturn;

yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
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
