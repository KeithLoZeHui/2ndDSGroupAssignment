#ifndef KEITH_TASK2_HPP
#define KEITH_TASK2_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <random>
#include <limits>
#include <cstdlib>

// Forward declarations
class PlayerQueue;
class GroupManager;

// Player structure
struct Player {
    char name[50];
    char PlayerID[10]; // Changed to accommodate PLY000 format (6 chars + null)
    char PlayerEmail[100]; // Restored email field
    int priority; // 1 = Early-bird, 2 = Wildcard, 3 = Normal
    char groupID[10]; // Auto-generated group ID
    char groupName[50]; // Group name
};

// Group struct to hold up to 5 players
struct Group {
    char groupID[10];
    Player players[5];
    int playerCount;
    Group() : playerCount(0) { groupID[0] = '\0'; }
};

// Constants for group management
const int MAX_GROUPS = 8;
const int MAX_MEMBERS_PER_GROUP = 5;

// Forward declarations
class GroupManager;

// Function declarations for CSV operations - implemented in CSVOperations.hpp
void load_checkedin_csv(char data[][9][120], int& rowCount);
void write_checkedin_csv(char data[][9][120], int rowCount);
void display_checkedin_csv();
int count_checkedin_csv_rows();

// Group management class
class GroupManager {
private:
    char data[100][9][120]; // Array to store CSV data with PlayerEmail, GroupID and GroupName (Updated to 9 columns)
    int rowCount;
    
    // Helper methods
    void loadData() {
        rowCount = 0;
        load_checkedin_csv(data, rowCount);
        // Ensure all group members have consistent group info
        syncGroupInfo();
    }
    
    void saveData() {
        write_checkedin_csv(data, rowCount);
    }
    
    // Helper method to ensure all members of a group have the same group ID and name
    void syncGroupInfo() {
        // Create a map of group IDs to group names
        struct GroupInfo {
            char groupID[10];
            char groupName[50];
            bool hasMembers;
        };
        
        GroupInfo groupInfos[MAX_GROUPS];
        int groupCount = 0;
        
        // Initialize group info
        for (int i = 0; i < MAX_GROUPS; i++) {
            groupInfos[i].groupID[0] = '\0';
            groupInfos[i].groupName[0] = '\0';
            groupInfos[i].hasMembers = false;
        }
        
        // First pass: collect all group IDs and names
        for (int i = 0; i < rowCount; i++) {
            // Skip withdrawn players or players without a group (Adjusted index for Withdrawn and GroupID)
            if (strcmp(data[i][6], "Yes") == 0 || data[i][7][0] == '\0') continue;
            
            bool found = false;
            for (int j = 0; j < groupCount; j++) {
                if (strcmp(groupInfos[j].groupID, data[i][7]) == 0) {
                    // Group already exists, ensure group name is consistent (Adjusted index for GroupName)
                    if (groupInfos[j].groupName[0] == '\0' && data[i][8][0] != '\0') {
                        strncpy(groupInfos[j].groupName, data[i][8], 49);
                        groupInfos[j].groupName[49] = '\0';
                    }
                    groupInfos[j].hasMembers = true;
                    found = true;
                    break;
                }
            }
            
            // New group found
            if (!found && groupCount < MAX_GROUPS) {
                strncpy(groupInfos[groupCount].groupID, data[i][7], 9); // Adjusted index
                groupInfos[groupCount].groupID[9] = '\0';
                
                if (data[i][8][0] != '\0') { // Adjusted index
                    strncpy(groupInfos[groupCount].groupName, data[i][8], 49); // Adjusted index
                    groupInfos[groupCount].groupName[49] = '\0';
                }
                
                groupInfos[groupCount].hasMembers = true;
                groupCount++;
            }
        }
        
        // Second pass: ensure all members of each group have the same group ID and name
        for (int i = 0; i < rowCount; i++) {
            // Skip withdrawn players or players without a group (Adjusted index)
            if (strcmp(data[i][6], "Yes") == 0 || data[i][7][0] == '\0') continue;
            
            for (int j = 0; j < groupCount; j++) {
                if (strcmp(groupInfos[j].groupID, data[i][7]) == 0) { // Adjusted index
                    // Ensure group name is consistent (Adjusted index)
                    if (groupInfos[j].groupName[0] != '\0') {
                        strncpy(data[i][8], groupInfos[j].groupName, 119); // Adjusted index
                        data[i][8][119] = '\0';
                    } else if (data[i][8][0] != '\0') { // Adjusted index
                        strncpy(groupInfos[j].groupName, data[i][8], 49); // Adjusted index
                        groupInfos[j].groupName[49] = '\0';
                    }
                    break;
                }
            }
        }
    }
    
public:
    GroupManager() : rowCount(0) {}
    
    // Group management methods (Updated function signatures)
    void countGroups(int& groupCount, int groupSizes[]);
    void assignToGroups();
    void mergeSmallGroups();
    void displayGroupStatistics();
    void createGroup();
    void organizeGroups();
    
    // Ensure all members of a group have the same group ID (Updated function signature and indices)
    void ensureSameGroupId(const char* groupID, const char* groupName) {
        // If group ID is empty, nothing to do
        if (groupID[0] == '\0') return;
        
        for (int i = 0; i < rowCount; i++) {
            // Skip this entry if it's withdrawn (Adjusted index)
            if (strcmp(data[i][6], "Yes") == 0) continue;
            
            // If this player has the same group ID, make sure it's exactly the same string (Adjusted index)
            if (data[i][7][0] != '\0' && strcmp(data[i][7], groupID) != 0) {
                // Check if this player is already in another group (Adjusted index)
                bool found_match = false;
                for (int j = 0; j < rowCount; j++) {
                    if (i != j && strcmp(data[j][6], "Yes") != 0 && strcmp(data[i][7], data[j][7]) == 0) { // Adjusted indices
                        found_match = true;
                        break;
                    }
                }
                
                // If not in another group, update to the new group ID and group name (Adjusted indices)
                if (!found_match) {
                    strncpy(data[i][7], groupID, 119); // Adjusted index
                    data[i][7][119] = '\0';
                    strncpy(data[i][8], groupName, 119); // Adjusted index
                    data[i][8][119] = '\0';
                }
            }
        }
        
        // Call syncGroupInfo to ensure all members of the same group have consistent info
        syncGroupInfo();
    }
};

// Function declarations - implemented in Utils.hpp (Updated function signatures)

// Split string by delimiter
int split(const char* s, char delimiter, char tokens[][100], int max_tokens);

// Generate random ID
void generateRandomID(char* output, int length);

// Count rows in checked-in CSV
int count_checkedin_csv_rows();

// Load checked-in CSV data into array (Updated function signature)
void load_checkedin_csv(char arr[][9][120], int& n);

// Write array data to checked-in CSV (Updated function signature)
void write_checkedin_csv(char arr[][9][120], int n);

// Ensure same group ID for all members (Updated function signature)
void ensure_same_group_id(const char* groupID, const char* groupName, char arr[][9][120], int n);

// Display checked-in players from CSV
void display_checkedin_csv();

// These functions are now part of the GroupManager class
void withdraw_from_csv(const char* name);
void replace_in_csv(const char* oldName, const char* newName, int priority);
void organize_groups();

// Custom FIFO Queue (First In, First Out) - Class declaration
class PlayerQueue {
private:
    Player* arr;
    int front, rear, capacity;
public:
    // Constructor and destructor
    PlayerQueue(int initial_capacity = 10);
    ~PlayerQueue();
    
    // Member function declarations
    bool isEmpty() { return front > rear; }
    bool isFull() { return rear == capacity - 1; }
    void resize() {
        int new_capacity = capacity * 2;
        Player* new_arr = new Player[new_capacity];
        for (int i = front; i <= rear; ++i) {
            new_arr[i - front] = arr[i];
        }
        rear = rear - front;
        front = 0;
        delete[] arr;
        arr = new_arr;
        capacity = new_capacity;
    }
    bool file_exists(const char* filename);
    bool is_valid_id(const char* s);
    bool is_valid_email(const char* s);
    bool is_duplicate_in_csv(const char* filename, const char* playerID, const char* email);
    
    // Updated function signatures to use 9-column array
    int count_players_in_group(const char* groupID); // This one reads CSV
    bool can_add_to_group(const char* groupID, int numNewPlayers); // This one calls count_players_in_group
    bool is_group_id_unique(const char* groupID); // This one reads CSV

    void enqueue();
    void dequeue();
    void withdraw(const char* name);
    void replace(const char* oldName, const char* newName, int priority);
    void display() {
        if (isEmpty()) {
            std::cout << "Queue is empty!\n";
            return;
        }
        std::cout << "\nCurrent Queue:\n";
        std::cout << "----------------------------\n";
        for (int i = front; i <= rear; ++i) {
            std::cout << i - front + 1 << ". " << arr[i].name << " (ID: " << arr[i].PlayerID << ", Priority: ";
            switch (arr[i].priority) {
                case 1: std::cout << "Early-bird"; break;
                case 2: std::cout << "Wildcard"; break;
                case 3: std::cout << "Normal"; break;
                default: std::cout << "Unknown";
            }
            
            // Display group information if available
            if (arr[i].groupID[0] != '\0') {
                std::cout << ", Group: " << arr[i].groupName << " [" << arr[i].groupID << "]";
            }
            
            std::cout << ")\n";
        }
        std::cout << "----------------------------\n";
    }
};

// Function declarations - Keep this declaration
bool is_valid_email(const char* email);

#endif // KEITH_TASK2_HPP

// Include all implementation files
#include "Utils.hpp"
#include "CSVOperations.hpp"
#include "PlayerQueue.hpp"
#include "GroupManager.hpp"