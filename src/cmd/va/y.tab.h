


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
#line 13 "a.y" /* yacc.c:1909  */

	Sym	*sym;
	int32	lval;
	double	dval;
	char	sval[8];
	Addr	addr;

#line 164 "y.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
