#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <map>      // For std::map
#include <vector>   // For std::vector
#include <array>    // For std::array
#include <string>   // For std::string
#include <iostream> // For std::cout, std::endl (if needed)

#include "planet.hpp"
#include "satellite.hpp"
#include "database.hpp"
#include "rocketEngine.hpp"



class Simulation {
public:
    std::vector<Planet> planets;
    std::vector<Satellite> satellites;
    std::vector<RocketEngine> engine;
    Database database;

    Simulation(const std::string& dbPath);

    void loadPlanets(const std::string& jsonFilePath, std::vector<std::string>& planetName, const std::string& startDate, const std::string& pointOfRef);
    void loadSatellites(const std::string& jsonFilePath, const std::string& jsonREFilePath, std::vector<std::string>& satelliteNames,
                        const std::string& primarySat, const std::string& rocketEngine, const std::string& startDate, const std::string& pointOfRef,
                        const std::string& startPlanet);
    std::map<std::string, std::vector<std::array<double, 3>>> run(double totalDuration, int timeStep, const std::string& startDate, const std::string& pointOfRef,
                                                                  const std::string& primarySat);

};

#endif // SIMULATION_HPP
