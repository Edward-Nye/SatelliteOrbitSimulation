#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>    // Add this for time-related functions
#include <string.h>
#include <sqlite3.h>
#include <cstdio>   // For fopen, fclose, perror, fprintf, stderr, snprintf, fgets, sscanf, printf
#include <cstdlib>  // For exit, EXIT_FAILURE, malloc, free
#include <cstring>  // For strcmp, strcpy, memcpy


#include "satellite.h"
#include "database.h"
#include "planet.h"


#define MAX_LINE_LENGTH 256
#define TOTAL_DURATION 3600 // Total duration in seconds (e.g., 1 hour)
#define TIME_STEP 1 // Time step in seconds
#define MAX_SATELLITES 100 // Adjust this as needed

Satellite *loadedSatellites[MAX_SATELLITES];
int satelliteCount = 0;
char *satelliteNames[MAX_SATELLITES];
int satelliteNamesCount = 0;

Planet *loadedPlanet = NULL;

void loadFromFile(const char *planetsFile, const char *satellitesFile, const char *planetName, int sunCentered) {
    FILE *file;
    char line[MAX_LINE_LENGTH];

    if (sunCentered != 0) {
        static char modifiedPlanetName[100]; // Adjust size as needed
        snprintf(modifiedPlanetName, sizeof(modifiedPlanetName), "%s_sc", planetName);
        planetName = modifiedPlanetName;
    }
    
    // Load planet data
    if (planetName != NULL) {
        file = fopen(planetsFile, "r");
        if (!file) {
            perror("Unable to open planet file");
            exit(EXIT_FAILURE);
        }

        while (fgets(line, sizeof(line), file)) {
            Planet p;
            char name[50];
            if (sscanf(line, "%49[^,], %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf",
                       name,
                       &p.position[0], &p.position[1], &p.position[2],
                       &p.velocity[0], &p.velocity[1], &p.velocity[2],
                       &p.acceleration[0], &p.acceleration[1], &p.acceleration[2],
                       &p.radius, &p.mass, &p.spin[0], &p.spin[1]) == 14) {
                if (strcmp(name, planetName) == 0) {
                    loadedPlanet = (Planet *)malloc(sizeof(Planet));
                    strcpy(loadedPlanet->name, name);
                    memcpy(loadedPlanet->position, p.position, sizeof(p.position));
                    memcpy(loadedPlanet->velocity, p.velocity, sizeof(p.velocity));
                    memcpy(loadedPlanet->acceleration, p.acceleration, sizeof(p.acceleration));
                    loadedPlanet->radius = p.radius;
                    loadedPlanet->mass = p.mass;
                    memcpy(loadedPlanet->spin, p.spin, sizeof(p.spin));
                    break;
                }
            } else {
                fprintf(stderr, "Error parsing line: %s\n", line);
            }
        }

        fclose(file);
    }

    // Load satellite data
    file = fopen(satellitesFile, "r");
    if (!file) {
        perror("Unable to open satellite file");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {
        Satellite s;
        char name[50];
        if (sscanf(line, "%49[^,], %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf",
                   name,
                   &s.position[0], &s.position[1], &s.position[2],
                   &s.velocity[0], &s.velocity[1], &s.velocity[2],
                   &s.acceleration[0], &s.acceleration[1], &s.acceleration[2],
                   &s.mass) == 11) {
            for (int i = 0; i < satelliteNamesCount; i++) {
                if (strcmp(name, satelliteNames[i]) == 0) {
                    loadedSatellites[satelliteCount] = (Satellite *)malloc(sizeof(Satellite));
                    strcpy(loadedSatellites[satelliteCount]->name, name);
                    memcpy(loadedSatellites[satelliteCount]->position, s.position, sizeof(s.position));
                    memcpy(loadedSatellites[satelliteCount]->velocity, s.velocity, sizeof(s.velocity));
                    memcpy(loadedSatellites[satelliteCount]->acceleration, s.acceleration, sizeof(s.acceleration));
                    loadedSatellites[satelliteCount]->mass = s.mass;
                    satelliteCount++;
                    if (satelliteCount >= MAX_SATELLITES) {
                        fprintf(stderr, "Maximum number of satellites reached.\n");
                        break;
                    }
                }
            }
        } else {
            fprintf(stderr, "Error parsing line: %s\n", line);
        }
    }

    fclose(file);
}



int main(int argc, char *argv[]) {
    const char *planetsFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/planets.txt";
    const char *satellitesFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/satellites.txt";
    const char *planetName = "Earth";  // Specify which planet to load
    int sunCentered = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            sunCentered = 1;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            planetName = argv[++i];
        } else if (strcmp(argv[i], "-sat") == 0) {
            while (i + 1 < argc && argv[i + 1][0] != '-') {
                satelliteNames[satelliteNamesCount] = strdup(argv[++i]);
                satelliteNamesCount++;
                if (satelliteNamesCount >= MAX_SATELLITES) {
                    fprintf(stderr, "Maximum number of satellite names reached.\n");
                    break;
                }
            }
        }
    }

    if (!sunCentered && planetName == NULL) {
        fprintf(stderr, "Error: Planet name must be specified for planet-centered system.\n");
        return 1;
    }

    if (satelliteNamesCount == 0) {
        fprintf(stderr, "Error: At least one satellite name must be specified.\n");
        return 1;
    }

    loadFromFile(planetsFile, satellitesFile, planetName, sunCentered);

    // Check if data was loaded successfully
    if (satelliteCount > 0) {
        printf("Loaded %d satellites:\n", satelliteCount);
        for (int i = 0; i < satelliteCount; i++) {
            printf("Satellite %d: %s\n", i + 1, loadedSatellites[i]->name);
        }
    } else {
        printf("No satellites found in the text file.\n");
        return 1; // Exit if no satellite data is found
    }

    if (loadedPlanet != NULL) {
        printf("Loaded planet: %s\n", loadedPlanet->name);
    } else {
        printf("Planet '%s' not found in the text file.\n", planetName);
        return 1; // Exit if planet data is not found
    }

    // Generate a unique table name using the current timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char satelliteTableName[64];
    char planetTableName[64];
    strftime(satelliteTableName, sizeof(satelliteTableName), "satellite_data_%Y%m%d_%H%M%S", tm_info);
    strftime(planetTableName, sizeof(planetTableName), "planet_data_%Y%m%d_%H%M%S", tm_info);

    // Open database
    sqlite3 *db;
    if (openDatabase("/Users/edward/Documents/SatelliteOrbitSimulation/data/OrbitalData.db", &db)){
        return 1;
    }

    // Create table
    if (createTables(db, satelliteTableName, planetTableName)) {
        closeDatabase(db);
        return 1;
    }

    for (int t = 0; t <= TOTAL_DURATION; t += TIME_STEP) {
        // Insert satellite data
        for (int i = 0; i < satelliteCount; i++) {
            if (insertSatelliteData(db, satelliteTableName, loadedSatellites[i]->name, loadedSatellites[i]->position, loadedSatellites[i]->velocity, loadedSatellites[i]->acceleration, loadedSatellites[i]->mass, t)) {
                closeDatabase(db);
                return 1;
            }
        }

        // Insert planet data
        if (insertPlanetData(db, planetTableName, loadedPlanet->name, loadedPlanet->position, loadedPlanet->velocity, loadedPlanet->acceleration, loadedPlanet->mass, t, loadedPlanet->spin)) {
            closeDatabase(db);
            return 1;
        }

        // Update positions
        for (int i = 0; i < satelliteCount; i++) {
            satupdatePosition(loadedSatellites[i], loadedPlanet, TIME_STEP);
        }
        PositionUpdate(loadedPlanet, TIME_STEP);

        // Apply dummy maneuvers for demonstration; you might have actual maneuver logic here
        for (int i = 0; i < satelliteCount; i++) {
            double deltaV[3] = {0, 0.1, 0}; // Example maneuver
            applyManeuver(loadedSatellites[i], deltaV);
        }

        // Output to console (optional)
        printf("Time: %d seconds\n", t);
        for (int i = 0; i < satelliteCount; i++) {
            printf("Satellite %d Position: (%f, %f, %f)\n", i + 1, loadedSatellites[i]->position[0], loadedSatellites[i]->position[1], loadedSatellites[i]->position[2]);
            printf("Satellite %d Velocity: (%f, %f, %f)\n", i + 1, loadedSatellites[i]->velocity[0], loadedSatellites[i]->velocity[1], loadedSatellites[i]->velocity[2]);
            printf("Satellite %d Acceleration: (%f, %f, %f)\n", i + 1, loadedSatellites[i]->acceleration[0], loadedSatellites[i]->acceleration[1], loadedSatellites[i]->acceleration[2]);
        }
    }

    // Close the database
    if (closeDatabase(db)) {
        return 1;
    }

    // Free memory
    for (int i = 0; i < satelliteCount; i++) {
        free(loadedSatellites[i]);
    }
    free(loadedPlanet);

    // Free satellite names memory
    for (int i = 0; i < satelliteNamesCount; i++) {
        free(satelliteNames[i]);
    }

    return 0;
}
