#include "planet.hpp"
#include <cmath>

// Constructor to initialize the planet
Planet::Planet(const std::string& name, const std::array<double, 3>& pos,
               const std::array<double, 3>& vel, const std::array<double, 3>& acc,
               double radius, double mass, const std::array<double, 2>& spin)
    : name(name), position(pos), velocity(vel), acceleration(acc), radius(radius), mass(mass), spin(spin) {}


// Method to update the planet's position based on its velocity and acceleration
void Planet::updatePosition(double timeStep) {
    
    double massSun =  1.988400E30;
    double G = 6.67430E-11;
    double distance = 0.0;

    // Compute the gravitational force
    distance = sqrt(position[0] * position[0] +
                    position[1] * position[1] +
                    position[2] * position[2]);

    double acc_sun = -(G * massSun) / (distance * distance * distance);
    

    // Update the acceleration based on the gravitational force
    for (int i = 0; i < 3; ++i) {
        acceleration[i] = acc_sun * position[i];
    }

    

    // Update velocity with the current acceleration
    for (int i = 0; i < 3; ++i) {
        velocity[i] += acceleration[i] * timeStep;
    }

    // Update position with the current velocity
    for (int i = 0; i < 3; ++i) {
        position[i] += velocity[i] * timeStep;
    }
}
