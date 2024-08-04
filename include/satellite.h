#ifndef SATELLITE_H
#define SATELLITE_H

typedef struct {
    double position[3];
    double velocity[3];
    double mass;
} Satellite;

void initializeSatelite(Satellite* sat, double pos[], double vel[], double mass);
void updatePosition(Satellite* sat, double timeStep);
void applyManeuver(Satellite* sat, double deltaV[]);


#endif // SATELLITE_H
