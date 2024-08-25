#include "simulation.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <map>


int main() {
    // Define paths to the database and input files
    std::string dbPath = "/Users/edward/Documents/SatelliteOrbitSimulation/data/OrbitalData.db";
    std::string planetsFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/planets.json";
    std::string satellitesFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/satellites.json";

    // Create a Simulation object
    Simulation simulation(dbPath);

    //Specific Start Date
    std::string startDate = "2025-01-01T00:00:00";

    // Load the specific planet and satellites to be simulated
    std::string planetName = "EARTH"; // Example: Simulate orbit around Earth
    std::string satelliteNames = "MOON"; // Example: Choose specific satellites
    std::string pointOfRef = "SUN"; // Point of reference for sim

    // Load data from files
    simulation.loadPlanets(planetsFile, planetName, startDate, pointOfRef);
    simulation.loadSatellites(satellitesFile, satelliteNames, startDate, pointOfRef);
    int Second = 1;
    int Min = Second * 60;
    int Hour = Min * 60;
    int Day = Hour * 24;
    int Week = Day * 7;
    int Month = Day * 31;
    double Year = Day * 365; 

    // Run the simulation
    double totalDuration = Year; // Total simulation duration in seconds (e.g., 1 day)
    int timeStep = Hour;         // Time step in seconds (e.g., 1 minute)

    auto positionalData = simulation.run(totalDuration, timeStep, startDate, pointOfRef);

    std::vector<double> planetX, planetY, planetZ;
    std::vector<double> satelliteX, satelliteY, satelliteZ;

    for (const auto& pos : positionalData[planetName]) {
        planetX.push_back(pos[0]);
        planetY.push_back(pos[1]);
        planetZ.push_back(pos[2]);
    }

    for (const auto& pos : positionalData[satelliteNames]) {
        satelliteX.push_back(pos[0]);
        satelliteY.push_back(pos[1]);
        satelliteZ.push_back(pos[2]);
    }

    // Save planet data to file
    std::ofstream planetFile("/Users/edward/Documents/SatelliteOrbitSimulation/data/planet_data.txt");
    if (planetFile.is_open()) {
        planetFile << "X Y Z\n";
        for (size_t i = 0; i < planetX.size(); ++i) {
            planetFile << planetX[i] << " " << planetY[i] << " " << planetZ[i] << "\n";
        }
        planetFile.close();
    } else {
        std::cerr << "Unable to open file for planet data.\n";
    }

    // Save satellite data to file
    std::ofstream satelliteFile("/Users/edward/Documents/SatelliteOrbitSimulation/data/satellite_data.txt");
    if (satelliteFile.is_open()) {
        satelliteFile << "X Y Z\n";
        for (size_t i = 0; i < satelliteX.size(); ++i) {
            satelliteFile << satelliteX[i] << " " << satelliteY[i] << " " << satelliteZ[i] << "\n";
        }
        std::cerr << "Able to open file for satellite data.\n";
        satelliteFile.close();
    } else {
        std::cerr << "Unable to open file for satellite data.\n";
    }


    return 0;
}
