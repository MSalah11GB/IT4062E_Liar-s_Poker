#ifndef FRIENDS_QUERIES_H
#define FRIENDS_QUERIES_H

#include <bits/stdc++.h>
#include <sqlite3.h>
#include "../createdb.h"
using namespace std;

void confirmFriendRequest(int user1_id, int user2_id, int invite_id);
void removeFriend(int user1_id, int user2_id);

#endif
