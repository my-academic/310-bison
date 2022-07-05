
#include<bits/stdc++.h>
using namespace std;

class additional_symbol_info{
public:
    string symbol_id_type;
    //function
    string number_of_parameters;
    vector<string> sequence_of_parameters;
    string return_type;

    // array
    int size_of_array;
    int current_index;

    // variable
    string variable_type;
    int int_value;
    double double_value;
    char char_value;
    
    bool isFunction(){
        return symbol_id_type == "FUNCTION";
    }
    bool isArray(){
        return symbol_id_type == "ARRAY";
    }
    bool isVariable(){
        return symbol_id_type == "VARIABLE";
    }
    
};

