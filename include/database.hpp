#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h>
#include <string>
#include "planet.hpp"
#include "satellite.hpp"

class Database {
private:
    sqlite3* db;

public:
    Database(const std::string& dbPath);
    ~Database();

    bool createTables(const std::string& satelliteTable, const std::string& planetTable);
    bool insertSatelliteData(const std::string& tableName, const Satellite& satellite, double time);
    bool insertPlanetData(const std::string& tableName, const Planet& planet, double time);
    void closeDatabase();
};

#endif // DATABASE_HPP
