#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h>
#include <string>

class Database {
private:
    sqlite3* db;

public:
    Database(const std::string& dbPath);
    ~Database();

    bool createTables(const std::string& satelliteTable, const std::string& planetTable);
    bool insertSatelliteData(const std::string& tableName, const Satellite& satellite, int time);
    bool insertPlanetData(const std::string& tableName, const Planet& planet, int time);
    void closeDatabase();
};

#endif // DATABASE_HPP
