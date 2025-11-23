#include <iostream>
#include <sqlite3.h>
#include <string>

int main()
{
    sqlite3 *db = nullptr;
    char *err_msg = nullptr;

    // Open database (creates file if it doesn't exist)
    int rc = sqlite3_open("mydb.db", &db);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    // SQL schema
    const std::string sql =
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS users ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " username TEXT NOT NULL UNIQUE,"
        " password TEXT NOT NULL,"
        " created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");"

        "CREATE TABLE IF NOT EXISTS games ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " start_time DATETIME NOT NULL,"
        " end_time DATETIME NOT NULL,"
        " winner_id INTEGER NOT NULL,"
        " FOREIGN KEY (winner_id) REFERENCES users(id),"
        " CHECK (end_time >= start_time)"
        ");"

        "CREATE TABLE IF NOT EXISTS rounds ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " game_id INTEGER NOT NULL,"
        " number INTEGER NOT NULL,"
        " FOREIGN KEY (game_id) REFERENCES games(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS user_game ("
        " user_id INTEGER NOT NULL,"
        " game_id INTEGER NOT NULL,"
        " elimated_round INTEGER NOT NULL,"
        " eliminated_order INTEGER NOT NULL,"
        " PRIMARY KEY (user_id, game_id),"
        " FOREIGN KEY (user_id) REFERENCES users(id),"
        " FOREIGN KEY (game_id) REFERENCES games(id)"
        ");";

    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    std::cout << "Database and tables created successfully!" << std::endl;

    sqlite3_close(db);
    return 0;
}
