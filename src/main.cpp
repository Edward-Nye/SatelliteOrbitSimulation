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

    // Load the specific planet and satellites to be simulated
    std::string planetName = "Earth"; // Example: Simulate orbit around Earth
    std::vector<std::string> satelliteNames = {"Moon"}; // Example: Choose specific satellites

    // Load data from files
    simulation.loadPlanets(planetsFile, planetName);
    simulation.loadSatellites(satellitesFile, satelliteNames);

    // Run the simulation
    int totalDuration = 86400; // Total simulation duration in seconds (e.g., 1 day)
    int timeStep = 1;         // Time step in seconds (e.g., 1 minute)

    simulation.run(totalDuration, timeStep);

    std::cout << "Simulation completed successfully!" << std::endl;

    return 0;
}
