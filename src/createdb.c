#include <stdio.h>
#include <sqlite3.h>

int main(int argc, char **argv)
{
    sqlite3 *db;
    char *err_msg = NULL;

    // Open database (creates file if it doesn't exist)
    int rc = sqlite3_open("mydb.db", &db);

    if (rc != SQLITE_OK)
    {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // SQL to create table
    const char *sql =
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL,"
        "created_at datetime DEFAULT CURRENT_TIMESTAMP"
        ");"

        "CREATE TABLE IF NOT EXISTS games ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "start_time datetime not null,"
        "end_time datetime not null,"
        "winner_id INTEGER not null,"
        "FOREIGN KEY (winner_id) REFERENCES users(id),"
        "check (end_time >= start_time)"
        ");"

        "create table if not exists rounds ("
        "id integer primary key autoincrement,"
        "game_id integer not null,"
        "number integer not null,"
        "foreign key (game_id) references games(id)"
        ");"

        "create table user_game ("
        "user_id integer not null,"
        "game_id integer not null,"
        "elimated_round integer not null,"
        "eliminated_order integer not null,"
        "primary key (user_id, game_id),"
        "foreign key (user_id) references users(id),"
        "foreign key (game_id) references games(id)"
        ");";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    printf("Database and table created successfully!\n");

    // Close database
    sqlite3_close(db);

    return 0;
}
