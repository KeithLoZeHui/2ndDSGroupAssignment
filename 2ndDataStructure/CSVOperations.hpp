#ifndef CSV_OPERATIONS_HPP
#define CSV_OPERATIONS_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include "KeithTask2.hpp"
#include "Utils.hpp"

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
void load_checkedin_csv(char arr[][8][120], int& n) {
    std::ifstream file("checkedin.csv");
    if (!file.is_open()) return;
    char line[500];
    n = 0;
    file.getline(line, 500); // Skip header
    while (file.getline(line, 500) && n < 100) {
        char cols[8][100]; // Updated to include GroupID and GroupName
        int num_cols = split(line, ',', cols, 8);
        for (int i = 0; i < 8; i++) {
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
void write_checkedin_csv(char arr[][8][120], int n) {
    std::ofstream file("checkedin.csv");
    file << "PlayerID,PlayerName,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 8; j++) { // Updated to include GroupID and GroupName
            file << arr[i][j] << (j < 7 ? "," : "\n"); // Updated condition
        }
    }
    file.close();
}

// Implementation of Ensure same group ID for all members
void ensure_same_group_id(const char* groupID, const char* groupName, char arr[][8][120], int n) {
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
    std::cout << "PlayerID | PlayerName | PriorityType | RegistrationTime | CheckInStatus | Withdrawn | GroupID | GroupName\n";
    std::cout << "------------------------------------------------------------------------------------------------------\n";
    
    int count = 0;
    while (file.getline(line, 500)) {
        char cols[8][100];
        int num_cols = split(line, ',', cols, 8);
        
        // Display the data in a formatted way
        std::cout << cols[0] << " | " 
                  << cols[1] << " | " 
                  << cols[2] << " | " 
                  << cols[3] << " | " 
                  << cols[4] << " | " 
                  << cols[5] << " | ";
        
        // Handle GroupID and GroupName if they exist
        if (num_cols > 6) {
            std::cout << cols[6];
        }
        std::cout << " | ";
        
        if (num_cols > 7) {
            std::cout << cols[7];
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
        char cols[8][100];
        int num_cols = split(line, ',', cols, 8);
        
        // Convert ID to int
        players[i].PlayerID = atoi(cols[0]);
        
        // Copy other fields
        strncpy(players[i].PlayerName, cols[1], 49);
        players[i].PlayerName[49] = '\0';
        
        strncpy(players[i].PriorityType, cols[2], 19);
        players[i].PriorityType[19] = '\0';
        
        strncpy(players[i].RegistrationTime, cols[3], 24);
        players[i].RegistrationTime[24] = '\0';
        
        strncpy(players[i].CheckInStatus, cols[4], 19);
        players[i].CheckInStatus[19] = '\0';
        
        strncpy(players[i].Withdrawn, cols[5], 4);
        players[i].Withdrawn[4] = '\0';
        
        // Handle GroupID and GroupName if they exist
        if (num_cols > 6) {
            strncpy(players[i].GroupID, cols[6], 9);
            players[i].GroupID[9] = '\0';
        } else {
            players[i].GroupID[0] = '\0';
        }
        
        if (num_cols > 7) {
            strncpy(players[i].GroupName, cols[7], 49);
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
    file << "PlayerID,PlayerName,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
    
    for (int i = 0; i < num_players; i++) {
        file << players[i].PlayerID << ","
             << players[i].PlayerName << ","
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
void withdraw_from_csv(const char* name) {
    int n = 0;
    char data[100][8][120];
    load_checkedin_csv(data, n);
    
    bool found = false;
    for (int i = 0; i < n; i++) {
        if (strcmp(data[i][1], name) == 0 && strcmp(data[i][5], "No") == 0) {
            strcpy(data[i][5], "Yes");
            found = true;
            break;
        }
    }
    
    if (found) {
        write_checkedin_csv(data, n);
        std::cout << "Player " << name << " has been withdrawn.\n";
    } else {
        std::cout << "Player " << name << " not found or already withdrawn.\n";
    }
}

// Replace a player in CSV
void replace_in_csv(const char* oldName, const char* newName, int priority) {
    int n = 0;
    char data[100][8][120];
    load_checkedin_csv(data, n);
    
    bool found = false;
    int foundIndex = -1;
    
    // Find the player to replace
    for (int i = 0; i < n; i++) {
        if (strcmp(data[i][1], oldName) == 0 && strcmp(data[i][5], "No") == 0) {
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
    strcpy(groupID, data[foundIndex][6]);
    strcpy(groupName, data[foundIndex][7]);
    
    // Mark the old player as withdrawn
    strcpy(data[foundIndex][5], "Yes");
    
    // Create a new entry for the replacement player
    if (n < 100) { // Make sure we don't overflow the array
        // Generate a unique player ID
        char uniquePlayerID[20];
        std::time_t now = std::time(nullptr);
        sprintf(uniquePlayerID, "RPL_%d", static_cast<int>(now % 10000));
        
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
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        
        // Copy the new player data
        strcpy(data[n][0], uniquePlayerID);
        strcpy(data[n][1], newName);
        strcpy(data[n][2], priorityStr);
        strcpy(data[n][3], timeStr);
        strcpy(data[n][4], "Checked-in");
        strcpy(data[n][5], "No");
        strcpy(data[n][6], groupID);
        strcpy(data[n][7], groupName);
        
        n++;
    }
    
    write_checkedin_csv(data, n);
    std::cout << "Player " << oldName << " has been replaced with " << newName << ".\n";
}

#endif // CSV_OPERATIONS_HPP
