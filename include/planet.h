#ifndef PLANET_H
#define PLANET_H

typedef struct {
    char name;
    double position[3];
    double velocity[3];
    double acceleration[3];
    double radius;
    double mass; // Ensure this is a double, not a pointer
} Planet;
