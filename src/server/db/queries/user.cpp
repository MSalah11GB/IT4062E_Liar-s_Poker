#include <bits/stdc++.h>
#include <cstdio>
#include <sqlite3.h>
#include <string>
#include "../createdb.h"
#include "user.h"
#include "../model/user.h"
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
        int id = sqlite3_column_int(stmt, 0);
        cout << "User id is: " << id << endl;
        sqlite3_finalize(stmt);
        return id;
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

vector<User> getFriendsOfUser(int userId)
{
    vector<User> friends = {};
    sqlite3_stmt *stmt;
    const char *sql = R"(SELECT id, username, online_status from users where id in (
                          SELECT user2_id
                             FROM friends
                      WHERE user1_id = ? AND status = 'ACC'

                      UNION

                      SELECT user1_id
                      FROM friends
                      WHERE user2_id = ? AND status = 'ACC')
                      ORDER BY username ASC;)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        // TODO: implement throw Error
        return friends;
    }
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, userId);
    while (true)
    {
        int rc = sqlite3_step(stmt);

        if (rc == SQLITE_ROW)
        {
            int friend_id = sqlite3_column_int(stmt, 0);
            string username(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            int online_status = sqlite3_column_int(stmt, 2);
            int elo = sqlite3_column_int(stmt, 3);
            User userFriend(friend_id, username, online_status, elo);
            friends.push_back(userFriend);
        }
        else if (rc == SQLITE_DONE)
        {
            break;
        }
        else
        {
            cout << "Error retrieving user friends: " << sqlite3_errmsg(db) << endl;
            break;
        }
    }

    sqlite3_finalize(stmt);
    cout << "User has friends: " << endl;
    for (int i = 0; i < static_cast<int>(friends.size()); i++)
    {
        cout << "Id: " << friends[i].id << ", name: " << friends[i].username << ", online status: " << friends[i].online_status << endl;
    }
    return friends;
}

vector<User> getFriendRequestsOfUser(int userId)
{
    vector<User> friendRequests = {};
    sqlite3_stmt *stmt;
    const char *sql = R"(
    SELECT id, username, online_status 
    FROM users 
    WHERE id IN (
        SELECT invite_id 
        FROM friends 
        WHERE invite_id != ?
          AND (user1_id = ? OR user2_id = ?)
          AND status = 'PEND'
    ) ORDER BY username ASC;)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        // TODO: implement throw error
        return friendRequests;
    }
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_int(stmt, 3, userId);
    while (true)
    {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            int friend_id = sqlite3_column_int(stmt, 0);
            string username(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            int online_status = sqlite3_column_int(stmt, 2);
            int elo = sqlite3_column_int(stmt, 3);
            User userFriend(friend_id, username, online_status, elo);
            friendRequests.push_back(userFriend);
        }
        else if (rc == SQLITE_DONE)
        {
            break;
        }
        else
        {
            cout << "Error retrieving user friend request: " << sqlite3_errmsg(db) << endl;
            break;
        }
    }
    sqlite3_finalize(stmt);
    cout << "User has friends invites: " << endl;
    for (int i = 0; i < static_cast<int>(friendRequests.size()); i++)
    {
        cout << "Id: " << friendRequests[i].id << ", name: " << friendRequests[i].username << ", online status: " << friendRequests[i].online_status << endl;
    }
    return friendRequests;
}

User getUser(int userId)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, username, online_status, elo FROM users WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        User user;
        return user;
    }

    sqlite3_bind_int(stmt, 1, userId);

    int rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        cout << "User exists!" << endl;
        int id = sqlite3_column_int(stmt, 0);
        string username(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
        int online_status = sqlite3_column_int(stmt, 2);
        int elo = sqlite3_column_int(stmt, 3);
        User user(id, username, online_status, elo);
        return user;
    }
    else if (rc == SQLITE_DONE)
    {
        cout << "No user found" << endl;
        sqlite3_finalize(stmt);
        User user;
        return user;
    }
    else
    {
        cout << "Error during step: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        User user;
        return user;
    }
}