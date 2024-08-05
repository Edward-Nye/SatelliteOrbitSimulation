#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>    // Add this for time-related functions
#include <string.h>
#include <sqlite3.h>

#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/satellite.h"
#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/database.h"
#include "/Users/edward/Documents/SatelliteOrbitSimulation/include/planet.h"


#define MAX_LINE_LENGTH 256
#define TOTAL_DURATION 3600 // Total duration in seconds (e.g., 1 hour)
#define TIME_STEP 1 // Time step in seconds

Satellite *loadedSatellite = NULL;
Planet *loadedPlanet = NULL;

void loadFromFile(const char *planetsFile, const char *satellitesFile, const char *planetName, const char *satelliteName, int sunCentered) {
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
            // Adjusted format to match the new input format
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
    if (satelliteName != NULL) {
        file = fopen(satellitesFile, "r");
        if (!file) {
            perror("Unable to open satellite file");
            exit(EXIT_FAILURE);
        }

        while (fgets(line, sizeof(line), file)) {
            Satellite s;
            char name[50];
            // Adjusted format to match the new input format
            if (sscanf(line, "%49[^,], %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf",
                       name,
                       &s.position[0], &s.position[1], &s.position[2],
                       &s.velocity[0], &s.velocity[1], &s.velocity[2],
                       &s.acceleration[0], &s.acceleration[1], &s.acceleration[2],
                       &s.mass) == 11) {
                if (strcmp(name, satelliteName) == 0) {
                    loadedSatellite = (Satellite *)malloc(sizeof(Satellite));
                    strcpy(loadedSatellite->name, name);
                    memcpy(loadedSatellite->position, s.position, sizeof(s.position));
                    memcpy(loadedSatellite->velocity, s.velocity, sizeof(s.velocity));
                    memcpy(loadedSatellite->acceleration, s.acceleration, sizeof(s.acceleration));
                    loadedSatellite->mass = s.mass;
                    break;
                }
            } else {
                fprintf(stderr, "Error parsing line: %s\n", line);
            }
        }

        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    const char *planetsFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/planets.txt";
    const char *satellitesFile = "/Users/edward/Documents/SatelliteOrbitSimulation/data/satellites.txt";
    const char *planetName = "Earth";  // Specify which planet to load
    const char *satelliteName = "NYESAT"; // Specify which satellite to load
    int sunCentered = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            sunCentered = 1;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            planetName = argv[++i];
        } else if (strcmp(argv[i], "-sat") == 0 && i + 1 < argc) {
            satelliteName = argv[++i];
        }
    }

    if (!sunCentered && planetName == NULL) {
        fprintf(stderr, "Error: Planet name must be specified for planet-centered system.\n");
        return 1;
    }

    if (satelliteName == NULL) {
        fprintf(stderr, "Error: Satellite name must be specified.\n");
        return 1;
    }

    loadFromFile(planetsFile, satellitesFile, planetName, satelliteName, sunCentered);

    // Check if data was loaded successfully
    if (loadedSatellite != NULL) {
        printf("Loaded satellite: %s\n", loadedSatellite->name);
    } else {
        printf("Satellite '%s' not found in the text file.\n", satelliteName);
        return 1; // Exit if satellite data is not found
    }

    if (loadedPlanet != NULL) {
        printf("Loaded planet: %s\n", loadedPlanet->name);
    } else {
        printf("Planet '%s' not found in the text file.\n", planetName);
        return 1; // Exit if planet data is not found
    }

    // Generate a unique table name using the current timestamp
    // Get current time for table names
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
        
        // Insert initial satellite data
        if (insertSatelliteData(db, satelliteTableName, loadedSatellite->name, loadedSatellite->position, loadedSatellite->velocity, loadedSatellite->acceleration, loadedSatellite->mass, t)) {
            closeDatabase(db);
            return 1;
        }
        // Insert initial planet data
        if (insertPlanetData(db, planetTableName, loadedPlanet->name, loadedPlanet->position, loadedPlanet->velocity, loadedPlanet->acceleration, loadedPlanet->mass, t, loadedPlanet->spin)) {
            closeDatabase(db);
            return 1;
        }

        // Update the satellite's position
        satupdatePosition(loadedSatellite, loadedPlanet, TIME_STEP);
        // Update the planet's position
        PositionUpdate(loadedPlanet, TIME_STEP);

        // Apply a dummy maneuver for demonstration; you might have actual maneuver logic here
        double deltaV[3] = {0, 0.1, 0}; // Example maneuver
        applyManeuver(loadedSatellite, deltaV);

        // Output to console (optional)
        printf("Time: %d seconds\n", t);
        printf("Position: (%f, %f, %f)\n", loadedSatellite->position[0], loadedSatellite->position[1], loadedSatellite->position[2]);
        printf("Velocity: (%f, %f, %f)\n", loadedSatellite->velocity[0], loadedSatellite->velocity[1], loadedSatellite->velocity[2]);
        printf("Acceleration: (%f, %f, %f)\n", loadedSatellite->acceleration[0], loadedSatellite->acceleration[1], loadedSatellite->acceleration[2]);
    }

    // Close the database
    if (closeDatabase(db)) {
        return 1;
    }
    
    free(loadedSatellite);
    free(loadedPlanet);

    return 0;
}