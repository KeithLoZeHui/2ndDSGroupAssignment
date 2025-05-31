#ifndef GROUP_MANAGER_HPP
#define GROUP_MANAGER_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include "Utils.hpp"

class GroupManager {
private:
    static const int MAX_GROUPS = 100;
    static const int MAX_MEMBERS_PER_GROUP = 5;
    char data[MAX_GROUPS][9][120]; // [row][column][data]
    int rowCount;
    
    void loadData();
    void saveData();
    bool is_group_full(const char* groupID);
    int count_players_in_group(const char* groupID);
    int count_groups();
    
public:
    GroupManager();
    void assign_players_to_groups();
    void merge_small_groups();
    void display_group_statistics();
    void create_new_group();
    void run();
};

// Constructor
GroupManager::GroupManager() {
    rowCount = 0;
}

// Load data from CSV
void GroupManager::loadData() {
    std::ifstream file("CheckedIn.csv");
    if (!file.is_open()) {
        std::cout << "Error: Could not open CheckedIn.csv\n";
        return;
    }
    
    char line[500];
    file.getline(line, 500); // Skip header
    
    rowCount = 0;
    while (file.getline(line, 500) && rowCount < MAX_GROUPS) {
        char temp[9][100]; // Temporary array with correct size for split
        int num_cols = split(line, ',', temp, 9);
        if (num_cols >= 9) {
            // Copy from temp array to data array
            for (int i = 0; i < 9; i++) {
                strncpy(data[rowCount][i], temp[i], 119);
                data[rowCount][i][119] = '\0';
            }
            rowCount++;
        }
    }
    
    file.close();
}

// Save data to CSV
void GroupManager::saveData() {
    std::ofstream file("CheckedIn.csv");
    if (!file.is_open()) {
        std::cout << "Error: Could not open CheckedIn.csv for writing\n";
        return;
    }
    
    // Write header
    file << "PlayerID,PlayerName,PlayerEmail,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
    
    // Write data
    for (int i = 0; i < rowCount; i++) {
        file << data[i][0] << ","
             << data[i][1] << ","
             << data[i][2] << ","
             << data[i][3] << ","
             << data[i][4] << ","
             << data[i][5] << ","
             << data[i][6] << ","
             << data[i][7] << ","
             << data[i][8] << "\n";
    }
    
    file.close();
}

// Check if a group is full
bool GroupManager::is_group_full(const char* groupID) {
    int count = 0;
    for (int i = 0; i < rowCount; i++) {
        if (strcmp(data[i][7], groupID) == 0 && strcmp(data[i][6], "No") == 0) {
            count++;
        }
    }
    return count >= MAX_MEMBERS_PER_GROUP;
}

// Count players in a group
int GroupManager::count_players_in_group(const char* groupID) {
    int count = 0;
    for (int i = 0; i < rowCount; i++) {
        if (strcmp(data[i][7], groupID) == 0 && strcmp(data[i][6], "No") == 0) {
            count++;
        }
    }
    return count;
}

// Count total number of groups
int GroupManager::count_groups() {
    char uniqueGroups[MAX_GROUPS][2][120]; // [0] = ID, [1] = Name
    int uniqueGroupCount = 0;
    
    for (int i = 0; i < rowCount; i++) {
        if (data[i][7][0] == '\0' || strcmp(data[i][6], "Yes") == 0) continue;
        
        bool found = false;
        for (int j = 0; j < uniqueGroupCount; j++) {
            if (strcmp(uniqueGroups[j][0], data[i][7]) == 0) {
                found = true;
                break;
            }
        }
        
        if (!found && uniqueGroupCount < MAX_GROUPS) {
            strncpy(uniqueGroups[uniqueGroupCount][0], data[i][7], 119);
            uniqueGroups[uniqueGroupCount][0][119] = '\0';
            uniqueGroupCount++;
        }
    }
    
    return uniqueGroupCount;
}

// Assign players to groups
void GroupManager::assign_players_to_groups() {
    loadData();
    
    // Find all unique group IDs
    char groupIDs[MAX_GROUPS][120];
    int groupCount = 0;
    
    for (int i = 0; i < rowCount; i++) {
        if (data[i][7][0] == '\0' || strcmp(data[i][6], "Yes") == 0) continue;
        
        bool found = false;
        for (int j = 0; j < groupCount; j++) {
            if (strcmp(groupIDs[j], data[i][7]) == 0) {
                found = true;
                break;
            }
        }
        
        if (!found && groupCount < MAX_GROUPS) {
            strncpy(groupIDs[groupCount], data[i][7], 119);
            groupIDs[groupCount][119] = '\0';
            groupCount++;
        }
    }
    
    // Assign unassigned players to groups
    for (int i = 0; i < rowCount; i++) {
        if (data[i][7][0] != '\0' || strcmp(data[i][6], "Yes") == 0) continue;
        
        // Try to find a non-full group
        for (int j = 0; j < groupCount; j++) {
            if (!is_group_full(groupIDs[j])) {
                strncpy(data[i][7], groupIDs[j], 119);
                data[i][7][119] = '\0';
                break;
            }
        }
    }
    
    saveData();
    std::cout << "Players assigned to groups.\n";
}

// Merge small groups
void GroupManager::merge_small_groups() {
    loadData();
    
    // Find all unique group IDs and their sizes
    char groupIDs[MAX_GROUPS][2][120]; // [0] = ID, [1] = Name
    int groupSizes[MAX_GROUPS] = {0};
    int groupCount = 0;
    
    for (int i = 0; i < rowCount; i++) {
        if (data[i][7][0] == '\0' || strcmp(data[i][6], "Yes") == 0) continue;
        
        bool found = false;
        for (int j = 0; j < groupCount; j++) {
            if (strcmp(groupIDs[j][0], data[i][7]) == 0) {
                groupSizes[j]++;
                found = true;
                break;
            }
        }
        
        if (!found && groupCount < MAX_GROUPS) {
            strncpy(groupIDs[groupCount][0], data[i][7], 119);
            groupIDs[groupCount][0][119] = '\0';
            strncpy(groupIDs[groupCount][1], data[i][8], 119);
            groupIDs[groupCount][1][119] = '\0';
            groupSizes[groupCount] = 1;
            groupCount++;
        }
    }
    
    // Find small groups (less than MAX_MEMBERS_PER_GROUP)
    bool hasSmallGroups = false;
    for (int i = 0; i < groupCount; i++) {
        if (groupSizes[i] < MAX_MEMBERS_PER_GROUP) {
            hasSmallGroups = true;
            break;
        }
    }
    
    if (!hasSmallGroups) {
        std::cout << "No small groups to merge.\n";
        return;
    }
    
    // Merge small groups
    for (int i = 0; i < groupCount; i++) {
        if (groupSizes[i] >= MAX_MEMBERS_PER_GROUP) continue;
        
        // Find another small group to merge with
        for (int j = i + 1; j < groupCount; j++) {
            if (groupSizes[j] >= MAX_MEMBERS_PER_GROUP) continue;
            
            // Check if merging would exceed MAX_MEMBERS_PER_GROUP
            if (groupSizes[i] + groupSizes[j] > MAX_MEMBERS_PER_GROUP) continue;
            
            // Merge group j into group i
            for (int k = 0; k < rowCount; k++) {
                if (strcmp(data[k][7], groupIDs[j][0]) == 0) {
                    strncpy(data[k][7], groupIDs[i][0], 119);
                    data[k][7][119] = '\0';
                    strncpy(data[k][8], groupIDs[i][1], 119);
                    data[k][8][119] = '\0';
                }
            }
            
            groupSizes[i] += groupSizes[j];
            groupSizes[j] = 0;
        }
    }
    
    saveData();
    std::cout << "Small groups merged.\n";
}

// Display group statistics
void GroupManager::display_group_statistics() {
    loadData();
    
    // Find all unique group IDs and their sizes
    char groupIDs[MAX_GROUPS][2][120]; // [0] = ID, [1] = Name
    int groupSizes[MAX_GROUPS] = {0};
    int groupCount = 0;
    
    for (int i = 0; i < rowCount; i++) {
        if (data[i][7][0] == '\0' || strcmp(data[i][6], "Yes") == 0) continue;
        
        bool found = false;
        for (int j = 0; j < groupCount; j++) {
            if (strcmp(groupIDs[j][0], data[i][7]) == 0) {
                groupSizes[j]++;
                found = true;
                break;
            }
        }
        
        if (!found && groupCount < MAX_GROUPS) {
            strncpy(groupIDs[groupCount][0], data[i][7], 119);
            groupIDs[groupCount][0][119] = '\0';
            strncpy(groupIDs[groupCount][1], data[i][8], 119);
            groupIDs[groupCount][1][119] = '\0';
            groupSizes[groupCount] = 1;
            groupCount++;
        }
    }
    
    // Calculate statistics
    int totalGroups = groupCount;
    int smallestSize = MAX_MEMBERS_PER_GROUP;
    int largestSize = 0;
    int totalMembers = 0;
    int sizeDistribution[MAX_MEMBERS_PER_GROUP + 1] = {0};
    
    for (int i = 0; i < groupCount; i++) {
        if (groupSizes[i] < smallestSize) smallestSize = groupSizes[i];
        if (groupSizes[i] > largestSize) largestSize = groupSizes[i];
        totalMembers += groupSizes[i];
        sizeDistribution[groupSizes[i]]++;
    }
    
    float averageSize = totalGroups > 0 ? (float)totalMembers / totalGroups : 0;
    
    // Display statistics
    std::cout << "\n--- Group Statistics ---\n";
    std::cout << "Total number of groups: " << totalGroups << "\n";
    std::cout << "Smallest group size: " << smallestSize << " members\n";
    std::cout << "Largest group size: " << largestSize << " members\n";
    std::cout << "Average group size: " << averageSize << " members\n\n";
    
    std::cout << "Group size distribution:\n";
    for (int i = 1; i <= MAX_MEMBERS_PER_GROUP; i++) {
        if (sizeDistribution[i] > 0) {
            std::cout << "Groups with " << i << " members: " << sizeDistribution[i] << "\n";
        }
    }
}

// Create new group
void GroupManager::create_new_group() {
    std::cout << "\n--- Create New Group ---\n";
    char groupName[50];
    std::cout << "Enter group name: ";
    std::cin.getline(groupName, 50);
    
    // Find the highest GroupID in the CSV
    int highestGroupID = -1;
    std::ifstream fileG("CheckedIn.csv");
    if (fileG.is_open()) {
        char line[500];
        fileG.getline(line, 500); // Skip header
        while (fileG.getline(line, 500)) {
            char cols[9][100];
            int num_cols = split(line, ',', cols, 9);
            if (num_cols >= 7 && strncmp(cols[7], "GRP", 3) == 0) {
                int idNum = atoi(cols[7] + 3);
                if (idNum > highestGroupID) highestGroupID = idNum;
            }
        }
        fileG.close();
    }
    
    // Generate new group ID
    highestGroupID++;
    char groupID[10];
    sprintf(groupID, "GRP%03d", highestGroupID);
    
    std::cout << "\nCreated group: " << groupName << " [" << groupID << "]\n";
    
    char addPlayers;
    std::cout << "\nWould you like to add players to this group? (y/n): ";
    std::cin >> addPlayers;
    std::cin.ignore(); // Clear the newline
    
    if (addPlayers == 'y' || addPlayers == 'Y') {
        loadData();
        
        // Display available players (those without a group)
        std::cout << "\nAvailable players:\n";
        bool playersFound = false;
        for (int i = 0; i < rowCount; i++) {
            if (data[i][7][0] == '\0' && strcmp(data[i][6], "No") == 0) {
                std::cout << i+1 << ". " << data[i][1] << " (" << data[i][0] << ")\n";
                playersFound = true;
            }
        }
        
        if (!playersFound) {
            std::cout << "No available players to add to the group.\n";
            return;
        }
        
        // Get number of players to add
        int numPlayers;
        std::cout << "\nEnter number of players to add (1-" << MAX_MEMBERS_PER_GROUP << "): ";
        std::cin >> numPlayers;
        std::cin.ignore(); // Clear the newline
        
        if (numPlayers < 1 || numPlayers > MAX_MEMBERS_PER_GROUP) {
            std::cout << "Invalid number of players.\n";
            return;
        }
        
        // Add players to the group
        for (int i = 0; i < numPlayers; i++) {
            int playerIndex;
            std::cout << "Enter player number to add: ";
            std::cin >> playerIndex;
            std::cin.ignore(); // Clear the newline
            
            if (playerIndex < 1 || playerIndex > rowCount) {
                std::cout << "Invalid player number.\n";
                i--;
                continue;
            }
            
            playerIndex--; // Convert to 0-based index
            
            // Check if player is available
            if (data[playerIndex][7][0] != '\0' || strcmp(data[playerIndex][6], "Yes") == 0) {
                std::cout << "Player is not available.\n";
                i--;
                continue;
            }
            
            // Assign player to group
            strncpy(data[playerIndex][7], groupID, 119);
            data[playerIndex][7][119] = '\0';
            strncpy(data[playerIndex][8], groupName, 119);
            data[playerIndex][8][119] = '\0';
            
            std::cout << "Added " << data[playerIndex][1] << " to the group.\n";
        }
        
        saveData();
    }
    
    std::cout << "\nGroup created successfully!\n";
}

// Run group management menu
void GroupManager::run() {
    while (true) {
        std::cout << "\n--- Group Management ---\n";
        std::cout << "1. Assign players to groups\n";
        std::cout << "2. Merge small groups\n";
        std::cout << "3. Display group statistics\n";
        std::cout << "4. Create new group\n";
        std::cout << "5. Return to main menu\n";
        std::cout << "Enter your choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline
        
        switch (choice) {
            case 1:
                assign_players_to_groups();
                break;
            case 2:
                merge_small_groups();
                break;
            case 3:
                display_group_statistics();
                break;
            case 4:
                create_new_group();
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

#endif // GROUP_MANAGER_HPP
