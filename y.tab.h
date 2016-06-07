
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     NASOKA = 259,
     VARIABLE = 260,
     WHILE = 261,
     IF = 262,
     PRINT = 263,
     NEWLINE = 264,
     POVTORUVAJ = 265,
     DO = 266,
     PATI = 267,
     OSNOVNA_KOMANDA = 268,
     PROCEDURA = 269,
     POCETOK = 270,
     KRAJ = 271,
     T_BROJ = 272,
     T_NASOKA = 273,
     CALL = 274,
     POVTORUVAJ_2 = 275,
     IFX = 276,
     ELSE = 277,
     NE = 278,
     EQ = 279,
     LE = 280,
     GE = 281,
     UMINUS = 282
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define NASOKA 259
#define VARIABLE 260
#define WHILE 261
#define IF 262
#define PRINT 263
#define NEWLINE 264
#define POVTORUVAJ 265
#define DO 266
#define PATI 267
#define OSNOVNA_KOMANDA 268
#define PROCEDURA 269
#define POCETOK 270
#define KRAJ 271
#define T_BROJ 272
#define T_NASOKA 273
#define CALL 274
#define POVTORUVAJ_2 275
#define IFX 276
#define ELSE 277
#define NE 278
#define EQ 279
#define LE 280
#define GE 281
#define UMINUS 282




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 24 "robol.y"

    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */



/* Line 1676 of yacc.c  */
#line 114 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


