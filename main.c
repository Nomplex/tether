#include <stdio.h>
#include <sqlite3.h>

int main(int argc, char *argv[])
{
    sqlite3 *db;

    if (sqlite3_open("test.db", &db) != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    sqlite3_close(db);

    return 0;
}
