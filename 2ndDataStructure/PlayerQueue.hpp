#ifndef PLAYER_QUEUE_HPP
#define PLAYER_QUEUE_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include "KeithTask2.hpp"
#include "Utils.hpp"

// Check if file exists
bool PlayerQueue::file_exists(const char* filename) {
    std::ifstream file(filename);
    return file.good();
}

// Check if string is a valid ID
bool PlayerQueue::is_valid_id(const char* s) {
    int len = strlen(s);
    if (len != 4) return false;
    for (int i = 0; i < len; i++) {
        if (!isalnum(s[i])) return false;
    }
    return true;
}

// Check if email is valid (basic check for @)
bool PlayerQueue::is_valid_email(const char* s) {
    // Simple check for @ symbol in email
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
        if (s[i] == '@') return true;
    }
    return false;
}

// Check for duplicates in CSV
bool PlayerQueue::is_duplicate_in_csv(const char* filename, const char* playerID, const char* email) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    char line[500];
    file.getline(line, 500); // Skip header
    
    while (file.getline(line, 500)) {
        char cols[8][100];
        int num_cols = split(line, ',', cols, 8);
        
        // Check for matching ID or email
        if (strcmp(cols[0], playerID) == 0) {
            return true;
        }
    }
    
    return false;
}

// Constructor
PlayerQueue::PlayerQueue(int initial_capacity) {
    capacity = initial_capacity;
    arr = new Player[capacity];
    front = 0;
    rear = -1;
}

// Destructor
PlayerQueue::~PlayerQueue() {
    delete[] arr;
}

void PlayerQueue::enqueue() {
    if (isFull()) {
        resize();
    }
    
    // Ask if registering as a group or individual
    char registerType;
    std::cout << "Register as (I)ndividual or (G)roup? (I/G): ";
    std::cin >> registerType;
    std::cin.ignore(); // Clear the newline
    
    if (registerType == 'G' || registerType == 'g') {
        // Group registration
        int groupSize = 0;
        while (groupSize < 2 || groupSize > MAX_MEMBERS_PER_GROUP) {
            std::cout << "Enter number of members in the group (2-" << MAX_MEMBERS_PER_GROUP << "): ";
            std::cin >> groupSize;
            std::cin.ignore(); // Clear the newline
            
            if (groupSize < 2 || groupSize > MAX_MEMBERS_PER_GROUP) {
                std::cout << "Invalid group size. Please enter a number between 2 and " << MAX_MEMBERS_PER_GROUP << ".\n";
            }
        }
        
        // Collect information for all group members
        Player groupMembers[MAX_MEMBERS_PER_GROUP];
        
        for (int memberIndex = 0; memberIndex < groupSize; memberIndex++) {
            std::cout << "\n--- Enter details for group member " << memberIndex + 1 << " ---\n";
            
            // Get player details
            std::cout << "Enter player name: ";
            std::cin.getline(groupMembers[memberIndex].name, 50);
            
            // Generate a random player ID
            generateRandomID(groupMembers[memberIndex].PlayerID, 4);
            
            // Ensure ID is unique in the queue
            while (!is_id_unique(groupMembers[memberIndex].PlayerID, arr, front, rear)) {
                generateRandomID(groupMembers[memberIndex].PlayerID, 4);
            }
            
            // Get email
            bool validEmail = false;
            while (!validEmail) {
                std::cout << "Enter player email: ";
                std::cin.getline(groupMembers[memberIndex].PlayerEmail, 100);
                
                if (is_valid_email(groupMembers[memberIndex].PlayerEmail)) {
                    validEmail = true;
                } else {
                    std::cout << "Invalid email format. Please try again.\n";
                }
            }
            
            // Get priority
            int priority = 0;
            while (priority < 1 || priority > 3) {
                std::cout << "Enter priority (1=Early-bird, 2=Wildcard, 3=Normal): ";
                std::cin >> priority;
                std::cin.ignore(); // Clear the newline
                
                if (priority < 1 || priority > 3) {
                    std::cout << "Invalid priority. Please enter 1, 2, or 3.\n";
                }
            }
            
            groupMembers[memberIndex].priority = priority;
        }
        
        // After collecting all player information, create a group
        char groupID[10];
        char groupName[50];
        
        // Generate a random group ID
        generateRandomID(groupID, 6);
        
        // Get group name
        std::cout << "\n--- Group Information ---\n";
        std::cout << "Enter group name: ";
        std::cin.getline(groupName, 50);
        
        std::cout << "\nCreated group: " << groupName << " [" << groupID << "]\n";
        
        // Assign group information to all members and add them to the queue
        for (int memberIndex = 0; memberIndex < groupSize; memberIndex++) {
            strncpy(groupMembers[memberIndex].groupID, groupID, 9);
            groupMembers[memberIndex].groupID[9] = '\0';
            
            strncpy(groupMembers[memberIndex].groupName, groupName, 49);
            groupMembers[memberIndex].groupName[49] = '\0';
            
            // Insert player maintaining priority order
            if (isFull()) {
                resize();
            }
            
            int i = rear;
            while (i >= front && arr[i].priority > groupMembers[memberIndex].priority) {
                arr[i + 1] = arr[i];
                i--;
            }
            arr[i + 1] = groupMembers[memberIndex];
            rear++;
            
            std::cout << "[FIFO] Player '" << groupMembers[memberIndex].name << "' registered in group '" 
                      << groupName << "' with priority " << groupMembers[memberIndex].priority << ".\n";
        }
    } else {
        // Individual registration
        Player p;
        
        // Get player details
        std::cout << "Enter player name: ";
        std::cin.getline(p.name, 50);
        
        // Generate a random player ID
        generateRandomID(p.PlayerID, 4);
        
        // Ensure ID is unique in the queue
        while (!is_id_unique(p.PlayerID, arr, front, rear)) {
            generateRandomID(p.PlayerID, 4);
        }
        
        // Get email
        bool validEmail = false;
        while (!validEmail) {
            std::cout << "Enter player email: ";
            std::cin.getline(p.PlayerEmail, 100);
            std::cout << "debug"<< std ::endl;
            if (is_valid_email(p.PlayerEmail)) {
                validEmail = true;
            } else {
                std::cout << "Invalid email format. Please try again.\n";
            }
        }
        
        // Get priority
        int priority = 0;
        while (priority < 1 || priority > 3) {
            std::cout << "Enter priority (1=Early-bird, 2=Wildcard, 3=Normal): ";
            std::cin >> priority;
            std::cin.ignore(); // Clear the newline
            
            if (priority < 1 || priority > 3) {
                std::cout << "Invalid priority. Please enter 1, 2, or 3.\n";
            }
        }
        
        // Group information
        char groupID[10] = {0};
        char groupName[50] = {0};
        
        // Ask if player wants to join an existing group
        char joinGroup;
        std::cout << "Join an existing group? (y/n): ";
        std::cin >> joinGroup;
        std::cin.ignore(); // Clear the newline
        
        bool assigned = false;
        
        if (joinGroup == 'y' || joinGroup == 'Y') {
            // Load existing groups from CSV
            char arr[100][8][120];
            int n = 0;
            load_checkedin_csv(arr, n);
            
            // Display existing groups
            std::cout << "\nExisting groups:\n";
            bool groupsFound = false;
            
            // Create a temporary array to store unique group IDs and names
            char uniqueGroups[MAX_GROUPS][2][120]; // [0] = ID, [1] = Name
            int uniqueGroupCount = 0;
            
            for (int i = 0; i < n; i++) {
                // Skip withdrawn players or players without a group
                if (strcmp(arr[i][5], "Yes") == 0 || arr[i][6][0] == '\0') continue;
                
                // Check if this group is already in our unique groups list
                bool found = false;
                for (int j = 0; j < uniqueGroupCount; j++) {
                    if (strcmp(uniqueGroups[j][0], arr[i][6]) == 0) {
                        found = true;
                        break;
                    }
                }
                
                // If not found, add to unique groups
                if (!found && uniqueGroupCount < MAX_GROUPS) {
                    strncpy(uniqueGroups[uniqueGroupCount][0], arr[i][6], 119);
                    uniqueGroups[uniqueGroupCount][0][119] = '\0';
                    
                    strncpy(uniqueGroups[uniqueGroupCount][1], arr[i][7], 119);
                    uniqueGroups[uniqueGroupCount][1][119] = '\0';
                    
                    uniqueGroupCount++;
                    groupsFound = true;
                }
            }
            
            if (groupsFound) {
                // Display unique groups
                for (int i = 0; i < uniqueGroupCount; i++) {
                    std::cout << i+1 << ". " << uniqueGroups[i][1] << " [" << uniqueGroups[i][0] << "]\n";
                }
                
                // Count members in each group
                int groupSizes[MAX_GROUPS] = {0};
                for (int i = 0; i < n; i++) {
                    if (strcmp(arr[i][5], "Yes") == 0) continue; // Skip withdrawn players
                    
                    for (int j = 0; j < uniqueGroupCount; j++) {
                        if (strcmp(arr[i][6], uniqueGroups[j][0]) == 0) {
                            groupSizes[j]++;
                            break;
                        }
                    }
                }
                
                // Ask which group to join
                int groupChoice = 0;
                while (groupChoice < 1 || groupChoice > uniqueGroupCount) {
                    std::cout << "Enter group number to join (0 to create new group): ";
                    std::cin >> groupChoice;
                    std::cin.ignore(); // Clear the newline
                    
                    if (groupChoice == 0) {
                        break;
                    }
                    
                    if (groupChoice < 1 || groupChoice > uniqueGroupCount) {
                        std::cout << "Invalid group number. Please try again.\n";
                    } else if (groupSizes[groupChoice-1] >= MAX_MEMBERS_PER_GROUP) {
                        std::cout << "This group is already full. Please choose another group.\n";
                        groupChoice = 0; // Reset to invalid choice to prompt again
                    }
                }
                
                if (groupChoice > 0) {
                    // Join existing group
                    strncpy(groupID, uniqueGroups[groupChoice-1][0], 9);
                    groupID[9] = '\0';
                    
                    strncpy(groupName, uniqueGroups[groupChoice-1][1], 49);
                    groupName[49] = '\0';
                    
                    assigned = true;
                    std::cout << "Assigned to group: " << groupName << " [" << groupID << "]" << std::endl;
                }
            } else {
                std::cout << "No existing groups found.\n";
            }
        }
        
        // If not assigned to an existing group, check for incomplete groups
        if (!assigned) {
            // Load existing groups from CSV
            char arr[100][8][120];
            int n = 0;
            load_checkedin_csv(arr, n);
            
            // Count members in each group
            char uniqueGroups[MAX_GROUPS][2][120]; // [0] = ID, [1] = Name
            int groupSizes[MAX_GROUPS] = {0};
            int uniqueGroupCount = 0;
            
            // First pass: collect all unique groups
            for (int i = 0; i < n; i++) {
                // Skip withdrawn players or players without a group
                if (strcmp(arr[i][5], "Yes") == 0 || arr[i][6][0] == '\0') continue;
                
                // Check if this group is already in our unique groups list
                bool found = false;
                for (int j = 0; j < uniqueGroupCount; j++) {
                    if (strcmp(uniqueGroups[j][0], arr[i][6]) == 0) {
                        found = true;
                        groupSizes[j]++;
                        break;
                    }
                }
                
                // If not found, add to unique groups
                if (!found && uniqueGroupCount < MAX_GROUPS) {
                    strncpy(uniqueGroups[uniqueGroupCount][0], arr[i][6], 119);
                    uniqueGroups[uniqueGroupCount][0][119] = '\0';
                    
                    strncpy(uniqueGroups[uniqueGroupCount][1], arr[i][7], 119);
                    uniqueGroups[uniqueGroupCount][1][119] = '\0';
                    
                    groupSizes[uniqueGroupCount] = 1;
                    uniqueGroupCount++;
                }
            }
            
            // Find an incomplete group with less than MAX_MEMBERS_PER_GROUP members
            for (int i = 0; i < uniqueGroupCount; i++) {
                if (groupSizes[i] < MAX_MEMBERS_PER_GROUP) {
                    // Assign to this incomplete group
                    strncpy(groupID, uniqueGroups[i][0], 9);
                    groupID[9] = '\0';
                    
                    strncpy(groupName, uniqueGroups[i][1], 49);
                    groupName[49] = '\0';
                    
                    assigned = true;
                    std::cout << "Assigned to incomplete group: " << groupID << " (" << groupName << ")" << std::endl;
                    break;
                }
            }
            
            if (!assigned) {
                std::cout << "No available incomplete group. Creating new group.\n";
                generateRandomID(groupID, 6); // Auto-generate group ID
                std::cout << "Auto-generated Group ID: " << groupID << "\n";
                
                std::cout << "Enter group name: ";
                std::cin.getline(groupName, 50);
            }
        }
        
        // Set group information
        p.priority = priority;
        strncpy(p.groupID, groupID, 9);
        p.groupID[9] = '\0';
        strncpy(p.groupName, groupName, 49);
        p.groupName[49] = '\0';
        
        // Insert player maintaining priority order
        int i = rear;
        while (i >= front && arr[i].priority > p.priority) {
            arr[i + 1] = arr[i];
            i--;
        }
        arr[i + 1] = p;
        rear++;
        
        std::cout << "[FIFO] Player '" << p.name << "' registered in group '" << p.groupID << "' with priority " << p.priority << ".\n";
    }
}

void PlayerQueue::dequeue() {
    if (isEmpty()) {
        std::cout << "Queue is empty!\n";
        return;
    }
    std::cout << "Dequeued player: " << arr[front].name << " (ID: " << arr[front].PlayerID << ")\n";
    
    // Convert priority number to string
    char priorityStr[20];
    switch(arr[front].priority) {
        case 1: strcpy(priorityStr, "Early-bird"); break;
        case 2: strcpy(priorityStr, "Wildcard"); break;
        case 3: strcpy(priorityStr, "Normal"); break;
        default: strcpy(priorityStr, "Normal");
    }
    
    // Get current timestamp
    char timeStr[25];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    
    // Create a unique player ID for the CSV
    char uniquePlayerID[20];
    // Ensure we have a truly unique ID by combining the player ID with a timestamp suffix
    sprintf(uniquePlayerID, "%s_%d", arr[front].PlayerID, static_cast<int>(now % 10000));
    
    if (file_exists("CheckedIn.csv")) {
        // If CheckedIn.csv exists, add player to it
        std::ofstream outfile("CheckedIn.csv", std::ios::app);
        outfile << uniquePlayerID << ","
                << arr[front].name << ","
                << priorityStr << ","
                << timeStr << ","
                << "Checked-in,"
                << "No,"
                << arr[front].groupID << ","
                << arr[front].groupName << "\n";
        outfile.close();
    } else {
        // Create new CheckedIn.csv file with header
        std::ofstream outfile("CheckedIn.csv");
        outfile << "PlayerID,PlayerName,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
        outfile << uniquePlayerID << ","
                << arr[front].name << ","
                << priorityStr << ","
                << timeStr << ","
                << "Checked-in,"
                << "No,"
                << arr[front].groupID << ","
                << arr[front].groupName << "\n";
        outfile.close();
    }
    front++;
}

void PlayerQueue::withdraw(const char* name) {
    if (isEmpty()) {
        std::cout << "Queue is empty!\n";
        return;
    }
    for (int i = front; i <= rear; ++i) {
        if (strcmp(arr[i].name, name) == 0) {
            std::cout << "Player " << name << " withdrawn from queue.\n";
            // Shift all elements after this one
            for (int j = i; j < rear; ++j) {
                arr[j] = arr[j + 1];
            }
            rear--;
            return;
        }
    }
    std::cout << "Player " << name << " not found in queue.\n";
}

void PlayerQueue::replace(const char* oldName, const char* newName, int priority) {
    withdraw(oldName);
    enqueue();
}

#endif // PLAYER_QUEUE_HPP
