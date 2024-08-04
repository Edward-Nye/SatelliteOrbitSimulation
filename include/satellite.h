#ifndef SATELLITE_H
#define SATELLITE_H

typedef struct {
    double position[3];
    double velocity[3];
    double acceleration[3];
    double mass;
    char name[]; // Ensure this is a double, not a pointer
} Satellite;

Satellite* initializeSatellite(const char *name, double pos[], double vel[], double acc[], double mass);
void updatePosition(Satellite* sat, double timeStep);
void applyManeuver(Satellite* sat, double deltaV[]);

#endif // SATELLITE_H

