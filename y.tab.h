/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     IF = 258,
     ELSE = 259,
     FOR = 260,
     WHILE = 261,
     INT = 262,
     FLOAT = 263,
     VOID = 264,
     RETURN = 265,
     ASSIGNOP = 266,
     NOT = 267,
     LPAREN = 268,
     RPAREN = 269,
     LCURL = 270,
     RCURL = 271,
     LTHIRD = 272,
     RTHIRD = 273,
     COMMA = 274,
     SEMICOLON = 275,
     PRINTLN = 276,
     ID = 277,
     ADDOP = 278,
     INCOP = 279,
     DECOP = 280,
     MULOP = 281,
     RELOP = 282,
     LOGICOP = 283,
     CONST_INT = 284,
     CONST_FLOAT = 285
   };
#endif
/* Tokens.  */
#define IF 258
#define ELSE 259
#define FOR 260
#define WHILE 261
#define INT 262
#define FLOAT 263
#define VOID 264
#define RETURN 265
#define ASSIGNOP 266
#define NOT 267
#define LPAREN 268
#define RPAREN 269
#define LCURL 270
#define RCURL 271
#define LTHIRD 272
#define RTHIRD 273
#define COMMA 274
#define SEMICOLON 275
#define PRINTLN 276
#define ID 277
#define ADDOP 278
#define INCOP 279
#define DECOP 280
#define MULOP 281
#define RELOP 282
#define LOGICOP 283
#define CONST_INT 284
#define CONST_FLOAT 285




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 18 "1805047.y"
{
	symbol_info* symbolValue;
	string *input_string;
}
/* Line 1529 of yacc.c.  */
#line 114 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

