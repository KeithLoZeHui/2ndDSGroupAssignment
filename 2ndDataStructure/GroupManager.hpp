#ifndef GROUP_MANAGER_HPP
#define GROUP_MANAGER_HPP

#include <iostream>
#include <cstring>
#include "KeithTask2.hpp"
#include "CSVOperations.hpp"
#include "Utils.hpp"

// Implementation of GroupManager::assignToGroups()
void GroupManager::assignToGroups() {
    // Load data first
    loadData();
    
    // Count how many players are in each group
    int groupCounts[MAX_GROUPS] = {0};
    char groupIDs[MAX_GROUPS][10];
    char groupNames[MAX_GROUPS][50];
    int groupCount = 0;
    
    // Initialize arrays
    for (int i = 0; i < MAX_GROUPS; i++) {
        groupIDs[i][0] = '\0';
        groupNames[i][0] = '\0';
    }
    
    // First pass: collect all group IDs and count members
    for (int i = 0; i < rowCount; i++) {
        // Skip withdrawn players (Index 6) or players without a group (Index 7)
        if (strcmp(data[i][6], "Yes") == 0 || data[i][7][0] == '\0') continue;
        
        bool found = false;
        for (int j = 0; j < groupCount; j++) {
            // GroupID is index 7
            if (strcmp(groupIDs[j], data[i][7]) == 0) {
                groupCounts[j]++;
                found = true;
                break;
            }
        }
        
        // New group found
        if (!found && groupCount < MAX_GROUPS) {
            strncpy(groupIDs[groupCount], data[i][7], 9); // GroupID is index 7
            groupIDs[groupCount][9] = '\0';
            
            // GroupName is index 8
            strncpy(groupNames[groupCount], data[i][8], 49);
            groupNames[groupCount][49] = '\0';
            
            groupCounts[groupCount] = 1;
            groupCount++;
        }
    }
    
    // Display current groups
    std::cout << "\n--- Current Groups ---\n";
    for (int i = 0; i < groupCount; i++) {
        std::cout << i+1 << ". " << groupNames[i] << " [" << groupIDs[i] << "] - " 
                  << groupCounts[i] << " members\n";
    }
    
    // Second pass: assign players without groups to existing groups that aren't full
    for (int i = 0; i < rowCount; i++) {
        // Skip withdrawn players (Index 6) or players already in a group (Index 7)
        if (strcmp(data[i][6], "Yes") == 0 || data[i][7][0] != '\0') continue;
        
        // Find a group with less than MAX_MEMBERS_PER_GROUP members
        bool assigned = false;
        for (int j = 0; j < groupCount; j++) {
            // Check if this group is full using the new function
            if (!is_group_full(groupIDs[j], data, rowCount)) {
                // Assign to this group (GroupID is index 7, GroupName is index 8)
                strncpy(data[i][7], groupIDs[j], 119);
                data[i][7][119] = '\0';
                
                strncpy(data[i][8], groupNames[j], 119);
                data[i][8][119] = '\0';
                
                groupCounts[j]++;
                assigned = true;
                
                std::cout << "Assigned player " << data[i][1] << " to group " 
                          << groupNames[j] << " [" << groupIDs[j] << "] (" 
                          << groupCounts[j] << "/" << MAX_MEMBERS_PER_GROUP << " members)\n";
                break;
            }
        }
        
        // If no existing group has space, create a new group if possible
        if (!assigned && groupCount < MAX_GROUPS) {
            char newGroupID[10];
            generateSequentialGroupID(newGroupID);
            
            std::cout << "Creating new group with ID: " << newGroupID << "\n";
            std::cout << "Enter group name: ";
            char newGroupName[50];
            std::cin.getline(newGroupName, 50);
            
            // Assign to new group (GroupID is index 7, GroupName is index 8)
            strncpy(data[i][7], newGroupID, 119);
            data[i][7][119] = '\0';
            
            strncpy(data[i][8], newGroupName, 119);
            data[i][8][119] = '\0';
            
            // Add to our tracking arrays
            strncpy(groupIDs[groupCount], newGroupID, 9);
            groupIDs[groupCount][9] = '\0';
            
            strncpy(groupNames[groupCount], newGroupName, 49);
            groupNames[groupCount][49] = '\0';
            
            groupCounts[groupCount] = 1;
            groupCount++;
            
            std::cout << "Assigned player " << data[i][1] << " to new group " 
                      << newGroupName << " [" << newGroupID << "]\n";
        }
        else if (!assigned) {
            std::cout << "Warning: Could not assign player " << data[i][1] 
                      << " to a group. All groups are full.\n";
        }
    }
    
    // Save the updated data
    saveData();
    
    std::cout << "\nGroup assignments complete.\n";
}

// Implementation of GroupManager::mergeSmallGroups()
void GroupManager::mergeSmallGroups() {
    // Load data first
    loadData();
    
    // Count how many players are in each group
    int groupCounts[MAX_GROUPS] = {0};
    char groupIDs[MAX_GROUPS][10];
    char groupNames[MAX_GROUPS][50];
    int groupCount = 0;
    
    // Initialize arrays
    for (int i = 0; i < MAX_GROUPS; i++) {
        groupIDs[i][0] = '\0';
        groupNames[i][0] = '\0';
    }
    
    // First pass: collect all group IDs and count members
    for (int i = 0; i < rowCount; i++) {
        // Skip withdrawn players (Index 6)
        if (strcmp(data[i][6], "Yes") == 0) continue;
        
        // If player has a group ID (Index 7)
        if (data[i][7][0] != '\0') {
            bool found = false;
            for (int j = 0; j < groupCount; j++) {
                // GroupID is index 7
                if (strcmp(groupIDs[j], data[i][7]) == 0) {
                    groupCounts[j]++;
                    found = true;
                    break;
                }
            }
            
            // New group found
            if (!found && groupCount < MAX_GROUPS) {
                strncpy(groupIDs[groupCount], data[i][7], 9); // GroupID is index 7
                groupIDs[groupCount][9] = '\0';
                
                strncpy(groupNames[groupCount], data[i][8], 49); // GroupName is index 8
                groupNames[groupCount][49] = '\0';
                
                groupCounts[groupCount] = 1;
                groupCount++;
            }
        }
    }
    
    // Display current groups
    std::cout << "\n--- Current Groups Before Merging ---\n";
    for (int i = 0; i < groupCount; i++) {
        std::cout << i+1 << ". " << groupNames[i] << " [" << groupIDs[i] << "] - " 
                  << groupCounts[i] << " members\n";
    }
    
    // Find small groups (less than 3 members)
    bool smallGroupsExist = false;
    for (int i = 0; i < groupCount; i++) {
        if (groupCounts[i] < 3) {
            smallGroupsExist = true;
            break;
        }
    }
    
    if (!smallGroupsExist) {
        std::cout << "\nNo small groups to merge.\n";
        return;
    }
    
    // Merge small groups
    bool changes = false;
    for (int i = 0; i < groupCount; i++) {
        // Skip if this group has already been processed or has enough members
        if (groupIDs[i][0] == '\0' || groupCounts[i] >= 3) continue;
        
        for (int j = i + 1; j < groupCount; j++) {
            // Skip if this group has already been processed
            if (groupIDs[j][0] == '\0') continue;
            
            // Check if merging would create a valid group size
            if (groupCounts[i] + groupCounts[j] <= MAX_MEMBERS_PER_GROUP) {
                std::cout << "\nMerging group " << groupNames[i] << " [" << groupIDs[i] << "] with " 
                          << groupNames[j] << " [" << groupIDs[j] << "]\n";
                
                // Ask which group name to keep
                std::cout << "Which group name to keep?\n";
                std::cout << "1. " << groupNames[i] << "\n";
                std::cout << "2. " << groupNames[j] << "\n";
                std::cout << "3. Enter a new name\n";
                std::cout << "Choice: ";
                int choice;
                std::cin >> choice;
                std::cin.ignore(); // Clear the newline
                
                char targetGroupID[10];
                char targetGroupName[50];
                
                // Set target group ID to the first group's ID
                strncpy(targetGroupID, groupIDs[i], 9);
                targetGroupID[9] = '\0';
                
                // Set the target group name based on user choice
                if (choice == 1) {
                    strncpy(targetGroupName, groupNames[i], 49);
                    targetGroupName[49] = '\0';
                } else if (choice == 2) {
                    strncpy(targetGroupName, groupNames[j], 49);
                    targetGroupName[49] = '\0';
                } else {
                    std::cout << "Enter new group name: ";
                    std::cin.getline(targetGroupName, 50);
                }
                
                // Update all members of the second group to the target group (Adjusted indices)
                for (int k = 0; k < rowCount; k++) {
                    if (strcmp(data[k][6], "Yes") != 0 && strcmp(data[k][7], groupIDs[j]) == 0) { // Withdrawn is index 6, GroupID is index 7
                        strncpy(data[k][7], targetGroupID, 119); // GroupID is index 7
                        data[k][7][119] = '\0';
                        
                        strncpy(data[k][8], targetGroupName, 119); // GroupName is index 8
                        data[k][8][119] = '\0';
                    }
                }
                
                // Update the first group's name if needed (Adjusted indices)
                if (choice != 1) {
                    for (int k = 0; k < rowCount; k++) {
                        if (strcmp(data[k][6], "Yes") != 0 && strcmp(data[k][7], groupIDs[i]) == 0) { // Withdrawn is index 6, GroupID is index 7
                            strncpy(data[k][8], targetGroupName, 119); // GroupName is index 8
                            data[k][8][119] = '\0';
                        }
                    }
                }
                
                // Update our tracking arrays
                groupCounts[i] += groupCounts[j];
                strncpy(groupNames[i], targetGroupName, 49);
                groupNames[i][49] = '\0';
                
                // Mark the second group as processed
                groupIDs[j][0] = '\0';
                groupCounts[j] = 0;
                
                changes = true;
                break; // Move to the next group
            }
        }
    }
    
    if (changes) {
        // Save the updated data
        saveData();
        
        // Display updated groups
        std::cout << "\n--- Groups After Merging ---\n";
        for (int i = 0; i < groupCount; i++) {
            if (groupIDs[i][0] != '\0') {
                std::cout << i+1 << ". " << groupNames[i] << " [" << groupIDs[i] << "] - " 
                          << groupCounts[i] << " members\n";
            }
        }
        
        std::cout << "\nGroup merging complete.\n";
    } else {
        std::cout << "\nNo groups were merged. They may be too large when combined.\n";
    }
}

// Implementation of GroupManager::displayGroupStatistics()
void GroupManager::displayGroupStatistics() {
    // Load data first
    loadData();
    
    // Count groups and their sizes
    int groupSizes[MAX_GROUPS];
    int groupCount = 0;
    
    // Initialize the groupSizes array
    for (int i = 0; i < MAX_GROUPS; i++) {
        groupSizes[i] = 0;
    }
    
    // Count the groups and their sizes
    countGroups(groupCount, groupSizes);
    
    // Display statistics
    std::cout << "\n--- Group Statistics ---\n";
    std::cout << "Total number of groups: " << groupCount << "\n";
    
    if (groupCount > 0) {
        // Find min, max, and calculate average
        int minSize = MAX_MEMBERS_PER_GROUP;
        int maxSize = 0;
        int totalPlayers = 0;
        
        for (int i = 0; i < groupCount; i++) {
            if (groupSizes[i] < minSize) minSize = groupSizes[i];
            if (groupSizes[i] > maxSize) maxSize = groupSizes[i];
            totalPlayers += groupSizes[i];
        }
        
        double avgSize = static_cast<double>(totalPlayers) / groupCount;
        
        std::cout << "Smallest group size: " << minSize << " members\n";
        std::cout << "Largest group size: " << maxSize << " members\n";
        std::cout << "Average group size: " << avgSize << " members\n";
        
        // Count groups by size
        int sizeDistribution[MAX_MEMBERS_PER_GROUP + 1] = {0};
        for (int i = 0; i < groupCount; i++) {
            sizeDistribution[groupSizes[i]]++;
        }
        
        std::cout << "\nGroup size distribution:\n";
        for (int i = 1; i <= MAX_MEMBERS_PER_GROUP; i++) {
            if (sizeDistribution[i] > 0) {
                std::cout << "Groups with " << i << " members: " << sizeDistribution[i] << "\n";
            }
        }
    }
}

// Implementation of GroupManager::createGroup()
void GroupManager::createGroup() {
    // Load data first
    loadData();
    
    // Get group details from user
    char groupName[50];
    std::cout << "\n--- Create New Group ---\n";
    std::cout << "Enter group name: ";
    std::cin.getline(groupName, 50);
    
    // Generate a random group ID
    char groupID[10];
    generateSequentialGroupID(groupID);
    
    std::cout << "\nCreated group: " << groupName << " [" << groupID << "]\n";
    
    // Ask user to add players to the group
    std::cout << "\nWould you like to add players to this group? (y/n): ";
    char choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear the newline
    
    if (choice == 'y' || choice == 'Y') {
        // Display available players who are not in a group
        std::cout << "\nAvailable players:\n";
        int availableCount = 0;
        
        for (int i = 0; i < rowCount; i++) {
            // Skip withdrawn players (Index 6) or players already in a group (Index 7)
            if (strcmp(data[i][6], "Yes") == 0 || data[i][7][0] != '\0') continue;
            
            std::cout << ++availableCount << ". " << data[i][1] << " (ID: " << data[i][0] << ")\n";
        }
        
        if (availableCount == 0) {
            std::cout << "No available players to add to the group.\n";
            return;
        }
        
        // Ask user which players to add
        int playerCount = 0;
        while (playerCount < MAX_MEMBERS_PER_GROUP) {
            std::cout << "\nEnter player name to add (or 'done' to finish): ";
            char playerName[50];
            std::cin.getline(playerName, 50);
            
            if (strcmp(playerName, "done") == 0) {
                break;
            }
            
            // Find the player in the data
            bool found = false;
            for (int i = 0; i < rowCount; i++) {
                // Skip withdrawn players (Index 6) or players already in a group (Index 7)
                if (strcmp(data[i][6], "Yes") == 0 || data[i][7][0] != '\0') continue;
                
                // PlayerName is index 1
                if (strcmp(data[i][1], playerName) == 0) {
                    // Assign to this group (GroupID is index 7, GroupName is index 8)
                    strncpy(data[i][7], groupID, 119);
                    data[i][7][119] = '\0';
                    
                    strncpy(data[i][8], groupName, 119);
                    data[i][8][119] = '\0';
                    
                    playerCount++;
                    found = true;
                    
                    std::cout << "Added player " << playerName << " to group " 
                              << groupName << " [" << groupID << "]\n";
                    break;
                }
            }
            
            if (!found) {
                std::cout << "Player not found or already in a group. Please try again.\n";
            }
            
            if (playerCount >= MAX_MEMBERS_PER_GROUP) {
                std::cout << "\nGroup is now full with " << MAX_MEMBERS_PER_GROUP << " members.\n";
                break;
            }
        }
    }
    
    // Save changes
    saveData();
    std::cout << "\nGroup created successfully!\n";
}

// Implementation of GroupManager::organizeGroups()
void GroupManager::organizeGroups() {
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
                assignToGroups();
                break;
            case 2:
                mergeSmallGroups();
                break;
            case 3:
                displayGroupStatistics();
                break;
            case 4:
                createGroup();
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

// Implementation of GroupManager::countGroups()
void GroupManager::countGroups(int& groupCount, int groupSizes[]) {
    // Initialize
    groupCount = 0;
    char groupIDs[MAX_GROUPS][10];
    
    for (int i = 0; i < MAX_GROUPS; i++) {
        groupIDs[i][0] = '\0';
        groupSizes[i] = 0;
    }
    
    // Count groups and their sizes
    for (int i = 0; i < rowCount; i++) {
        // Skip withdrawn players (Index 6) or players without a group (Index 7)
        if (strcmp(data[i][6], "Yes") == 0 || data[i][7][0] == '\0') continue;
        
        bool found = false;
        for (int j = 0; j < groupCount; j++) {
            // GroupID is index 7
            if (strcmp(groupIDs[j], data[i][7]) == 0) {
                groupSizes[j]++;
                found = true;
                break;
            }
        }
        
        // If not found, add to unique groups
        if (!found && groupCount < MAX_GROUPS) {
            strncpy(groupIDs[groupCount], data[i][7], 9); // GroupID is index 7
            groupIDs[groupCount][9] = '\0';
            groupSizes[groupCount] = 1;
            groupCount++;
        }
    }
}

// Function to organize groups (wrapper for the class method)
void organize_groups() {
    GroupManager manager;
    manager.organizeGroups();
}

#endif // GROUP_MANAGER_HPP
