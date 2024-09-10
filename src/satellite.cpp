#include "satellite.hpp"
#include "planet.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <limits> 


// Constructor to initialize the satellite
Satellite::Satellite(const std::string& name, const std::array<double, 3>& pos,
                     const std::array<double, 3>& vel, const std::array<double, 3>& acc,
                     double mass, std::array<double, 6> preState, std::array<double, 3> rot,
                     std::array<double, 3> rotR)
    : name(name), position(pos), velocity(vel), acceleration(acc), mass(mass), preState(preState),
    rotation(rot), rotationRates(rotR) {}


// Method to update the satellite's position based on the gravitational force from a planet
void Satellite::updatePosition(const std::vector<Planet>& planets, const std::vector<Satellite>& satellites, double timeStep, const std::string& pointOfRef, double time) {
    // Compute the distance vector between the satellite and the planet
    std::vector<std::vector<double>> distanceVector(planets.size(), std::vector<double>(3, 0.0));
    std::vector<std::vector<double>> SSdistanceVector(satellites.size(), std::vector<double>(3, 0.0));
    std::vector<std::vector<double>> SPdistanceVector(satellites.size(), std::vector<double>(3, 0.0));
    const double G = 6.67430E-11;
    std::vector<double> distancePlanet(planets.size(), 0.0); // Distance from Satellite to Planet
    std::vector<double> distancePlanetS(satellites.size(), 0.0);
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
        } else {
            clSatellite = -1; 
        }
                     
        std::cout << "Made It!!" << std::endl;
        std::cout << planetSOI[clPlanet] << ", " << distancePlanet[clPlanet] << std::endl;
        // Update the acceleration based on the gravitational force
        // Inside planetSOI
        i = 0;
        for (const auto& planet : planets){
            std::cout << planet.name << " SOI: " << planetSOI[i] << ", " << "Distance: " << distancePlanet[i] << std::endl;

            if (planetSOI[clPlanet] >= distancePlanet[i]) {
                std::array<double, 3> velocityRel;
                std::array<double, 3> positionRel;

                bool isZero = std::all_of(preState.begin(), preState.end(), [](double x) { return x == 0.0; });
            
                if (isZero){
                    if (clSatellite == -1){
                        for (int j = 0; j < 3; ++j) {
                            acceleration[j] = (acc_planet[i] * distanceVector[i][j]) + (acc_sun * distanceVector[i][j]);
                        
                        }
                    } else if (clSatellite >= 0){
                        k = 0;
                        for (const auto& satellite : satellites){
                            if (distanceOfSatellite[k] < 1.5E9) {
                                for (int j = 0; j < 3; ++j) {
                                    SSacceleration[j] += (acc_satellite[k] * SSdistanceVector[k][j]);
                                }    
                            } else {
                                for (int j = 0; j < 3; ++j) {
                                    SSacceleration[j] = 0.0;
                                } 
                            }
                            ++k;
                        }
                        for (int j = 0; j < 3; ++j) {
                            acceleration[j] = (acc_planet[i] * distanceVector[i][j]) + (acc_sun * distanceVector[i][j]) + (SSacceleration[j]);
                        }
                    
                    
                    }    
                    std::cout << "First Itteration IN planetSOI" << std::endl;
                    std::cout << name << " Is IN planetSOI: " << planetSOI[i] << " (" << planet.name << ") >= " << distancePlanet[i] << std::endl;
                    // Update velocity with the current acceleration
                    for (int j = 0; j < 3; ++j) {
                        velocity[j] += acceleration[j] * timeStep;
                        velocityRel[j] = velocity[j] - planet.velocity[j]; 
                    }

                    // Update position with the current velocity
                    for (int j = 0; j < 3; ++j) {
                        position[j] += velocity[j] * timeStep;
                        positionRel[j] = position[j] - planet.position[j];
                    }
                    std::cout << "Relative position: " << positionRel[0] << ", " << positionRel[1] << ", " << positionRel[2] << std::endl;
                    std::cout << "Relative velocity: " << velocityRel[0] << ", " << velocityRel[1] << ", " << velocityRel[2] << std::endl;
                    std::cout << "Relative acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << std::endl;
        
                } else {
                    distanceSun = 0.0;
                    distancePlanet[i] = 0.0;
                    std::fill(distanceSatellite.begin(), distanceSatellite.end(), 0.0);
                    for (int j = 0; j < 3; ++j) {
                        distancePlanet[i] += preState[j] * preState[j];
                        distanceSun += (preState[j] + planet.position[j]) * (preState[j] + planet.position[j]);
                        k = 0;
                        for (const auto& satellite : satellites){
                            if (name != satellite.name) {
                                SSdistanceVector[k][j] = ((preState[j] + planet.position[j]) - satellite.position[j]);
                                distanceSatellite[k] += SSdistanceVector[k][j] * SSdistanceVector[k][j];
                            }
                            ++k;
                        }
                        std::cout << "BEGIN preState position: " << preState[j] << ", " <<  "preState velocity :" << preState[j+3] << std::endl; 
                    }
                    

                    distanceSun = sqrt(distanceSun);
                    distancePlanet[i] = sqrt(distancePlanet[i]);

                    acc_sun = -(G * massSun) / (distanceSun * distanceSun * distanceSun);
                    acc_planet[i] = -(G * planet.mass) / (distancePlanet[i] * distancePlanet[i] * distancePlanet[i]);
                    
                    if (clSatellite == -1){
                        for (int j = 0; j < 3; ++j) {
                        acceleration[j] = (acc_planet[i] * preState[j]) + (acc_sun * preState[j]);
                        }
                    } else if (clSatellite >= 0){
                        k = 0;
                        for (const auto& satellite : satellites){
                            distanceSatellite[k] = sqrt(distanceSatellite[k]);
                            if (distanceOfSatellite[k] < 1.5E9 && satellite.name != name) {
                                for (int j = 0; j < 3; ++j) {
                                    acc_satellite[k] = -(G * satellite.mass) / (distanceSatellite[k] * distanceSatellite[k] * distanceSatellite[k]);
                                    SSacceleration[j] += (acc_satellite[k] * SSdistanceVector[k][j]);
                                    std::cout << "TEST_VEL: " << acc_satellite[k] << std::endl;
                                }    
                            } else {
                                for (int j = 0; j < 3; ++j) {
                                    SSacceleration[j] = 0.0;
                                } 
                            }
                            ++k;
                        }
                        for (int j = 0; j < 3; ++j) {
                            
                            acceleration[j] = (acc_planet[i] * preState[j]) + (acc_sun * preState[j]) + (SSacceleration[j]);
                        }  
                    } 

                    std::cout << "Itteration IN planetSOI" << std::endl;
                    std::cout << name << " Is IN planetSOI: " << planetSOI[i] << " (" << planet.name << ") >= " << distancePlanet[i] << std::endl;
                    // Update velocity with the current acceleration
                    for (int j = 0; j < 3; ++j) { 
                        std::cout << "PS_VEL: " << acceleration[j] << std::endl;
                        velocityRel[j] = preState[j+3] + (acceleration[j] * timeStep);
                        velocity[j] = (velocityRel[j] + planet.velocity[j]) ;
                        std::cout << "PLAN POS: " << planet.position[j] << std::endl;
                        std::cout << "VEL: " << velocity[j] << std::endl;
                        std::cout << "VEL REL: " << velocityRel[j] << std::endl; 
                    }
                
                    // Update position with the current velocity
                    for (int j = 0; j < 3; ++j) {
                        positionRel[j] = preState[j] + (velocityRel[j] * timeStep);
                        position[j] = (preState[j] + planet.position[j]);
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
            if (planetSOI[clPlanet] < distancePlanet[i]) {
                for (int j = 0; j < 3; ++j) {
                    acceleration[j] = (acc_sun * position[j]); 
                }
                
                for (int j = 0; j < 3; ++j) {
                    acceleration[j] += (acc_planet[i] * distanceVector[i][j]);  
                }
                std::cout << name << " Is OUT planetSOI: " << planetSOI[i] << " (" << planet.name << ") < " << distancePlanet[i] << std::endl;
                // Update velocity with the current acceleration
                for (int j = 0; j < 3; ++j) {
                    velocity[j] += acceleration[j] * timeStep;
                }

                // Update position with the current velocity
                for (int j = 0; j < 3; ++j) {
                    position[j] += velocity[j] * timeStep;
                }
                if (planets[clPlanet].name == planet.name) {
                    preState = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
                }
            }
            ++i;
        }
    }


    int i = 0;
    std::vector<double> distanceOfSatellite(satellites.size(), 0.0);
    std::vector<double> satelliteSOI(satellites.size(), 0.0);
    std::vector<double> acc_satellite(satellites.size(), 0.0);
    std::vector<double> SSacceleration(3, 0.0);
    for (const auto& planet : planets){
        if (planet.name == "EARTH") {
            std::cout << "Satellite Frame of Reference:" << planet.name << std::endl;
            for (int j = 0; j < 3; ++j) {
                if (i < distanceVector.size() && j < distanceVector[i].size()) {
                    distanceVector[i][j] = position[j] - planet.position[j];
                    distancePlanet[i] += distanceVector[i][j] * distanceVector[i][j];
                }
            }
            int k = 0;
            
            for (const auto& satellite : satellites){
                if (k < SSdistanceVector.size() && k < SPdistanceVector.size() && k < distanceSatellite.size() && k < distanceOfSatellite.size()) {
                    std::cout << "Satellites: "<< name << ", " << satellite.name << std::endl;
                    if (name != satellite.name){
                        for (int j = 0; j < 3; ++j) {
                            SSdistanceVector[k][j] = position[j] - satellite.position[j];
                            SPdistanceVector[k][j] = satellite.position[j] - planet.position[j];
                            distanceSatellite[k] += SSdistanceVector[k][j] * SSdistanceVector[k][j];
                            distanceOfSatellite[k] += SPdistanceVector[k][j] * SPdistanceVector[k][j]; 
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
                }
                
                ++k;
            }
            // Initialize variables to store the smallest and second smallest elements
            double smallest = std::numeric_limits<double>::max();
            double clSatellite = std::numeric_limits<double>::max();
            

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
                                acc_satellite[k] = -(G * satellite.mass) / (distanceSatellite[k] * distanceSatellite[k] * distanceSatellite[k]);
                                SSacceleration[j] += (acc_satellite[k] * SSdistanceVector[k][j]);
                                std::cout << "SS acceleration: "<< SSacceleration[j] << std::endl;
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




void Satellite::SAS(std::string mode, const std::vector<Planet>& planets, const std::string& pointOfRef, double timeStep) {
    std::array<double, 3> target{0.0, 0.0, 0.0};
    std::array<double, 3> direction{0.0, 0.0, 0.0};
    std::array<double, 3> downDirection{0.0, 0.0, 0.0};
    double yaw = 0.0;
    double pitch = 0.0;
    double roll = 0.0;

    for (const auto& planet : planets){
        if (planet.name == pointOfRef) {
            for (int j = 0; j < 3; ++j){
                target[j] = planet.position[j];
            }
        } else {
            break;
        }
    }
    
    if (mode == "Prograde") {
        // Yaw aligns with the velocity direction
        yaw = atan2(velocity[1], velocity[0]); // Calculate heading (yaw)
        pitch = atan2(velocity[2], sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1])); // Pitch relative to the ground plane
        
    }
    else if (mode == "Retrograde") {
        // Same as prograde but opposite direction
        yaw = atan2(-velocity[1], -velocity[0]);
        pitch = atan2(-velocity[2], sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1]));
        
    }
    else if (mode == "radIN") {
        // Aim towards a specific target (e.g., another satellite or ground station)
        for (int j = 0; j < 3; ++j){
            direction[j] = target[j] - position[j];
        }
        
        yaw = atan2(direction[1], direction[0]);
        pitch = atan2(direction[2], sqrt(direction[0] * direction[0] + direction[1] * direction[1]));
        
    }
    else if (mode == "radOUT") {
        // Nadir means pointing directly towards the center of the orbiting body
        for (int j = 0; j < 3; ++j){
            direction[j] = -(target[j] - position[j]);
        }  // Vector from satellite to the center
        yaw = atan2(downDirection[1], downDirection[0]);
        pitch = atan2(downDirection[2], sqrt(downDirection[0] * downDirection[0] + downDirection[1] * downDirection[1]));
        
    }
    else if (mode == "Fixed") {
        // Do nothing, keep current rotation values
        roll = rotation[0];
        pitch = rotation[1];
        yaw = rotation[2];
    }
    else {
        std::cerr << "Unknown mode: " << mode << std::endl;
    }

    rotationRates[0] = (roll - rotation[0])/timeStep;
    rotationRates[1] = (pitch - rotation[1])/timeStep;
    rotationRates[2] = (yaw - rotation[2])/timeStep;

    rotation[0] = roll;
    rotation[1] = pitch;
    rotation[2] = yaw;

}




/* Method to apply a maneuver (change in velocity) to the satellite
void Satellite::applyManeuver(const std::array<double, 3>& deltaV) {
    for (int i = 0; i < 3; ++i) {
        velocity[i] += deltaV[i];

    }
*/
inline double deg2rad(double degrees) {
    return degrees * M_PI / 180.0;
}

void Satellite::getDCM(double DCM[3][3]) {
    double yaw = deg2rad(rotation[2]);
    double pitch = deg2rad(rotation[1]);
    double roll = deg2rad(rotation[0]);

    // Calculate trigonometric values
    double cy = cos(yaw);
    double sy = sin(yaw);
    double cp = cos(pitch);
    double sp = sin(pitch);
    double cr = cos(roll);
    double sr = sin(roll);
    
     // Compute the Direction Cosine Matrix (DCM)
    DCM[0][0] = cy * cp;
    DCM[0][1] = cy * sp * sr - sy * cr;
    DCM[0][2] = cy * sp * cr + sy * sr;

    DCM[1][0] = sy * cp;
    DCM[1][1] = sy * sp * sr + cy * cr;
    DCM[1][2] = sy * sp * cr - cy * sr;

    DCM[2][0] = -sp;
    DCM[2][1] = cp * sr;
    DCM[2][2] = cp * cr;
}

