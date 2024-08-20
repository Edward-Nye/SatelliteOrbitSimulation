#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include "Planet.hpp"
#include "Satellite.hpp"
#include "Database.hpp"

class Simulation {
public:
    std::vector<Planet> planets;
    std::vector<Satellite> satellites;
    Database database;

    Simulation(const std::string& dbPath);

    void loadPlanets(const std::string& planetsFile, const std::string& planetName);
    void loadSatellites(const std::string& satellitesFile, const std::vector<std::string>& satelliteNames);
    void run(int totalDuration, int timeStep);
};

#endif // SIMULATION_HPP
