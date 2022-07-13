#include <bits/stdc++.h>
using namespace std;

// line and error count
extern int line_count;
extern int lexical_error_count;
int syntax_error_count, err_line;

extern char *yytext;

// log and error file
FILE *logout, *errorout;
string integer = "int";
string fraction = "float";
string _void = "void";
string intermediate = "intermediate";

// storing variable data type
string datatype;
vector<symbol_info *> variables;

// for function parameters
string return_type;
vector<symbol_info *> parameters;
int count_of_parameters_without_name = 0;
bool isFunctionStarted = true;
vector<symbol_info *> args;

symbol_info *current_function = nullptr;

// vector<string> paramTypeList;
// symbol_info *current_function=NULL;

int bucket_size = 31;
symbol_table *symbolTable = new symbol_table(bucket_size);

string keywordArray[] = {"IF", "ELSE", "FOR", "WHILE", "INT", "FLOAT", "VOID", "RETURN", "PRINTLN"};
set<string> keywords(keywordArray, keywordArray + 9);

enum non_terminals
{
  start,
  program,
  unit,
  func_declaration,
  func_definition,
  parameter_list,
  compound_statement,
  var_declaration,
  type_specifier,
  declaration_list,
  statements,
  statement,
  expression_statement,
  variable,
  expression,
  logic_expression,
  rel_expression,
  simple_expression,
  term,
  unary_expression,
  factor,
  argument_list,
  arguments
};

vector<stack<string>> non_terminals_stack(50);
set<int> error_lines;

void printCompatibilityRelatedThings(symbol_info *s)
{
  if (s == nullptr)
    return;
  cout << s->getName() << ", id_type: " << s->id_type << ", variable_type: " << s->variable_type << ", array_type: " << s->array_type << ", current_index: " << s->current_index << ", return_type: " << s->return_type << endl
       << endl;
  ;
}

void setCompatibleRelatedThings(symbol_info *s1, symbol_info *s2)
{
  s1->id_type = s2->id_type;
  s1->variable_type = s2->variable_type;
  s1->array_type = s2->array_type;
  s1->current_index = s2->current_index;
}

bool checkCompatibility(symbol_info *s1, symbol_info *s2)
{
  return s1->variable_type == s2->variable_type;
}

void yyerror(char *s)
{
  // fprintf(stderr,"%s\n",s);
  //  err_line = line_count;
  //  cout<<"Error at line no "<<line_count<<" : "<<s<<" lexeme: "<<yytext<<endl;
  //  logfile<<"Error at line "<<line_count<<": "<<s<<endl<<endl;
  //  errorfile<<"Error at line "<<line_count<<": "<<s<<endl<<endl;
  //  syntax_error++;
  return;
}

void printLog(string str1, string str2, string str3)
{
  // printf("Line %d: %s : %s\n%s\n", line_count, str1.c_str(), str2.c_str(), str3.c_str());
  fprintf(logout, "Line %d: %s : %s\n%s\n", line_count, str1.c_str(), str2.c_str(), str3.c_str());
}

void printError(string str1)
{
  // cout << line_count << endl;
  if (!error_lines.count(line_count))
    fprintf(errorout, "Error at line %d: %s\n", line_count, str1.c_str());
  error_lines.insert(line_count);
}

void notDeclaredError(string str1)
{
  printError(str1 + " is not declared");
}

void stackPush(non_terminals nt, string str)
{
  // cout<<nt<<": "<<str<<endl;
  non_terminals_stack[nt].push(str);
}

bool checkNull(symbol_info *s1, symbol_info *s2)
{
  return s1 == nullptr || s2 == nullptr;
}

bool checkArray(symbol_info *s1, symbol_info *s2, string str)
{
  if (s1->id_type == ARRAY || s2->id_type == ARRAY)
  {
    printError("type ARRAY can not be an operand of " + str);
    return true;
  }
  return false;
}

bool checkVoid(symbol_info *s1, symbol_info *s2, string str)
{
  if (s1->variable_type == _void || s2->variable_type == _void)
  {
    printError("type VOID can not be an operand of " + str);
    return true;
  }
  return false;
}

bool checkFunction(symbol_info *s1, symbol_info *s2, string str)
{
  if (s1->id_type == FUNCTION || s2->id_type == FUNCTION)
  {
    printError("type FUNCTION can not be an operand of " + str);
    return true;
  }
  return false;
}

bool checkNull(symbol_info *s1)
{
  return s1 == nullptr;
}

bool checkArray(symbol_info *s1, string str)
{
  if (s1->id_type == ARRAY)
  {
    printError("type ARRAY can not be an operand of " + str);
    return true;
  }
  return false;
}

bool checkVoid(symbol_info *s1, string str)
{
  if (s1->variable_type == _void)
  {
    printError("type VOID can not be an operand of " + str);
    return true;
  }
  return false;
}

bool checkFunction(symbol_info *s1, string str)
{
  if (s1->id_type == FUNCTION)
  {
    printError("type FUNCTION can not be an operand of " + str);
    return true;
  }
  return false;
}

bool checkNullFunctionArrayVoid(symbol_info *symbolInfo, string optr)
{
  return checkNull(symbolInfo) || checkFunction(symbolInfo, optr) || checkArray(symbolInfo, optr) || checkVoid(symbolInfo, optr);
}

bool checkNullFunctionArrayVoid(symbol_info *s1, symbol_info *s2, string optr)
{
  return checkNull(s1, s2) || checkFunction(s1, s2, optr) || checkArray(s1, s2, optr) || checkVoid(s1, s2, optr);
}

symbol_info *findSymbol(symbol_info *symbolInfo)
{
  symbol_info *s = symbolTable->lookup(symbolInfo->getName());
  if (s == nullptr)
    printError("Undeclared variable " + symbolInfo->getName());
  return s;
}

string stackPop(non_terminals nt)
{
  string str = non_terminals_stack[nt].top();
  non_terminals_stack[nt].pop();
  return str;
}

void setVariableAndArrayRelatedValues(symbol_info *symbolInfo, string type, symbol_info *s, int int_value = 0, float float_value = 0)
{
  symbolInfo->id_type = s->id_type;
  symbolInfo->current_index = s->current_index;
  symbolInfo->size_of_array = s->size_of_array;
  symbolInfo->variable_type = type;
  symbolInfo->array_type = type;
  symbolInfo->int_value = int_value;
  symbolInfo->float_value = float_value;
  // cout << symbolInfo->getName() << " " << symbolInfo << endl;
}

void insertDeclarationListRecord(symbol_info *symbolInfo, bool is_array = false, int size_of_array = 0)
{
  if (is_array)
  {
    if (size_of_array <= 0)
      printError("array size can not be less than 1");
    symbolInfo->size_of_array = size_of_array;
    symbolInfo->id_type = ARRAY;
    symbolInfo->current_index = -2;
  }
  else
  {
    symbolInfo->id_type = VARIABLE;
  }
  variables.push_back(symbolInfo);
}

void setVariableAndArrayValues(string type)
{
  for (int i = 0; i < variables.size(); i++)
  {
    // setVariableRelatedValues(variables[i], type);
    symbol_info *s = symbolTable->lookupCurrentScope(variables[i]->getName());
    // cout << "checking " ;
    // printCompatibilityRelatedThings(s);
    if (s == nullptr)
    {
      symbolTable->insert(variables[i]->getName(), variables[i]->getType());
      s = symbolTable->lookup(variables[i]->getName());
      // cout << s << " " << variables[i] << " "<< s->getName() << endl;
      setVariableAndArrayRelatedValues(s, type, variables[i]);
    }
    else
    {
      string err("Multiple declaration of " + s->getName());
      printError(err);
    }
    // delete variables[i];
  }

  variables.clear();
}

void insertIntoParameters(symbol_info *symbolInfo, string type)
{
  symbolInfo->id_type = VARIABLE;
  symbolInfo->variable_type = type;
  symbolInfo->array_type = type;
  parameters.push_back(symbolInfo);
  // cout << " "
  // cout << "in parameters ";
  // printCompatibilityRelatedThings(symbolInfo);
}

void setFunctionParameters()
{
  // symbolTable->printAllScopeTable();
  // cout << current_function->getName() << endl;
  for (int i = 0; !current_function->is_parameters_inserted && i < parameters.size(); i++)
  {
    current_function->sequence_of_parameters.push_back(parameters[i]);
  }
  current_function->is_parameters_inserted = true;
  for (int i = 0; i < parameters.size(); i++)
  {
    current_function->sequence_of_parameters[i]->setName(parameters[i]->getName());
  }
}

void setFunctionRelatedValues(symbol_info *symbolInfo, string return_type, bool is_defined)
{
  // symbolTable->printAllScopeTable();
  for (int i = 0; i < parameters.size(); i++)
  {
    // auto s = symbolTable->lookup(parameters[i]->getName());
    // cout << "checking " << parameters[i]->getName() << " ";
    // printCompatibilityRelatedThings(s);
    // cout << "here" << endl;
    // printCompatibilityRelatedThings(parameters[i]);

    // setCompatibleRelatedThings(s, parameters[i]);
    // symbolInfo->sequence_of_parameters.push_back(s);
  }
}

void checkFunctionParameters(string return_type, symbol_info *symbolInfo, bool is_defined)
{
  symbol_info *s = symbolTable->lookup(symbolInfo->getName());
  set<string> params;
  for (int i = 0; i < parameters.size(); i++)
  {
    if (params.count(parameters[i]->getName()))
      printError("same name can not be in multiple parameteres");
    params.insert(parameters[i]->getName());
  }
  // setFunctionRelatedValues(s, return_type, is_defined);

  s->id_type = FUNCTION;
  s->return_type = return_type;
  s->is_defined = is_defined;
  current_function = s;
  // printCompatibilityRelatedThings(s);
}

void setAndClearFunctionThings()
{
  setFunctionParameters();
  parameters.clear();
  current_function = nullptr;
}

void setFunctionValues(string return_type, symbol_info *symbolInfo, bool is_defined)
{
  symbol_info *s = symbolTable->lookup(symbolInfo->getName());
  if (s == nullptr)
  {
    symbolTable->insert(symbolInfo->getName(), symbolInfo->getType());
    checkFunctionParameters(return_type, symbolInfo, is_defined);
  }
  else if (s->id_type != FUNCTION)
  {
    printError("same name");
  }
  else if (!s->is_defined && is_defined)
  {
    if (s->return_type != return_type)
      printError("return type does not match with declaration");
    else
    {
      checkFunctionParameters(return_type, symbolInfo, is_defined);
    }
  }
  else
  {
    // error
    fprintf(errorout, "line %d : function error\n", line_count);
  }
  isFunctionStarted = true;
}

void enterNewScope()
{
  symbolTable->enterScope();
  for (size_t i = 0; isFunctionStarted && i < parameters.size(); i++)
  {

    symbolTable->insert(parameters[i]->getName(), parameters[i]->getType());
    symbol_info *s = symbolTable->lookup(parameters[i]->getName());
    setCompatibleRelatedThings(s, parameters[i]);
  }

  setFunctionParameters();

  parameters.clear();
  isFunctionStarted = false;

  // symbolTable->printAllScopeTable();
}

void printTable()
{
  symbolTable->printAllScopeTable(logout);
}

void exitScope()
{
  // symbolTable->printAllScopeTable();
  symbolTable->exitScope();
}

void setArrayRelatedValues(symbol_info *symbolInfo, bool change_index = false, int index_value = -2, bool change_value = false, float value = 0, bool change_size = false, int size = 0)
{
  // cout << symbolInfo->current_index << endl;
  if (change_index)
  {
    if (index_value >= symbolInfo->size_of_array)
      printError("index out of range");
    else
      symbolInfo->current_index = index_value;
  }
  if (change_value)
  {
    if (symbolInfo->array_type == fraction)
    {
      symbolInfo->float_array[symbolInfo->current_index] = value;
    }
    else
    {
      symbolInfo->int_array[symbolInfo->current_index] = int(value);
    }
  }
  if (change_size)
  {
    symbolInfo->size_of_array = size;
  }
}

symbol_info *checkArrayIndex(string var_name, symbol_info *idx)
{
  // cout <<
  if (idx->variable_type == fraction)
  {
    printError("Expression inside third brackets not an integer");
    return nullptr;
  }
  else
  {
    symbol_info *s = symbolTable->lookup(var_name);
    if (s == nullptr)
      notDeclaredError(var_name);
    else if (s->id_type != ARRAY)
      printError(var_name + " is not an array");
    else
    {
      setArrayRelatedValues(s, true, idx->int_value, false, 0);
      symbol_info *t = new symbol_info(var_name + "[" + idx->getName() + "]", intermediate);
      t->id_type = VARIABLE;
      t->variable_type = s->array_type;

      return t;
    }
  }
  return nullptr;
}

void resetArrayIndex(symbol_info *s)
{
  // cout << line_count << endl;
  if (s == nullptr)
    return;
  s->current_index = -2;
}

symbol_info *checkAssignCompatibility(symbol_info *lhs, symbol_info *rhs)
{
  // cout << "++" << endl;
  // printCompatibilityRelatedThings(lhs);
  // cout<< "--" << endl;
  // printCompatibilityRelatedThings(rhs);
  // cout << ".." << endl;
  if (lhs == nullptr || rhs == nullptr)
  {
  }
  else if ((lhs->id_type == ARRAY && lhs->current_index != -2 && lhs->array_type != rhs->variable_type) || (lhs->id_type == VARIABLE && lhs->variable_type != rhs->variable_type))
  {
    printError("Type Mismatch");
  }
  else if (lhs->id_type == ARRAY && lhs->current_index == -2)
  {
    printError("Type Mismatch, " + lhs->getName() + " is an array");
  }
  // printCompatibilityRelatedThings(lhs);
  return lhs;
}

symbol_info *setIntermediateValues(string symbol_type, string variable_type, float float_value = 0)
{
  string name = to_string(float_value);
  if (variable_type == integer)
  {
    name = to_string(int(float_value));
  }
  symbol_info *target = new symbol_info(name, symbol_type);
  target->id_type = VARIABLE;
  target->variable_type = variable_type;
  target->float_value = float_value;
  target->int_value = (int)float_value;
  // cout << line_count << endl;
  // printCompatibilityRelatedThings(target);
  return target;
}

symbol_info *checkAndDoMulopThings(symbol_info *left, string optr, symbol_info *right)
{
  if (checkNullFunctionArrayVoid(left, right, optr))
    return nullptr;

  if (optr == "%")
  {
    if (right->variable_type == fraction || left->variable_type == fraction)
    {
      printError("Non-Integer operand on modulus operator");
      return nullptr;
    }
  }

  symbol_info *s = new symbol_info(left->getName() + optr + right->getName(), "intermediate");
  setCompatibleRelatedThings(s, left->variable_type == fraction ? left : right);

  return s;
}

symbol_info *checkAdditionCompatibility(symbol_info *left, string optr, symbol_info *right)
{
  if (checkNull(left, right))
    return nullptr;
  // cout << line_count << " ";
  // printCompatibilityRelatedThings(left);
  // printCompatibilityRelatedThings(right);
  if (checkArray(left, right, optr))
    return nullptr;

  if (checkVoid(left, right, optr))
    return nullptr;

  symbol_info *s = new symbol_info(left->getName() + optr + right->getName(), "intermediate");
  setCompatibleRelatedThings(s, left->variable_type == fraction ? left : right);
  cout << line_count << endl;
  printCompatibilityRelatedThings(s);
  return s;
}

symbol_info *checkRELOPCompetibility(symbol_info *left, string optr, symbol_info *right)
{
  if (checkNull(left, right))
    return nullptr;

  if (checkArray(left, right, optr))
    return nullptr;

  if (checkVoid(left, right, optr))
    return nullptr;

  symbol_info *s = new symbol_info(left->getName() + optr + right->getName(), "intermediate");
  s->id_type = VARIABLE;
  s->variable_type = integer;
  return s;
}

symbol_info *checkLogicCompetibility(symbol_info *left, string optr, symbol_info *right)
{
  if (checkNull(left, right))
    return nullptr;

  if (checkArray(left, right, optr))
    return nullptr;

  if (checkVoid(left, right, optr))
    return nullptr;

  symbol_info *s = new symbol_info(left->getName() + optr + right->getName(), "intermediate");
  s->id_type = VARIABLE;
  s->variable_type = integer;
  return s;
}

symbol_info *checkFunctionArguments(symbol_info *symbolInfo)
{
  symbolInfo = findSymbol(symbolInfo);
  if (symbolInfo == nullptr)
    return nullptr;
  if (symbolInfo->id_type != FUNCTION)
  {
    printError("is not function");
    return nullptr;
  }
  if (args.size() != symbolInfo->sequence_of_parameters.size())
  {
    printError("size mismatch");
    return nullptr;
  }
  string argus = symbolInfo->getName() + "(";
  for (int i = 0; i < args.size(); i++)
  {
    if (args[i]->id_type == ARRAY)
    {
      printError("Type mismatch, " + args[i]->getName() + " is an array");
      return nullptr;
    }
    else if (args[i]->id_type == FUNCTION)
    {
      printError("Type mismatch, " + args[i]->getName() + " is an function");
      return nullptr;
    }

    if (args[i]->variable_type != symbolInfo->sequence_of_parameters[i]->variable_type)
    {
      printError("Type mismatch, " + args[i]->getName() + " is a " + args[i]->variable_type + ", expected " + symbolInfo->sequence_of_parameters[i]->variable_type);
      // printError("Type mismatch, found " + args[i]->variable_type + ", expected " + symbolInfo->sequence_of_parameters[i]->variable_type);
      return nullptr;
    }
    argus += args[i]->getName() + ",";
  }
  // cout << argus << endl;
  argus += ")";
  symbol_info *s = new symbol_info(argus, intermediate);
  s->id_type = VARIABLE;
  s->variable_type = symbolInfo->return_type;
  return s;
}

void checkFuncReturnCompatibility(symbol_info *symbolInfo)
{
  if (symbolInfo->variable_type != current_function->return_type)
  {
    printError("return value does not match");
  }
}

symbol_info *checkINDECopCompatibility(symbol_info *symbolInfo, string optr)
{
  if (checkNullFunctionArrayVoid(symbolInfo, optr))
    return nullptr;

  symbol_info *s = new symbol_info(symbolInfo->getName() + optr, intermediate);
  s->id_type = VARIABLE;
  s->variable_type = symbolInfo->variable_type;
  return s;
}

symbol_info *checkINCOPCompatibility(symbol_info *symbolInfo)
{
  return checkINDECopCompatibility(symbolInfo, "++");
}

symbol_info *checkDECOPCompatibility(symbol_info *symbolInfo)
{
  return checkINDECopCompatibility(symbolInfo, "--");
}

symbol_info *checkUnaryADDOPThings(string optr, symbol_info *symbolInfo)
{
  if (checkNullFunctionArrayVoid(symbolInfo, optr))
    return nullptr;

  symbol_info *s = new symbol_info(optr + symbolInfo->getName(), intermediate);
  s->id_type = VARIABLE;
  s->variable_type = symbolInfo->variable_type;
  return s;
}

symbol_info *checkNotCompatibility(symbol_info *symbolInfo)
{
  if (checkNullFunctionArrayVoid(symbolInfo, "!"))
    return nullptr;

  symbol_info *s = new symbol_info("!" + symbolInfo->getName(), intermediate);
  s->id_type = VARIABLE;
  s->variable_type = integer;
  return s;
}

symbol_info *checkLPAREN_Expression_RPAREN(symbol_info *symbolInfo)
{
  if (checkNullFunctionArrayVoid(symbolInfo, "()"))
    return nullptr;

  symbol_info *s = new symbol_info("(" + symbolInfo->getName() + ")", intermediate);
  s->id_type = VARIABLE;
  s->variable_type = symbolInfo->variable_type;
  return s;
}

// void print_error_recovery_mode(string msg){
//   logfile<<"Error at line "<<line_count<<": "<<msg<<endl<<endl;
// 	errorfile<<"Error at line "<<line_count<<": "<<msg<<endl<<endl;
//   syntax_error++;
// }

// void insert_variable_to_table(string datatype, string name){
//   SymbolInfo *s = new SymbolInfo(name,"ID");
//   s->setVariableDataType(datatype);
//   s->setIDType("VARIABLE");
//   //cout<<"handle_variable func 91"<<endl;
//   symboltable->Insert(s);
// }

// void handle_variable(string name){
//   if(datatype=="VOID"){
// 		logfile<<"Error at line "<<line_count<<": Variable type cannot be void"<<endl<<endl;
//     errorfile<<"Error at line "<<line_count<<": Variable type cannot be void"<<endl<<endl;
//     syntax_error++;
//   }
//   else{
//     SymbolInfo *temp = symboltable->Look_up_cur_scope(name);
//     if(temp==NULL){
// 			//cout<<"calling insert 105\n";
//       insert_variable_to_table(datatype, name);
//     }
//     else{
//       if(temp->getIDType()=="VARIABLE" || temp->getIDType()=="FUNCTION"){
//         errorfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp->getName()<<endl<<endl;
// 				logfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp->getName()<<endl<<endl;
//         syntax_error++;
//       }
//       else{
// 				//cout<<"calling insert 115\n";
//         insert_variable_to_table(datatype, name);
//       }
//       //cout<<"\nalready variable defined check kora lagbe variable ar function e kina. array hoile error na \n";
//     }
//   }
// }

// void handle_array(string name, string arrsize){
//   if(datatype=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(array data type can't be void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(array data type can't be void)"<<endl<<endl;
//     syntax_error++;
//     return;
//   }
//   else{
//     SymbolInfo *temp = symboltable->Look_up(name, "ARRAY");
//     if(temp==NULL){
//       SymbolInfo *s = new SymbolInfo(name, "ID");
//       s->setIDType("ARRAY");
//       s->setVariableDataType(datatype);
//       s->setArraySize(stoi(arrsize));
//       s->setArrayIndex(0);
//       if(datatype=="INT"){
//         for(int i=0;i<stoi(arrsize);i++) s->intArr.push_back(0);
//       }
//       else if(datatype=="FLOAT"){
//         for(int i=0;i<stoi(arrsize);i++) s->floatArr.push_back(0);
//       }
// 			//cout<<"calling insert 149\n";
//       symboltable->Insert(s);
//     }
//     else{
//       errorfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp->getName()<<endl<<endl;
//       syntax_error++;
//     }
//   }
// }

// bool search_in_parameter_list(string name){
// 	for(SymbolInfo *s:parameters){
// 		if(s->getName()==name){
// 			return true;
// 		}
// 	}
// 	return false;
// }

// void handle_func_parameter(string type, string name){
//   //cout<<"datatype: "<<datatype<<"    type: "<<type<<" name: "<<name<<endl;
//   if(datatype=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(parameter data type can't be void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(parameter data type can't be void)"<<endl<<endl;
//     syntax_error++;
//     return;
//   }
// 	if(search_in_parameter_list(name)){
// 		errorfile<<"Error at line "<<line_count<<": Multiple declaration of "<<name<<" in parameter"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Multiple declaration of "<<name<<" in parameter"<<endl<<endl;
//     syntax_error++;
//     return;
// 	}

//   SymbolInfo *s = new SymbolInfo(name,"ID");
//   s->setVariableDataType(datatype);
//   s->setIDType("VARIABLE");
//   parameters.push_back(s);
// }

// void handle_func_declaration(string retType, string name){
//   SymbolInfo *temp1 = symboltable->Look_up(name, "FUNCTION");
//   SymbolInfo *temp2 = symboltable->Look_up(name, "VARIABLE");

//   if(temp1==NULL && temp2==NULL){
//     SymbolInfo *s = new SymbolInfo(name, "ID");
//     s->setIDType("FUNCTION");
//     s->setFuncRetType(retType);
//     s->paramTypeList = paramTypeList;
// 		//cout<<"calling insert 203\n";
//     symboltable->Insert(s);
//     //symboltable->Print_All_ScopeTable();
//   }
//   else{
//     if(temp1!=NULL) {
// 			errorfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp1->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp1->getName()<<endl<<endl;
// 		}
//     else {
// 			errorfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp2->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp2->getName()<<endl<<endl;
// 		}
//     syntax_error++;
//   }
// }

// void handle_func_defination(string datatype, string name){
//   SymbolInfo *temp1 = symboltable->Look_up(name, "FUNCTION");
//   SymbolInfo *temp2 = symboltable->Look_up(name, "VARIABLE");

//   if(temp1==NULL && temp2==NULL){
//     SymbolInfo *s = new SymbolInfo(name, "ID");
//     s->setIDType("FUNCTION");
//     s->setFuncRetType(datatype);
//     for(int i=0;i<parameters.size();i++){
// 			SymbolInfo *t = parameters[i];
//       s->parameterList.push_back(t);
//       s->paramTypeList.push_back(parameters[i]->getVariableDataType());
//     }
//     s->setFuncDefined(true);
// 		//cout<<"calling insert 239\n";
//     symboltable->Insert(s);
//     current_function=s;
// 		//paramTypeList.clear();
// 		//parameters.clear();
//     return;
//   }
//   else if(temp1!=NULL && !temp1->getFuncDefined()){
//     if(datatype!=temp1->getFuncRetType()){
//       errorfile<<"Error at line "<<line_count<<": Return type mismatch with function declaration in function "<<temp1->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Return type mismatch with function declaration in function "<<temp1->getName()<<endl<<endl;
//       syntax_error++;
//       return;
//     }
//     if(temp1->paramTypeList.size()!=parameters.size()){
//       errorfile<<"Error at line "<<line_count<<": Total number of arguments mismatch with declaration in function "<<temp1->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Total number of arguments mismatch with declaration in function "<<temp1->getName()<<endl<<endl;
//       syntax_error++;
//       return;
//     }
//     bool typemismatch=false;
// 		int i;
//     for(i=0;i<parameters.size();i++){
//       if(parameters[i]->getVariableDataType()!=temp1->paramTypeList[i]){
//         typemismatch=true;
//         break;
//       }
//     }
//     if(typemismatch){
//       errorfile<<"Error at line "<<line_count<<": "<<i+1<<"th argument mismatch in function "<<temp1->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": "<<i+1<<"th argument mismatch in function "<<temp1->getName()<<endl<<endl;
//       syntax_error++;
//       return;
//     }
//     temp1->parameterList = parameters;
//     temp1->setFuncDefined(true);
//     current_function=temp1;
//   }
//   else{
//     if(temp1!=NULL){
// 			errorfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp1->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp1->getName()<<endl<<endl;
// 		}
//     else {
// 			errorfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp2->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Multiple Declaration of "<<temp2->getName()<<endl<<endl;
// 		}
//     syntax_error++;
//   }
// }

// void handle_func_return_type(SymbolInfo *exp){
//   if(current_function!=NULL && current_function->getIDType()=="FUNCTION" && exp->getVariableDataType()!=current_function->getFuncRetType()){
// 		if(current_function->getFuncRetType()=="FLOAT" && exp->getVariableDataType()=="INT"){
// 			current_function->floatArr.push_back(exp->intArr[0]);
// 		}
// 		else if(current_function->getFuncRetType()=="INT" && exp->getVariableDataType()=="FLOAT"){
// 			current_function->intArr.push_back(int(exp->floatArr[0]));
// 			errorfile<<"Error at line "<<line_count<<": Return type Mismatch of "<<current_function->getName()<<" (float is returning to int type)"<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Return type Mismatch of "<<current_function->getName()<<" (float is returning to int type)"<<endl<<endl;
// 	    syntax_error++;
// 	    return;
// 		}
// 		else{
// 			errorfile<<"Error at line "<<line_count<<": Return type Mismatch of "<<current_function->getName()<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Return type Mismatch of "<<current_function->getName()<<endl<<endl;
//     	syntax_error++;
//     	return;
// 		}
//   }
//   if(current_function!=NULL && current_function->getIDType()=="FUNCTION"){
//     if(current_function->getFuncRetType()=="INT") current_function->intArr.push_back(exp->intArr[0]);
//     else if(current_function->getFuncRetType()=="FLOAT") current_function->floatArr.push_back(exp->floatArr[0]);
//   }
// }

// SymbolInfo* check_function(SymbolInfo *f){
// 	SymbolInfo *called_func = symboltable->Look_up(f->getName(),"FUNCTION");
//   if(called_func==NULL){
//     errorfile<<"Error at line "<<line_count<<": Undeclared function "<<f->getName()<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Undeclared function "<<f->getName()<<endl<<endl;
//     syntax_error++;
//     return NULL;
//   }
// 	return called_func;
// }

// void check_func_arguements(SymbolInfo *f){
//   //cout<<f->getName()<<endl;
//   if(f==NULL){
//     errorfile<<"Error at line "<<line_count<<": function Not defined "<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": function Not defined "<<endl<<endl;
//     syntax_error++;
//     return;
//   }
//   SymbolInfo *called_func = symboltable->Look_up(f->getName(),"FUNCTION");
//   if(parameters.size()!=called_func->paramTypeList.size()){
//     errorfile<<"Error at line "<<line_count<<": Total number of arguments mismatch in function "<<f->getName()<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Total number of arguments mismatch in function "<<f->getName()<<endl<<endl;
//     syntax_error++;
//     return;
//   }
//   //cout<<"here i am 211   size "<<parameters.size()<<endl;
//   bool typemismatch=false;
// 	int i;
//   for(i=0;i<parameters.size();i++){
//     //cout<<"line : "<<line_count<<" "<<f->getName()<<" caller type: "<<parameters[i]->getVariableDataType()<<" callee type: "<<called_func->paramTypeList[i]<<endl;
// 		string t1="";
// 		if(parameters[i]!=NULL) t1 = parameters[i]->getVariableDataType();
// 		string t2 = called_func->paramTypeList[i];
//     if(parameters[i]!=NULL && t1!=t2){
// 			if(t1=="INT" && t2=="FLOAT"){
// 				continue;
// 			}
//       typemismatch=true;
//       break;
//     }
//   }
//   //cout<<"here i am 2\n";
//   if(typemismatch){
// 		errorfile<<"Error at line "<<line_count<<": "<<i+1<<"th argument mismatch in function "<<f->getName()<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": "<<i+1<<"th argument mismatch in function "<<f->getName()<<endl<<endl;
//     syntax_error++;
//     return;
//   }
//   //cout<<"here i am 5\n";
// }

// SymbolInfo* handle_var_ID(SymbolInfo *id){
//   SymbolInfo *temp = symboltable->Look_up(id->getName());
//   if(temp!=NULL && temp->getIDType()=="ARRAY"){
//     errorfile<<"Error at line "<<line_count<<": Type mismatch, "<<temp->getName()<<" is an array"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type mismatch, "<<temp->getName()<<" is an array"<<endl<<endl;
//     syntax_error++;
//     return NULL;
//   }

// 	if(temp!=NULL && temp->getIDType()=="FUNCTION"){
//     errorfile<<"Error at line "<<line_count<<": Type mismatch, "<<temp->getName()<<" is a function"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type mismatch, "<<temp->getName()<<" is a function"<<endl<<endl;
//     syntax_error++;
//     return NULL;
//   }

//   if(temp==NULL){
//     errorfile<<"Error at line "<<line_count<<": Undeclared Variable "<<id->getName()<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Undeclared Variable "<<id->getName()<<endl<<endl;
//     syntax_error++;
//     return NULL;
//   }
//   return temp;
// }

// SymbolInfo* handle_assignop(SymbolInfo *var, SymbolInfo *exp){

//   if(var==NULL || exp==NULL) return NULL;

//   string type1 = var->getVariableDataType();
//   string type2 = exp->getVariableDataType();
//   var->intArr.push_back(0);
//   var->floatArr.push_back(0);

//   //cout<<var->intArr[0]<<endl;
//   //cout<<"id type: "<<var->getIDType()<<" "<<exp->getIDType()<<" var int 0: "<<exp->intArr[0]<<endl;

//   if(type1=="VOID" || type2=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(assign operator can't take void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(assign operator can't take void)"<<endl<<endl;
//     syntax_error++;
//     return var;
//   }

//   if(exp->getIDType()=="FUNCTION"){
//     string rettype = exp->getFuncRetType();
//     if(rettype=="VOID"){
//       errorfile<<"Error at line "<<line_count<<": Type Mismatch(assign operator can't take void. function return type is void)"<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Type Mismatch(assign operator can't take void. function return type is void)"<<endl<<endl;
//       syntax_error++;
//       return var;
//     }
//     if(rettype==type1){
//       if(type1=="INT") var->intArr[0] = exp->intArr[0];
//       else if(type1=="FLOAT") var->floatArr[0] = exp->floatArr[0];
//     }
//     else if(rettype=="INT" && type1=="FLOAT"){
//       var->floatArr[0] = exp->intArr[0]*1.0;
//     }
//     else if(rettype=="FLOAT" && type1=="INT"){
//       errorfile<<"Error at line "<<line_count<<": Type Mismatch(return type float is assigned to int type variable)"<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Type Mismatch(return type float is assigned to int type variable)"<<endl<<endl;
//       syntax_error++;
//       var->intArr[0] = exp->floatArr[0];
//     }
//   }

//   else if(exp->getIDType()=="VARIABLE"){
//     if(type1==type2){
//       if(type1=="INT") var->intArr[0] = exp->intArr[0];
//       else if(type1=="FLOAT") var->floatArr[0] = exp->floatArr[0];
//     }
//     else if(type1=="INT" && type2=="FLOAT"){
//       errorfile<<"Error at line "<<line_count<<": Type Mismatch(float is assigned to int type variable)"<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Type Mismatch(float is assigned to int type variable)"<<endl<<endl;
//       syntax_error++;
//       var->intArr[0] = exp->floatArr[0];
//     }
//     else if(type1=="FLOAT" && type2=="INT"){
//       var->floatArr[0] = exp->intArr[0]*1.0;
//     }
//   }

//   else if(exp->getIDType()=="ARRAY"){
//     if(type1==type2){
//       if(type1=="INT") var->intArr[0] = exp->intArr[exp->getArrayIndex()];
//       else if(type1=="FLOAT") var->floatArr[0] = exp->floatArr[exp->getArrayIndex()];
//     }
//     else if(type1=="INT" && type2=="FLOAT"){
//       errorfile<<"Error at line "<<line_count<<": Type Mismatch(float is assigned to int type variable)"<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Type Mismatch(float is assigned to int type variable)"<<endl<<endl;
//       syntax_error++;
//       var->intArr[0] = exp->floatArr[exp->getArrayIndex()];
//     }
//     else if(type1=="FLOAT" && type2=="INT"){
//       var->floatArr[0] = exp->intArr[exp->getArrayIndex()]*1.0;
//     }
//   }

//   return var;
// }

// SymbolInfo* handle_addop_unary_exp(SymbolInfo *op, SymbolInfo *exp){
//   if(exp->getVariableDataType()=="VOID" || exp->getFuncRetType()=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(void)"<<endl<<endl;
//     syntax_error++;
//     return NULL;
//   }

//   SymbolInfo *t = exp;
//   if(op->getName()=="-"){
//     if(exp->getIDType()=="VARIABLE"){
//       if(exp->getVariableDataType()=="INT") t->intArr[0] = -exp->intArr[0];
//       else if(exp->getVariableDataType()=="FLOAT") t->floatArr[0] = -exp->floatArr[0];
//     }
//     else if(exp->getIDType()=="ARRAY"){
//       if(exp->getVariableDataType()=="INT") t->intArr[0] = -exp->intArr[exp->getArrayIndex()];
//       else if(exp->getVariableDataType()=="FLOAT") t->floatArr[0] = -exp->floatArr[exp->getArrayIndex()];
//     }
//     else if(exp->getIDType()=="FUNCTION"){
//       if(exp->getVariableDataType()=="INT") t->intArr[0] = -exp->intArr[0];
//       else if(exp->getVariableDataType()=="FLOAT") t->floatArr[0] = -exp->floatArr[0];
//     }
//   }
//   return t;
// }

// SymbolInfo* handle_not_unary_exp(SymbolInfo *exp){
//   if(exp->getVariableDataType()=="VOID" || exp->getFuncRetType()=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(void)"<<endl<<endl;
//     syntax_error++;
//     return NULL;
//   }

//   SymbolInfo *t = new SymbolInfo();
//   t->setIDType("VARIABLE");
//   t->intArr.push_back(0);
//   t->setVariableDataType("INT");

//   if(exp->getIDType()=="VARIABLE"){
//     if(exp->getVariableDataType()=="INT" && exp->intArr[0]==0) t->intArr[0] = 1;
//     else if(exp->getVariableDataType()=="FLOAT" && exp->floatArr[0]==0) t->intArr[0] = 1;
//   }
//   else if(exp->getIDType()=="ARRAY"){
//     if(exp->getVariableDataType()=="INT" && exp->intArr[exp->getArrayIndex()]==0) t->intArr[0] = 1;
//     else if(exp->getVariableDataType()=="FLOAT" && exp->floatArr[exp->getArrayIndex()]==0) t->intArr[0] = 1;
//   }
//   else if(exp->getIDType()=="FUNCTION"){
//     if(exp->getVariableDataType()=="INT" && exp->intArr[0]==0) t->intArr[0] = 1;
//     else if(exp->getVariableDataType()=="FLOAT" && exp->floatArr[0]==0) t->intArr[0] = 1;
//   }

//   return t;
// }

// SymbolInfo *handle_ex_op_ex_function(SymbolInfo *e1, SymbolInfo *op, SymbolInfo *e2){
// 	SymbolInfo *t = new SymbolInfo();
//   t->setIDType("VARIABLE");
//   t->intArr.push_back(0);
//   t->floatArr.push_back(0);

// 	if(e1->getVariableDataType()=="VOID" || e2->getVariableDataType()=="VOID" || e1->getFuncRetType()=="VOID" || e2->getFuncRetType()=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(simple expression or term type can't be void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(simple expression or term type can't be void)"<<endl<<endl;
//     syntax_error++;
//     return t;
//   }

// 	if(e1->getIDType()=="VARIABLE"){
//     if(e2->getIDType()=="FUNCTION"){
//       if(e1->getVariableDataType()=="INT"){
//         if(e2->getFuncRetType()=="INT"){
//           t->setVariableDataType("INT");
//           if(op->getName()=="+") t->intArr[0] = e1->intArr[0] + e2->intArr[0]; //var_int+var_int
//           else if(op->getName()=="-") t->intArr[0] = e1->intArr[0] - e2->intArr[0]; //var_int-var_int
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[0] && e2->intArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[0] || e2->intArr[0];
//         }
//         else if(e2->getFuncRetType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(op->getName()=="+") t->floatArr[0] = e1->intArr[0] + e2->floatArr[0]; //var_int+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->intArr[0] - e2->floatArr[0]; //var_int-var_float
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[0] && e2->floatArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[0] || e2->floatArr[0];
//         }
//       }
//       else if(e1->getVariableDataType()=="FLOAT"){
//         if(e2->getFuncRetType()=="INT"){
//           t->setVariableDataType("FLOAT");
//           if(op->getName()=="+") t->floatArr[0] = e1->floatArr[0] + e2->intArr[0]; //var_float+var_int
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[0] - e2->intArr[0]; //var_float-var_int
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[0] && e2->intArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[0] || e2->intArr[0];
//         }
//         else if(e2->getFuncRetType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(op->getName()=="+") t->floatArr[0] = e1->floatArr[0] + e2->floatArr[0]; //var_float+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[0] - e2->floatArr[0]; //var_float-var_float
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[0] && e2->floatArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[0] || e2->floatArr[0];
//         }
//       }
//     }
// 	}

// 	if(e1->getIDType()=="ARRAY"){
//     if(e2->getIDType()=="FUNCTION"){
//       if(e1->getVariableDataType()=="INT"){
//         if(e2->getFuncRetType()=="INT"){
//           t->setVariableDataType("INT");
//           if(op->getName()=="+") t->intArr[0] = e1->intArr[e1->getArrayIndex()] + e2->intArr[0]; //var_int+var_int
//           else if(op->getName()=="-") t->intArr[0] = e1->intArr[e1->getArrayIndex()] - e2->intArr[0]; //var_int-var_int
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[e1->getArrayIndex()] && e2->intArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[e1->getArrayIndex()] || e2->intArr[0];
//         }
//         else if(e2->getFuncRetType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(op->getName()=="+") t->floatArr[0] = e1->intArr[e1->getArrayIndex()] + e2->floatArr[0]; //var_int+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->intArr[e1->getArrayIndex()] - e2->floatArr[0]; //var_int-var_float
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[e1->getArrayIndex()] && e2->floatArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[e1->getArrayIndex()] || e2->floatArr[0];
//         }
//       }
//       else if(e1->getVariableDataType()=="FLOAT"){
//         if(e2->getFuncRetType()=="INT"){
//           t->setVariableDataType("FLOAT");
//           if(op->getName()=="+") t->floatArr[0] = e1->floatArr[e1->getArrayIndex()] + e2->intArr[0]; //var_float+var_int
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[e1->getArrayIndex()] - e2->intArr[0]; //var_float-var_int
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[e1->getArrayIndex()] && e2->intArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[e1->getArrayIndex()] || e2->intArr[0];
//         }
//         else if(e2->getFuncRetType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(op->getName()=="+") t->floatArr[0] = e1->floatArr[e1->getArrayIndex()] + e2->floatArr[0]; //var_float+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[e1->getArrayIndex()] - e2->floatArr[0]; //var_float-var_float
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[e1->getArrayIndex()] && e2->floatArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[e1->getArrayIndex()] || e2->floatArr[0];
//         }
//       }
//     }
// 	}

// 	if(e1->getIDType()=="FUNCTION"){
// 		if(e2->getIDType()=="VARIABLE"){
// 			if(e1->getFuncRetType()=="INT"){
// 				if(e2->getVariableDataType()=="INT"){
// 					t->setVariableDataType("INT");
// 					if(op->getName()=="+") t->intArr[0] = e1->intArr[0] + e2->intArr[0]; //var_int+var_int
//           else if(op->getName()=="-") t->intArr[0] = e1->intArr[0] - e2->intArr[0];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[0] && e2->intArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[0] || e2->intArr[0];
// 				}
// 				else if(e2->getVariableDataType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->intArr[0] + e2->floatArr[0]; //var_int+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->intArr[0] - e2->floatArr[0];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[0] && e2->floatArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[0] || e2->floatArr[0];
// 				}
// 			}
// 			else if(e1->getFuncRetType()=="FLOAT"){
// 				if(e2->getVariableDataType()=="INT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->floatArr[0] + e2->intArr[0]; //var_float+var_int
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[0] - e2->intArr[0];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[0] && e2->intArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[0] || e2->intArr[0];
// 				}
// 				else if(e2->getVariableDataType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->floatArr[0] + e2->floatArr[0]; //var_float+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[0] - e2->floatArr[0];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[0] && e2->floatArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[0] || e2->floatArr[0];
// 				}
// 			}
// 		}
// 		else if(e2->getIDType()=="ARRAY"){
// 			if(e1->getFuncRetType()=="INT"){
// 				if(e2->getVariableDataType()=="INT"){
// 					t->setVariableDataType("INT");
// 					if(op->getName()=="+") t->intArr[0] = e1->intArr[0] + e2->intArr[e2->getArrayIndex()]; //var_int+var_int
//           else if(op->getName()=="-") t->intArr[0] = e1->intArr[0] - e2->intArr[e2->getArrayIndex()];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[0] && e2->intArr[e2->getArrayIndex()];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[0] || e2->intArr[e2->getArrayIndex()];
// 				}
// 				else if(e2->getVariableDataType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->intArr[0] + e2->floatArr[e2->getArrayIndex()]; //var_int+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->intArr[0] - e2->floatArr[e2->getArrayIndex()];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[0] && e2->floatArr[e2->getArrayIndex()];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[0] || e2->floatArr[e2->getArrayIndex()];
// 				}
// 			}
// 			else if(e1->getFuncRetType()=="FLOAT"){
// 				if(e2->getVariableDataType()=="INT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->floatArr[0] + e2->intArr[e2->getArrayIndex()]; //var_float+var_int
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[0] - e2->intArr[e2->getArrayIndex()];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[0] && e2->intArr[e2->getArrayIndex()];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[0] || e2->intArr[e2->getArrayIndex()];
// 				}
// 				else if(e2->getVariableDataType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->floatArr[0] + e2->floatArr[e2->getArrayIndex()]; //var_float+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[0] - e2->floatArr[e2->getArrayIndex()];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[0] && e2->floatArr[e2->getArrayIndex()];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[0] || e2->floatArr[e2->getArrayIndex()];
// 				}
// 			}
// 		}
// 		else if(e2->getIDType()=="FUNCTION"){
// 			if(e1->getFuncRetType()=="INT"){
// 				if(e2->getFuncRetType()=="INT"){
// 					t->setVariableDataType("INT");
// 					if(op->getName()=="+") t->intArr[0] = e1->intArr[0] + e2->intArr[0]; //var_int+var_int
//           else if(op->getName()=="-") t->intArr[0] = e1->intArr[0] - e2->intArr[0];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[0] && e2->intArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[0] || e2->intArr[0];
// 				}
// 				else if(e2->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->intArr[0] + e2->floatArr[0]; //var_int+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->intArr[0] - e2->floatArr[0];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->intArr[0] && e2->floatArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->intArr[0] || e2->floatArr[0];
// 				}
// 			}
// 			else if(e1->getFuncRetType()=="FLOAT"){
// 				if(e2->getFuncRetType()=="INT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->floatArr[0] + e2->intArr[0]; //var_float+var_int
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[0] - e2->intArr[0];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[0] && e2->intArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[0] || e2->intArr[0];
// 				}
// 				else if(e2->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					if(op->getName()=="+") t->floatArr[0] = e1->floatArr[0] + e2->floatArr[0]; //var_float+var_float
//           else if(op->getName()=="-") t->floatArr[0] = e1->floatArr[0] - e2->floatArr[0];
// 					else if(op->getName()=="&&") t->intArr[0] = e1->floatArr[0] && e2->floatArr[0];
// 					else if(op->getName()=="||") t->intArr[0] = e1->floatArr[0] || e2->floatArr[0];
// 				}
// 			}
// 		}
// 	}
// 	if(op->getName()=="&&" || op->getName()=="||") t->setVariableDataType("INT");
// 	 return t;

//  }

// SymbolInfo* handle_simexp_relop_simexp(SymbolInfo *exp1, SymbolInfo *relop, SymbolInfo *exp2){
//   SymbolInfo *t = new SymbolInfo();
//   t->setIDType("VARIABLE");
//   t->intArr.push_back(0);
//   t->setVariableDataType("INT");

//   //cout<<"exp 1 id type: "<<exp1->getIDType()<<"   exp2 id type: "<<exp2->getIDType()<<endl;
//   string type1 = exp1->getVariableDataType();
//   string type2 = exp2->getVariableDataType();

//   if(type1=="VOID" || type2=="VOID"||exp1->getFuncRetType()=="VOID" || exp2->getFuncRetType()=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(can't compare void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(can't compare void)"<<endl<<endl;
//     syntax_error++;
//     return t;
//   }

//   if(relop->getName()=="<" || relop->getName()=="<="){
//     if(exp1->getIDType()=="VARIABLE"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]<exp2->intArr[0]) t->intArr[0]=1;
//             else if(exp1->intArr[0]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]<exp2->floatArr[0]) t->intArr[0]=1;
//             else if(exp1->intArr[0]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]<exp2->intArr[0]) t->intArr[0]=1;
//             else if(exp1->floatArr[0]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]<exp2->floatArr[0]) t->intArr[0]=1;
//             else if(exp1->floatArr[0]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]<exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->intArr[0]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]<exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->intArr[0]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]<exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->floatArr[0]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]<exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->floatArr[0]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//     else if(exp1->getIDType()=="ARRAY"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]<exp2->intArr[0]) t->intArr[0]=1;
//             else if(exp1->intArr[exp1->getArrayIndex()]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]<exp2->floatArr[0]) t->intArr[0]=1;
//             else if(exp1->intArr[exp1->getArrayIndex()]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]<exp2->intArr[0]) t->intArr[0]=1;
//             else if(exp1->floatArr[exp1->getArrayIndex()]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]<exp2->floatArr[0]) t->intArr[0]=1;
//             else if(exp1->floatArr[exp1->getArrayIndex()]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]<exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->intArr[exp1->getArrayIndex()]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]<exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->intArr[exp1->getArrayIndex()]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]<exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->floatArr[exp1->getArrayIndex()]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]<exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->floatArr[exp1->getArrayIndex()]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//   }
//   else if(relop->getName()==">" || relop->getName()==">="){
//     if(exp1->getIDType()=="VARIABLE"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]>exp2->intArr[0]) t->intArr[0]=1;
//             else if(exp1->intArr[0]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]>exp2->floatArr[0]) t->intArr[0]=1;
//             else if(exp1->intArr[0]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]>exp2->intArr[0]) t->intArr[0]=1;
//             else if(exp1->floatArr[0]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]>exp2->floatArr[0]) t->intArr[0]=1;
//             else if(exp1->floatArr[0]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]>exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->intArr[0]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]>exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->intArr[0]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]>exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->floatArr[0]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]>exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->floatArr[0]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//     else if(exp1->getIDType()=="ARRAY"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]>exp2->intArr[0]) t->intArr[0]=1;
//             else if(exp1->intArr[exp1->getArrayIndex()]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]>exp2->floatArr[0]) t->intArr[0]=1;
//             else if(exp1->intArr[exp1->getArrayIndex()]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]>exp2->intArr[0]) t->intArr[0]=1;
//             else if(exp1->floatArr[exp1->getArrayIndex()]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]>exp2->floatArr[0]) t->intArr[0]=1;
//             else if(exp1->floatArr[exp1->getArrayIndex()]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]>exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->intArr[exp1->getArrayIndex()]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]>exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->intArr[exp1->getArrayIndex()]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]>exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->floatArr[exp1->getArrayIndex()]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]>exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//             else if(exp1->floatArr[exp1->getArrayIndex()]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//   }
//   else if(relop->getName()=="=="){
//     if(exp1->getIDType()=="VARIABLE"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//     else if(exp1->getIDType()=="ARRAY"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]==exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]==exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]==exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]==exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//   }
//   else if(relop->getName()=="!="){
//     if(exp1->getIDType()=="VARIABLE"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]!=exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]!=exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]!=exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]!=exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]!=exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]!=exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]!=exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]!=exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//     else if(exp1->getIDType()=="ARRAY"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]!=exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]!=exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]!=exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]!=exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]!=exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]!=exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]!=exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]!=exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//   }

//   return t;
// }

// SymbolInfo* handle_relexp_logicop_relexp(SymbolInfo *exp1,SymbolInfo *logiop, SymbolInfo *exp2){
//   string type1 = exp1->getVariableDataType();
//   string type2 = exp2->getVariableDataType();
// 	//cout<<line_count<<" "<<exp2->getName()<<" "<<exp2->getIDType()<<endl;
//   SymbolInfo *t = new SymbolInfo();
//   t->setIDType("VARIABLE");
//   t->setVariableDataType("INT");
//   t->intArr.push_back(0);

//   if(type1=="VOID" || type2=="VOID" || exp1->getFuncRetType()=="VOID" || exp2->getFuncRetType()=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(logic expression type can't be void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(logic expression type can't be void)"<<endl<<endl;
//     syntax_error++;
//     return t;
//   }

// 	if(exp1->getIDType()=="FUNCTION" || exp2->getIDType()=="FUNCTION"){
// 		return handle_ex_op_ex_function(exp1,logiop,exp2);
// 	}

//   if(logiop->getName()=="&&"){
//     if(exp1->getIDType()=="VARIABLE"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0] && exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]&&exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]&&exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]&&exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0]&&exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0]&&exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0]&&exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0]&&exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//     else if(exp1->getIDType()=="ARRAY"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]&&exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]&&exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]&&exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]&&exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()]&&exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()]&&exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]&&exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()]&&exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//   }

//   else if(logiop->getName()=="||"){
//     if(exp1->getIDType()=="VARIABLE"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0] || exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0] || exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0] || exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0] || exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[0] || exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[0] || exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[0] || exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[0] || exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//     else if(exp1->getIDType()=="ARRAY"){
//       if(exp2->getIDType()=="VARIABLE"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()] || exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()] || exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()] || exp2->intArr[0]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()] || exp2->floatArr[0]) t->intArr[0]=1;
//           }
//         }
//       }
//       else if(exp2->getIDType()=="ARRAY"){
//         if(type1=="INT"){
//           if(type2=="INT"){
//             if(exp1->intArr[exp1->getArrayIndex()] || exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->intArr[exp1->getArrayIndex()] || exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//         else if(type1=="FLOAT"){
//           if(type2=="INT"){
//             if(exp1->floatArr[exp1->getArrayIndex()] || exp2->intArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//           else if(type2=="FLOAT"){
//             if(exp1->floatArr[exp1->getArrayIndex()] || exp2->floatArr[exp2->getArrayIndex()]) t->intArr[0]=1;
//           }
//         }
//       }
//     }
//   }
//   return t;
// }

// SymbolInfo* handle_simexp_addop_term(SymbolInfo *exp, SymbolInfo *addop, SymbolInfo *ter){
//   SymbolInfo *t = new SymbolInfo();
//   t->setIDType("VARIABLE");
//   t->intArr.push_back(0);
//   t->floatArr.push_back(0);

// 	//cout<<line_count<<" "<< exp->getIDType()<<" "<<ter->getIDType()<<endl;

//   if(exp->getVariableDataType()=="VOID" || ter->getVariableDataType()=="VOID" || exp->getFuncRetType()=="VOID" || ter->getFuncRetType()=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type Mismatch(simple expression or term type can't be void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type Mismatch(simple expression or term type can't be void)"<<endl<<endl;
//     syntax_error++;
//     return t;
//   }

// 	if(exp==NULL || addop==NULL || ter == NULL) return t;

// 	if(exp->getIDType()=="FUNCTION" || ter->getIDType()=="FUNCTION"){
// 		return handle_ex_op_ex_function(exp,addop,ter);
// 	}

//   if(ter->getIDType()=="VARIABLE"){
//     if(exp->getIDType()=="VARIABLE"){
//       if(ter->getVariableDataType()=="INT"){
//         if(exp->getVariableDataType()=="INT"){
//           t->setVariableDataType("INT");
//           if(addop->getName()=="+") t->intArr[0] = ter->intArr[0] + exp->intArr[0]; //var_int+var_int
//           else t->intArr[0] = ter->intArr[0] - exp->intArr[0]; //var_int-var_int
//         }
//         else if(exp->getVariableDataType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->intArr[0] + exp->floatArr[0]; //var_int+var_float
//           else t->floatArr[0] = ter->intArr[0] - exp->floatArr[0]; //var_int-var_float
//         }
//       }
//       else if(ter->getVariableDataType()=="FLOAT"){
//         if(exp->getVariableDataType()=="INT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->floatArr[0] + exp->intArr[0]; //var_float+var_int
//           else t->floatArr[0] = ter->floatArr[0] - exp->intArr[0]; //var_float-var_int
//         }
//         else if(exp->getVariableDataType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->floatArr[0] + exp->floatArr[0]; //var_float+var_float
//           else t->floatArr[0] = ter->floatArr[0] - exp->floatArr[0]; //var_float-var_float
//         }
//       }
//     }
//     else if(exp->getIDType()=="ARRAY"){
//       if(ter->getVariableDataType()=="INT"){
//         if(exp->getVariableDataType()=="INT"){
//           t->setVariableDataType("INT");
//           if(addop->getName()=="+") t->intArr[0] = ter->intArr[0] + exp->intArr[exp->getArrayIndex()]; //var_int+arr_int
//           else t->intArr[0] = ter->intArr[0] - exp->intArr[exp->getArrayIndex()]; //var_int-arr_int
//         }
//         else if(exp->getVariableDataType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->intArr[0] + exp->floatArr[exp->getArrayIndex()]; //var_int+arr_float
//           else t->floatArr[0] = ter->intArr[0] - exp->floatArr[exp->getArrayIndex()]; //var_int-arr_float
//         }
//       }
//       else if(ter->getVariableDataType()=="FLOAT"){
//         if(exp->getVariableDataType()=="INT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->floatArr[0] + exp->intArr[exp->getArrayIndex()]; //var_float+arr_int
//           else t->floatArr[0] = ter->floatArr[0] - exp->intArr[exp->getArrayIndex()]; //var_float-arr_int
//         }
//         else if(exp->getVariableDataType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->floatArr[0] + exp->floatArr[exp->getArrayIndex()]; //var_float+arr_float
//           else t->floatArr[0] = ter->floatArr[0] - exp->floatArr[exp->getArrayIndex()]; //var_float-arr_float
//         }
//       }

//     }
//   }
//   else if(ter->getIDType()=="ARRAY"){
//     if(exp->getIDType()=="VARIABLE"){
//       if(ter->getVariableDataType()=="INT"){
//         if(exp->getVariableDataType()=="INT"){
//           t->setVariableDataType("INT");
//           if(addop->getName()=="+") t->intArr[0] = ter->intArr[ter->getArrayIndex()] + exp->intArr[0]; //arr_int+var_int
//           else t->intArr[0] = ter->intArr[ter->getArrayIndex()] - exp->intArr[0]; //arr_int-var_int
//         }
//         else if(exp->getVariableDataType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->intArr[ter->getArrayIndex()] + exp->floatArr[0]; //arr_int+var_float
//           else t->floatArr[0] = ter->intArr[ter->getArrayIndex()] - exp->floatArr[0]; //arr_int-var_float
//         }
//       }
//       else if(ter->getVariableDataType()=="FLOAT"){
//         if(exp->getVariableDataType()=="INT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] + exp->intArr[0]; //arr_float+var_int
//           else t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] - exp->intArr[0]; //arr_float-var_int
//         }
//         else if(exp->getVariableDataType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] + exp->floatArr[0]; //arr_float+var_float
//           else t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] - exp->floatArr[0]; //arr_float-var_float
//         }
//       }

//     }
//     else if(exp->getIDType()=="ARRAY"){
//       if(ter->getVariableDataType()=="INT"){
//         if(exp->getVariableDataType()=="INT"){
//           t->setVariableDataType("INT");
//           if(addop->getName()=="+") t->intArr[0] = ter->intArr[ter->getArrayIndex()] + exp->intArr[exp->getArrayIndex()]; //arr_int+arr_int
//           else t->intArr[0] = ter->intArr[ter->getArrayIndex()] - exp->intArr[exp->getArrayIndex()]; //arr_int-arr_int
//         }
//         else if(exp->getVariableDataType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->intArr[ter->getArrayIndex()] + exp->floatArr[exp->getArrayIndex()]; //arr_int+arr_float
//           else t->floatArr[0] = ter->intArr[ter->getArrayIndex()] - exp->floatArr[exp->getArrayIndex()]; //arr_int-arr_float
//         }
//       }
//       else if(ter->getVariableDataType()=="FLOAT"){
//         if(exp->getVariableDataType()=="INT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] + exp->intArr[exp->getArrayIndex()]; //arr_float+arr_int
//           else t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] - exp->intArr[exp->getArrayIndex()]; //arr_float-arr_int
//         }
//         else if(exp->getVariableDataType()=="FLOAT"){
//           t->setVariableDataType("FLOAT");
//           if(addop->getName()=="+") t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] + exp->floatArr[exp->getArrayIndex()]; //arr_float+arr_float
//           else t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] - exp->floatArr[exp->getArrayIndex()]; //arr_float-arr_float
//         }
//       }

//     }
//   }

//   return t;
// }

// void print_divide_by_zero(){
// 		errorfile<<"Error at line "<<line_count<<": Divide by Zero"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Divide by Zero"<<endl<<endl;
// 		syntax_error++;
// }

// SymbolInfo* handle_term_mulop_unary_exp(SymbolInfo *ter, SymbolInfo *mulop, SymbolInfo *exp){
//   SymbolInfo *t = new SymbolInfo();
//   t->setIDType("VARIABLE");
//   t->intArr.push_back(0);
//   t->floatArr.push_back(0);

//   if(exp->getVariableDataType()=="VOID" || ter->getVariableDataType()=="VOID" || exp->getFuncRetType()=="VOID" || ter->getFuncRetType()=="VOID"){
//     errorfile<<"Error at line "<<line_count<<": Type mismatch(unary expression or term type can't be void)"<<endl<<endl;
// 		logfile<<"Error at line "<<line_count<<": Type mismatch(unary expression or term type can't be void)"<<endl<<endl;
//     syntax_error++;
//     return t;
//   }

//   //cout<<"\n ter: "<<ter->getName()<<endl;

//   if(mulop->getName()=="*"){
//     if(ter->getIDType()=="VARIABLE"){
//       if(exp->getIDType()=="VARIABLE"){
//         if(ter->getVariableDataType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             t->intArr[0] = ter->intArr[0] * exp->intArr[0]; //var_int*var_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->intArr[0] * exp->floatArr[0]; //var_int*var_float
//           }
//         }
//         else if(ter->getVariableDataType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[0] * exp->intArr[0]; //var_float*var_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[0] * exp->floatArr[0]; //var_float*var_float
//           }
//         }
//       }
//       else if(exp->getIDType()=="ARRAY"){
//         if(ter->getVariableDataType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             t->intArr[0] = ter->intArr[0] * exp->intArr[exp->getArrayIndex()]; //var_int*arr_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->intArr[0] * exp->floatArr[exp->getArrayIndex()]; //var_int*arr_float
//           }
//         }
//         else if(ter->getVariableDataType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[0] * exp->intArr[exp->getArrayIndex()]; //var_float*arr_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[0] * exp->floatArr[exp->getArrayIndex()]; //var_float*arr_float
//           }
//         }

//       }
// 		else if(exp->getIDType()=="FUNCTION"){
// 			if(ter->getVariableDataType()=="INT"){
// 				if(exp->getFuncRetType()=="INT"){
// 					t->setVariableDataType("INT");
// 					t->intArr[0] = ter->intArr[0] * exp->intArr[0]; //var_int*var_int
// 				}
// 				else if(exp->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->intArr[0] * exp->floatArr[0]; //var_int*var_float
// 				}
// 			}
// 			else if(ter->getVariableDataType()=="FLOAT"){
// 				if(exp->getFuncRetType()=="INT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->floatArr[0] * exp->intArr[0]; //var_float*var_int
// 				}
// 				else if(exp->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->floatArr[0] * exp->floatArr[0]; //var_float*var_float
// 				}
// 			}
// 		}
//     }
//     else if(ter->getIDType()=="ARRAY"){
//       if(exp->getIDType()=="VARIABLE"){
//         if(ter->getVariableDataType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             t->intArr[0] = ter->intArr[ter->getArrayIndex()] * exp->intArr[0]; //arr_int*var_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->intArr[ter->getArrayIndex()] * exp->floatArr[0]; //arr_int*var_float
//           }
//         }
//         else if(ter->getVariableDataType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] * exp->intArr[0]; //arr_float*var_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] * exp->floatArr[0]; //arr_float*var_float
//           }
//         }

//       }
//       else if(exp->getIDType()=="ARRAY"){
//         if(ter->getVariableDataType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             t->intArr[0] = ter->intArr[ter->getArrayIndex()] * exp->intArr[exp->getArrayIndex()]; //arr_int*arr_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->intArr[ter->getArrayIndex()] * exp->floatArr[exp->getArrayIndex()]; //arr_int*arr_float
//           }
//         }
//         else if(ter->getVariableDataType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] * exp->intArr[exp->getArrayIndex()]; //arr_float*arr_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] * exp->floatArr[exp->getArrayIndex()]; //arr_float*arr_float
//           }
//         }

//       }

// 		else if(exp->getIDType()=="FUNCTION"){
// 			if(ter->getVariableDataType()=="INT"){
// 				if(exp->getFuncRetType()=="INT"){
// 					t->setVariableDataType("INT");
// 					t->intArr[0] = ter->intArr[ter->getArrayIndex()] * exp->intArr[0]; //arr_int*var_int
// 				}
// 				else if(exp->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->intArr[ter->getArrayIndex()] * exp->floatArr[0]; //arr_int*var_float
// 				}
// 			}
// 			else if(ter->getVariableDataType()=="FLOAT"){
// 				if(exp->getFuncRetType()=="INT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] * exp->intArr[0]; //arr_float*var_int
// 				}
// 				else if(exp->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] * exp->floatArr[0]; //arr_float*var_float
// 				}
// 			}
// 		}

//     }

// 		else if(ter->getIDType()=="FUNCTION"){
// 			if(exp->getIDType()=="VARIABLE"){
//         if(ter->getFuncRetType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             t->intArr[0] = ter->intArr[0] * exp->intArr[0]; //var_int*var_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->intArr[0] * exp->floatArr[0]; //var_int*var_float
//           }
//         }
//         else if(ter->getFuncRetType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[0] * exp->intArr[0]; //var_float*var_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[0] * exp->floatArr[0]; //var_float*var_float
//           }
//         }
//       }
//       else if(exp->getIDType()=="ARRAY"){
//         if(ter->getFuncRetType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             t->intArr[0] = ter->intArr[0] * exp->intArr[exp->getArrayIndex()]; //var_int*arr_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->intArr[0] * exp->floatArr[exp->getArrayIndex()]; //var_int*arr_float
//           }
//         }
//         else if(ter->getFuncRetType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[0] * exp->intArr[exp->getArrayIndex()]; //var_float*arr_int
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             t->floatArr[0] = ter->floatArr[0] * exp->floatArr[exp->getArrayIndex()]; //var_float*arr_float
//           }
//         }

//       }
// 		else if(exp->getIDType()=="FUNCTION"){
// 			if(ter->getFuncRetType()=="INT"){
// 				if(exp->getFuncRetType()=="INT"){
// 					t->setVariableDataType("INT");
// 					t->intArr[0] = ter->intArr[0] * exp->intArr[0]; //var_int*var_int
// 				}
// 				else if(exp->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->intArr[0] * exp->floatArr[0]; //var_int*var_float
// 				}
// 			}
// 			else if(ter->getFuncRetType()=="FLOAT"){
// 				if(exp->getFuncRetType()=="INT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->floatArr[0] * exp->intArr[0]; //var_float*var_int
// 				}
// 				else if(exp->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					t->floatArr[0] = ter->floatArr[0] * exp->floatArr[0]; //var_float*var_float
// 				}
// 			}
// 		}
// 		}

//   }

//   else if(mulop->getName()=="/"){
//     if(ter->getIDType()=="VARIABLE"){
//       if(exp->getIDType()=="VARIABLE"){
//         if(ter->getVariableDataType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             if(exp->intArr[0]!=0) t->intArr[0] = ter->intArr[0] / exp->intArr[0]; //var_int/var_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[0]!=0) t->floatArr[0] = ter->intArr[0] / exp->floatArr[0]; //var_int/var_float
// 						else print_divide_by_zero();
//           }
//         }
//         else if(ter->getVariableDataType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->intArr[0]!=0) t->floatArr[0] = ter->floatArr[0] / exp->intArr[0]; //var_float/var_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[0]!=0) t->floatArr[0] = ter->floatArr[0] / exp->floatArr[0]; //var_float/var_float
// 						else print_divide_by_zero();
//           }
//         }
//       }
//       else if(exp->getIDType()=="ARRAY"){
//         if(ter->getVariableDataType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             if(exp->intArr[exp->getArrayIndex()]!=0) t->intArr[0] = ter->intArr[0] / exp->intArr[exp->getArrayIndex()]; //var_int/arr_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->intArr[0] / exp->floatArr[exp->getArrayIndex()]; //var_int/arr_float
// 						else print_divide_by_zero();
//           }
//         }
//         else if(ter->getVariableDataType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->intArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->floatArr[0] / exp->intArr[exp->getArrayIndex()]; //var_float/arr_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->floatArr[0] / exp->floatArr[exp->getArrayIndex()]; //var_float/arr_float
// 						else print_divide_by_zero();
// 					}
//         }

//       }

// 			else if(exp->getIDType()=="FUNCTION"){
// 				if(ter->getVariableDataType()=="INT"){
// 					if(exp->getFuncRetType()=="INT"){
// 						t->setVariableDataType("INT");
// 						if(exp->intArr[0]!=0) t->intArr[0] = ter->intArr[0] / exp->intArr[0]; //var_int * var_int
// 						else print_divide_by_zero();
// 					}
// 					else if(exp->getFuncRetType()=="FLOAT"){
// 						t->setVariableDataType("FLOAT");
// 						if(exp->intArr[0]!=0) t->floatArr[0] = ter->intArr[0] / exp->floatArr[0]; //var_int/var_float
// 						else print_divide_by_zero();
// 					}
// 				}
// 				else if(ter->getVariableDataType()=="FLOAT"){
// 					if(exp->getFuncRetType()=="INT"){
// 						t->setVariableDataType("FLOAT");
// 						if(exp->intArr[0]!=0) t->floatArr[0] = ter->floatArr[0] / exp->intArr[0]; //var_float/var_int
// 						else print_divide_by_zero();
// 					}
// 					else if(exp->getFuncRetType()=="FLOAT"){
// 						t->setVariableDataType("FLOAT");
// 						if(exp->floatArr[0]!=0) t->floatArr[0] = ter->floatArr[0] / exp->floatArr[0]; //var_float/var_float
// 						else print_divide_by_zero();
// 					}
// 				}
// 			}

//     }
//     else if(ter->getIDType()=="ARRAY"){
//       if(exp->getIDType()=="VARIABLE"){
//         if(ter->getVariableDataType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             if(exp->intArr[0]) t->intArr[0] = ter->intArr[ter->getArrayIndex()] / exp->intArr[0]; //arr_int/var_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[0]) t->floatArr[0] = ter->intArr[ter->getArrayIndex()] / exp->floatArr[0]; //arr_int/var_float
// 						else print_divide_by_zero();
//           }
//         }
//         else if(ter->getVariableDataType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->intArr[0]) t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] / exp->intArr[0]; //arr_float/var_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[0]) t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] / exp->floatArr[0]; //arr_float/var_float
// 						else print_divide_by_zero();
//           }
//         }

//       }
//       else if(exp->getIDType()=="ARRAY"){
//         if(ter->getVariableDataType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             if(exp->intArr[exp->getArrayIndex()]!=0) t->intArr[0] = ter->intArr[ter->getArrayIndex()] / exp->intArr[exp->getArrayIndex()]; //arr_int/arr_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->intArr[ter->getArrayIndex()] / exp->floatArr[exp->getArrayIndex()]; //arr_int/arr_float
// 						else print_divide_by_zero();
//           }
//         }
//         else if(ter->getVariableDataType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->intArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] / exp->intArr[exp->getArrayIndex()]; //arr_float/arr_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] / exp->floatArr[exp->getArrayIndex()]; //arr_float/arr_float
// 						else print_divide_by_zero();
//           }
//         }

//       }

// 			else if(exp->getIDType()=="FUNCTION"){
// 				if(ter->getVariableDataType()=="INT"){
// 					if(exp->getFuncRetType()=="INT"){
// 						t->setVariableDataType("INT");
// 						if(exp->intArr[0]!=0) t->intArr[0] = ter->intArr[ter->getArrayIndex()] / exp->intArr[0]; //var_int * var_int
// 						else print_divide_by_zero();
// 					}
// 					else if(exp->getFuncRetType()=="FLOAT"){
// 						t->setVariableDataType("FLOAT");
// 						if(exp->intArr[0]!=0) t->floatArr[0] = ter->intArr[ter->getArrayIndex()] / exp->floatArr[0]; //var_int/var_float
// 						else print_divide_by_zero();
// 					}
// 				}
// 				else if(ter->getVariableDataType()=="FLOAT"){
// 					if(exp->getFuncRetType()=="INT"){
// 						t->setVariableDataType("FLOAT");
// 						if(exp->intArr[0]!=0) t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] / exp->intArr[0]; //var_float/var_int
// 						else print_divide_by_zero();
// 					}
// 					else if(exp->getFuncRetType()=="FLOAT"){
// 						t->setVariableDataType("FLOAT");
// 						if(exp->floatArr[0]!=0) t->floatArr[0] = ter->floatArr[ter->getArrayIndex()] / exp->floatArr[0]; //var_float/var_float
// 						else print_divide_by_zero();
// 					}
// 				}
// 			}

//     }

// 		else if(ter->getIDType()=="FUNCTION"){
// 			if(exp->getIDType()=="VARIABLE"){
//         if(ter->getFuncRetType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             if(exp->intArr[0]!=0) t->intArr[0] = ter->intArr[0] / exp->intArr[0]; //var_int/var_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[0]!=0) t->floatArr[0] = ter->intArr[0] / exp->floatArr[0]; //var_int/var_float
// 						else print_divide_by_zero();
//           }
//         }
//         else if(ter->getFuncRetType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->intArr[0]!=0) t->floatArr[0] = ter->floatArr[0] / exp->intArr[0]; //var_float/var_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[0]!=0) t->floatArr[0] = ter->floatArr[0] / exp->floatArr[0]; //var_float/var_float
// 						else print_divide_by_zero();
//           }
//         }
//       }
//       else if(exp->getIDType()=="ARRAY"){
//         if(ter->getFuncRetType()=="INT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("INT");
//             if(exp->intArr[exp->getArrayIndex()]!=0)t->intArr[0] = ter->intArr[0] / exp->intArr[exp->getArrayIndex()]; //var_int/arr_int
// 						else print_divide_by_zero();
//           }
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->intArr[0] / exp->floatArr[exp->getArrayIndex()]; //var_int/arr_float
// 						else print_divide_by_zero();
//           }
//         }
//         else if(ter->getFuncRetType()=="FLOAT"){
//           if(exp->getVariableDataType()=="INT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->intArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->floatArr[0] / exp->intArr[exp->getArrayIndex()]; //var_float/arr_int
// 						else print_divide_by_zero();
// 					}
//           else if(exp->getVariableDataType()=="FLOAT"){
//             t->setVariableDataType("FLOAT");
//             if(exp->floatArr[exp->getArrayIndex()]!=0) t->floatArr[0] = ter->floatArr[0] / exp->floatArr[exp->getArrayIndex()]; //var_float/arr_float
// 						else print_divide_by_zero();
//           }
//         }

//       }
// 		else if(exp->getIDType()=="FUNCTION"){
// 			if(ter->getFuncRetType()=="INT"){
// 				if(exp->getFuncRetType()=="INT"){
// 					t->setVariableDataType("INT");
// 					if(exp->intArr[0]!=0) t->intArr[0] = ter->intArr[0] / exp->intArr[0]; //var_int/var_int
// 					else print_divide_by_zero();
// 				}
// 				else if(exp->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					if(exp->floatArr[0]!=0) t->floatArr[0] = ter->intArr[0] / exp->floatArr[0]; //var_int/var_float
// 					else print_divide_by_zero();
// 				}
// 			}
// 			else if(ter->getFuncRetType()=="FLOAT"){
// 				if(exp->getFuncRetType()=="INT"){
// 					t->setVariableDataType("FLOAT");
// 					if(exp->intArr[0]!=0) t->floatArr[0] = ter->floatArr[0] / exp->intArr[0]; //var_float/var_int
// 					else print_divide_by_zero();
// 				}
// 				else if(exp->getFuncRetType()=="FLOAT"){
// 					t->setVariableDataType("FLOAT");
// 					if(exp->floatArr[0]!=0) t->floatArr[0] = ter->floatArr[0] / exp->floatArr[0]; //var_float/var_float
// 					else print_divide_by_zero();
// 				}
// 			}
// 		}
// 		}

//   }

//   else if(mulop->getName()=="%"){

//     if((ter->getVariableDataType()=="INT" || ter->getFuncRetType()=="INT") && (exp->getVariableDataType()=="INT" || exp->getFuncRetType()=="INT")){

//       t->setVariableDataType("INT");

//       if(ter->getIDType()=="ARRAY"){
//         if(exp->getIDType()=="ARRAY"){
// 					if(exp->intArr[exp->getArrayIndex()]!=0){
//           	t->intArr[0] = ter->intArr[ter->getArrayIndex()] % exp->intArr[exp->getArrayIndex()];
// 					}
// 					else{
// 						errorfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 						logfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 						syntax_error++;
// 					}
//         }
//         else if(exp->getIDType()=="VARIABLE"){
// 					if(exp->intArr[0]!=0)
//           	t->intArr[0] = ter->intArr[ter->getArrayIndex()] % exp->intArr[0];
// 					else{
// 							errorfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 							logfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 							syntax_error++;
// 						}
//         }
// 				else if(exp->getIDType()=="FUNCTION"){
// 					if(exp->intArr[0]!=0)
//           	t->intArr[0] = ter->intArr[ter->getArrayIndex()] % exp->intArr[0];
// 					else{
// 							errorfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 							logfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 							syntax_error++;
// 						}
//         }
//       }
//       else if(ter->getIDType()=="VARIABLE"){
//         if(exp->getIDType()=="ARRAY"){
// 					if(exp->intArr[exp->getArrayIndex()]!=0)
//           	t->intArr[0] = ter->intArr[0] % exp->intArr[exp->getArrayIndex()];
// 					else{
// 								errorfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 								logfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 								syntax_error++;
// 							}
//         }
//         else if(exp->getIDType()=="VARIABLE"){
// 					if(exp->intArr[0]!=0)
//           	t->intArr[0] = ter->intArr[0] % exp->intArr[0];
// 					else{
// 							errorfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 							logfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 							syntax_error++;
// 						}
//         }

// 				else if(exp->getIDType()=="FUNCTION"){
// 					if(exp->intArr[0]!=0)
// 						t->intArr[0] = ter->intArr[0] % exp->intArr[0];
// 					else{
// 							errorfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 							logfile<<"Error at line "<<line_count<<": Modulus by Zero"<<endl<<endl;
// 							syntax_error++;
// 						}
// 				}
//       }
//     }
//     else{
//       errorfile<<"Error at line "<<line_count<<": Non-Integer operand on modulus operator"<<endl<<endl;
// 			logfile<<"Error at line "<<line_count<<": Non-Integer operand on modulus operator"<<endl<<endl;
//       syntax_error++;
//     }
//   }

//   return t;
// }
