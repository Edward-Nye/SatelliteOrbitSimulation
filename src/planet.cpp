#include "planet.hpp"
#include "satellite.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <limits> 

// Constructor to initialize the planet
Planet::Planet(const std::string& name, const std::array<double, 3>& pos,
               const std::array<double, 3>& vel, const std::array<double, 3>& acc,
               double radius, double mass, const std::array<double, 2>& spin)
    : name(name), position(pos), velocity(vel), acceleration(acc), radius(radius), mass(mass), spin(spin) {}


// Method to update the planet's position based on its velocity and acceleration
void Planet::updatePosition(const std::vector<Satellite>& satellites, double timeStep, const std::string& pointOfRef) {
    std::vector<std::vector<double>> distanceVector(satellites.size(), std::vector<double>(3, 0.0));
    double G = 6.67430E-11;
    std::vector<double> distanceSatellite(satellites.size(), 0.0);// Distance from Satellite to Planet
    std::vector<double> acc_satellite(satellites.size(), 0.0);
    std::vector<double> SPacceleration(3, 0.0);
    if (pointOfRef == "SUN") {
        
        double massSun =  1.988400E30;
         
        double distanceSun = 0.0; // Distance from Planet to Sun
         

        for (int j = 0; j < 3; ++j) {
            distanceSun += position[j] * position[j]; 
        }
        int k = 0;
        for (const auto& satellite : satellites){
            for (int j = 0; j < 3; ++j) {
                distanceVector[k][j] = position[j] - satellite.position[j];
                distanceSatellite[k] += distanceVector[k][j] * distanceVector[k][j];
            }
            distanceSatellite[k] = sqrt(distanceSatellite[k]);
            acc_satellite[k] = -(G * satellite.mass) / (distanceSatellite[k] * distanceSatellite[k] * distanceSatellite[k]);
            for (int j = 0; j < 3; ++j) {
                SPacceleration[j] += acc_satellite[k] * distanceVector[k][j];
            }
        }
        distanceSun = sqrt(distanceSun);
        double acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
      
        for (int j = 0; j < 3; ++j) {
            acceleration[j] = (acc_sun * position[j]) + SPacceleration[j];   
        }
            
        

    }
    
    if (name == pointOfRef) {
        std::cout << "Planet Frame of Reference:" << name << std::endl;
        int k = 0;
        for (const auto& satellite : satellites){
            for (int j = 0; j < 3; ++j) {
                distanceVector[k][j] = position[j] - satellite.position[j];
                distanceSatellite[k] += distanceVector[k][j] * distanceVector[k][j];
            }
            distanceSatellite[k] = sqrt(distanceSatellite[k]);
            acc_satellite[k] = -(G * satellite.mass) / (distanceSatellite[k] * distanceSatellite[k] * distanceSatellite[k]);
            for (int j = 0; j < 3; ++j) {
                acceleration[j] += acc_satellite[k] * distanceVector[k][j];
            }
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
