#include <bits/stdc++.h>
using namespace std;

// line and error count
extern int line_count;
extern int lexical_error_count;
int syntax_error_count = 0;

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
// set<int> error_lines;

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
  fprintf(logout, "Line %d: %s : %s\n\n%s\n\n", line_count, str1.c_str(), str2.c_str(), str3.c_str());
}

void printError(string str1)
{
  // printf("Error at line %d: %s\n", line_count, str1.c_str());
  fprintf(errorout, "Error at line %d: %s\n\n", line_count, str1.c_str());
  syntax_error_count++;
  // error_lines.insert(line_count);
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
  return symbolTable->lookup(symbolInfo->getName());
}

symbol_info *findVariable(symbol_info *symbolInfo)
{
  symbol_info *s = findSymbol(symbolInfo);
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
  if (type == _void)
  {
    printError("Variable type cannot be void");
  }
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
  if (current_function == nullptr)
    return;
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

// void setFunctionRelatedValues(symbol_info *symbolInfo, string return_type, bool is_defined)
// {
//   // symbolTable->printAllScopeTable();
//   for (int i = 0; i < parameters.size(); i++)
//   {
//     // auto s = symbolTable->lookup(parameters[i]->getName());
//     // cout << "checking " << parameters[i]->getName() << " ";
//     // printCompatibilityRelatedThings(s);
//     // cout << "here" << endl;
//     // printCompatibilityRelatedThings(parameters[i]);

//     // setCompatibleRelatedThings(s, parameters[i]);
//     // symbolInfo->sequence_of_parameters.push_back(s);
//   }
// }

void checkFunctionParameters(string return_type, symbol_info *symbolInfo, bool is_defined)
{
  symbol_info *s = symbolTable->lookup(symbolInfo->getName());
  set<string> params;
  for (int i = 0; i < parameters.size(); i++)
  {
    if (params.count(parameters[i]->getName()))
      printError("Multiple declaration of " + parameters[i]->getName() + " in parameter");
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

void checkParametersCompatibilityWithDeclaration(symbol_info *symbolInfo)
{
  if (parameters.size() != symbolInfo->sequence_of_parameters.size())
    printError("Total number of arguments mismatch with declaration in function " + symbolInfo->getName());
  for (int i = 0; i < parameters.size(); i++)
  {
    if (parameters[i]->variable_type != symbolInfo->sequence_of_parameters[i]->variable_type)
      printError("Type mismatch with declaration in function " + symbolInfo->getName());
  }
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
    printError("Multiple declaration of " + s->getName());
  }
  else if (!s->is_defined && is_defined)
  {
    if (s->return_type != return_type)
      printError("Return type mismatch with function declaration in function " + s->getName());
    checkParametersCompatibilityWithDeclaration(s);
    checkFunctionParameters(return_type, symbolInfo, is_defined);
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
      printError("array index out of bound");
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
      printError("Undeclared array " + var_name);
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
  // if(float_value == 0)
  string name = to_string(float_value);
  name.erase(name.find_last_not_of('0') + 1, string::npos);
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
    if (right->getName() == "0")
    {
      printError("Modulus by Zero");
      return nullptr;
    }
  }
  if (optr == "/" && right->getName() == "0" && right->getName() == "0.")
  {
    printError("Division by Zero");
    return nullptr;
  }

  symbol_info *s = new symbol_info(left->getName() + optr + right->getName(), "intermediate");
  s->id_type = VARIABLE;
  s->variable_type = left->variable_type == fraction || right->variable_type == fraction ? fraction : integer;
  // cout << line_count << endl;

  return s;
}

symbol_info *checkAdditionCompatibility(symbol_info *left, string optr, symbol_info *right)
{
  // cout << line_count << endl;
  // cout << "1++" << endl;
  // printCompatibilityRelatedThings(left);
  // cout << "2--" << endl;
  // printCompatibilityRelatedThings(right);
  // cout << "3__" << endl;
  if (checkNullFunctionArrayVoid(left, right, optr))
    return nullptr;

  symbol_info *s = new symbol_info(left->getName() + optr + right->getName(), "intermediate");
  s->id_type = VARIABLE;
  s->variable_type = left->variable_type == fraction || right->variable_type == fraction ? fraction : integer;
  return s;
}

symbol_info *checkRELOPCompetibility(symbol_info *left, string optr, symbol_info *right)
{
  if (checkNullFunctionArrayVoid(left, right, optr))
    return nullptr;

  symbol_info *s = new symbol_info(left->getName() + optr + right->getName(), "intermediate");
  s->id_type = VARIABLE;
  s->variable_type = integer;
  return s;
}

symbol_info *checkLogicCompetibility(symbol_info *left, string optr, symbol_info *right)
{
  if (checkNullFunctionArrayVoid(left, right, optr))
    return nullptr;

  symbol_info *s = new symbol_info(left->getName() + optr + right->getName(), "intermediate");
  s->id_type = VARIABLE;
  s->variable_type = integer;
  return s;
}

symbol_info *checkFunctionArguments(symbol_info *si)
{
  symbol_info *symbolInfo = findSymbol(si);
  if (symbolInfo == nullptr)
  {
    printError("Undeclared function " + si->getName());
    args.clear();
    return nullptr;
  }
  if (symbolInfo->id_type != FUNCTION)
  {
    printError("is not function");
    args.clear();
    return nullptr;
  }
  // cout << line_count << "  aaaaaaa" << endl;
  // cout << args.size() << endl;
  if (args.size() != symbolInfo->sequence_of_parameters.size())
  {
    printError("Total number of arguments mismatch in function " + symbolInfo->getName());
    args.clear();
    return nullptr;
  }
  string argus = symbolInfo->getName() + "(";
  for (int i = 0; i < args.size(); i++)
  {
    if (args[i]->id_type == ARRAY)
    {
      printError("Type mismatch, " + args[i]->getName() + " is an array");
      args.clear();
      return nullptr;
    }
    else if (args[i]->id_type == FUNCTION)
    {
      printError("Type mismatch, " + args[i]->getName() + " is a function");
      args.clear();
      return nullptr;
    }

    if (args[i]->variable_type != symbolInfo->sequence_of_parameters[i]->variable_type)
    {
      printError(to_string(i + 1) + "th argument mismatch in function " + symbolInfo->getName());
      args.clear();
      return nullptr;
    }
    argus += args[i]->getName() + (i == args.size() - 1 ? "" : ",");
  }
  // cout << argus << endl;
  argus += ")";
  symbol_info *s = new symbol_info(argus, intermediate);
  s->id_type = VARIABLE;
  s->variable_type = symbolInfo->return_type;
  args.clear();
  return s;
}

void checkFuncReturnCompatibility(symbol_info *symbolInfo)
{
  if (current_function != nullptr && symbolInfo->variable_type != current_function->return_type)
  {
    printError("return type does not match");
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

void printLineAndErrors()
{
  symbolTable->printAllScopeTable(logout);
  fprintf(logout, "\nTotal lines: %d\nTotal errors: %d\n\n", line_count, syntax_error_count + lexical_error_count);
}
