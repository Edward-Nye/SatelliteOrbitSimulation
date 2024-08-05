#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/satellite.h"
#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/planet.h"



#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> // Include this for malloc and exit

#define G 6.67430E-11 // Gravitational Constant


/* void satelliteInitialize(Satellite *sat, const char *name, double pos[], double vel[], double acc[], double mass) {
    // Allocate memory for the satellite struct including space for the name
    strcpy(sat->name, name);
    
    // Check if memory allocation was successful
    if (sat == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the satellite properties
    for (size_t i = 0; i < 3; ++i) {
        sat->position[i] = pos[i];
        sat->velocity[i] = vel[i];
        sat->acceleration[i] = acc[i];
    }
    sat->mass = mass;
    
    
} */

void satupdatePosition(Satellite* sat, Planet *pl, double timeStep) {
    
    double R =  sqrt(pow(sat->position[0],2) + pow(sat->position[1],2) + pow(sat->position[2],2));
    double Ap = -(G * pl->mass)/pow(R,3);
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
