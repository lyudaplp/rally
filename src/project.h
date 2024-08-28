#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *surname;
    char *team;
    int hours;
    int minutes;
    int seconds;
} Driver;

// Structure for managing the database of drivers
typedef struct {
    Driver *drivers;
    int num_drivers;
    int capacity;
} RallyRace;

int total_seconds(const Driver *driver) {
    return driver->hours * 3600 + driver->minutes * 60 + driver->seconds;
}


// Function to sort the drivers by their race time
void sort_drivers_by_time(RallyRace *race) {
    for (int i = 0; i < race->num_drivers - 1; i++) {
        for (int j = 0; j < race->num_drivers - i - 1; j++) {
            int time_j = total_seconds(&race->drivers[j]);
            int time_j1 = total_seconds(&race->drivers[j + 1]);

            // If the current driver's time is greater than the next driver's time
            // swap them
            if ((time_j > time_j1 && time_j1 != 0) || (time_j == 0 && time_j1 != 0)) {
                Driver temp = race->drivers[j];
                race->drivers[j] = race->drivers[j + 1];
                race->drivers[j + 1] = temp;
            }
        }
    }
}

// Function to initialize the RallyRace structure
void init_rally_race(RallyRace *race) {
    race->drivers = NULL;
    race->num_drivers = 0;
    race->capacity = 0;
}

// Function to free all dynamically allocated memory in RallyRace
void free_rally_race(RallyRace *race) {
    for (int i = 0; i < race->num_drivers; i++) {
        free(race->drivers[i].surname);
        free(race->drivers[i].team);
    }
    free(race->drivers);
}

// Function to resize the drivers array
void resize_drivers_exact(RallyRace *race, int additional_drivers) {
    race->capacity = race->num_drivers + additional_drivers;  // Increase capacity by the number of new drivers
    race->drivers = (Driver *)realloc(race->drivers, race->capacity * sizeof(Driver));
    if (!race->drivers) {
        perror("Failed to allocate memory");
        exit(1);
    }
}
