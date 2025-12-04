#include <string>
#include "user.h"
using namespace std;

User::User()
{
    this->id = 0;
}
User::User(int id, string username, int online_status, int elo)
{
    this->id = id;
    this->username = username;
    this->online_status = online_status;
    this->elo = elo;
}
