#include "1805047_scope_table.h"
using namespace std;

class symbol_table
{
private:
    scope_table *scopeTable;
    int b_size;

public:
    symbol_table(int b_size)
    {
        scopeTable = new scope_table(b_size, nullptr);
    }
    ~symbol_table()
    {
        // // cout << "calling the destructor of symbol table" << endl;
        delete scopeTable;
    }
    void enterScope()
    {
        scopeTable->setCount(scopeTable->getCount() + 1);
        scope_table *newScopeTable = new scope_table(scopeTable->getBucketSize(), scopeTable);
        scopeTable = newScopeTable;
    }
    void exitScope()
    {
        // delete operation

        if (scopeTable == nullptr)
        {
            // cout << "no scope found" << endl;
            return;
        }
        scope_table *temp = scopeTable;
        scopeTable = scopeTable->getParentScope();
        // // cout << "ScopeTable with id " << temp->getId() << " removed" << endl;
        temp->setFlagForDeletingParentScope(false);
        delete temp;
    }

    bool insert(string name, string type)
    {
        if (scopeTable == nullptr)
        {
            scopeTable = new scope_table(b_size, nullptr);
            // return false;
        }
        return scopeTable->insert(name, type);
    }
    bool remove(string name)
    {
        return scopeTable->dlt(name);
    }
    symbol_info *lookup(string name)
    {
        scope_table *temp = this->scopeTable;
        while (temp != nullptr)
        {
            pair<int, symbol_info *> symbolInfoAndPosition = temp->lookup(name, false);
            if (symbolInfoAndPosition.second == nullptr)
            {
                temp = temp->getParentScope();
            }
            else
            {
                // // cout << "Found in ScopeTable# " + temp->getId() + " at position " + to_string(scopeTable->call_hash(name)) + ", " + to_string(symbolInfoAndPosition.first) << endl;
                return symbolInfoAndPosition.second;
            }
        }
        // // cout << "Not found" << endl;
        return nullptr;
    }

    symbol_info* lookupCurrentScope(string name) {
        scope_table *temp = this->scopeTable;
        do
        {
            pair<int, symbol_info *> symbolInfoAndPosition = temp->lookup(name, false);
            if (symbolInfoAndPosition.second == nullptr)
            {
                temp = temp->getParentScope();
            }
            else
            {
                // // cout << "Found in ScopeTable# " + temp->getId() + " at position " + to_string(scopeTable->call_hash(name)) + ", " + to_string(symbolInfoAndPosition.first) << endl;
                return symbolInfoAndPosition.second;
            }
        } while (temp != nullptr && false);
        // // cout << "Not found" << endl;
        return nullptr;
    } 

    void printCurrentScopeTable(FILE* logout)
    {
        this->scopeTable->print(logout);
    }

    void printAllScopeTable(FILE* logout)
    {
        scope_table *temp = this->scopeTable;
        while (temp != nullptr)
        {
            temp->print(logout);
            temp = temp->getParentScope();
        }
    }
    void printAllScopeTable()
    {
        scope_table *temp = this->scopeTable;
        while (temp != nullptr)
        {
            temp->print();
            temp = temp->getParentScope();
        }
    }
};
