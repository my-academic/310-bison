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

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%%

start : program
{
	string str = stackPop(program);
	stackPush(start, str);
	printLog("start", "program", "");
	printLineAndErrors();
}
	;

program : program unit 
{
	string str = stackPop(program) + "\n" + stackPop(unit);
	stackPush(program, str);
	printLog("program", "program unit", str + "\n");
}
	| unit
{
	string str = stackPop(unit);
	stackPush(program, str);
	printLog("program", "unit", str + "\n");
}
	;
	
unit : var_declaration
{
	string str = stackPop(var_declaration);
	stackPush(unit, str);
	printLog("unit", "var_declaration", str + "\n");
}
     | func_declaration
{
	string str = stackPop(func_declaration);
	stackPush(unit, str);
	printLog("unit", "func_declaration", str + "\n");
}
     | func_definition
{
	string str = stackPop(func_definition);
	stackPush(unit, str);
	printLog("unit", "func_definition", str + "\n");
}
     ;
     
func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON
{
	string str = stackPop(type_specifier) + $2->getName() + "(" + stackPop(parameter_list) + ");";
	setFunctionValues(*$1, $2, false);
	setAndClearFunctionThings();
	stackPush(func_declaration, str);
	printLog("func_declaration", "type_specifier ID LPAREN parameter_list RPAREN SEMICOLON", str + "\n");
}
		| type_specifier ID LPAREN RPAREN SEMICOLON
{
	string str = stackPop(type_specifier) + " " + $2->getName() + "();";
	setFunctionValues(*$1, $2, false);
	setAndClearFunctionThings();
	stackPush(func_declaration, str);
	printLog("func_declaration", "type_specifier ID LPAREN RPAREN SEMICOLON", str + "\n");
}
		;
		 
func_definition : type_specifier ID LPAREN parameter_list RPAREN  
{
	setFunctionValues(*$1, $2, true);
} compound_statement 
{
	string str = stackPop(type_specifier) + " " + $2->getName() + "(" + stackPop(parameter_list) + ")" + stackPop(compound_statement);
	stackPush(func_definition, str);
	printLog("func_definition", "type_specifier ID LPAREN parameter_list RPAREN compound_statement", str);
	current_function = nullptr;
}
		| type_specifier ID LPAREN RPAREN
{
	setFunctionValues(*$1, $2, true);
}  compound_statement
{
	string str = stackPop(type_specifier) + " " + $2->getName() + "()" + stackPop(compound_statement);
	// setFunctionValues(*$1, $2, true);
	stackPush(func_definition, str);
	printLog("func_definition", "type_specifier ID LPAREN RPAREN compound_statement", str);
	current_function = nullptr;
}
 		;				


parameter_list  : parameter_list COMMA type_specifier ID
{
	string str = stackPop(parameter_list) + "," + stackPop(type_specifier) + " " + $4->getName();
	stackPush(parameter_list, str);
	insertIntoParameters($4, *$3);
	printLog("parameter_list", "parameter_list COMMA type_specifier ID", str);
}
		| parameter_list COMMA type_specifier
{
	string str = stackPop(parameter_list) + "," + stackPop(type_specifier);
	stackPush(parameter_list, str);
	string name("#");
	name = name + to_string(count_of_parameters_without_name++);
	insertIntoParameters(new symbol_info(name, "ID"), *$3);
	printLog("parameter_list", "parameter_list COMMA type_specifier", str);
}
 		| type_specifier ID
{
	string str = stackPop(type_specifier) + " " + $2->getName();
	stackPush(parameter_list, str);
	insertIntoParameters($2, *$1);
	printLog("parameter_list", "type_specifier ID", str);
}
		| type_specifier
{
	string str = stackPop(type_specifier);
	stackPush(parameter_list, str);
	string name("#");
	name = name + to_string(count_of_parameters_without_name++);
	insertIntoParameters(new symbol_info(name, "ID"), *$1);
	printLog("parameter_list", "type_specifier", str);
}
 		;

 		
compound_statement : LCURL 
{
	enterNewScope();
} statements RCURL 
{
	string str = "{\n" + stackPop(statements) + "\n}";
	stackPush(compound_statement, str);
	printLog("compound_statement", "LCURL statements RCURL", str + "\n");

	printTable();
	exitScope();
}
 		    | LCURL 
{
	enterNewScope();
} RCURL
{
	string str = "{}";
	stackPush(compound_statement, str);
	enterNewScope();
	printLog("compound_statement", "LCURL RCURL", str + "\n");
}
 		    ;
 		    
var_declaration : type_specifier declaration_list SEMICOLON
{
	setVariableAndArrayValues(*$1);
	stackPush(var_declaration, stackPop(type_specifier) + " " + stackPop(declaration_list) + ";");
	printLog("var_declaration", "type_specifier declaration_list SEMICOLON", *($1) + " " + *$2 + ";");
}
 		 ;
 		 
type_specifier	: INT
{
	$$ = new string(yytext);
	printLog("type_specifier", "INT", yytext);
	stackPush(type_specifier, integer);
}
 		| FLOAT
{
	$$ = new string(yytext);
	printLog("type_specifier", "FLOAT", yytext);
	stackPush(type_specifier, fraction);
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
	insertDeclarationListRecord($3, true, stoi(*$5));
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
	insertDeclarationListRecord($1, true, stoi(*$3));
	stackPush(declaration_list, *$$);
	printLog("declaration_list", "ID LTHIRD CONST_INT RTHIRD", *$$);
}
 		  ;
 		  
statements : statement
{
	string str = stackPop(statement);
	stackPush(statements, str);
	printLog("statements", "statement", str + "\n");
}
	   | statements statement
{

	string str = stackPop(statements) + "\n" + stackPop(statement);
	stackPush(statements, str);
	printLog("statements", "statements statement", str + "\n");
}
	   ;
	   
statement : var_declaration
{
	string str =  stackPop(var_declaration);
	stackPush(statement, str);
	printLog("statement", "var_declaration", str + "\n");
}
	  | expression_statement
{
	string str =  stackPop(expression_statement);
	stackPush(statement, str);
	printLog("statement", "expression_statement", str + "\n");
}
	  | compound_statement
{
	string str =  stackPop(compound_statement);
	stackPush(statement, str);
	printLog("statement", "compound_statement", str + "\n");
}
	  | FOR LPAREN expression_statement expression_statement expression RPAREN statement
{
	string str1 = stackPop(expression_statement);
	string str2 = stackPop(expression_statement);
	string str = "for(" + str2 + str1 + stackPop(expression) + ")" + stackPop(statement);
	stackPush(statement, str);
	printLog("statement", "FOR LPAREN expression_statement expression_statement expression RPAREN statement", str + "\n");
}
	  | IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
{
	string str = "if(" + stackPop(expression) + ")" + stackPop(statement);
	stackPush(statement, str);
	printLog("statement", "IF LPAREN expression RPAREN statement", str + "\n");
}
	  | IF LPAREN expression RPAREN statement ELSE statement
{
	string str1 = stackPop(statement);
	string str2 = stackPop(statement);
	string str = "if (" + stackPop(expression) + ")" + str2 + "\nelse " + str1;
	stackPush(statement, str);
	printLog("statement", "IF LPAREN expression RPAREN statement ELSE statement", str + "\n");
}
	  | WHILE LPAREN expression RPAREN statement
{
	string str = "while (" + stackPop(expression) + ")" + stackPop(statement);
	stackPush(statement, str);
	printLog("statement", "WHILE LPAREN expression RPAREN statement", str + "\n");
}
	  | PRINTLN LPAREN ID RPAREN SEMICOLON
{
	findVariable($3);
	string str = "printf(" + $3->getName() + ");";
	stackPush(statement, str);
	printLog("statement", "PRINTLN LPAREN ID RPAREN SEMICOLON", str + "\n");
}
	  | RETURN expression SEMICOLON
{
	checkFuncReturnCompatibility($2);
	string str = "return " + stackPop(expression) + ";";
	stackPush(statement, str);
	printLog("statement", "RETURN expression SEMICOLON", str + "\n");
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
	$$ = findVariable($1);
	stackPush(variable, $1->getName());
	printLog("variable", "ID", $1->getName());
}
	 | ID LTHIRD expression RTHIRD 
{
	$$ = checkArrayIndex($1->getName(), $3);
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

	$$ = checkAssignCompatibility($1, $3);
	resetArrayIndex($1);
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
	$$ = checkLogicCompetibility($1, *$2, $3);
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
	$$ = checkRELOPCompetibility($1, *$2, $3);
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
	$$ = checkAdditionCompatibility($1, *$2, $3);
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

	$$ = checkAndDoMulopThings($1, *$2, $3);
	string str = stackPop(term) + *$2 + stackPop(unary_expression);
	stackPush(term, str);
	printLog("term", "term MULOP unary_expression", str);
}
     ;

unary_expression : ADDOP unary_expression 
{
	$$ = checkUnaryADDOPThings(*$1, $2);
	string str = *$1 + stackPop(unary_expression);
	stackPush(unary_expression, str);
	printLog("unary_expression", "ADDOP unary_expression", str);
} 
		 | NOT unary_expression 
{
	$$ = checkNotCompatibility($2);
	string str = "!" + stackPop(unary_expression);
	stackPush(unary_expression, str);
	printLog("unary_expression", "ADDOP unary_expression", str);
}
		 | factor 
{
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
	$$ = checkFunctionArguments($1);
}
	| LPAREN expression RPAREN
{
	$$ = checkLPAREN_Expression_RPAREN($2);
	string str = "(" + stackPop(expression) + ")";
	stackPush(factor, str);
	printLog("factor", "LPAREN expression RPAREN", str);
}
	| CONST_INT 
{
	$$ = setIntermediateValues("intermediate", integer, stoi(*$1));
	stackPush(factor, *$1);
	printLog("factor", "CONST_INT", *$1);
}
	| CONST_FLOAT
{
	$$ = setIntermediateValues("intermediate", "float", stof(*$1));
	stackPush(factor, *$1);
	printLog("factor", "CONST_FLOAT", *$1);
}
	| variable INCOP 
{
	$$ = checkINCOPCompatibility($1);
	string str = stackPop(variable) + "++";
	stackPush(factor, str);
	printLog("factor", "variable INCOP", str);
}
	| variable DECOP
{
	$$ = checkDECOPCompatibility($1);
	string str = stackPop(variable) + "--";
	stackPush(factor, str);
	printLog("factor", "variable DECOP", str);
}
	;
	
argument_list : arguments
{
	string str = stackPop(arguments);
	stackPush(argument_list, str);
	printLog("argument_list", "arguments", str);
}
				|			
{
	stackPush(argument_list, "");
	printLog("argument_list", "", "");
}
			  ;
	
arguments : arguments COMMA logic_expression
{
	args.push_back($3);
	string str = stackPop(arguments) + "," + stackPop(logic_expression);
	stackPush(arguments, str);
	printLog("arguments", "arguments COMMA logic_expression", str);
}
	      | logic_expression
{
	args.push_back($1);
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