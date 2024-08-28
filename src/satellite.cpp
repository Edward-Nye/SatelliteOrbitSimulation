#include "satellite.hpp"
#include "planet.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <limits> 


// Constructor to initialize the satellite
Satellite::Satellite(const std::string& name, const std::array<double, 3>& pos,
                     const std::array<double, 3>& vel, const std::array<double, 3>& acc,
                     double mass, std::array<double, 6> preState)
    : name(name), position(pos), velocity(vel), acceleration(acc), mass(mass), preState(preState) {}

void calculateDistanceVectors(const std::array<double, 3>& pos1, const std::array<double, 3>& pos2, std::array<double, 3>& distVec, double& distSq) {
    distSq = 0.0;
    for (int i = 0; i < 3; ++i) {
        distVec[i] = pos1[i] - pos2[i];
        distSq += distVec[i] * distVec[i];
    }
}

// Helper function to calculate gravitational acceleration
double calculateGravitationalAcceleration(double mass, double distance) {
    const double G = 6.67430E-11;
    return -(G * mass) / (distance * distance * distance);
}

// Method to update the satellite's position based on the gravitational force from a planet
void Satellite::updatePosition(const std::vector<Planet>& planets, const std::vector<Satellite>& satellites, double timeStep, const std::string& pointOfRef, double time) {
    // Compute the distance vector between the satellite and the planet
    std::vector<std::vector<double>> distanceVector(planets.size(), std::vector<double>(3, 0.0));
    std::vector<std::vector<double>> SSdistanceVector(satellites.size(), std::vector<double>(3, 0.0));
    std::vector<std::vector<double>> SPdistanceVector(satellites.size(), std::vector<double>(3, 0.0));
    
    std::vector<double> distancePlanet(planets.size(), 0.0); // Distance from Satellite to Planet
    std::vector<double> distanceSatellite(satellites.size(), 0.0);
    if (pointOfRef == "SUN") {
        double massSun =  1.988400E30;
        double distanceSun = 0.0; // Distance from Satellite to Sun
        std::vector<double> distanceOfPlanet(planets.size(), 0.0);
        std::vector<double> distanceOfSatellite(satellites.size(), 0.0); //Distance from Planet to Sun
        std::vector<double> planetSOI(planets.size(), 0.0);
        std::vector<double> satelliteSOI(satellites.size(), 0.0);
        std::vector<double> acc_planet(planets.size(), 0.0);
        std::vector<double> acc_satellite(satellites.size(), 0.0);
        std::vector<double> SSacceleration(3, 0.0);
        int i = 0;
        int k = 0;
        for (const auto& planet : planets){
                
            for (int j = 0; j < 3; ++j) {
                distanceVector[i][j] = position[j] - planet.position[j];
                distancePlanet[i] += distanceVector[i][j] * distanceVector[i][j];
                distanceSun += position[j] * position[j];
                distanceOfPlanet[i] += planet.position[j] * planet.position[j];
            }

            // Compute the gravitational force
            distanceSun = sqrt(distanceSun);
            distancePlanet[i] = sqrt(distancePlanet[i]);
            distanceOfPlanet[i] = sqrt(distanceOfPlanet[i]);
            // Defermine if Planet's planetSOI
            planetSOI[i] = distanceOfPlanet[i] * pow((planet.mass/massSun),(2.0/5.0));
            std::cout << planet.name << " Mass: " << planet.mass << std::endl;

            acc_planet[i] = -(G * planet.mass) / (distancePlanet[i] * distancePlanet[i] * distancePlanet[i]);
            ++i;
        }

        double acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
        // Find the iterator to the smallest element
        auto minIt = std::min_element(distancePlanet.begin(), distancePlanet.end());
    
        // Calculate the position of the smallest element
        size_t clPlanet = std::distance(distancePlanet.begin(), minIt);

        for (const auto& satellite : satellites){
            if (name != satellite.name){
                for (int j = 0; j < 3; ++j) {
                    SSdistanceVector[k][j] = position[j] - satellite.position[j];
                    SPdistanceVector[k][j] = satellite.position[j] - planets[clPlanet].position[j];
                    distanceSatellite[k] += SSdistanceVector[k][j] * SSdistanceVector[k][j];
                    distanceOfSatellite[k] += SPdistanceVector[k][j] *SPdistanceVector[k][j]; 
                }
                distanceSatellite[k] = sqrt(distanceSatellite[k]);
                distanceOfSatellite[k] = sqrt(distanceOfSatellite[k]);
                if (distanceOfSatellite[k] < 1.5E9) {
                    satelliteSOI[k] = distanceOfSatellite[k] * pow((satellite.mass/planets[clPlanet].mass),(2.0/5.0));
                    acc_satellite[k] = -(G * satellite.mass) / (distanceSatellite[k] * distanceSatellite[k] * distanceSatellite[k]);
                }
                if (distanceOfSatellite[k] >= 1.5E9) {
                    satelliteSOI[k] = 0.0;
                    acc_satellite[k] = 0;
                }
            }
            if (name == satellite.name){
                distanceSatellite[k] = 0.0;
                distanceOfSatellite[k] = 0.0;
                satelliteSOI[k] = 0.0;
                acc_satellite[k] = 0.0;
            }
            ++k;
        }
        
        // Initialize variables to store the smallest and second smallest elements
        int smallest = std::numeric_limits<int>::max();
        int clSatellite = std::numeric_limits<int>::max();
        if (distanceSatellite.size() >= 2) {
        
            

            // First loop: find the smallest element
            for (size_t j = 0; j < satellites.size(); ++j) {
                if (distanceSatellite[j] < smallest) {
                    smallest = distanceSatellite[j];
                }
            }

            // Second loop: find the second smallest element
            for (size_t j = 0; j < distanceSatellite.size(); ++j) {
                if (distanceSatellite[j] > smallest && distanceSatellite[j] < clSatellite) {
                    clSatellite = distanceSatellite[j];
                }
            }
        }
        if (distanceSatellite.size() < 2) {
            clSatellite = -1; 
        }
                            

        // Update the acceleration based on the gravitational force
        // Inside planetSOI
        if (planetSOI[clPlanet] >= distancePlanet[clPlanet]) {
            std::array<double, 3> velocityRel;
            std::array<double, 3> positionRel;

            bool isZero = std::all_of(preState.begin(), preState.end(), [](double x) { return x == 0.0; });
            
            if (isZero){
                if (clSatellite == -1){
                    for (int j = 0; j < 3; ++j) {
                        acceleration[j] = (acc_planet[clPlanet] * distanceVector[clPlanet][j]) + (acc_sun * distanceVector[clPlanet][j]);
                    }
                }                
                if (clSatellite >= 0){
                    k = 0;
                    for (const auto& satellite : satellites){
                        if (distanceOfSatellite[k] < 1.5E9) {
                            for (int j = 0; j < 3; ++j) {
                                SSacceleration[j] += (acc_satellite[k] * SSdistanceVector[k][j]);
                            }    
                        }
                        if (distanceOfSatellite[k] >= 1.5E9) {
                            for (int j = 0; j < 3; ++j) {
                                SSacceleration[j] = 0.0;
                            } 
                        }
                        ++k;
                    }
                    for (int j = 0; j < 3; ++j) {
                        acceleration[j] = (acc_planet[clPlanet] * distanceVector[clPlanet][j]) + (acc_sun * distanceVector[clPlanet][j]) + (SSacceleration[j]);
                    }
                    
                    
                }    
                std::cout << "First Itteration IN planetSOI" << std::endl;
                std::cout << name << " Is IN planetSOI: " << planetSOI[clPlanet] << " >= " << distancePlanet[clPlanet] << std::endl;
                // Update velocity with the current acceleration
                for (int j = 0; j < 3; ++j) {
                    velocity[j] += acceleration[j] * timeStep;
                    velocityRel[j] = velocity[j] - planets[clPlanet].velocity[j]; 
                }

                // Update position with the current velocity
                for (int j = 0; j < 3; ++j) {
                    position[j] += velocity[j] * timeStep;
                    positionRel[j] = position[j] - planets[clPlanet].position[j];
                }
                std::cout << "Relative position: " << positionRel[0] << ", " << positionRel[1] << ", " << positionRel[2] << std::endl;
                std::cout << "Relative velocity: " << velocityRel[0] << ", " << velocityRel[1] << ", " << velocityRel[2] << std::endl;
                std::cout << "Relative acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << std::endl;
        
            }
            
            else {
                distanceSun = 0.0;
                distancePlanet.assign(distancePlanet.size(), 0.0);
                distanceSatellite.assign(distanceSatellite.size(), 0.0);
                for (int j = 0; j < 3; ++j) {
                    distancePlanet[clPlanet] += preState[j] * preState[j];
                    distanceSun += (preState[j] + planets[clPlanet].position[j]) * (preState[j] + planets[clPlanet].position[j]);
                    k = 0;
                    for (const auto& satellite : satellites){
                        if (name != satellite.name) {
                            SSdistanceVector[k][j] = ((preState[j] + planets[clPlanet].position[j]) - satellite.position[j]);
                            distanceSatellite[k] += SSdistanceVector[k][j] * SSdistanceVector[k][j];
                        }
                        if (name == satellite.name) {
                            distanceSatellite[k] += 0.0;
                        }
                        ++k;
                    }
                    std::cout << "BEGIN preState position: " << preState[j] << ", " <<  "preState velocity :" << preState[j+3] << std::endl; 
                }

                distanceSun = sqrt(distanceSun);
                distancePlanet[clPlanet] = sqrt(distancePlanet[clPlanet]);

                acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
                acc_planet[clPlanet] = -(G * planets[clPlanet].mass) / (distancePlanet[clPlanet] * distancePlanet[clPlanet] * distancePlanet[clPlanet]);
                
                if (clSatellite == -1){
                    for (int j = 0; j < 3; ++j) {
                        acceleration[j] = (acc_planet[clPlanet] * preState[j]) + (acc_sun * preState[j]);
                    }
                }                
                if (clSatellite >= 0){
                    k = 0;
                    for (const auto& satellite : satellites){
                        distanceSatellite[k] = sqrt(distanceSatellite[k]);
                        if (distanceOfSatellite[k] < 1.5E9) {
                            for (int j = 0; j < 3; ++j) {
                                acc_satellite[j] = -(G * satellite.mass) / (distanceSatellite[k] * distanceSatellite[k] * distanceSatellite[k]);
                                SSacceleration[j] += (acc_satellite[k] * SSdistanceVector[k][j]);
                            }    
                        }
                        if (distanceOfSatellite[k] >= 1.5E9) {
                            for (int j = 0; j < 3; ++j) {
                                SSacceleration[j] = 0.0;
                            } 
                        }
                        ++k;
                    }
                    for (int j = 0; j < 3; ++j) {
                        acceleration[j] = (acc_planet[clPlanet] * preState[j]) + (acc_sun * preState[j]) + (SSacceleration[j]);
                    }  
                } 

                std::cout << "Itteration IN planetSOI" << std::endl;
                std::cout << name << " Is IN planetSOI: " << planetSOI[clPlanet] << " >= " << distancePlanet[clPlanet] << std::endl;
                // Update velocity with the current acceleration
                for (int j = 0; j < 3; ++j) { 
                    velocityRel[j] = preState[j+3] + (acceleration[j] * timeStep);
                    velocity[j] = (velocityRel[j+3] + planets[clPlanet].velocity[j]) ;
                    std::cout << "PLAN POS: " << planets[clPlanet].position[j] << std::endl;
                    std::cout << "VEL: " << velocity[j] << std::endl;
                    std::cout << "VEL REL: " << velocityRel[j] << std::endl; 
                }
                
                // Update position with the current velocity
                for (int j = 0; j < 3; ++j) {
                    positionRel[j] = preState[j] + (velocityRel[j] * timeStep);
                    position[j] = (preState[j] + planets[clPlanet].position[j]);
                    std::cout << "POS: " << position[j] << std::endl;
                    std::cout << "POS_REL: " << positionRel[j] << std::endl;
                }
                std::cout << "Relative position: " << positionRel[0] << ", " << positionRel[1] << ", " << positionRel[2] << std::endl;
                std::cout << "Relative velocity: " << velocityRel[0] << ", " << velocityRel[1] << ", " << velocityRel[2] << std::endl;
                std::cout << "Relative acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << std::endl;
            }

            for (int j = 0; j < 3; ++j) {
                preState[j] = positionRel[j];
                preState[j+3] = velocityRel[j];
                std::cout << "preState position: " << preState[j] << ", " <<  "preState velocity :" << preState[j+3] << std::endl;
                
            }
            
        }


        // Outside planetSOI
        if (planetSOI[clPlanet] < distancePlanet[clPlanet]) {
            for (int j = 0; j < 3; ++j) {
                acceleration[j] = (acc_sun * position[j]); 
            }
            i = 0;
            for (const auto& planet : planets){
                for (int j = 0; j < 3; ++i) {
                    acceleration[j] += (acc_planet[i] * distanceVector[i][j]);   
                }
                ++i;
            }
            std::cout << name << " Is OUT planetSOI: " << planetSOI[clPlanet] << " < " << distancePlanet[clPlanet] << std::endl;
            // Update velocity with the current acceleration
            for (int j = 0; j < 3; ++j) {
                velocity[j] += acceleration[j] * timeStep;
            }

            // Update position with the current velocity
            for (int j = 0; j < 3; ++j) {
                position[j] += velocity[j] * timeStep;
            }
            preState = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        }
    }


    int i = 0;
    std::vector<double> distanceOfSatellite(satellites.size(), 0.0);
    std::vector<double> satelliteSOI(satellites.size(), 0.0);
    std::vector<double> acc_satellite(satellites.size(), 0.0);
    std::vector<double> SSacceleration(3, 0.0);
    for (const auto& planet : planets){
        if (planet.name == pointOfRef) {
            std::cout << "Satellite Frame of Reference:" << planet.name << std::endl;
            for (int j = 0; j < 3; ++j) {
                distanceVector[i][j] = position[j] - planet.position[j];
                distancePlanet[i] += distanceVector[i][j] * distanceVector[i][j];
            }
            int k = 0;
            
            for (const auto& satellite : satellites){
                std::cout << "Satellites: "<< name << ", " << satellite.name << std::endl;
                if (name != satellite.name){
                    for (int j = 0; j < 3; ++j) {
                        SSdistanceVector[k][j] = position[j] - satellite.position[j];
                        SPdistanceVector[k][j] = satellite.position[j] - planet.position[j];
                        distanceSatellite[k] += SSdistanceVector[k][j] * SSdistanceVector[k][j];
                        distanceOfSatellite[k] += SPdistanceVector[k][j] *SPdistanceVector[k][j]; 
                    }
                    std::cout << "Satellite != " << satellite.name << std::endl;
                    distanceSatellite[k] = sqrt(distanceSatellite[k]);
                    distanceOfSatellite[k] = sqrt(distanceOfSatellite[k]);
                    if (distanceOfSatellite[k] < 1.5E9) {
                        satelliteSOI[k] = distanceOfSatellite[k] * pow((satellite.mass/planet.mass),(2.0/5.0));
                        if (distanceSatellite[k] != 0) {
                            acc_satellite[k] = -(G * satellite.mass) / (distanceSatellite[k] * distanceSatellite[k] * distanceSatellite[k]);
                        } else {
                            acc_satellite[k] = 0.0; // Avoid division by zero
                        }
                    } else {
                        satelliteSOI[k] = 0.0;
                        acc_satellite[k] = 0.0;
                    }
                } else {
                    distanceSatellite[k] = 0.0;
                    distanceOfSatellite[k] = 0.0;
                    satelliteSOI[k] = 0.0;
                    acc_satellite[k] = 0.0;
        
                }
                ++k;
            }
            // Initialize variables to store the smallest and second smallest elements
            int smallest = std::numeric_limits<double>::max();
            int clSatellite = std::numeric_limits<double>::max();
            

            if (distanceSatellite.size() >= 2) {

                // First loop: find the smallest element
                for (size_t j = 0; j < distanceSatellite.size(); ++j) {
                    if (distanceSatellite[j] < smallest) {
                        smallest = distanceSatellite[j];
                    }
                }
                
                // Second loop: find the second smallest element
                for (size_t j = 0; j < distanceSatellite.size(); ++j) {
                    if (distanceSatellite[j] > smallest && distanceSatellite[j] < clSatellite) {
                        clSatellite = distanceSatellite[j];
                    }
                }
                
                distancePlanet[i] = sqrt(distancePlanet[i]);
                double acc_planet = -(G * planet.mass) / (distancePlanet[i] * distancePlanet[i] * distancePlanet[i]);
                int k = 0;
                bool inSOI = false;
                for (const auto& satellite : satellites){
                    if (satelliteSOI[k] >= distanceSatellite[k] && name != satellite.name){ // Inside satellites SOI
                        for (int j = 0; j < 3; ++j){
                            acceleration[j] = (acc_satellite[k] * SSdistanceVector[k][j]) + (acc_planet * SSdistanceVector[k][j]);
                        }
                        inSOI = true;
                        std::cout << "SAT IN SOI" << std::endl;
                        break;
                    }
                    
                    ++k;
                 
                }
                if (!inSOI){
                    k = 0;
                    for (const auto& satellite : satellites){
                        if (distanceOfSatellite[k] < 1.5E9 && name != satellite.name) {
                            for (int j = 0; j < 3; ++j) {
                                acc_satellite[j] = -(G * satellite.mass) / (distanceSatellite[k] * distanceSatellite[k] * distanceSatellite[k]);
                                SSacceleration[j] += (acc_satellite[k] * SSdistanceVector[k][j]);
                                std::cout << "SS acceleration: "<< acc_satellite[j] << std::endl;
                            }    
                        } else {
                            for (int j = 0; j < 3; ++j) {
                                SSacceleration[j] = 0.0;
                            } 
                        }
                        
                        ++k;
                    }

                    for (int j = 0; j < 3; ++j) {
                        acceleration[j] = (acc_planet * distanceVector[i][j]) + (SSacceleration[j]);
                    }
                    
                }

            } else {
                clSatellite = -1; 
                distancePlanet[i] = sqrt(distancePlanet[i]);
                double acc_planet = -(G * planet.mass) / (distancePlanet[i] * distancePlanet[i] * distancePlanet[i]);
                for (int j = 0; j < 3; ++j) {
                    acceleration[j] = (acc_planet * distanceVector[i][j]);
                }
            }
            
            
            
            
            //std::cout << planet.mass << ", " << distancePlanet << ", " << G << std::endl;

            

            
            
            // Update velocity with the current acceleration
            for (int j = 0; j < 3; ++j) {
                velocity[j] += acceleration[j] * timeStep;
            }

            // Update position with the current velocity
            for (int j = 0; j < 3; ++j) {
                position[j] += velocity[j] * timeStep;
            }
        }
        std::cout << name << " -Relative position: " << position[0] << ", " << position[1] << ", " << position[2] << std::endl;
        std::cout << name << " -Relative velocity: " << velocity[0] << ", " << velocity[1] << ", " << velocity[2] << std::endl;
        std::cout << name << " -Relative acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << std::endl;

        ++i;
    }
std::cout << "Made It!!" << std::endl;
}

/* Method to apply a maneuver (change in velocity) to the satellite
void Satellite::applyManeuver(const std::array<double, 3>& deltaV) {
    for (int i = 0; i < 3; ++i) {
        velocity[i] += deltaV[i];

    }
*/

