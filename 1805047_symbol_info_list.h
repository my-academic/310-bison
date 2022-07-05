

#include "1805047_symbol_info.h"
using namespace std;

#ifndef SYMBOL_INFO_LIST_H
#define SYMBOL_INFO_LIST_H

class symbol_info_list
{
private:
    symbol_info *tail;
    symbol_info *head;

    int position(symbol_info *x)
    {
        int c = 0;
        symbol_info *temp = head;
        while (temp != tail)
        {
            temp = temp->getNext();
            c++;
        }
        return c;
    }

public:
    symbol_info_list()
    {
        head = nullptr;
        tail = nullptr;
    }
    ~symbol_info_list()
    {
        // cout << "calling destructor of linked list" << endl;
        while(tail != head) {
            symbol_info* temp = tail->getPrev();
            delete tail;
            tail = temp;
        }
        delete tail;
        // delete head;
        // tail = nullptr;
        // delete tail;
        // cout << endl;
    }
    int insert(string k, string v)
    {
        if (isEmpty())
        {
            head = new symbol_info(k, v);
            tail = head;
            return position(tail);
        }
        symbol_info *y = new symbol_info(k, v);
        y->setPrev(tail);
        tail->setNext(y);
        tail = tail->getNext();
        return position(tail);
    }
    bool dlt(symbol_info *x)
    {
        if (x == head)
        {
            // cout << "head" << endl;
            head = x->getNext();
            if (head == nullptr)
            {
                // cout << "head = nullptr" << endl;
                tail = nullptr;
            }
            else
                head->setPrev(nullptr);
        }
        else if (x == tail)
        {
            tail = x->getPrev();
            tail->setNext(nullptr);
        }
        else
        {
            x->getPrev()->setNext(x->getNext());
            x->getNext()->setPrev(x->getPrev());
        }
        delete x;
        return true;
    }
    bool isEmpty()
    {
        return head == nullptr;
    }

    symbol_info *getTail() const { return tail; }

    symbol_info *getHead() const
    {
        return head;
    }

    void print()
    {
        symbol_info *temp = head;
        while (temp != nullptr)
        {
            temp->print();
            temp = temp->getNext();
            // cout << " ";
        }
        // temp = nullptr;
        // delete temp;
        return;
    }

    pair<int, symbol_info *> search(string key)
    {
        symbol_info *x = head;
        int pos = 0;
        while (x != nullptr)
        {
            if (x->getName() == key)
                return {pos, x};
            pos++;
            x = x->getNext();
        }
        return {-1, nullptr};
    }
};

#endif