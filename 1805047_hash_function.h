#include<bits/stdc++.h>
using namespace std;

unsigned int sdbmhash(string key)
{
    unsigned int hash = 0;
    for (auto c : key)
        hash = c + (hash << 6) + (hash << 16) - hash;
        
    return hash;
}

// static unsigned long
// sdbm(str)
// unsigned char *str;
// {
//     unsigned long hash = 0;
//     int c;

//     while (c = *str++)
//         hash = c + (hash << 6) + (hash << 16) - hash;

//     return hash;
// }