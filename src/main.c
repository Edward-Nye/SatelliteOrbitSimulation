#include <stdio.h>
#include "satellite.h"

int main() {
    Satellite sat;
    double initailPosition[3] = {7000, 0 , 0};
    double initialVelocity[3] = {0, 7.5, 0};
    double mass= 1000;

    initializeSatellite(&sat, initailPosition, initialVelocity, mass);

    printf("Initial position: (%f, %f, %f)\n", sat.position[0], sat.position[1], sat.position[2]);
    printf("Initial velocity: (%f, %f, %f)\n", sat.velocity{0}, sat.velocity[1], sat.velocity[2]);

    double tiimeStep = 1;

    updatePosition(&sat, timeStep);

    printf("Updated position: (%f, %f, %f)\n", sat.position[0], sat.position[1], sat.position[2]);

    double deltaV[3] = {0, 0.1, 0};

    applyManeuver(&sat, deltaV);

    printf("Velocity after maneuver: (%f, %f, %f)\n", sat.velocity[0], sat.velocity[1], sat.velocity[2]);

    return 0;
}