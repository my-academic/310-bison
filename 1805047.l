%option noyywrap

%x state_string
%x state_single_line_comment
%x state_multi_line_comment

%{
#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<stdlib.h>
#include<string>
#include "1805047_symbol_table.h"
#include "y.tab.h"

extern YYSTYPE yylval;
extern FILE *logout, *errorout;
extern symbol_table *symbolTable;
extern void enterNewScope();
void yyerror(char *);

int line_count=1;
int start_point = 1;
int lexical_error_count = 0;
string taking_string = "";

using namespace std;
// symbol_table // symbolTable(bucket_size);


string fixString(string str){
    // // cout << "before " << str << endl;
    // for(int i = 0; i < str.size(); i++){
    //     if(str[i] == '\\' && str[i+1] == 't'){
    //         str.erase(i + 1, 1);
    //         // cout << "tab" << endl;
    //         str[i] = '\t';
    //     }
    //     else if(str[i] == '\\' && str[i+1] == 'n'){
    //         str.erase(i + 1, 1);
    //         str[i] = '\n';
    //     }
    //     else if(str[i] == '\\' && str[i+1] == '\"'){
    //         str.erase(i + 1, 1);
    //         str[i] = '\"';
    //     }
    //     else if(str[i] == '\\' && str[i+1] == '\''){
    //         str.erase(i + 1, 1);
    //         str[i] = '\'';
    //     }
    //     else if(str[i] == '\\' && str[i+1] == '\\'){
    //         str.erase(i + 1, 1);
    //         str[i] = '\\';
    //     }
    //     else if(str[i] == '\\' && str[i+1] == 'b'){
    //         str.erase(i + 1, 1);
    //         str[i] = '\b';
    //     }
    //     else if(str[i] == '\\' && str[i+1] == 'f'){
    //         str.erase(i + 1, 1);
    //         str[i] = '\f';
    //     }
    //     else if(str[i] == '\\' && str[i + 1] == '\n'){
    //         //new line
    //         // cout << line_count << endl;
    //         str.erase(i , 2);
    //         i--;
    //     }
    // }
    // return str;
}

void printOpAndPunc(string type, char* id){
    // fprintf(tokenout, "<%s, %s> ",type.c_str(), id);
    // fprintf(logout, "\nLine no %d: Token <%s> Lexeme %s found\n", line_count, type.c_str(), id);
    // string str(id);
    // if(str == "{"){
    //     // symbolTable.enterScope();
    // }
    // else if(str == "}"){
    //     // symbolTable.exitScope();
    // }
}

void passYYLVAL(string type, string id) {
    symbol_info *symbolInfo = symbolTable->lookup(id);
    if(symbolInfo == nullptr){
        symbolInfo = new symbol_info(id, type);
    }
    yylval.symbolValue = symbolInfo;
}

int printInt(string type, char* id){
    /* symbol_info *symbolInfo = new symbol_info(id, type);
    fprintf(tokenout, "<%s, %s> ",type.c_str(), id);
    fprintf(logout, "\nLine no %d: Token <%s> Lexeme %s found\n", line_count, type.c_str(), id); */
    passYYLVAL(type, id);
    return CONST_INT;
}

int printFloat(string type, char* id){
    passYYLVAL(type, id);
    return CONST_FLOAT;
}

int printID(string type, char* id){
    passYYLVAL(type, id);
    return ID;
}

void printString(string type, string str){
    // string actual_string = fixString(str);
    // fprintf(tokenout, "<%s, %s> ",type.c_str(), actual_string.c_str());
    // fprintf(logout, "\nLine no %d: Token <%s> Lexeme \"%s\" found --> <STRING, %s>\n", start_point, type.c_str(), str.c_str(), actual_string.c_str());
}


void printChar(string type, char id, string yytext){
    // fprintf(tokenout, "<%s, %c> ",type.c_str(), id);
    // fprintf(logout, "\nLine no %d: Token <%s> Lexeme %s found --> <CHAR_CONST, %c>\n", line_count, type.c_str(), yytext.c_str(), id );
    // string str(1, id);
    // // bool success = // symbolTable.insert(yytext, type);
    // // if(success)
    // //     // symbolTable.printAllScopeTable();
}
int printKeyword(string type, char* id){

    // 
    //
    
    string up(id);

        
    // transform(up.begin(), up.end(), up.begin(), ::toupper);
    // fprintf(tokenout, "<%s> ", up.c_str());
    // fprintf(logout, "\nLine no %d: Token <%s> Lexeme %s found\n", line_count, up.c_str(), id);
    yylval.input_string = new string(id);


    if(up == "if")
        return IF;
    if(up == "else")
        return ELSE;
    if(up == "for")
        return FOR;
    if(up == "while")
        return WHILE;
    if(up == "int")
        return INT;
    if(up == "float")
        return FLOAT;
    if(up == "void")
        return VOID;
    if(up == "return")
        return RETURN;
    if(up == "printf")
        return PRINTLN;

}

char convertStringToChar(string str){
    // if(str == "\\n") return '\n';
    // if(str == "\\t") return '\t';
    // if(str == "\\\\") return '\\';
    // if(str == "\\\'") return '\'';
    // if(str == "\\\"") return '\"';
    // if(str == "\\a") return '\a';
    // if(str == "\\f") return '\f';
    // if(str == "\\r") return '\r';
    // if(str == "\\b") return '\b';
    // if(str == "\\v") return '\v';
    // if(str == "\\0") return '\0';
    
    // //error
    // return ' ';
}

void printErrorLog(string str, int error_line = line_count, string error = yytext){
    // cout << "here " <<  error  << " oka ";
    // fprintf(logout, "\nError at line no %d: %s %s\n", error_line, str.c_str(), error.c_str());
    // error_count++;
}

void printCommentLog(){
    // taking_string.erase(taking_string.size() - 1, 1);
    // fprintf(logout, "\nLine no %d: Token <COMMENT> Lexeme %s found\n", start_point, taking_string.c_str());
}


%}


delim [ \t\v\r\f]
ws {delim}+
digit [0-9]
letter [A-Za-z]
special_character ((\\n)|(\\t)|(\\\\)|(\\\')|(\\\")|(\\a)|(\\f)|(\\r)|(\\b)|(\\v)|(\\0))
special_character_without_newline ((\\t)|(\\\\)|(\\\')|(\\\")|(\\a)|(\\f)|(\\r)|(\\b)|(\\v)|(\\0))
float_number (({digit}*(\.{digit}+)?)|({digit}+))(E[+-]?{digit}+)?
const_char ((\'([^\\\'(\n)]|{special_character})\'))

addop [+-]
mulop [*/%]
incop (\+\+)
decop (--)
relop (<|(<=)|>|(>=)|(==)|(!=))
assignop =
logicop (&&|\|\|)
not (!)
lparen [(]
rparen [)]
lcurl [{]
rcurl [}]
lthird [[]
rthird []]
comma (,)
semicolon (;)
identifier [A-Za-z_]+[A-Za-z0-9_]*
keywords ((if)|(else)|(for)|(while)|(do)|(int)|(char)|(float)|(double)|(void)|(return)|(default)|(continue)|(printf))
special_character_2 ((\n)|(\t)|(\\)|(\')|(\")|(\a)|(\f)|(\r)|(\b)|(\v)|(\0))
special_character_3 ((\t)|(\\)|(\')|(\")|(\a)|(\f)|(\r)|(\b)|(\v)|(\0))

decimal_point_error ({digit}*[.]{digit}*[.])
ill_formed_number {digit}*([.]{digit}*)*([E]+[+-]*({digit}*)([.]{digit}*)*)*

non_id {float_number}({digit}*[.]*)*{identifier}+
multi_character_constant_error ((\'(([^\\\'(\n)]|.){2,})\'))
unfinished_character ((\'([^\\\'(\n)]|{special_character_without_newline})*[(\n)]))
empty_character ['][']



%%
[\n]                {line_count ++;}
{ws}                {}

{keywords}          {
                        return printKeyword("keywords", yytext);
}

{digit}+            {
                        yylval.input_string = new string(yytext);
                        return CONST_INT;
                    }

{float_number}      {
                        yylval.input_string = new string(yytext);
                        return CONST_FLOAT;
}
{const_char}        {
                        // char* str= yytext;
                        // string actual_string(yytext);
                        // int len = strlen(yytext);
                        // str[len - 1] = '\0';
                        // str++;
                        // len = strlen(str);
                        // char a = str[0];
                        // if(len>= 2) a = convertStringToChar(str);
                        // if(!(len >= 2 && a == ' '))
                        //     printChar("CONST_CHAR", a, actual_string);
                        // else {
                        //     printErrorLog("invalid character");
                        // }
}


{addop}             {
                        yylval.input_string = new string(yytext);
                        return ADDOP;
}

{incop}          {
                        yylval.input_string = new string(yytext);
                        return INCOP;
}

{decop}          {
                        yylval.input_string = new string(yytext);
                        return DECOP;
}
{mulop}            {
                        yylval.input_string = new string(yytext);
                        return MULOP;
}    


{relop}             {
                        yylval.input_string = new string(yytext);
                        return RELOP;
}

{assignop}             {
                        yylval.input_string = new string(yytext);
                        return ASSIGNOP;
}


{logicop}             {
                        yylval.input_string = new string(yytext);
                        return LOGICOP;
}


{not}             {
                        // printOpAndPunc("NOT", yytext);
                        return NOT;
}


{lparen}             {
                        // printOpAndPunc("LPAREN", yytext);
                        return LPAREN;
}

{rparen}             {
                        // printOpAndPunc("RPAREN", yytext);
                        return RPAREN;
}


{lcurl}             {
                        // printOpAndPunc("LCURL", yytext);
                        // enterNewScope();
                        // cout << "{ found" << endl;
                        return LCURL;
}

{rcurl}             {
                        // printOpAndPunc("RCURL", yytext);
                        // symbolTable->exitScope();
                        return RCURL;
}


{lthird}            {
                        // printOpAndPunc("LTHIRD", yytext);
                        return LTHIRD;
}

{rthird}            {
                        // printOpAndPunc("RTHIRD", yytext);
                        return RTHIRD;
}

{comma}             {
                        // printOpAndPunc("COMMA", yytext);
                        return COMMA;
}
{semicolon}         {
                        // printOpAndPunc("SEMICOLON", yytext);
                        return SEMICOLON;
}

{identifier}        {
                        return printID("ID", yytext);
}


["]                 {
                        BEGIN state_string;
                        // printf("starting string state\n");
                        taking_string = "";
                        start_point = line_count;
}
<state_string>["]   {
                        BEGIN INITIAL;

                    //    cout << yytext << endl;
                        // printf("close string state\n");
                        // cout <<"final "<< endl <<  taking_string << endl;
                       printString("STRING", taking_string);
}

<state_string>\\\"    {
                    //    printf("\nhandled\n");
                       taking_string += yytext;
}

<state_string>\\\n    {
                    //    printf("\nhandled\n");
                       taking_string += yytext;
                       line_count ++;
}

<state_string>(.|{special_character_3}) {
                        // cout << "printing " << yytext << " " << (int)yytext[0] << endl;
                        taking_string += yytext;
}
<state_string>\n  {
                        // cout << "unfinished_string " << yytext << endl;
                        taking_string = "\"" + taking_string;
                        printErrorLog("Unterminated String", start_point, taking_string);
                        line_count ++;
                        BEGIN INITIAL;
}




"//"                {
                        BEGIN state_single_line_comment;
                        taking_string = yytext;
                        start_point = line_count;
                        // printf("statring single line comment\n");
}

<state_single_line_comment>.*[\\][\n] {
                        line_count++;
                        taking_string += yytext;
                        // cout << "here " << yytext << endl;  
                        // cout << line_count << endl;
}

<state_single_line_comment>.*\n {
                        line_count++;
                        taking_string += yytext;
                        taking_string.erase(taking_string.size() - 1, 1);
                        // printf("ending comment\n");
                        printCommentLog();
                        BEGIN INITIAL;
}

"/*"                {
                        BEGIN state_multi_line_comment;
                        taking_string = yytext;
                        start_point = line_count;
                        // printf("starting multiline comment\n");
}

<state_multi_line_comment>"*/" {
                        BEGIN INITIAL;
                        taking_string += yytext;
                        printCommentLog();
                        // printf("ending multiline\n");
}
<state_multi_line_comment>(.|{special_character_2}) {
                        // cout << yytext << endl;
                        taking_string += yytext;
                        if(yytext[0] == '\n') line_count ++;

                        // printf("ending multiline \t %s", yytext);
}
<state_multi_line_comment><<EOF>> {
                        printErrorLog("Unterminated Comment", start_point, taking_string);
                        // cout << "unfinished_comment" << endl;
                        BEGIN INITIAL;
}

{decimal_point_error}({digit}*[.]*)* {
                        // cout << yytext << endl;
                        // cout << "Too many decimal points" << endl;
                        printErrorLog("Too many decimal points");
}

{ill_formed_number} {
                        // cout << yytext << endl;
                        // cout << "Ill formed number" << endl;
                        printErrorLog("Ill formed number");
}

{non_id}            {
                        // cout << yytext << endl;
                        // cout << "Invalid prefix on ID or invalid suffix on Number" << endl;
                        printErrorLog("Invalid prefix on ID or invalid suffix on Number");
}


{multi_character_constant_error}  {
                        // cout << yytext << endl;
                        // cout << "Multi character constant error" << endl;
                        printErrorLog("Multi character constant error");
}

{unfinished_character}      {
                        string str = yytext;
                        // cout << line_count << " ";
                        unput(yytext[str.size() - 1]);
                        str.erase(str.size() - 1, 1);
                        yytext = (char *) str.c_str();
                        printErrorLog("Unterminated character");
                        // cout << yytext << endl;
                        // for(int i = 0; i < str.size(); i++){
                        //     if(str[i] == '\n') line_count ++;
                        // }
                        
                        // cout << line_count << endl;
}

{empty_character}       {
                        // cout << yytext << endl;
                        // cout << "empty_character" << endl;
                        printErrorLog("Empty character constant error");
}

.                       {
                            // cout<< "unrecognized_character" << endl;
                            printErrorLog("Unrecognized character");
}


%%


// int yywrap(void)
// {
// 	return 1;
// }



// int main(int argc,char *argv[]){
	
// 	if(argc!=2){
// 		printf("Please provide input file name and try again\n");
// 		return 0;
// 	}
	
// 	FILE *fin=fopen(argv[1],"r");
// 	if(fin==NULL){
// 		printf("Cannot open specified file\n");
// 		return 0;
// 	}
	
// 	logout= fopen("1805047_log.txt","w");
// 	tokenout= fopen("1805047_token.txt","w");

// 	yyin= fin;
// 	yylex();
//     // symbolTable.printAllScopeTable();
//     fprintf(logout, "Total lines: %d\n", line_count);
//     fprintf(logout, "Total errors: %d\n", error_count);
// 	fclose(yyin);
// 	fclose(tokenout);
// 	fclose(logout);
// 	return 0;
// }
