#include <bits/stdc++.h>
#include <cstdio>
#include <sqlite3.h>
#include <string>
#include "../createdb.h"
#include "user.h"
using namespace std;

void insertUser(string username, string password, string confirmPassword)
{
    cout << "Connection state: " << (db == nullptr) << endl;
    if (password.compare(confirmPassword) != 0)
    {
        cout << "Different between password" << endl;
        return;
    }
    char cmd[100 + username.length() + password.length()];
    sprintf(cmd, "INSERT INTO users (username, password, online_status) VALUES ( '%s', '%s', 0)", username.c_str(), password.c_str());
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

int verifyUser(string username, string password)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM users WHERE username = ? AND password = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        cout << "User exists!" << endl;
        sqlite3_finalize(stmt);
        return 1;
    }
    else if (rc == SQLITE_DONE)
    {
        cout << "No user found" << endl;
        sqlite3_finalize(stmt);
        return 0;
    }
    else
    {
        cout << "Error during step: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return -1;
    }
}
