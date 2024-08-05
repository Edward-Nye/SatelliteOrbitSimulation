#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/planet.h"

#include <stddef.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> // Include this for malloc and exit

#define G 6.67430E-11 // Gravitational Constant
#define sunMass 1.989E30

Planet* initializePlanet(const char *name, double pos[], double vel[], double acc[], double radius, double mass, double spin[]) {
    // Allocate memory for the satellite struct including space for the name
    size_t nameLength = strlen(name) + 1; // +1 for the null terminator
    Planet *pl = (Planet *)malloc(sizeof(Planet) + nameLength);
    
    // Check if memory allocation was successful
    if (pl == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the satellite properties
    for (size_t i = 0; i < 3; ++i) {
        pl->position[i] = pos[i];
        pl->velocity[i] = vel[i];
        pl->acceleration[i] = acc[i]; 
    }
    pl->radius = radius;
    pl->mass = mass;
    for (size_t i = 0; i < 2; ++i){
        pl->spin[i] = spin[i];
    }
    strcpy(pl->name, name); // Copy the name into the flexible array member
    
    return pl;
}

void plupdatePosition(Planet* pl, double timeStep) {
    
    double R =  sqrt(pow(pl->position[0],2) + pow(pl->position[1],2) + pow(pl->position[2],2));
    double Ap = -(G * sunMass)/pow(R,3);
    for (size_t i = 0; i < 3; ++i) {
        pl->acceleration[i] = pl->position[i] * Ap;
        pl->velocity[i] += pl->acceleration[i] * timeStep;  //Change in Velocity
        pl->position[i] += pl->velocity[i] * timeStep;      //Change in Position
        
    }
}