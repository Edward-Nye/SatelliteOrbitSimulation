#include "satellite.hpp"
#include "planet.hpp"
#include <cmath>
#include <iostream>

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
    double distancePlanet = 0.0; // Distance from Satellite to Planet
    double distanceSun = 0.0; // Distance from Satellite to Sun
    double distanceOfPlanet = 0.0;

    for (int i = 0; i < 3; ++i) {
        distanceVector[i] = planet.position[i] - position[i];
        distancePlanet += distanceVector[i] * distanceVector[i];
        distanceSun += position[i] * position[i];
        distanceOfPlanet += planet.position[i] * planet.position[i]; 
    }

    // Compute the gravitational force
    distanceSun = sqrt(distanceSun);
    distancePlanet = sqrt(distancePlanet);
    distanceOfPlanet = sqrt(distanceOfPlanet);
    double acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
    double acc_planet = -(G * planet.mass) / (distancePlanet * distancePlanet * distancePlanet);
    
    // Defermine if Satellite is within the SOI
    double SOI = pow((planet.mass/massSun),(2.0/5.0));
    std::cout << planet.name << " Mass: " << planet.mass << std::endl;
    
    // Update the acceleration based on the gravitational force
    if (SOI >= distancePlanet) {
        for (int i = 0; i < 3; ++i) {
            acceleration[i] = (acc_planet * distanceVector[i]) + (acc_sun * distanceVector[i]);
        }
        std::cout << name << " Is IN SOI: " << SOI << " >= " << distancePlanet << std::endl;
    }

    if (SOI < distancePlanet) {
        for (int i = 0; i < 3; ++i) {
            acceleration[i] = (acc_sun * position[i]) +(acc_planet * distanceVector[i]);   
        }
        std::cout << name << " Is OUT SOI: " << SOI << " < " << distancePlanet << std::endl;
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
