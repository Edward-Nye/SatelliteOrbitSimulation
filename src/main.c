#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>    // Add this for time-related functions
#include <string.h>
#include <sqlite3.h>

#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/satellite.h"
#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/database.h"

#define TOTAL_DURATION 3600 // Total duration in seconds (e.g., 1 hour)
#define TIME_STEP 1 // Time step in seconds

int main() {
    
    
    double initialPosition[3] = {2E6, 1, 1}; // Example values
    double initialVelocity[3] = {0, 7.79E3, 0};
    double initialAcceleration[3] = {0, 0, 0};
    double mass = 1000; // in kg
    const char *satName = "NYESAT";

    // Generate a unique table name using the current timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char tableName[64];
    strftime(tableName, sizeof(tableName), "satellite_data_%Y%m%d_%H%M%S", tm_info);

    // Initialize Satellite
    Satellite *sat = initializeSatellite(satName, initialPosition, initialVelocity, initialAcceleration, mass);

    // Open database
    sqlite3 *db;
    if (openDatabase("/Users/edward/Documents/SatelliteOrbitSimulation/data/OrbitalData.db", &db)){
        free(sat);
        return 1;
    }

    // Create table
    if (createTable(db, tableName)) {
        closeDatabase(db);
        free(sat);
        return 1;
    }

    for (int t = 0; t <= TOTAL_DURATION; t += TIME_STEP) {
        
        // Insert initial satellite data
        if (insertSatelliteData(db, tableName, sat->name, sat->position, sat->velocity, sat->acceleration, sat->mass, t)) {
            closeDatabase(db);
            free(sat);
            return 1;
        }

        // Update the satellite's position
        updatePosition(sat, TIME_STEP);

        // Apply a dummy maneuver for demonstration; you might have actual maneuver logic here
        double deltaV[3] = {0, 0.1, 0}; // Example maneuver
        applyManeuver(sat, deltaV);

        // Output to console (optional)
        printf("Time: %d seconds\n", t);
        printf("Position: (%f, %f, %f)\n", sat->position[0], sat->position[1], sat->position[2]);
        printf("Velocity: (%f, %f, %f)\n", sat->velocity[0], sat->velocity[1], sat->velocity[2]);
        printf("Acceleration: (%f, %f, %f)\n", sat->acceleration[0], sat->acceleration[1], sat->acceleration[2]);
    }

    // Close the database
    if (closeDatabase(db)) {
        free(sat);
        return 1;
    }
    free(sat);

    return 0;
}