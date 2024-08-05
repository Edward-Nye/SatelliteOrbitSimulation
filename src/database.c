#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/database.h"
#include <stdio.h>
#include <sqlite3.h>

int openDatabase(const char *filename, sqlite3 **db)  {
    int rc =  sqlite3_open(filename, db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }
    return 0;
}

int createTables(sqlite3 *db, const char *satelliteTableName, const char *planetTableName) {
    char sql[2048];
    // SQL to create satellite table
    snprintf(sql, sizeof(sql), 
            "CREATE TABLE IF NOT EXISTS %s ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT NOT NULL, "
            "timestamp INTEGER, "
            "positionX REAL, "
            "positionY REAL, "
            "positionZ REAL, "
            "velocityX REAL, "
            "velocityY REAL, "
            "velocityZ REAL, "
            "accelerationX REAL, "
            "accelerationY REAL, "
            "accelerationZ REAL, "
            "mass REAL);", satelliteTableName);
    
    printf("SQL Statement: %s\n", sql);
    char *errMsg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }
    // SQL to create planet table
    snprintf(sql, sizeof(sql), 
            "CREATE TABLE IF NOT EXISTS %s ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT NOT NULL, "
            "timestamp INTEGER, "
            "positionX REAL, "
            "positionY REAL, "
            "positionZ REAL, "
            "velocityX REAL, "
            "velocityY REAL, "
            "velocityZ REAL, "
            "accelerationX REAL, "
            "accelerationY REAL, "
            "accelerationZ REAL, "
            "mass REAL, "
            "spinX REAL, "
            "spinY REAL)", planetTableName);

    printf("SQL Statement for Planet Table: %s\n", sql);
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error for planet table: %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    return SQLITE_OK;
}

int insertSatelliteData(sqlite3 *db, const char *tableName, const char *name, double position[], double velocity[], double acceleration[], double mass, int timestamp){
    char sql[512];
    snprintf(sql, sizeof(sql),
        "INSERT INTO %s (name, timestamp, positionX, positionY, positionZ, velocityX, velocityY, velocityZ, accelerationX, accelerationY, accelerationZ, mass) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", tableName);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return 1; // Return error code
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

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1; // Return error code
    }

    sqlite3_finalize(stmt);
    return 0; // Success
}

int insertPlanetData(sqlite3 *db, const char *tableName, const char *name, double position[], double velocity[], double acceleration[], double mass, int timestamp, double spin[]){
    char sql[512];
    snprintf(sql, sizeof(sql),
        "INSERT INTO %s (name, timestamp, positionX, positionY, positionZ, velocityX, velocityY, velocityZ, accelerationX, accelerationY, accelerationZ, mass, spinX, spinY) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", tableName);
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return 1; // Return error code
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
    sqlite3_bind_double(stmt, 13, spin[0]);
    sqlite3_bind_double(stmt, 14, spin[1]);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1; // Return error code
    }

    sqlite3_finalize(stmt);
    return 0; // Success
}

int closeDatabase(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
    }
    return 0; // Success
}