#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

//Functions
int openDatabase(const char *filename, sqlite3 **db);
int createTable(sqlite3 *db, const char *tableName);
int insertSatelliteData(sqlite3 *db, const char *tableName, const char *name, double position[], double velocity[], double acceleration[], double mass, int timestamp);
int closeDatabase(sqlite3 *db);

#endif // DATABASE_H
