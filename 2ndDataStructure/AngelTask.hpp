#ifndef ANGELTASK_HPP
#define ANGELTASK_HPP

#include <string> // Required for std::string in PlayerCSV
#include <cstring> // Required for C-style string functions in PlayerCSV (like strcpy, etc.)

// Define the maximum size for string fields in PlayerCSV
// (You might need to adjust these based on your CSV data)
#define MAX_NAME_SIZE 100
#define MAX_EMAIL_SIZE 100
#define MAX_PRIORITY_SIZE 50
#define MAX_TIME_SIZE 50
#define MAX_STATUS_SIZE 20
#define MAX_GROUP_SIZE 10

// Struct to hold player data from CSV
// NOTE: PlayerID is now a char array to properly store PLY### format
struct PlayerCSV {
    char PlayerID[10]; // Changed from std::string to char array to store PLY### format
    char PlayerName[MAX_NAME_SIZE];
    char PlayerEmail[MAX_EMAIL_SIZE];
    char PriorityType[MAX_PRIORITY_SIZE];
    char RegistrationTime[MAX_TIME_SIZE];
    char CheckInStatus[MAX_STATUS_SIZE]; // e.g., "Checked-in", "Left"
    char Withdrawn[MAX_STATUS_SIZE]; // e.g., "Yes", "No"
    char GroupID[MAX_GROUP_SIZE];
    char GroupName[MAX_NAME_SIZE]; // Assuming group name can be longer
};

// Declaration for Angel's Task 3 function
void processLiveStreamAndOverflow();

// Function declarations for CSV operations related to PlayerCSV
PlayerCSV* loadPlayersFromCSV(const char* filename, int& num_players);
void savePlayersToCSV(const char* filename, PlayerCSV* players, int num_players);

#endif // ANGELTASK_HPP 