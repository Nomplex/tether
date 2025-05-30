#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>

#define DB "tether.db"

bool tether_init(sqlite3 *db);
void tether_add(char *path, char *nickname, sqlite3 *db);
void tether_remove(char *nickname, sqlite3 *db);

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;

    sqlite3 *db;

    if (sqlite3_open(DB, &db) != SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr, "Could not open database\n");
        exit(EXIT_FAILURE);
    }

    if (!tether_init(db)) {
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "add") == 0)
        tether_add(argv[2], argv[3], db);
    else if (strcmp(argv[1], "rm") == 0)
        tether_remove(argv[2], db);

    return 0;
}

void tether_add(char *path, char *nickname, sqlite3 *db)
{
    if (!path || !nickname)
        exit(EXIT_FAILURE);

    char *error = NULL;
    char addSQL[1024];
    char dir[1024];

    strcmp(path, ".") == 0 ? getcwd(dir, 1024) : strcpy(dir, path);

    if (dir == NULL) {
        fprintf(stderr, "Couldn't get the current working directory");
        exit(EXIT_FAILURE);
    }

    snprintf(addSQL, sizeof(addSQL), "INSERT INTO tethers (path, nickname) VALUES ('%s', '%s');", dir, nickname);
    if (sqlite3_exec(db, addSQL, NULL, NULL, &error) != SQLITE_OK) {
        fprintf(stderr, "Something went wrong with adding into the database\n");
        sqlite3_free(error);
    }
}

void tether_remove(char *nickname, sqlite3 *db)
{
    if (!nickname)
        exit(EXIT_FAILURE);

    char *error = NULL;
    char rmSQL[1024];

    snprintf(rmSQL, sizeof(rmSQL), "DELETE FROM tethers WHERE nickname = '%s';", nickname);
    if (sqlite3_exec(db, rmSQL, NULL, NULL, &error) != SQLITE_OK) {
        fprintf(stderr, "Something went wrong with removing an item from the database\n");
        sqlite3_free(error);
    }
}

bool tether_init(sqlite3 *db)
{
    char *error = NULL;
    char *initSQL = "CREATE TABLE IF NOT EXISTS tethers("
                    "id INTEGER PRIMARY KEY,"
                    "path TEXT NOT NULL,"
                    "nickname TEXT NOT NULL UNIQUE);";

    if (sqlite3_exec(db, initSQL, NULL, NULL, &error) != SQLITE_OK) {
        fprintf(stderr, "Something went wrong with initializing the database\n");
        sqlite3_free(error);
        return false;
    }

    return true;
}
