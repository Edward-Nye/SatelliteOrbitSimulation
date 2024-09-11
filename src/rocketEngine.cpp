#include "rocketEngine.hpp"
#include "satellite.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <cmath>




RocketEngine::RocketEngine(const std::string& name, double thrust, double specificImpulse,
                double fuelMass, double burnRate, double efficiency)
    : name(name), thrust(thrust), specificImpulse(specificImpulse), 
        fuelMass(fuelMass), burnRate(burnRate), efficiency(efficiency) {}



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

void RocketEngine::applyForces(std::vector<Satellite>& satellites, const std::string& primarySat, double timeStep) {

    const double g0 = 9.81; // gravitational acceleration (m/s^2)
    thrust = burnRate * specificImpulse * g0 * efficiency;
    // Calculate the amount of fuel consumed during the time step
    double fuelConsumed = burnRate * timeStep;
    for (auto& satellite : satellites){
        if (satellite.name == primarySat) {
            double T_mass = fuelMass + satellite.mass;

            if (fuelMass > 0) {
                fuelMass = std::max(0.0, fuelMass - (fuelConsumed)*0.01);
                std::cout << "Fuel remaining: " << fuelMass << " kg\n";
            } else {
                std::cout << "No fuel remaining!" << std::endl;
            }
            std::array<std::array<double, 3>, 3> DCM;
            // Assuming the thrust is applied in the forward direction (along the x-axis).
            std::array<double, 3> thrustForce = {thrust, 0.0, 0.0};
            DCM = satellite.getDCM();

            std::array<double, 3> transformedThrust;

            for (int i = 0; i < 3; ++i) {
                transformedThrust[i] = 0.0;
                for (int j = 0; j < 3; ++j) {
                    transformedThrust[i] += DCM[i][j] * thrustForce[j];
                }
            }

            // Update satellite acceleration based on transformed thrust force
            // Assuming that the acceleration is directly proportional to the thrust
            // and the mass of the satellite (Newton's second law: F = ma)
            
            for (int i = 0; i < 3; ++i) {
                satellite.acceleration[i] = transformedThrust[i] / T_mass;
            }
            break;
        }
    }
    
    std::cout << "Applied thrust force: [" << thrust << "]N \n";
}