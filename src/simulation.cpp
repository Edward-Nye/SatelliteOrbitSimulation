#include "simulation.hpp"
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include "SpiceUsr.h"
#include <string>
#include <array>
#include <vector>
#include <nlohmann/json.hpp>
#include "planet.hpp"



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

// Function to load planet data
void Simulation::loadPlanets(const std::string& jsonFilePath, std::vector<std::string>& planetNames, const std::string& startDate, const std::string& pointOfRef) {
    
    // Load the SPICE kernels
    furnsh_c("/Users/edward/Documents/spice_kernels/de440s.bsp");
    furnsh_c("/Users/edward/Documents/spice_kernels/naif0012.tls.txt");

    // Open the JSON file
    std::ifstream inputFile(jsonFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open JSON file: " << jsonFilePath << std::endl;
        return;
    }

    nlohmann::json jsonData;
    inputFile >> jsonData;
    inputFile.close();

    // Convert the start date to ephemeris time (ET)
    SpiceDouble et;
    str2et_c(startDate.c_str(), &et);

    for (const std::string& planetName : planetNames) {
        // Iterate over the planets in the JSON file
        for (const auto& planetData : jsonData["planets"]) {
            std::string name = planetData["name"];

            if (name == planetName) {
                // Retrieve position and velocity from the SPICE Toolkit
                SpiceDouble state[6];
                SpiceDouble lt;

                spkezr_c(planetName.c_str(), et, "J2000", "NONE", pointOfRef.c_str(), state, &lt);

                std::array<double, 3> pos = {state[0] * 1E3, state[1] * 1E3, state[2] * 1E3}; // convert to meters
                std::array<double, 3> vel = {state[3] * 1E3, state[4] * 1E3, state[5] * 1E3}; // convert to meters/second
                std::array<double, 3> acc = {0.0, 0.0, 0.0}; // Default or calculated based on your simulation

                // Load other parameters from the JSON file
                double radius = planetData["radius"];
                double mass = planetData["mass"];
                std::array<double, 2> spin = planetData["spin"].get<std::array<double, 2>>();

                // Store the planet data in the simulation's planet list
                planets.emplace_back(name, pos, vel, acc, radius, mass, spin);

                std::cout << "Loaded planet: " << name << std::endl;
                std::cout << "Position: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
                std::cout << "Velocity: " << vel[0] << ", " << vel[1] << ", " << vel[2] << std::endl;
                std::cout << "Mass: " << mass << std::endl;
                break;
            }
        }
    }
    

    // Unload the SPICE kernels after use
    unload_c("/Users/edward/Documents/spice_kernels/de440s.bsp");
    
    
}

void Simulation::loadSatellites(const std::string& jsonFilePath, std::vector<std::string>& satelliteNames, const std::string& startDate, const std::string& pointOfRef, const std::string& startPlanet) {
    
    // Load the SPK file using SPICE
    furnsh_c("/Users/edward/Documents/spice_kernels/de440s.bsp");
    furnsh_c("/Users/edward/Documents/spice_kernels/naif0012.tls.txt");

    // Open the JSON file
    std::ifstream inputFile(jsonFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open JSON file: " << jsonFilePath << std::endl;
        return;
    }

    nlohmann::json jsonData;
    inputFile >> jsonData;
    inputFile.close();

    // Convert the start date to ephemeris time (ET)
    SpiceDouble et;
    str2et_c(startDate.c_str(), &et);
    for (const std::string& satelliteName : satelliteNames) {
        // Iterate over the planets in the JSON file
        for (const auto& satelliteData : jsonData["satellites"]) {
            std::string name = satelliteData["name"];

            if (name == satelliteName) {
            
                std::array<double, 3> pos = satelliteData["position"].get<std::array<double, 3>>();
                std::array<double, 3> vel = satelliteData["velocity"].get<std::array<double, 3>>();

                if (satelliteName == "MOON") {
                    // Retrieve position and velocity from the SPICE Toolkit
                
                    SpiceDouble state[6];
                    SpiceDouble lt;

                    spkezr_c(satelliteName.c_str(), et, "J2000", "NONE", pointOfRef.c_str(), state, &lt);

                    pos = {state[0] * 1E3, state[1] * 1E3, state[2] * 1E3}; // convert to meters
                    vel = {state[3] * 1E3, state[4] * 1E3, state[5] * 1E3}; // convert to meters/second
                    // Default or calculated based on your simulation
                }

                if (pointOfRef == "SUN" && satelliteName != "MOON") {
                    for (auto& planet : planets) {
                        if (planet.name == startPlanet){
                            pos = {pos[0] + planet.position[0], pos[1] + planet.position[1], pos[2] + planet.position[2]};
                            vel = {vel[0] + planet.velocity[0], vel[1] + planet.velocity[1], vel[2] + planet.velocity[2]};
                        }
                    }
                }
            
            
                std::array<double, 3> acc = {0.0, 0.0, 0.0};
                std::array<double, 6> preState = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
                std::array<double, 3> rot = {0.0, 0.0, 0.0};
                std::array<double, 3> rotR = {0.0, 0.0, 0.0};
                // Load other parameters from the JSON file
                //double radius = satelliteData["radius"];
                double mass = satelliteData["mass"];
                //std::array<double, 2> spin = satelliteData["spin"].get<std::array<double, 2>>();

                // Store the planet data in the simulation's planet list
                satellites.emplace_back(name, pos, vel, acc, mass, preState, rot, rotR);

                std::cout << "Loaded Satellite: " << name << std::endl;
                std::cout << "Position: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
                std::cout << "Velocity: " << vel[0] << ", " << vel[1] << ", " << vel[2] << std::endl;
                std::cout << "Mass: " << mass << std::endl;
                break;
            }
        }
    }
    // Unload the SPICE kernels after use
    unload_c("/Users/edward/Documents/spice_kernels/de440s.bsp");
    
}


std::map<std::string, std::vector<std::array<double, 3>>> Simulation::run(double totalDuration, int timeStep, const std::string& startDate, const std::string& pointOfRef) {
    
    std::map<std::string, std::vector<std::array<double, 3>>> positionalData;
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
        return {};
    }

    double currentTime = 0;
    while (currentTime <= totalDuration) {
        
        for (auto& satellite : satellites) {
            
            satellite.updatePosition(planets, satellites, timeStep, pointOfRef, currentTime);
            std::cout << "made it!" << std::endl;
            database.insertSatelliteData(satelliteTable, satellite, currentTime); /////////////

            // Store the satellite's position in the positionalData map
            positionalData[satellite.name].push_back(satellite.position);

            std::cout << "Time: " << currentTime
                      << " | Satellite: " << satellite.name
                      << " | Position: (" << satellite.position[0] << ", "
                      << satellite.position[1] << ", "
                      << satellite.position[2] << ")" << std::endl;
        }
        
        for (auto& planet : planets) {
            
            planet.updatePosition(satellites, timeStep, pointOfRef);
            
            database.insertPlanetData(planetTable, planet, currentTime);

            // Store the planet's position in the positionalData map
            positionalData[planet.name].push_back(planet.position);

            std::cout << "Time: " << currentTime
                      << " | Planet: " << planet.name
                      << " | Position: (" << planet.position[0] << ", "
                      << planet.position[1] << ", "
                      << planet.position[2] << ")" << std::endl;
        }

        currentTime += timeStep;
    }

    std::cout << "Simulation completed successfully!" << std::endl;

    return positionalData;
}



