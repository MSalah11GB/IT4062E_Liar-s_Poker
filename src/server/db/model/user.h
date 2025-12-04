#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <string>
using namespace std;

class User
{
public:
    int id;
    string username;
    int online_status;
    int elo;

    User();
    User(int id, string username, int online_status, int elo);
};

#endif