#include <string>
#include "user.h"
using namespace std;

User::User(int id, string username, int online_status)
{
    this->id = id;
    this->username = username;
    this->online_status = online_status;
}