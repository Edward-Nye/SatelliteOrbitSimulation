#include "rocketEngine.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cmath>




RocketEngine::RocketEngine(const std::string& name, double thrust, double specificImpulse,
                double fuelMass, double burnRate, double efficiency)
    : name(name), thrust(thrust), specificImpulse(specificImpulse), 
        fuelMass(fuelMass), burnRate(burnRate), efficiency(efficiency) {}

void RocketEngine::updateThrust(double timeStep) {
    // Thrust stays constant for this simple model, but can be updated to include
    // variations based on specific engine conditions, fuel consumption, etc.
    std::cout << "Thrust at time " << timeStep << " is: " << thrust << " N\n";
}

void RocketEngine::consumeFuel(double timeStep) {
    // Calculate the amount of fuel consumed during the time step
    double fuelConsumed = burnRate * timeStep;
    
    if (fuelMass > 0) {
        fuelMass = std::max(0.0, fuelMass - fuelConsumed);
        std::cout << "Fuel remaining: " << fuelMass << " kg\n";
    } else {
        std::cout << "No fuel remaining!" << std::endl;
    }
}

void RocketEngine::applyForces(std::vector<std::array<double, 3>>& forces) {
    // Assuming the thrust is applied in the forward direction (along the x-axis).
    std::array<double, 3> thrustForce = {thrust, 0.0, 0.0};
    forces.push_back(thrustForce);
    
    std::cout << "Applied thrust force: [" << thrustForce[0] << ", " 
              << thrustForce[1] << ", " << thrustForce[2] << "]\n";
}