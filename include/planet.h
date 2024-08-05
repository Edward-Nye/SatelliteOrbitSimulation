#ifndef PLANET_H
#define PLANET_H

typedef struct {
    double position[3];
    double velocity[3];
    double acceleration[3];
    double radius;
    double mass;
    double spin[2];
    char name[]; // Ensure this is a double, not a pointer
} Planet;

Planet* initializePlanet(const char *name, double pos[], double vel[], double acc[], double radius, double mass, double spin[]);
void plupdatePosition(Planet* pl, double timeStep);

#endif // PLANET_H