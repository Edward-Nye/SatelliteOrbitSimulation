#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include "planet.hpp"
#include "satellite.hpp"
#include "database.hpp"

class Simulation {
public:
    std::vector<Planet> planets;
    std::vector<Satellite> satellites;
    Database database;

    Simulation(const std::string& dbPath);

    void loadPlanets(const std::string& planetsFile, const std::string& planetName, const std::string& startDate);
    void loadSatellites(const std::string& satellitesFile, const std::string& satelliteName,  const std::string& startDate);
    void run(int totalDuration, int timeStep, const std::string& startDate);
};

#endif // SIMULATION_HPP
