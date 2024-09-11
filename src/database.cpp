#include "database.hpp"
#include "satellite.hpp"
#include "planet.hpp"
#include <iostream>

// Constructor to open the database connection
Database::Database(const std::string& dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    } else {
        std::cout << "Opened database successfully: " << dbPath << std::endl;
    }
}

// Destructor to close the database connection
Database::~Database() {
    if (db) {
        sqlite3_close(db);
        std::cout << "Database closed successfully." << std::endl;
    }
}

// Method to create the necessary tables in the database
bool Database::createTables(const std::string& satelliteTable, const std::string& planetTable) {
    char* errMsg = nullptr;
    std::string sql = "CREATE TABLE IF NOT EXISTS " + satelliteTable + " ("
                      "Time INT, "
                      "Name TEXT, "
                      "PosX REAL, PosY REAL, PosZ REAL, "
                      "VelX REAL, VelY REAL, VelZ REAL, "
                      "AccX REAL, AccY REAL, AccZ REAL, "
                      "Mass REAL, "
                      "Roll REAL, Pitch REAL, Yaw REAL, "
                      "RollR REAL, PitchR REAL, YawR REAL, "
                      "Engine_Name TEXT, "
                      "Thrust REAL, "
                      "Specific_Impulse REAL, "
                      "Burn_Rate REAL, "
                      "Efficiency REAL );";
                      
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot create satellite table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        std::cout << "Satellite table created successfully: " << satelliteTable << std::endl;
    }

    sql = "CREATE TABLE IF NOT EXISTS " + planetTable + " ("
          "Time INT, "
          "Name TEXT, "
          "PosX REAL, PosY REAL, PosZ REAL, "
          "VelX REAL, VelY REAL, VelZ REAL, "
          "AccX REAL, AccY REAL, AccZ REAL, "
          "Mass REAL, "
          "SpinX REAL, SpinY REAL);";

    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot create planet table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        std::cout << "Planet table created successfully: " << planetTable << std::endl;
    }

    return true;
}

// Method to insert satellite data into the database
bool Database::insertSatelliteData(const std::string& tableName, const Satellite& satellite, const std::vector<RocketEngine>& engine, double time) {
    std::string sql = "INSERT INTO " + tableName + " (Time, Name, PosX, PosY, PosZ, VelX, VelY, VelZ, AccX, AccY, AccZ, Mass,"
                      + " Roll, Pitch, Yaw, RollR, PitchR, YawR, Engine_Name, Thrust, Specific_Impulse, Burn_Rate, Efficiency) VALUES ("
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
                      + std::to_string(satellite.mass + engine[0].fuelMass) + ", "
                      + std::to_string(satellite.rotation[0]) + ", "
                      + std::to_string(satellite.rotation[1]) + ", "
                      + std::to_string(satellite.rotation[2]) + ", "
                      + std::to_string(satellite.rotationRates[0]) + ", "
                      + std::to_string(satellite.rotationRates[1]) + ", "
                      + std::to_string(satellite.rotationRates[2]) + ", '"
                      + engine[0].name + "', "
                      + std::to_string(engine[0].thrust) + ", "
                      + std::to_string(engine[0].specificImpulse) + ", "
                      + std::to_string(engine[0].burnRate) + ", "
                      + std::to_string(engine[0].specificImpulse) + ");";

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr)) {
        std::cerr << "Cannot insert satellite data: " << sqlite3_errmsg(db) << std::endl;
        return false;
    } else {
        std::cout << "Satellite data inserted successfully: " << satellite.name << " at time " << time << std::endl;
    }

    return true;
}

// Method to insert planet data into the database
bool Database::insertPlanetData(const std::string& tableName, const Planet& planet, double time) {
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
    } else {
        std::cout << "Planet data inserted successfully: " << planet.name << " at time " << time << std::endl;
    }

    return true;
}
