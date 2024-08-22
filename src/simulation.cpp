#include "simulation.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

// Constructor to initialize the simulation with a database connection
Simulation::Simulation(const std::string& dbPath) : database(dbPath) {}

// Method to load planets from a file
void Simulation::loadPlanets(const std::string& planetsFile, const std::string& planetName) {
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

        if (name == planetName) {
            planets.emplace_back(name, pos, vel, acc, radius, mass, spin);
            break;
        }
    }

    file.close();
}

// Method to load satellites from a file
void Simulation::loadSatellites(const std::string& satellitesFile, const std::vector<std::string>& satelliteNames) {
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

        if (std::find(satelliteNames.begin(), satelliteNames.end(), name) != satelliteNames.end()) {
            satellites.emplace_back(name, pos, vel, acc, mass);
        }
    }

    file.close();
}

// Method to run the simulation
void Simulation::run(int totalDuration, int timeStep) {
    int currentTime = 0;
    while (currentTime <= totalDuration) {
        for (auto& planet : planets) {
            planet.updatePosition(timeStep);
            database.insertPlanetData("PlanetData", planet, currentTime);
        }

        for (auto& satellite : satellites) {
            for (const auto& planet : planets) {
                satellite.updatePosition(planet, timeStep);
            }
            // Log the satellite's position
            std::cout << "Time: " << currentTime 
                      << " | Satellite: " << satellite.name 
                      << " | Position: (" 
                      << satellite.position[0] << ", " 
                      << satellite.position[1] << ", " 
                      << satellite.position[2] << ")" << std::endl;

            database.insertSatelliteData("SatelliteData", satellite, currentTime);
        }

        currentTime += timeStep;
    }
}
