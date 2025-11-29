#include <bits/stdc++.h>
#include "../../server/db/createdb.cpp"
#include "../constant/process.h"

using namespace std;

void signalHandler(int signum)
{
    cout << "Receive signal " << SIGNALS.at(signum) << ". Perform closing program!" << endl;
    sqlite3_close(db);
    cout << "Database close succesfully" << endl;
    return;
}