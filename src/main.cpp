#include "simulation.hpp"
#include <iostream>
#include <vector>


int main() {
    // Define paths to the database and input files
    std::string dbPath = "/Users/edward/Documents/SatelliteOrbitSimulation/data/OrbitalData.db";
    std::string planetsFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/planets.txt";
    std::string satellitesFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/satellites.txt";

    // Create a Simulation object
    Simulation simulation(dbPath);

    //Specific Start Date
    std::string startDate = "2025-01-01T00:00:00";

    // Load the specific planet and satellites to be simulated
    std::string planetName = "EARTH"; // Example: Simulate orbit around Earth
    std::string satelliteNames = "MOON"; // Example: Choose specific satellites

    // Load data from files
    simulation.loadPlanets(planetsFile, planetName, startDate);
    simulation.loadSatellites(satellitesFile, satelliteNames, startDate);

    // Run the simulation
    int totalDuration = 31556000; // Total simulation duration in seconds (e.g., 1 day)
    int timeStep = 3600;         // Time step in seconds (e.g., 1 minute)

    simulation.run(totalDuration, timeStep, startDate);

    std::cout << "Simulation completed successfully!" << std::endl;

    return 0;
}
