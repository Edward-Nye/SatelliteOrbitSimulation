#include "satellite.hpp"
#include "planet.hpp"
#include <cmath>

// Constructor to initialize the satellite
Satellite::Satellite(const std::string& name, const std::array<double, 3>& pos,
                     const std::array<double, 3>& vel, const std::array<double, 3>& acc,
                     double mass)
    : name(name), position(pos), velocity(vel), acceleration(acc), mass(mass) {}

// Method to update the satellite's position based on the gravitational force from a planet
void Satellite::updatePosition(const Planet& planet, double timeStep) {
    // Compute the distance vector between the satellite and the planet
    std::array<double, 3> distanceVector;
    double massSun =  1.988400E30;
    double G = 6.67430E-11;
    double distancePlanet = 0.0;
    double distanceSun = 0.0;

    for (int i = 0; i < 3; ++i) {
        distanceVector[i] = planet.position[i] - position[i];
        distancePlanet += distanceVector[i] * distanceVector[i];
        distanceSun += position[i] * position[i];
    }
    
    // Compute the gravitational force
    distanceSun = sqrt(distanceSun);
    distancePlanet = sqrt(distancePlanet);
    double acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
    double acc_planet = -(G * planet.mass) / (distancePlanet * distancePlanet * distancePlanet);

    // Update the acceleration based on the gravitational force
    for (int i = 0; i < 3; ++i) {
        acceleration[i] = (acc_planet * distanceVector[i]) + (acc_sun * position[i]);
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

// Method to apply a maneuver (change in velocity) to the satellite
void Satellite::applyManeuver(const std::array<double, 3>& deltaV) {
    for (int i = 0; i < 3; ++i) {
        velocity[i] += deltaV[i];
    }
}
