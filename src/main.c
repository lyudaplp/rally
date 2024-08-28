#include<stdio.h>
#include"project.h"
#include <stdlib.h>
#include <string.h>

/*commands 
add the drive           A <surname> <team>
                        A Kankkunen Renault
update driver's time    U <surname> <hour> <minute> <second>
                        U Kankkunen 0 52 16
print race              L
save to file            W <filename>
load to file            O <filename>
quit the programm       Q */

void add_driver(RallyRace *race, const char *surname, const char *team) {
    // Check if driver already exists
    for (int i = 0; i < race->num_drivers; i++) {
        if (strcmp(race->drivers[i].surname, surname) == 0) {
            printf("Error: Driver with surname %s already exists.\n", surname);
            return;
        }
    }

    // Resize the array to accommodate exactly one more driver
    resize_drivers_exact(race, 1);

    // Allocate memory for the new driver's surname and team
    race->drivers[race->num_drivers].surname = (char *)malloc(strlen(surname) + 1); // +1 for the null terminator
    race->drivers[race->num_drivers].team = (char *)malloc(strlen(team) + 1);       // +1 for the null terminator

    if (race->drivers[race->num_drivers].surname == NULL || race->drivers[race->num_drivers].team == NULL) {
        perror("Failed to allocate memory for driver details");
        exit(1);
    }

    // Copy the provided surname and team into the allocated space
    strcpy(race->drivers[race->num_drivers].surname, surname);
    strcpy(race->drivers[race->num_drivers].team, team);

    // Initialize the driver's time
    race->drivers[race->num_drivers].hours = 0;
    race->drivers[race->num_drivers].minutes = 0;
    race->drivers[race->num_drivers].seconds = 0;

    // Increase the count of drivers
    race->num_drivers++;

    printf("SUCCESS\n");
}

// Function to update a driver's time
void update_time(RallyRace *race, const char *surname, int hour, int minute, int second) {
    for (int i = 0; i < race->num_drivers; i++) {
        if (strcmp(race->drivers[i].surname, surname) == 0) {
            // Update time
            race->drivers[i].seconds += second;
            race->drivers[i].minutes += race->drivers[i].seconds / 60;
            race->drivers[i].seconds %= 60;

            race->drivers[i].minutes += minute;
            race->drivers[i].hours += race->drivers[i].minutes / 60;
            race->drivers[i].minutes %= 60;

            race->drivers[i].hours += hour;
            printf("SUCCESS\n");
            return;
        }
    }

    printf("Error: The driver %s doesn't exist.\n", surname);
}

// Function to print driver results
void print(RallyRace *race, FILE *stream) {
    
    sort_drivers_by_time(race);
    for (int i = 0; i < race->num_drivers; i++) {
        fprintf(stream, "%s %s %d %d %d\n",
                race->drivers[i].surname,
                race->drivers[i].team,
                race->drivers[i].hours,
                race->drivers[i].minutes,
                race->drivers[i].seconds);
    }
    if (stream == stdout) {
        printf("SUCCESS\n");
    }
}

// Function to save driver results to a file
void save_to_file(RallyRace *race, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to open file %s for writing.\n", filename);
        return;
    }

    print(race, file);
    fclose(file);
    printf("SUCCESS\n");
}


void load_from_file(RallyRace *race, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s for reading.\n", filename);
        return;
    }

    free_rally_race(race);
    init_rally_race(race);

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char surname[100], team[100];
        int hours, minutes, seconds;
        if (sscanf(line, "%99s %99s %d %d %d", surname, team, &hours, &minutes, &seconds) == 5) {
            add_driver(race, surname, team);
            race->drivers[race->num_drivers - 1].hours = hours;
            race->drivers[race->num_drivers - 1].minutes = minutes;
            race->drivers[race->num_drivers - 1].seconds = seconds;
        } else {
            printf("Error: Invalid data format in file %s.\n", filename);
            free_rally_race(race);
            init_rally_race(race);
            fclose(file);
            return;
        }
    }

    fclose(file);
}

void quit(RallyRace *race) {
    free_rally_race(race);
    printf("SUCCESS.\n");
}

int main(void) {

    RallyRace race;
    init_rally_race(&race);
    char command[256];
    char action;

    while (1) {
        // printf("Enter command: ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("fgets failed");
            return 1;
        }

        // Get the first letter of the command
        if (sscanf(command, "%c", &action) != 1) {
            continue;  // if fail continue while
        }

        // Commands
        if (action == 'A') {
            char surname[100], team[100];
            // A Kankkunen Renault
            if (sscanf(command + 1, "%s %s", surname, team) == 2) {
                add_driver(&race, surname, team);
            } else {
                printf("Invalid parameters for command A.\n");
            }
        } else if (action == 'L') {
            
            print(&race, stdout);  // Print to the console
        } else if (action == 'U') {
            // U Kankkunen 0 52 16
            char surname[100];
            int hour, minute, second;
            if (sscanf(command + 1, "%s %d %d %d", surname, &hour, &minute, &second) == 4) {
                if (hour < 0 ) {
                    printf("Hour cannot be negative.\n");
                } else if (minute < 0 || minute > 59){
                    printf("Minute cannot be negative or greater than 59.\n");
                } else if (second < 0 || second > 59){
                    printf("Second cannot be negative or greater than 59.\n");
                } else {
                    update_time(&race, surname, hour, minute, second);
                }
            } else {
                printf("Invalid parameters for command U.\n");
            }
        } else if (action == 'W') {
            char filename[100];
            if (sscanf(command + 1, "%s", filename) == 1) {
                save_to_file(&race, filename);
            } else {
                printf("Invalid parameters for command W.\n");
            }
        } else if (action == 'O') {
            char filename[100];
            if (sscanf(command + 1, "%s", filename) == 1) {
                load_from_file(&race, filename);
            } else {
                printf("Invalid parameters for command O.\n");
            }
        } else if (action == 'Q') {
            quit(&race);
            break;  // go out
        } else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}