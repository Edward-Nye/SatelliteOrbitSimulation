#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

//Functions
int openDatabase(const char *filename, sqlite3 **db);
int createTables(sqlite3 *db, const char *satellitetableName, const char *planetTableName);
int insertSatelliteData(sqlite3 *db, const char *tableName, const char *name, double position[], double velocity[], double acceleration[], double mass, int timestamp);
int insertPlanetData(sqlite3 *db, const char *tableName, const char *name, double position[], double velocity[], double acceleration[], double mass, int timestamp, double spin[]);
int closeDatabase(sqlite3 *db);

#endif // DATABASE_H
