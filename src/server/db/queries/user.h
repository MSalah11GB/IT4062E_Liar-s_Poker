#ifndef USER_QUERIES_H
#define USER_QUERIES_H

#include <cstdio>
#include <sqlite3.h>
#include <string>
using namespace std;

void insertUser(string username, string password, string confirmPassword);
int verifyUser(string username, string password);
#endif
