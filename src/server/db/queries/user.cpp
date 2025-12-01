#include <bits/stdc++.h>
#include <cstdio>
#include <sqlite3.h>
#include <string>
#include "../createdb.h"
#include "user.h"
using namespace std;

void insertUser(string username, string password)
{
    cout << "Connection state: " << (db == nullptr) << endl;
    char cmd[100 + username.length() + password.length()];
    sprintf(cmd, "INSERT INTO users (username, password) VALUES ( '%s', '%s')", username.c_str(), password.c_str());
    char *zErrMsg = NULL;
    cout << cmd << endl;
    int rc = sqlite3_exec(db, cmd, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        cout << "Error insert user: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Insert user successfully" << endl;
    }
}
