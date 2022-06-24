#include<bits/stdc++.h>
using namespace std;


FILE *logout;
FILE *tokenout;

class symbol_info
{
private:
    string name;
    string type;
    symbol_info *next;
    
    bool khali;
    symbol_info *prev;
public:
    symbol_info(string n, string t): name(n), type(t){
        next = nullptr;
    }

    ~symbol_info();

    string getName() const { return name; }
    void setName(const string &name_) { name = name_; }

    string getType() const { return type; }
    void setType(const string &type_) { type = type_; }

    symbol_info *getNext() const { return next; }
    void setNext(symbol_info *next_) { next = next_; }

    symbol_info *getPrev() const { return prev; }
    void setPrev(symbol_info *prev_) { prev = prev_; }

    bool getKhali() const { return khali; }
    void setKhali(bool khali_) { khali = khali_; }

    void print() {
        fprintf(logout, " < %s : %s> ", name.c_str(), type.c_str());
        // cout << "< " << name << " : " << type << "> ";
    }
    
};

symbol_info::~symbol_info()
{
    // cout << "calling destructor of symbol info" << endl;
    next = nullptr;
    // free(next);
    // delete next;
}
