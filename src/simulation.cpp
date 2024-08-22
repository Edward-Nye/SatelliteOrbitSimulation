#include "simulation.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include "SpiceUsr.h"
#include <string>
#include <array>
#include <vector>


// Current time
std::string getCurrentTimeString() {
    // Get current time as a time_t object
    std::time_t now = std::time(nullptr);
    
    // Convert time_t to tm struct
    std::tm* tm_now = std::localtime(&now);
    
    // Use a stringstream to format the time
    std::stringstream ss;
    ss << std::put_time(tm_now, "%Y%m%d_%H%M%S"); // Format: YYYYMMDD_HHMMSS

    return ss.str();
}



void convertStringToET(const std::string& startDate, SpiceDouble* et) {
    str2et_c(startDate.c_str(), et);
}


// Constructor to initialize the simulation with a database connection
Simulation::Simulation(const std::string& dbPath) : database(dbPath) {}

// Method to load planets from a file
void Simulation::loadPlanets(const std::string& planetsFile, const std::string& planetName, const std::string& startDate) {
    
    // Load the SPK file using SPICE
    furnsh_c("/Users/edward/Documents/spice_kernels/de440s.bsp");

    std::ifstream file(planetsFile);
    if (!file.is_open()) {
        std::cerr << "Unable to open planet file: " << planetsFile << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::array<double, 3> pos, vel, acc;
        double radius, mass;
        std::array<double, 2> spin;

        ss >> name >> pos[0] >> pos[1] >> pos[2]
           >> vel[0] >> vel[1] >> vel[2]
           >> acc[0] >> acc[1] >> acc[2]
           >> radius >> mass >> spin[0] >> spin[1];

        std::cout << "Loaded planet: " << name << std::endl; // Add this line
        std::cout << "Specified planet: " << planetName<< std::endl; // Add this line

        if (name == planetName) {
                       
            // Initialize the SPICE Toolkit
            furnsh_c("/Users/edward/Documents/spice_kernels/naif0012.tls.txt");

            // Convert the start date to ephemeris time (ET)
            SpiceDouble et;
            str2et_c(startDate.c_str(), &et);

            // Retrieve the position and velocity from the SPK file
            SpiceDouble state[6];
            SpiceDouble lt;

            spkezr_c(planetName.c_str(), et, "J2000", "NONE", "SUN", state, &lt);

            // Update position and velocity with SPK data
            pos[0] = state[0];
            pos[1] = state[1];
            pos[2] = state[2];
            vel[0] = state[3];
            vel[1] = state[4];
            vel[2] = state[5];

            planets.emplace_back(name, pos, vel, acc, radius, mass, spin);
            std::cout << "Planet Location " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
            break;
        }
    }

    file.close();

    // Unload the SPK file after use
    unload_c("/Users/edward/Documents/spice_kernels/de440s.bsp");
    
}

void Simulation::loadSatellites(const std::string& satellitesFile, const std::string& satelliteName,  const std::string& startDate) {
    
    // Load the SPK file using SPICE
    furnsh_c("/Users/edward/Documents/spice_kernels/de440s.bsp");

    std::ifstream file(satellitesFile);
    if (!file.is_open()) {
        std::cerr << "Unable to open satellite file: " << satellitesFile << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::array<double, 3> pos, vel, acc;
        double mass;

        ss >> name >> pos[0] >> pos[1] >> pos[2]
           >> vel[0] >> vel[1] >> vel[2]
           >> acc[0] >> acc[1] >> acc[2]
           >> mass;

        std::cout << "Loaded satellite: " << name << std::endl; // Add this line

        if (name == satelliteName) {
                       
            // Convert the start date to ephemeris time (ET)
            SpiceDouble et;
            str2et_c(startDate.c_str(), &et);

            // Retrieve the position and velocity from the SPK file
            SpiceDouble state[6];
            SpiceDouble lt;

            spkezr_c(satelliteName.c_str(), et, "J2000", "NONE", "SUN", state, &lt);

            // Update position and velocity with SPK data
            pos[0] = state[0];
            pos[1] = state[1];
            pos[2] = state[2];
            vel[0] = state[3];
            vel[1] = state[4];
            vel[2] = state[5];

            satellites.emplace_back(name, pos, vel, acc, mass);
            std::cout << "Satellite Location " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
            break;
        }
    }

    file.close();

    // Unload the SPK file after use
    unload_c("/Users/edward/Documents/spice_kernels/de440s.bsp");
    
}


void Simulation::run(int totalDuration, int timeStep, const std::string& startDate) {
    std::cout << "Simulation started..." << std::endl; // Test output
    std::cout << "Number of planets: " << planets.size() << std::endl;
    std::cout << "Number of satellites: " << satellites.size() << std::endl;

    SpiceDouble et;
    
    // Convert a calendar date to ephemeris time
    str2et_c(startDate.c_str(), &et);
    std::cout << "Ephemeris Time: " << et << std::endl;

    // Get current time as string
    std::string currentTimeStr = getCurrentTimeString();
    
    //Generate unique table names
    std::string satelliteTable = "satellite_data_" + currentTimeStr;
    std::string planetTable = "planet_data_0" + currentTimeStr;

    // Call createTables with unique names
    if (!database.createTables(satelliteTable, planetTable)){
        std::cerr << "Failed to create tables." << std::endl;
    }

    int currentTime = 0;
    while (currentTime <= totalDuration) {
        for (auto& planet : planets) {
            planet.updatePosition(timeStep);
            database.insertPlanetData(planetTable, planet, currentTime);
        }

        for (auto& satellite : satellites) {
            for (const auto& planet : planets) {
                satellite.updatePosition(planet, timeStep);
            }
            database.insertSatelliteData(satelliteTable, satellite, currentTime);

            std::cout << "Time: " << currentTime
                      << " | Satellite: " << satellite.name
                      << " | Position: (" << satellite.position[0] << ", "
                      << satellite.position[1] << ", "
                      << satellite.position[2] << ")" << std::endl;
        }

        currentTime += timeStep;
    }
}


