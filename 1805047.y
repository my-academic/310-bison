%{
#include<iostream>

#include<cstdlib>
#include<cstring>
#include<cmath>
#include "1805047_symbol_table.h"
#include "utils.h"


using namespace std;
int yyparse(void);
int yylex(void);
extern FILE *yyin;

%}

%union {
	symbol_info* symbolValue;
	string *input_string;
}



%token IF ELSE FOR WHILE INT FLOAT VOID RETURN ASSIGNOP NOT LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON PRINTLN
%token <symbolValue> ID 
%token <input_string> ADDOP INCOP DECOP MULOP RELOP LOGICOP CONST_INT CONST_FLOAT 
/* %token<symbolvalue> STRING */

%type <symbolValue> program unit parameter_list func_definition func_declaration compound_statement statements statement  variable logic_expression rel_expression simple_expression term unary_expression factor arguments argument_list expression expression_statement

%type <input_string> type_specifier declaration_list var_declaration 
/* %left
%right*/



%%

start : program
							{
								string str = stackPop(program);
								stackPush(start, str);
								printLog("start", "program", str);
							}
	;

program : program unit 
							{
								string str = stackPop(program) + "\n" + stackPop(unit);
								stackPush(program, str);
								printLog("program", "program unit", str);
							}
	| unit
							{
								string str = stackPop(unit);
								stackPush(program, str);
								printLog("program", "unit", str);
							}
	;
	
unit : var_declaration
							{
								string str = stackPop(var_declaration);
								stackPush(unit, str);
								printLog("unit", "var_declaration", str);
							}
     | func_declaration
							{
								string str = stackPop(func_declaration);
								stackPush(unit, str);
								printLog("unit", "func_declaration", str);
							}
     | func_definition
							{
								string str = stackPop(func_definition);
								stackPush(unit, str);
								printLog("unit", "func_definition", str);
							}
     ;
     
func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON
							{
								string str = stackPop(type_specifier) + $2->getName() + "(" + stackPop(parameter_list) + ");";
								setFunctionValues(*$1, $2, false);
								stackPush(func_declaration, str);
								printLog("func_declaration", "type_specifier ID LPAREN parameter_list RPAREN SEMICOLON", str);
							}
		| type_specifier ID LPAREN RPAREN SEMICOLON
							{
								string str = stackPop(type_specifier) + " " + $2->getName() + "();";
								setFunctionValues(*$1, $2, false);
								stackPush(func_declaration, str);
								printLog("func_declaration", "type_specifier ID LPAREN RPAREN SEMICOLON", str);
							}
		;
		 
func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement 
							{
								string str = stackPop(type_specifier) + " " + $2->getName() + "(" + stackPop(parameter_list) + ")" + stackPop(compound_statement);
								setFunctionValues(*$1, $2, true);
								stackPush(func_definition, str);
								printLog("func_definition", "type_specifier ID LPAREN parameter_list RPAREN compound_statement", str);
							}
		| type_specifier ID LPAREN RPAREN compound_statement
							{
								string str = stackPop(type_specifier) + " " + $2->getName() + "()" + stackPop(compound_statement);
								setFunctionValues(*$1, $2, true);
								stackPush(func_definition, str);
								printLog("func_definition", "type_specifier ID LPAREN RPAREN compound_statement", str);
							}
 		;				


parameter_list  : parameter_list COMMA type_specifier ID
							{
								string str = stackPop(parameter_list) + "," + stackPop(type_specifier) + " " + $4->getName();
								stackPush(parameter_list, str);
								insertIntoParameters($4);
								printLog("parameter_list", "parameter_list COMMA type_specifier ID", str);
							}
		| parameter_list COMMA type_specifier
							{
								string str = stackPop(parameter_list) + "," + stackPop(type_specifier);
								stackPush(parameter_list, str);
								string name("#");
								name = name + to_string(count_of_parameters_without_name++);
								insertIntoParameters(new symbol_info(name, *$3));
								printLog("parameter_list", "parameter_list COMMA type_specifier", str);
							}
 		| type_specifier ID
							{
								string str = stackPop(type_specifier) + " " + $2->getName();
								stackPush(parameter_list, str);
								insertIntoParameters($2);
								printLog("parameter_list", "type_specifier ID", str);
							}
		| type_specifier
							{
								string str = stackPop(type_specifier);
								stackPush(parameter_list, str);
								string name("#");
								name = name + to_string(count_of_parameters_without_name++);
								insertIntoParameters(new symbol_info(name, *$1));
								printLog("parameter_list", "type_specifier", str);
							}
 		;

 		
compound_statement : LCURL statements RCURL 
							{
								string str = "{\n\t" + stackPop(statements) + "\n}";
								stackPush(compound_statement, str);
								printLog("compound_statement", "LCURL statements RCURL", str);

								printTable();
								exitScope();
							}
 		    | LCURL RCURL
							{
								string str = "{}";
								stackPush(compound_statement, str);
								enterNewScope();
								printLog("compound_statement", "LCURL RCURL", str);
							}
 		    ;
 		    
var_declaration : type_specifier declaration_list SEMICOLON
							{
								setVariableValues(*$1);
								stackPush(var_declaration, stackPop(type_specifier) + " " + stackPop(declaration_list) + ";");
								printLog("var_declaration", "type_specifier declaration_list SEMICOLON", *($1) + " " + *$2 + ";");
							}
 		 ;
 		 
type_specifier	: INT
							{
								$$ = new string(yytext);
								printLog("type_specifier", "INT", yytext);
								stackPush(type_specifier, "int");
							}
 		| FLOAT
							{
								$$ = new string(yytext);
								printLog("type_specifier", "FLOAT", yytext);
								stackPush(type_specifier, "float");
							}
 		| VOID
							{
								$$ = new string(yytext);
								printLog("type_specifier", "VOID", yytext);
								stackPush(type_specifier, "void");
							}
 		;
 		
declaration_list : declaration_list COMMA ID
							{
								$$ = new string((*$1) + ","+ $3->getName());
								insertDeclarationListRecord($3);
								stackPush(declaration_list, stackPop(declaration_list) + "," + $3->getName());
								printLog("declaration_list", "declaration_list COMMA ID", *$$);
							}
 		  | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD
							{
								$$ = new string((*$1) + ","+ $3->getName() + "[" + *$5 + "]");
								insertDeclarationListRecord($3);
								stackPush(declaration_list, stackPop(declaration_list) + "," + $3->getName());
								printLog("declaration_list", "declaration_list COMMA ID LTHIRD CONST_INT RTHIRD", *$$);
							}
 		  | ID
							{
								$$ = new string($1 -> getName());
								insertDeclarationListRecord($1);
								stackPush(declaration_list, $1->getName());
								printLog("declaration_list", "ID", *$$);
							}
 		  | ID LTHIRD CONST_INT RTHIRD
							{
								$$ = new string($1->getName() + "[" + *$3 + "]");
								stackPush(declaration_list, *$$);
								printLog("declaration_list", "ID LTHIRD CONST_INT RTHIRD", *$$);
							}
 		  ;
 		  
statements : statement
							{
								string str = stackPop(statement);
								stackPush(statements, str);
								printLog("statements", "statement", str);
							}
	   | statements statement
							{
								string str = stackPop(statements) + "\n" + stackPop(statement);
								stackPush(statements, str);
								printLog("statements", "statements statement", str);
							}
	   ;
	   
statement : var_declaration
							{
								string str =  stackPop(var_declaration);
								stackPush(statement, str);
								printLog("statement", "var_declaration", str);
							}
	  | expression_statement
							{
								string str =  stackPop(expression_statement);
								stackPush(statement, str);
								printLog("statement", "expression_statement", str);
							}
	  | compound_statement
							{
								
							}
	  | FOR LPAREN expression_statement expression_statement expression RPAREN statement
							{
								
							}
	  /* | IF LPAREN expression RPAREN statement
							{
								
							}
	  | IF LPAREN expression RPAREN statement ELSE statement
							{
								
							} */
	  | WHILE LPAREN expression RPAREN statement
							{
								
							}
	  | PRINTLN LPAREN ID RPAREN SEMICOLON
							{
								
							}
	  | RETURN expression SEMICOLON
							{
								string str = "return " + stackPop(expression) + ";";
								stackPush(statement, str);
								printLog("statement", "RETURN expression SEMICOLON", str);
							}
	  ;
	  
expression_statement 	: SEMICOLON	
							{
								stackPush(expression_statement, ";");
								printLog("expression_statement", "SEMICOLON", ";");
							}		
			| expression SEMICOLON 
							{
								string str = stackPop(expression) + ";";
								stackPush(expression_statement, str);
								printLog("expression_statement", "expression SEMICOLON", str);
							}
			;
	  
variable : ID 		
							{
								$$ = $1;
								stackPush(variable, $1->getName());
								printLog("variable", "ID", $1->getName());
							}
	 | ID LTHIRD expression RTHIRD 
							{
								string str = $1->getName() + "["
								+ stackPop(expression) + "]";
								stackPush(variable, str);
								printLog("variable", "ID LTHIRD expression RTHIRD ", str);
							}
	 ;
	 
expression : logic_expression	
							{
								string str = stackPop(logic_expression);
								stackPush(expression, str);
								printLog("expression", "logic_expression", str);
							}
	   | variable ASSIGNOP logic_expression 
							{
								string str = stackPop(variable) + "=" + stackPop(logic_expression);
								stackPush(expression, str);
								printLog("expression", "variable ASSIGNOP logic_expression", str);
							}	
	   ;
			
logic_expression : rel_expression 	
							{
								string str = stackPop(rel_expression);
								stackPush(logic_expression, str);
								printLog("logic_expression", "rel_expression", str);
							}
		 | rel_expression LOGICOP rel_expression 
							{
								string str1 = stackPop(rel_expression);
								string str2 = stackPop(rel_expression);
								string str = str2 + *$2 + str1;
								stackPush(logic_expression, str);
								printLog("logic_expression", "rel_expression LOGICOP rel_expression ", str);
							}	
		 ;
			
rel_expression	: simple_expression 
							{
								string str = stackPop(simple_expression);
								stackPush(rel_expression, str);
								printLog("rel_expression", "simple_expression", str);
							}
		| simple_expression RELOP simple_expression	
							{
								string str1 = stackPop(simple_expression);
								string str2 = stackPop(simple_expression);
								string str = str2 + *$2 + str1;
								stackPush(rel_expression, str);
								printLog("rel_expression", "simple_expression RELOP simple_expression	", str);
							}
		;
				
simple_expression : term 
							{
								$$ = $1;
								string str = stackPop(term);
								stackPush(simple_expression, str);
								printLog("simple_expression", "term", str);
							}
		  | simple_expression ADDOP term 
							{
								// cout << *$2 << endl;
								string str = stackPop(simple_expression) + *$2 + stackPop(term);
								stackPush(simple_expression, str);
								printLog("simple_expression", "simple_expression ADDOP term", str);
							}
		  ;
					
term :	unary_expression
							{
								string str = stackPop(unary_expression);
								stackPush(term, str);
								printLog("term", "unary_expression", str);
							}
     |  term MULOP unary_expression
							{
								string str = stackPop(term) + *$2 + stackPop(unary_expression);
								stackPush(term, str);
								printLog("term", "term MULOP unary_expression", str);
							}
     ;

unary_expression : ADDOP unary_expression 
							{
								// string str = *$1 + stackPop(unary_expression);
								// stackPush(unary_expression, str);
								// printLog("unary_expression", "ADDOP unary_expression", str);
							} 
		 | NOT unary_expression 
							{
								
							}
		 | factor 
							{
								$$ = $1;
								string str = stackPop(factor);
								stackPush(unary_expression, str);
								printLog("unary_expression", "factor", str);
							}
		 ;
	
factor	: variable 
							{
								string str = stackPop(variable);
								stackPush(factor, str);
								printLog("factor", "variable", str);
							}
	| ID LPAREN argument_list RPAREN
							{
								string str = $1->getName() + "(" + stackPop(argument_list) + ")";
								stackPush(factor, str);
								printLog("factor", "ID LPAREN argument_list RPAREN", str);
							}
	| LPAREN expression RPAREN
							{
								string str = "(" + stackPop(expression) + ")";
								stackPush(factor, str);
								printLog("factor", "LPAREN expression RPAREN", str);
							}
	| CONST_INT 
							{
								stackPush(factor, *$1);
								printLog("factor", "CONST_INT", *$1);
							}
	| CONST_FLOAT
							{
								stackPush(factor, *$1);
								printLog("factor", "CONST_FLOAT", *$1);
							}
	| variable INCOP 
							{
								
							}
	| variable DECOP
							{
								
							}
	;
	
argument_list : arguments
							{
								string str = stackPop(arguments);
								stackPush(argument_list, str);
								printLog("argument_list", "arguments", str);
							}
			  ;
	
arguments : arguments COMMA logic_expression
							{
								string str = stackPop(arguments) + "," + stackPop(logic_expression);
								stackPush(arguments, str);
								printLog("arguments", "arguments COMMA logic_expression", str);
							}
	      | logic_expression
							{
								string str = stackPop(logic_expression);
								stackPush(arguments, str);
								printLog("arguments", "logic_expression", str);
							}
	      ;
 

%%
int main(int argc,char *argv[])
{
	FILE *fp;
	if((fp=fopen(argv[1],"r"))==NULL)
	{
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	logout= fopen("log.txt","w");
 	errorout= fopen("error.txt","w");

	yyin=fp;
	yyparse();

	fclose(yyin);
	fclose(logout);
	fclose(errorout);
	return 0;
}