#include "simulation.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <array>
#include <string>


int main() {
    // Define paths to the database and input files
    std::string dbPath = "/Users/edward/Documents/data/OrbitalData.db";
    std::string planetsFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/planets.json";
    std::string satellitesFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/satellites.json";

    // Create a Simulation object
    Simulation simulation(dbPath);

    //Specific Start Date
    std::string startDate = "2025-01-01T00:00:00";

    // Load the specific planet and satellites to be simulated
    std::vector<std::string> planetNames = {"EARTH"/*, "VENUS", "MERCURY", "MARS BARYCENTER", "JUPITER BARYCENTER", "SATURN BARYCENTER", "URANUS BARYCENTER", "NEPTUNE BARYCENTER"*/}; // List of Planets
    std::string startPlanet = "EARTH"; // Start Planet
    std::vector<std::string> satelliteNames = {"NYESAT"}; // List of Satellites
    std::string pointOfRef = "EARTH BARYCENTER"; // Point of reference for sim

    // Load data from files
    simulation.loadPlanets(planetsFile, planetNames, startDate, pointOfRef);
    simulation.loadSatellites(satellitesFile, satelliteNames, startDate, pointOfRef, startPlanet);
    int Second = 1;
    int Min = Second * 60;
    int Hour = Min * 60;
    int Day = Hour * 24;
    int Week = Day * 7;
    int Month = Day * 31;
    double Year = Day * 365; 

    // Run the simulation
    double totalDuration = Min; // Total simulation duration in seconds (e.g., 1 day)
    int timeStep = Second;         // Time step in seconds (e.g., 1 minute)

    auto positionalData = simulation.run(totalDuration, timeStep, startDate, pointOfRef);

    // Vectors to store XYZ positions for planets and satellites
    std::map<std::string, std::vector<double>> planetX, planetY, planetZ;
    std::map<std::string, std::vector<double>> satelliteX, satelliteY, satelliteZ;

    // Process planet positions
    for (const auto& planetName : planetNames) {
        if (positionalData.find(planetName) != positionalData.end()) {
            for (const auto& pos : positionalData[planetName]) {
                planetX[planetName].push_back(pos[0]);
                planetY[planetName].push_back(pos[1]);
                planetZ[planetName].push_back(pos[2]);
            }
        }
    }

    // Process satellite positions
    for (const auto& satelliteName : satelliteNames) {
        if (positionalData.find(satelliteName) != positionalData.end()) {
            for (const auto& pos : positionalData[satelliteName]) {
                satelliteX[satelliteName].push_back(pos[0]);
                satelliteY[satelliteName].push_back(pos[1]);
                satelliteZ[satelliteName].push_back(pos[2]);
            }
        }
    }

    // Save planet data to file
    std::ofstream planetFile("/Users/edward/Documents/SatelliteOrbitSimulation/data/planet_data.txt");
    if (planetFile.is_open()) {
        planetFile << "Name X Y Z\n";
        for (const auto& planetName : planetNames) {
            for (size_t i = 0; i < planetX[planetName].size(); ++i) {
                planetFile << planetName << " "
                           << planetX[planetName][i] << " "
                           << planetY[planetName][i] << " "
                           << planetZ[planetName][i] << "\n";
            }
        }
        planetFile.close();
    } else {
        std::cerr << "Unable to open file for planet data.\n";
    }

    // Save satellite data to file
    std::ofstream satelliteFile("/Users/edward/Documents/SatelliteOrbitSimulation/data/satellite_data.txt");
    if (satelliteFile.is_open()) {
        satelliteFile << "Name X Y Z\n";
        for (const auto& satelliteName : satelliteNames) {
            for (size_t i = 0; i < satelliteX[satelliteName].size(); ++i) {
                satelliteFile << satelliteName << " "
                              << satelliteX[satelliteName][i] << " "
                              << satelliteY[satelliteName][i] << " "
                              << satelliteZ[satelliteName][i] << "\n";
            }
        }
        satelliteFile.close();
    } else {
        std::cerr << "Unable to open file for satellite data.\n";
    }

    return 0;
}
