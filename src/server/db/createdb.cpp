#include <iostream>
#include <sqlite3.h>
#include <string>

sqlite3 *db;

void createdb()
{
    db = nullptr;
    char *err_msg = nullptr;

    // Open database (creates file if it doesn't exist)
    int rc = sqlite3_open("appdb.db", &db);

    if (rc != SQLITE_OK)
    {
        std::cout << "Error connecting to database" << std::endl;
        sqlite3_close(db);
        return;
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
        " rounds INTEGER NOT NULL,"
        " CHECK (end_time >= start_time)"
        ");"

        "CREATE TABLE IF NOT EXISTS friends ("
        " user1_id INTEGER,"
        " user2_id INTEGER,"
        " invite_id INTEGER NOT NULL UNIQUE,"
        " status TEXT NOT NULL,"
        " PRIMARY KEY (user1_id, user2_id),"
        " FOREIGN KEY (user1_id) REFERENCES users(id),"
        " FOREIGN KEY (user2_id) REFERENCES users(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS users_games ("
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
        return;
    }
    std::cout << "new" << std::endl;
    std::cout << "Database and tables created successfully!" << std::endl;
    std::cout << "After connecting, connection state now is: " << (db == nullptr) << std::endl;
}