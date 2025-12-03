#include <bits/stdc++.h>
#include <sqlite3.h>
#include "../createdb.h"
#include "friends.h"

void confirmFriendRequest(int user1_id, int user2_id, int invite_id)
{
    if (user1_id > user2_id)
    {
        int temp = user1_id;
        user1_id = user2_id;
        user2_id = temp;
    }
    sqlite3_stmt *stmt;
    const char *sql = R"(
    UPDATE friends
    SET status = 'ACC'
    WHERE invite_id = ? 
    AND user1_id = ?
    AND user2_id = ? 
    AND status = 'PEND' 
    ;)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        // TODO: implement throw error
        return;
    }

    sqlite3_bind_int(stmt, 1, invite_id);
    sqlite3_bind_int(stmt, 2, user1_id);
    sqlite3_bind_int(stmt, 3, user2_id);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        int changes = sqlite3_changes(db);
        if (changes == 0)
        {
            cout << "friend request have already been accepted or doesn't exist" << endl;
        }
        else
        {
            cout << "friend request accepted" << endl;
        }
    }
    else if (rc != SQLITE_DONE)
    {
        cout << "Error during step: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_finalize(stmt);
    return;
}
void removeFriend(int user1_id, int user2_id)
{
    if (user1_id > user2_id)
    {
        int temp = user1_id;
        user1_id = user2_id;
        user2_id = temp;
    }
    sqlite3_stmt *stmt;
    const char *sql = R"(
    UPDATE friends 
    SET status = 'DECL'
    WHERE user1_id = ?
    AND user2_id = ?
    AND status = 'ACC'
    ;)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cout << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        // TODO: implement throw error
        return;
    }

    sqlite3_bind_int(stmt, 1, user1_id);
    sqlite3_bind_int(stmt, 2, user2_id);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE)
    {
        int changes = sqlite3_changes(db);
        if (changes == 0)
        {
            cout << "Error, remove friend connection not existed" << endl;
        }
        else
        {
            cout << "Remove friend successfully" << endl;
        }
    }
    else if (rc != SQLITE_DONE)
    {
        cout << "Error during step: " << sqlite3_errmsg(db) << endl;
        return;
    }
    sqlite3_finalize(stmt);
    return;
}