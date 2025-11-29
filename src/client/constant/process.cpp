#include <string>
#include <unordered_map>
#include "process.h"
using namespace std;

const unordered_map<int, string> SIGNALS = {
    {-3, "Error performing SQL queries"},
    {-2, "Error connecting to database"},
    {-1, "Closing application"},
    {2, "SIGINT"},
    {15, "SIGTERM"},
    {18, "SIGTSTP"},
    {20, "SIGTSTP"},
    {24, "SIGTSTP"},
};