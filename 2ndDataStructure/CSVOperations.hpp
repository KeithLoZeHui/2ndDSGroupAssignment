#ifndef CSV_OPERATIONS_HPP
#define CSV_OPERATIONS_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include "KeithTask2.hpp"
#include "Utils.hpp"
#include "PlayerQueue.hpp"
#include "GroupManager.hpp"
#include "AngelTask.hpp"

// Implementation of Count rows in checked-in CSV
int count_checkedin_csv_rows() {
    std::ifstream file("checkedin.csv");
    if (!file.is_open()) return 0;
    char line[500];
    int count = 0;
    while (file.getline(line, 500)) count++;
    return count > 0 ? count - 1 : 0; // Subtract header if file not empty
}

// Implementation of Load checked-in CSV data into array
void load_checkedin_csv(char arr[][9][120], int& n) {
    std::ifstream file("checkedin.csv");
    if (!file.is_open()) return;
    char line[500];
    n = 0;
    file.getline(line, 500); // Skip header
    while (file.getline(line, 500) && n < 100) {
        char cols[9][100]; // Updated to include PlayerEmail, GroupID and GroupName
        int num_cols = split(line, ',', cols, 9);
        for (int i = 0; i < 9; i++) {
            // If we don't have enough columns (old format), set empty string
            if (i >= num_cols) {
                arr[n][i][0] = '\0';
            } else {
                strncpy(arr[n][i], cols[i], 119);
                arr[n][i][119] = '\0';
            }
        }
        n++;
    }
    file.close();
}

// Implementation of Write array data to checked-in CSV
void write_checkedin_csv(char arr[][9][120], int n) {
    std::ofstream file("checkedin.csv");
    file << "PlayerID,PlayerName,PlayerEmail,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 9; j++) { // Updated to include PlayerEmail, GroupID and GroupName
            file << arr[i][j] << (j < 8 ? "," : "\n"); // Updated condition
        }
    }
    file.close();
}

// Implementation of Ensure same group ID for all members
void ensure_same_group_id(const char* groupID, const char* groupName, char arr[][9][120], int n) {
    // If group ID is empty, nothing to do
    if (groupID[0] == '\0') return;
    
    for (int i = 0; i < n; i++) {
        // Skip this entry if it's withdrawn
        if (strcmp(arr[i][5], "Yes") == 0) continue;
        
        // If this player has the same group ID, make sure it's exactly the same string
        if (arr[i][6][0] != '\0' && strcmp(arr[i][6], groupID) != 0) {
            // Check if this player is already in another group
            bool found_match = false;
            for (int j = 0; j < n; j++) {
                if (i != j && strcmp(arr[j][5], "Yes") != 0 && strcmp(arr[i][6], arr[j][6]) == 0) {
                    found_match = true;
                    break;
                }
            }
            
            // If not in another group, update to the new group ID and group name
            if (!found_match) {
                strncpy(arr[i][6], groupID, 119);
                arr[i][6][119] = '\0';
                strncpy(arr[i][7], groupName, 119);
                arr[i][7][119] = '\0';
            }
        }
    }
}

// Implementation of Display checked-in players from CSV
void display_checkedin_csv() {
    std::ifstream file("checkedin.csv");
    if (!file.is_open()) {
        std::cout << "No checked-in players found.\n";
        return;
    }
    
    char line[500];
    file.getline(line, 500); // Skip header
    
    std::cout << "\n--- Checked-In Players ---\n";
    std::cout << "PlayerID | PlayerName | PlayerEmail | PriorityType | RegistrationTime | CheckInStatus | Withdrawn | GroupID | GroupName\n";
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";
    
    int count = 0;
    while (file.getline(line, 500)) {
        char cols[9][100];
        int num_cols = split(line, ',', cols, 9);
        
        // Display the data in a formatted way
        std::cout << cols[0] << " | " 
                  << cols[1] << " | " 
                  << cols[2] << " | " 
                  << cols[3] << " | " 
                  << cols[4] << " | " 
                  << cols[5] << " | "
                  << cols[6] << " | ";
        
        // Handle GroupID and GroupName if they exist
        if (num_cols > 7) {
            std::cout << cols[7];
        }
        std::cout << " | ";
        
        if (num_cols > 8) {
            std::cout << cols[8];
        }
        
        std::cout << "\n";
        count++;
    }
    
    if (count == 0) {
        std::cout << "No checked-in players found.\n";
    }
    
    std::cout << "------------------------------------------------------------------------------------------------------\n";
    file.close();
}

// Load players from CSV file
PlayerCSV* loadPlayersFromCSV(const char* filename, int& num_players) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        num_players = 0;
        return nullptr;
    }
    
    // Count the number of lines (excluding header)
    char line[500];
    int count = 0;
    file.getline(line, 500); // Skip header
    while (file.getline(line, 500)) {
        count++;
    }
    
    // Reset file to beginning
    file.clear();
    file.seekg(0);
    file.getline(line, 500); // Skip header again
    
    // Allocate memory for players
    PlayerCSV* players = new PlayerCSV[count];
    num_players = count;
    
    // Read player data
    int i = 0;
    while (file.getline(line, 500) && i < count) {
        char cols[9][100]; // Corrected to handle 9 columns
        int num_cols = split(line, ',', cols, 9); // Corrected to split up to 9 columns
        
        // Copy PlayerID string to char array
        strncpy(players[i].PlayerID, cols[0], 9);
        players[i].PlayerID[9] = '\0'; // Ensure null termination
        
        // Copy other fields
        strncpy(players[i].PlayerName, cols[1], 49);
        players[i].PlayerName[49] = '\0';
        
        // Copy email field
        strncpy(players[i].PlayerEmail, cols[2], 99);
        players[i].PlayerEmail[99] = '\0';
        
        strncpy(players[i].PriorityType, cols[3], 19);
        players[i].PriorityType[19] = '\0';
        
        strncpy(players[i].RegistrationTime, cols[4], 24);
        players[i].RegistrationTime[24] = '\0';
        
        strncpy(players[i].CheckInStatus, cols[5], 19);
        players[i].CheckInStatus[19] = '\0';
        
        strncpy(players[i].Withdrawn, cols[6], 4);
        players[i].Withdrawn[4] = '\0';
        
        // Handle GroupID and GroupName if they exist
        if (num_cols > 7) {
            strncpy(players[i].GroupID, cols[7], 9); // Corrected index
            players[i].GroupID[9] = '\0';
        } else {
            players[i].GroupID[0] = '\0';
        }
        
        // Handle GroupName (9th column, index 8)
        if (num_cols > 8) { // Corrected index check
            strncpy(players[i].GroupName, cols[8], 49); // Corrected index
            players[i].GroupName[49] = '\0';
        } else {
            players[i].GroupName[0] = '\0';
        }
        
        i++;
    }
    
    file.close();
    return players;
}

// Save players to CSV file
void savePlayersToCSV(const char* filename, PlayerCSV* players, int num_players) {
    std::ofstream file(filename);
    file << "PlayerID,PlayerName,PlayerEmail,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
    
    for (int i = 0; i < num_players; i++) {
        file << players[i].PlayerID << ","
             << players[i].PlayerName << ","
             << players[i].PlayerEmail << ","
             << players[i].PriorityType << ","
             << players[i].RegistrationTime << ","
             << players[i].CheckInStatus << ","
             << players[i].Withdrawn << ","
             << players[i].GroupID << ","
             << players[i].GroupName << "\n";
    }
    
    file.close();
}

// Withdraw a player from CSV
void withdraw_from_csv(const char* identifier) {
    int n = 0;
    char data[100][9][120];
    load_checkedin_csv(data, n);
    
    bool found = false;
    for (int i = 0; i < n; i++) {
        // Match by Player Name (index 1), PlayerID (index 0), or last 3 digits of PlayerID, and not withdrawn
        if ((strcmp(data[i][1], identifier) == 0 || strcmp(data[i][0], identifier) == 0 ||
            (strlen(identifier) == 3 && strlen(data[i][0]) == 6 && strcmp(data[i][0] + 3, identifier) == 0))
            && strcmp(data[i][6], "No") == 0) {
            strcpy(data[i][6], "Yes");
            found = true;
            break;
        }
    }
    
    if (found) {
        write_checkedin_csv(data, n);
        std::cout << "Player " << identifier << " has been withdrawn.\n";
    } else {
        std::cout << "Player " << identifier << " not found or already withdrawn.\n";
    }
}

// Replace a player in CSV
void replace_in_csv(const char* oldName, const char* newName, int priority) {
    int n = 0;
    char data[100][9][120];
    load_checkedin_csv(data, n);
    
    bool found = false;
    int foundIndex = -1;
    
    // Find the player to replace
    for (int i = 0; i < n; i++) {
        if (strcmp(data[i][1], oldName) == 0 && strcmp(data[i][6], "No") == 0) {
            found = true;
            foundIndex = i;
            break;
        }
    }
    
    if (!found) {
        std::cout << "Player " << oldName << " not found or already withdrawn.\n";
        return;
    }
    
    // Store the group information
    char groupID[120];
    char groupName[120];
    strcpy(groupID, data[foundIndex][7]);
    strcpy(groupName, data[foundIndex][8]);
    
    // Mark the old player as withdrawn
    strcpy(data[foundIndex][6], "Yes");
    
    // Create a new entry for the replacement player
    if (n < 100) { // Make sure we don't overflow the array
        // Generate a sequential player ID in PLY000 format
        char uniquePlayerID[10]; // Size to accommodate PLY000 format
        generateSequentialPlayerID(uniquePlayerID);
        
        // Convert priority to string
        char priorityStr[20];
        switch(priority) {
            case 1: strcpy(priorityStr, "Early-bird"); break;
            case 2: strcpy(priorityStr, "Wildcard"); break;
            case 3: strcpy(priorityStr, "Normal"); break;
            default: strcpy(priorityStr, "Normal");
        }
        
        // Get current timestamp
        char timeStr[25];
        std::time_t now = std::time(nullptr);
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        
        // Copy the new player data
        strcpy(data[n][0], uniquePlayerID);
        strcpy(data[n][1], newName);
        // Generate a default email based on the player name
        char email[120];
        sprintf(email, "%s@example.com", newName);
        strcpy(data[n][2], email);
        strcpy(data[n][3], priorityStr);
        strcpy(data[n][4], timeStr);
        strcpy(data[n][5], "Checked-in");
        strcpy(data[n][6], "No");
        strcpy(data[n][7], groupID);
        strcpy(data[n][8], groupName);
        
        n++;
    }
    
    write_checkedin_csv(data, n);
    std::cout << "Player " << oldName << " has been replaced with " << newName << ".\n";
}

#endif // CSV_OPERATIONS_HPP
