#ifndef USER_QUERIES_H
#define USER_QUERIES_H

#include <cstdio>
#include <sqlite3.h>
#include <string>
#include "../model/user.h"
using namespace std;

void insertUser(string username, string password, string confirmPassword);
int verifyUser(string username, string password);
vector<User> getFriendsOfUser(int id);
#endif
