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
void Satellite::updatePosition(const Planet& planet, double timeStep, const std::string& pointOfRef, std::array<double, 6>& preState, double time) {
    // Compute the distance vector between the satellite and the planet
    std::array<double, 3> distanceVector;
    double G = 6.67430E-11;
    double distancePlanet = 0.0; // Distance from Satellite to Planet
    
    if (pointOfRef == "SUN") {
        double massSun =  1.988400E30;
        double distanceSun = 0.0; // Distance from Satellite to Sun
        double distanceOfPlanet = 0.0;

        for (int i = 0; i < 3; ++i) {
            distanceVector[i] = position[i] - planet.position[i];
            distancePlanet += distanceVector[i] * distanceVector[i];
            distanceSun += position[i] * position[i];
            distanceOfPlanet += planet.position[i] * planet.position[i]; 
        }

        // Compute the gravitational force
        distanceSun = sqrt(distanceSun);
        distancePlanet = sqrt(distancePlanet);
        distanceOfPlanet = sqrt(distanceOfPlanet);
        // Defermine if Planet's SOI
        double SOI = distanceOfPlanet * pow((planet.mass/massSun),(2.0/5.0));
        std::cout << planet.name << " Mass: " << planet.mass << std::endl;

        double acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
        double acc_planet = -(G * planet.mass) / (distancePlanet * distancePlanet * distancePlanet);
    

    
        // Update the acceleration based on the gravitational force
        // Inside SOI
        if (SOI >= distancePlanet) {
            std::array<double, 3> velocityRel;
            std::array<double, 3> positionRel;

            
            
            if (time == 0){
                for (int i = 0; i < 3; ++i) {
                acceleration[i] = (acc_planet * distanceVector[i]) + (acc_sun * distanceVector[i]);
                }
                std::cout << "First Itteration IN SOI" << std::endl;
                std::cout << name << " Is IN SOI: " << SOI << " >= " << distancePlanet << std::endl;
                // Update velocity with the current acceleration
                for (int i = 0; i < 3; ++i) {
                    velocity[i] += acceleration[i] * timeStep;
                    velocityRel[i] = velocity[i] - planet.velocity[i]; 
                }

                // Update position with the current velocity
                for (int i = 0; i < 3; ++i) {
                    position[i] += velocity[i] * timeStep;
                    positionRel[i] = position[i] - planet.position[i];
                }
                std::cout << "Relative position: " << positionRel[0] << ", " << positionRel[1] << ", " << positionRel[2] << std::endl;
                std::cout << "Relative velocity: " << velocityRel[0] << ", " << velocityRel[1] << ", " << velocityRel[2] << std::endl;
                std::cout << "Relative acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << std::endl;
        
            }

            else {
                
                for (int i = 0; i < 3; ++i) {
                    distancePlanet += preState[i] * preState[i];
                    distanceSun += (preState[i] + planet.position[i]) * (preState[i] + planet.position[i]);
                    std::cout << "BEGIN preState position: " << preState[i] << ", " <<  "preState velocity :" << preState[i+3] << std::endl; 
                }
                
                distanceSun = sqrt(distanceSun);
                distancePlanet = sqrt(distancePlanet);

                acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
                acc_planet = -(G * planet.mass) / (distancePlanet * distancePlanet * distancePlanet);

                
                
                for (int i = 0; i < 3; ++i) {
                    acceleration[i] = (acc_planet * preState[i]) + (acc_sun * preState[i]);
                }
                std::cout << "Itteration IN SOI" << std::endl;
                std::cout << name << " Is IN SOI: " << SOI << " >= " << distancePlanet << std::endl;
                // Update velocity with the current acceleration
                for (int i = 0; i < 3; ++i) { 
                    velocityRel[i] = preState[i+3] + (acceleration[i] * timeStep);
                    velocity[i] = (velocityRel[i+3] + planet.velocity[i]) ;
                    std::cout << "PLAN POS: " << planet.position[i] << std::endl;
                    std::cout << "VEL: " << velocity[i] << std::endl;
                    std::cout << "VEL REL: " << velocityRel[i] << std::endl; 
                }
                
                // Update position with the current velocity
                for (int i = 0; i < 3; ++i) {
                    positionRel[i] = preState[i] + (velocityRel[i] * timeStep);
                    position[i] = (preState[i] + planet.position[i]);
                    std::cout << "POS: " << position[i] << std::endl;
                    std::cout << "POS_REL: " << positionRel[i] << std::endl;
                }
                std::cout << "Relative position: " << positionRel[0] << ", " << positionRel[1] << ", " << positionRel[2] << std::endl;
                std::cout << "Relative velocity: " << velocityRel[0] << ", " << velocityRel[1] << ", " << velocityRel[2] << std::endl;
                std::cout << "Relative acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << std::endl;
            }

            for (int i = 0; i < 3; ++i) {
                preState[i] = positionRel[i];
                preState[i+3] = velocityRel[i];
                std::cout << "preState position: " << preState[i] << ", " <<  "preState velocity :" << preState[i+3] << std::endl;
                
            }
            
        }


        // Outside SOI
        if (SOI < distancePlanet) {
            for (int i = 0; i < 3; ++i) {
                acceleration[i] = (acc_sun * position[i]) +(acc_planet * distanceVector[i]);   
            }
            std::cout << name << " Is OUT SOI: " << SOI << " < " << distancePlanet << std::endl;
            // Update velocity with the current acceleration
            for (int i = 0; i < 3; ++i) {
                velocity[i] += acceleration[i] * timeStep;
            }

            // Update position with the current velocity
            for (int i = 0; i < 3; ++i) {
                position[i] += velocity[i] * timeStep;
            }
        }
    }
    

    if (planet.name == pointOfRef) {
        std::cout << "Satellite Frame of Reference:" << planet.name << std::endl;
        for (int i = 0; i < 3; ++i) {
            distanceVector[i] = position[i] - planet.position[i];
            distancePlanet += distanceVector[i] * distanceVector[i];
        }
        distancePlanet = sqrt(distancePlanet);
        //std::cout << planet.mass << ", " << distancePlanet << ", " << G << std::endl;

        double acc_planet = -(G * planet.mass) / (distancePlanet * distancePlanet * distancePlanet);
        for (int i = 0; i < 3; ++i) {
                acceleration[i] = (acc_planet * distanceVector[i]);
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
    

    
}

// Method to apply a maneuver (change in velocity) to the satellite
void Satellite::applyManeuver(const std::array<double, 3>& deltaV) {
    for (int i = 0; i < 3; ++i) {
        velocity[i] += deltaV[i];
    }
}
