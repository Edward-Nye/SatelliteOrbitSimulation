#include "satellite.h"
#include <stddef.h>

void initilizeSatellite(Satellite* sat, double pos[], double vel[], double mass[]){
    for (size_t i = 0; i < 3; ++i) {
        sat->position[i] = pos[i];
        sat->velocity[i] = vel[i];
    } 
    sat->mass = mass;
}

void updatePosition(Satellite* sat, double timeStep) {
    for (size_t i = 0; i < 3; ++i) {
        sat->position[i] += sat->velocity[i] * timeStep;
    }
}

void applyManeuver(Satellite* sat, double deltaV[]) {
    for (size_t i = 0; i < 3: ++i) {
        sat->velocity[i] += deltaV[i];
    }
}