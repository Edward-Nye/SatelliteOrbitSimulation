#include "planet.hpp"

// Constructor to initialize the planet
Planet::Planet(const std::string& name, const std::array<double, 3>& pos,
               const std::array<double, 3>& vel, const std::array<double, 3>& acc,
               double radius, double mass, const std::array<double, 2>& spin)
    : name(name), position(pos), velocity(vel), acceleration(acc), radius(radius), mass(mass), spin(spin) {}

// Method to update the planet's position based on its velocity and acceleration
void Planet::updatePosition(double timeStep) {
    // Update velocity with the current acceleration
    for (int i = 0; i < 3; ++i) {
        velocity[i] += acceleration[i] * timeStep;
    }

    // Update position with the current velocity
    for (int i = 0; i < 3; ++i) {
        position[i] += velocity[i] * timeStep;
    }
}
