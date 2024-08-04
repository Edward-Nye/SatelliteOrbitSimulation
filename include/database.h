#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

//Functions
int openDatabase(const char *filename, sqlite3 **db);
int createTable(sqlite3 *db);
int insertSatelliteData(sqlite3 *db, double position[], double velocity[], double mass);
int closeDatabase(sqlite3 *db);

#endif // DATABASE_H
