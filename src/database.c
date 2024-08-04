#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/database.h"
#include <stdio.h>

int openDatabase(const char *filename, sqlite3 **db)  {
    int rc =  sqlite3_open(filename, db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }
    return 0;
}

int createTable(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS Satellites ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT,"
                      "timestamp INTEGER,"
                      "positionX REAL, positionY REAL, positionZ REAL,"
                      "velocityX REAL, velocityY REAL, velocityZ REAL,"
                      "accelerationX REAL, accelerationY REAL, AccelerationZ REAL,"
                      "mass REAL);";
    
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }
    return 0;
}

int insertSatelliteData(sqlite3 *db, const char *name, double position[], double velocity[], double acceleration[], double mass, int timestamp){
    const char *sql = "INSERT INTO Satellites (name, timestamp, positionX, positionY, positionZ, velocityX, velocityY, velocityZ, accelerationX, accelerationY, accelerationZ, mass) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Faild to prepare statment: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, timestamp);
    sqlite3_bind_double(stmt, 3, position[0]);
    sqlite3_bind_double(stmt, 4, position[1]);
    sqlite3_bind_double(stmt, 5, position[2]);
    sqlite3_bind_double(stmt, 6, velocity[0]);
    sqlite3_bind_double(stmt, 7, velocity[1]);
    sqlite3_bind_double(stmt, 8, velocity[2]);
    sqlite3_bind_double(stmt, 9, acceleration[0]);
    sqlite3_bind_double(stmt, 10, acceleration[1]);
    sqlite3_bind_double(stmt, 11, acceleration[2]);
    sqlite3_bind_double(stmt, 12, mass);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : rc;
}

int closeDatabase(sqlite3 *db) {
    return sqlite3_close(db);
}