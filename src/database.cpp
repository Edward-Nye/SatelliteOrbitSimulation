#include "database.hpp"
#include "satellite.hpp"
#include "planet.hpp"
#include <iostream>

// Constructor to open the database connection
Database::Database(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db)) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

// Destructor to close the database connection
Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

// Method to create the necessary tables in the database
bool Database::createTables(const std::string& satelliteTable, const std::string& planetTable) {
    std::string sql = "CREATE TABLE IF NOT EXISTS " + satelliteTable + " ("
                      "Time INT, "
                      "Name TEXT, "
                      "PosX REAL, PosY REAL, PosZ REAL, "
                      "VelX REAL, VelY REAL, VelZ REAL, "
                      "AccX REAL, AccY REAL, AccZ REAL, "
                      "Mass REAL);";
                      
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr)) {
        std::cerr << "Cannot create satellite table: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sql = "CREATE TABLE IF NOT EXISTS " + planetTable + " ("
          "Time INT, "
          "Name TEXT, "
          "PosX REAL, PosY REAL, PosZ REAL, "
          "VelX REAL, VelY REAL, VelZ REAL, "
          "AccX REAL, AccY REAL, AccZ REAL, "
          "Mass REAL, "
          "SpinX REAL, SpinY REAL);";

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr)) {
        std::cerr << "Cannot create planet table: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

// Method to insert satellite data into the database
bool Database::insertSatelliteData(const std::string& tableName, const Satellite& satellite, int time) {
    std::string sql = "INSERT INTO " + tableName + " (Time, Name, PosX, PosY, PosZ, VelX, VelY, VelZ, AccX, AccY, AccZ, Mass) VALUES ("
                      + std::to_string(time) + ", '"
                      + satellite.name + "', "
                      + std::to_string(satellite.position[0]) + ", "
                      + std::to_string(satellite.position[1]) + ", "
                      + std::to_string(satellite.position[2]) + ", "
                      + std::to_string(satellite.velocity[0]) + ", "
                      + std::to_string(satellite.velocity[1]) + ", "
                      + std::to_string(satellite.velocity[2]) + ", "
                      + std::to_string(satellite.acceleration[0]) + ", "
                      + std::to_string(satellite.acceleration[1]) + ", "
                      + std::to_string(satellite.acceleration[2]) + ", "
                      + std::to_string(satellite.mass) + ");";

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr)) {
        std::cerr << "Cannot insert satellite data: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

// Method to insert planet data into the database
bool Database::insertPlanetData(const std::string& tableName, const Planet& planet, int time) {
    std::string sql = "INSERT INTO " + tableName + " (Time, Name, PosX, PosY, PosZ, VelX, VelY, VelZ, AccX, AccY, AccZ, Mass, SpinX, SpinY) VALUES ("
                      + std::to_string(time) + ", '"
                      + planet.name + "', "
                      + std::to_string(planet.position[0]) + ", "
                      + std::to_string(planet.position[1]) + ", "
                      + std::to_string(planet.position[2]) + ", "
                      + std::to_string(planet.velocity[0]) + ", "
                      + std::to_string(planet.velocity[1]) + ", "
                      + std::to_string(planet.velocity[2]) + ", "
                      + std::to_string(planet.acceleration[0]) + ", "
                      + std::to_string(planet.acceleration[1]) + ", "
                      + std::to_string(planet.acceleration[2]) + ", "
                      + std::to_string(planet.mass) + ", "
                      + std::to_string(planet.spin[0]) + ", "
                      + std::to_string(planet.spin[1]) + ");";

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr)) {
        std::cerr << "Cannot insert planet data: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}
