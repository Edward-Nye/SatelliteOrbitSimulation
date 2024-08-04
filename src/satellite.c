#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/satellite.h"

#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> // Include this for malloc and exit

#define G 6.67430E-11 // Gravitational Constant
#define earthMass 5.974E24

Satellite* initializeSatellite(const char *name, double pos[], double vel[], double acc[], double mass) {
    // Allocate memory for the satellite struct including space for the name
    size_t nameLength = strlen(name) + 1; // +1 for the null terminator
    Satellite *sat = (Satellite *)malloc(sizeof(Satellite) + nameLength);
    
    // Check if memory allocation was successful
    if (sat == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the satellite properties
    for (size_t i = 0; i < 3; ++i) {
        sat->position[i] = pos[i];
        sat->velocity[i] = vel[i];
    }
    sat->mass = mass;
    strcpy(sat->name, name); // Copy the name into the flexible array member
    
    return sat;
}

void updatePosition(Satellite* sat, double timeStep) {
    
    double R =  sqrt(pow(sat->position[0],2) + pow(sat->position[1],2) + pow(sat->position[2],2));
    double Ap = -(G * earthMass)/pow(R,3);
    for (size_t i = 0; i < 3; ++i) {
        sat->acceleration[i] = sat->position[i] * Ap;
        sat->velocity[i] += sat->acceleration[i] * timeStep;  //Change in Velocity
        sat->position[i] += sat->velocity[i] * timeStep;      //Change in Position
        
    }
}

void applyManeuver(Satellite* sat, double deltaV[]) {
    for (size_t i = 0; i < 3; ++i) {
        sat->velocity[i] += deltaV[i];
    }
}
