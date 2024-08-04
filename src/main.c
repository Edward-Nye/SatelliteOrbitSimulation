#include <stdio.h>
#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/satellite.h"
#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/database.h"


int main() {
    Satellite sat;
    double initialPosition[3] = {7000, 0, 0}; // Example values
    double initialVelocity[3] = {0, 7.5, 0};
    double mass = 1000; // in kg

    // Initialize Satellite
    initializeSatellite(&sat, initialPosition, initialVelocity, mass);

    // Open database
    sqlite3 *db;
    if (openDatabase("OrbitalData.db", &db)){
        return 1;
    }

    // Create table
    if (createTable(db)) {
        closeDatabase(db);
        return 1;
    }

    // Insert initial satellite data
    if (insertSatelliteData(db, sat.position, sat.velocity, sat.mass)) {
        closeDatabase(db);
        return 1;
    }



    printf("Initial position: (%f, %f, %f)\n", sat.position[0], sat.position[1], sat.position[2]);
    printf("Initial velocity: (%f, %f, %f)\n", sat.velocity[0], sat.velocity[1], sat.velocity[2]);

    double timeStep = 5; // seconds
    updatePosition(&sat, timeStep);

    printf("Updated position: (%f, %f, %f)\n", sat.position[0], sat.position[1], sat.position[2]);

    double deltaV[3] = {0, 0.1, 0}; // Example maneuver
    applyManeuver(&sat, deltaV);

    printf("Velocity after maneuver: (%f, %f, %f)\n", sat.velocity[0], sat.velocity[1], sat.velocity[2]);

    // Insert updated satellite data
    if (insertSatelliteData(db, sat.position, sat.velocity, sat.mass)) {
        closeDatabase(db);
        return 1;
    }

    // Close database
    closeDatabase(db);

    return 0;
}
