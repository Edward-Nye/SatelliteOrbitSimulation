#ifndef ROCKETENGINE_HPP
#define ROCKETENGINE_HPP

#include <string>
#include <array>
#include <vector>

class RocketEngine {
public:
    // Characteristics of the rocket engine
    std::string name;              // Name or model of the rocket engine
    double thrust;                 // Thrust produced by the engine (in Newtons)
    double specificImpulse;        // Specific impulse of the engine (in seconds)
    double fuelMass;               // Mass of the fuel (in kilograms)
    double burnRate;               // Fuel burn rate (in kilograms per second)
    double efficiency;             // Efficiency of the engine (dimensionless)

    // Constructor
    RocketEngine(const std::string& name, double thrust, double specificImpulse,
                 double fuelMass, double burnRate, double efficiency);

    // Methods to update and manage the engine
    void updateThrust(double timeStep); // Updates the thrust based on time
    void consumeFuel(double timeStep);  // Reduces fuel mass based on burn rate
    void applyForces(std::vector<std::array<double, 3>>& forces); // Applies forces based on thrust

       

};

#endif // ROCKETENGINE_HPP
