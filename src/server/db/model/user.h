#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <string>
using namespace std;

class User
{
public:
    int id;
    string username;

    User(int id, string username);
};

#endif