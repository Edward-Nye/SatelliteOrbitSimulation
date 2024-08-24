#include "planet.hpp"
#include "satellite.hpp"
#include <cmath>
#include <iostream>

// Constructor to initialize the planet
Planet::Planet(const std::string& name, const std::array<double, 3>& pos,
               const std::array<double, 3>& vel, const std::array<double, 3>& acc,
               double radius, double mass, const std::array<double, 2>& spin)
    : name(name), position(pos), velocity(vel), acceleration(acc), radius(radius), mass(mass), spin(spin) {}


// Method to update the planet's position based on its velocity and acceleration
void Planet::updatePosition(const Satellite& satellite, double timeStep, const std::string& pointOfRef) {
    std::array<double, 3> distanceVector;
    double G = 6.67430E-11;
    double distanceSatellite = 0.0;// Distance from Satellite to Planet
    
    if (pointOfRef == "SUN") {
        
        double massSun =  1.988400E30;
         
        double distanceSun = 0.0; // Distance from Planet to Sun
        double distanceOfSatellite = 0.0; // Distance of Satellite from sun 

        for (int i = 0; i < 3; ++i) {
            distanceVector[i] = satellite.position[i] - position[i];
            distanceSatellite += distanceVector[i] * distanceVector[i];
            distanceSun += position[i] * position[i];
            distanceOfSatellite += satellite.position[i] * satellite.position[i]; 
        }

        distanceSun = sqrt(distanceSun);
        distanceSatellite = sqrt(distanceSatellite);
        distanceOfSatellite = sqrt(distanceOfSatellite);


        double acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
        double acc_satellite = -(G * satellite.mass) / (distanceSatellite * distanceSatellite * distanceSatellite);

        // Defermine if Satellite's SOI
        double SOI = distanceOfSatellite * pow((satellite.mass/massSun),(2.0/5.0));

        // Update the acceleration based on the gravitational force
        // Inside SOI
        if (SOI >= distanceSatellite) {
            for (int i = 0; i < 3; ++i) {
                acceleration[i] = (acc_satellite * distanceVector[i]) + (acc_sun * distanceVector[i]);
            }
    
        }
        // Outside SOI
        if (SOI < distanceSatellite) {
            for (int i = 0; i < 3; ++i) {
                acceleration[i] = (acc_sun * position[i]) +(acc_satellite * distanceVector[i]);   
            }
            
        }

    }
    
    if (name == pointOfRef) {
        std::cout << "Planet Frame of Reference:" << name << std::endl;
        for (int i = 0; i < 3; ++i) {
            distanceVector[i] = satellite.position[i] - position[i];
            distanceSatellite += distanceVector[i] * distanceVector[i];
        }
        
        distanceSatellite = sqrt(distanceSatellite);

        double acc_satellite = -(G * satellite.mass) / (distanceSatellite * distanceSatellite * distanceSatellite);
        for (int i = 0; i < 3; ++i) {
                acceleration[i] = (acc_satellite * distanceVector[i]);
            }
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
