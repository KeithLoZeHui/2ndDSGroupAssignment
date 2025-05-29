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

// Count the number of players in a specific group
int PlayerQueue::count_players_in_group(const char* groupID) {
    std::ifstream file("CheckedIn.csv");
    if (!file.is_open()) return 0;
    
    char line[500];
    file.getline(line, 500); // Skip header
    
    int count = 0;
    while (file.getline(line, 500)) {
        char cols[8][100];
        int num_cols = split(line, ',', cols, 8);
        
        // Check if player is in this group and not withdrawn
        if (num_cols >= 7 && strcmp(cols[5], "No") == 0 && strcmp(cols[6], groupID) == 0) {
            count++;
        }
    }
    
    return count;
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

bool PlayerQueue::is_group_id_unique(const char* groupID) {
    std::ifstream file("CheckedIn.csv");
    if (!file.is_open()) {
        std::cout << "Debug: CheckedIn.csv not found or could not be opened. Group ID '" << groupID << "' is unique.\n";
        return true; // If file doesn't exist or can't be opened, ID is unique
    }
    
    char line[500];
    file.getline(line, 500); // Skip header
    
    std::cout << "Debug: Checking for uniqueness of group ID '" << groupID << "' in CheckedIn.csv...\n";

    while (file.getline(line, 500)) {
        char cols[8][100];
        int num_cols = split(line, ',', cols, 8);
        
        // Check for matching group ID (column 6) and ensure player is not withdrawn
        if (num_cols >= 7 && strcmp(cols[5], "No") == 0 && strcmp(cols[6], groupID) == 0) {
            std::cout << "Debug: Found matching group ID '" << groupID << "'. Not unique.\n";
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;
}

// Helper function to check if adding players to a group would exceed the limit
bool PlayerQueue::can_add_to_group(const char* groupID, int numNewPlayers) {
    if (groupID[0] == '\0') return true; // No group ID means can create new group
    
    int currentCount = count_players_in_group(groupID);
    return (currentCount + numNewPlayers) <= MAX_MEMBERS_PER_GROUP;
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
            
            // Generate a sequential player ID (Changed from random)
            generateSequentialPlayerID(groupMembers[memberIndex].PlayerID);
            
            // Ensure ID is unique in the queue (This check might be less critical with sequential IDs but keep for safety)
            while (!is_id_unique(groupMembers[memberIndex].PlayerID, arr, front, rear)) {
                // If somehow not unique, generate next sequential ID
                generateSequentialPlayerID(groupMembers[memberIndex].PlayerID);
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
        
        // Check if creating a new group is possible
        char existing_arr[100][8][120];
        int existing_n = 0;
        load_checkedin_csv(existing_arr, existing_n);
        
        // Check if we can create a new group
        int currentGroupCount = 0;
        char uniqueGroups[MAX_GROUPS][10];
        for(int i = 0; i < existing_n; ++i) {
            if(existing_arr[i][6][0] != '\0' && strcmp(existing_arr[i][5], "No") == 0) {
                bool found = false;
                for(int j = 0; j < currentGroupCount; ++j) {
                    if(strcmp(uniqueGroups[j], existing_arr[i][6]) == 0) {
                        found = true;
                        break;
                    }
                }
                if(!found && currentGroupCount < MAX_GROUPS) {
                    strncpy(uniqueGroups[currentGroupCount], existing_arr[i][6], 9);
                    uniqueGroups[currentGroupCount][9] = '\0';
                    currentGroupCount++;
                }
            }
        }

        if (currentGroupCount >= MAX_GROUPS) {
            std::cout << "\nCannot create a new group. Maximum number of groups reached.\n";
            std::cout << "Please register players individually to potentially fill existing incomplete groups.\n";
            return;
        }

        // Check if we can add to an existing incomplete group
        bool assignedToExisting = false;
        for(int i = 0; i < currentGroupCount; ++i) {
            if (can_add_to_group(uniqueGroups[i], groupSize)) {
                strncpy(groupID, uniqueGroups[i], 9);
                groupID[9] = '\0';
                
                // Find the group name
                for(int j = 0; j < existing_n; ++j) {
                    if(strcmp(existing_arr[j][6], groupID) == 0 && strcmp(existing_arr[j][5], "No") == 0) {
                        strncpy(groupName, existing_arr[j][7], 49);
                        groupName[49] = '\0';
                        // **Debug:** Print the group name found
                        std::cout << "Debug: Found existing group name: " << groupName << " for ID: " << groupID << "\n";
                        break;
                    }
                }
                
                assignedToExisting = true;
                std::cout << "Assigned to existing group: " << groupName << " [" << groupID << "]\n";
                break;
            }
        }

        if (!assignedToExisting) {
            // Generate a new unique group ID with retry limit
            const int MAX_RETRIES = 10;
            int retries = 0;
            bool unique = false;
            
            std::cout << "Debug: Generating unique group ID...\n";
            do {
                // Use sequential ID generator for group ID
                generateSequentialGroupID(groupID);
                unique = is_group_id_unique(groupID);
                retries++;
                
                if (retries >= MAX_RETRIES) {
                    std::cout << "Error: Could not generate a unique group ID after " << MAX_RETRIES << " attempts.\n";
                    return;
                }
            } while (!unique);
            
            std::cout << "Debug: Unique group ID generated: " << groupID << "\n";
            
            // Get group name
            std::cout << "\n--- Group Information ---\n";
            std::cout << "Enter group name: ";
            std::cin.getline(groupName, 50);
            
            std::cout << "\nCreated new group: " << groupName << " [" << groupID << "]\n";
        }
        
        std::cout << "Debug: Adding group members to queue...\n";
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
            // Ensure i is within valid bounds before accessing arr[i]
            while (i >= front && i < capacity && arr[i].priority > groupMembers[memberIndex].priority) {
                // Ensure arr[i+1] is within valid bounds before writing
                if (i + 1 < capacity) {
                    arr[i + 1] = arr[i];
                } else {
                    // This case should ideally not happen if resize works correctly,
                    // but adding a safeguard can help debug.
                    std::cerr << "Error: Attempting to write out of bounds during priority insertion.\n";
                    // Depending on desired behavior, you might handle this differently,
                    // e.g., return false, resize again, or exit.
                    // For now, we'll just break the loop to prevent crash.
                    break;
                }
                i--;
            }
            
            // Ensure insertion index is within valid bounds
            if (i + 1 >= front && i + 1 < capacity) {
                arr[i + 1] = groupMembers[memberIndex];
                rear++;
            } else {
                 std::cerr << "Error: Attempting to insert out of bounds during priority insertion.\n";
                 // Handle error: Player not added to queue, or other error handling
            }
            
            std::cout << "Debug: Player " << groupMembers[memberIndex].name << " added to queue.\n";
        }
        std::cout << "Debug: Finished adding group members to queue.\n";
    } else {
        // Individual registration
        Player p;
        
        // Get player details
        std::cout << "Enter player name: ";
        std::cin.getline(p.name, 50);
        
        // Generate a sequential player ID (Changed from random)
        generateSequentialPlayerID(p.PlayerID);
        
        // Ensure ID is unique in the queue (This check might be less critical with sequential IDs but keep for safety)
        while (!is_id_unique(p.PlayerID, arr, front, rear)) {
             // If somehow not unique, generate next sequential ID
            generateSequentialPlayerID(p.PlayerID);
        }
        
        // Get email
        bool validEmail = false;
        while (!validEmail) {
            std::cout << "Enter player email: ";
            std::cin.getline(p.PlayerEmail, 100);
            
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
            int groupSizes[MAX_GROUPS] = {0};
            
            // First pass: collect unique groups and their sizes
            for (int i = 0; i < n; i++) {
                // Skip withdrawn players or players without a group
                if (strcmp(arr[i][5], "Yes") == 0 || arr[i][6][0] == '\0') continue;
                
                bool found = false;
                for (int j = 0; j < uniqueGroupCount; j++) {
                    if (strcmp(uniqueGroups[j][0], arr[i][6]) == 0) {
                        groupSizes[j]++;
                        found = true;
                        break;
                    }
                }
                
                if (!found && uniqueGroupCount < MAX_GROUPS) {
                    strncpy(uniqueGroups[uniqueGroupCount][0], arr[i][6], 119);
                    uniqueGroups[uniqueGroupCount][0][119] = '\0';
                    
                    strncpy(uniqueGroups[uniqueGroupCount][1], arr[i][7], 119);
                    uniqueGroups[uniqueGroupCount][1][119] = '\0';
                    
                    groupSizes[uniqueGroupCount] = 1;
                    uniqueGroupCount++;
                    groupsFound = true;
                }
            }
            
            // Display available groups that aren't full
            if (groupsFound) {
                std::cout << "\nAvailable groups:\n";
                for (int i = 0; i < uniqueGroupCount; i++) {
                    if (groupSizes[i] < MAX_MEMBERS_PER_GROUP) {
                        std::cout << i+1 << ". " << uniqueGroups[i][1] << " [" << uniqueGroups[i][0] 
                                  << "] (" << groupSizes[i] << "/" << MAX_MEMBERS_PER_GROUP << " members)\n";
                    }
                }
            }
            
            // Find an incomplete group with less than MAX_MEMBERS_PER_GROUP members
            for (int i = 0; i < uniqueGroupCount; i++) {
                if (groupSizes[i] < MAX_MEMBERS_PER_GROUP) {
                    // Assign to this incomplete group
                    strncpy(groupID, uniqueGroups[i][0], 9);
                    groupID[9] = '\0';

                    // **Fix:** Copy the group name as well
                    strncpy(groupName, uniqueGroups[i][1], 49);
                    groupName[49] = '\0';

                    assigned = true;
                    std::cout << "Assigned to incomplete group: " << groupName << " [" << groupID << "] (" 
                              << groupSizes[i] + 1 << "/" << MAX_MEMBERS_PER_GROUP << " members)\n";
                    break;
                }
            }
            
            if (!assigned) {
                std::cout << "No available incomplete group. Creating new group.\n";
                // Use sequential ID generator for group ID
                generateSequentialGroupID(groupID);
                std::cout << "Auto-generated Group ID: " << groupID << "\n";
                
                std::cout << "Enter group name: ";
                std::cin.getline(groupName, 50);
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
            
            // First pass: collect all unique groups and their sizes
            for (int i = 0; i < n; i++) {
                // Skip withdrawn players or players without a group
                if (strcmp(arr[i][5], "Yes") == 0 || arr[i][6][0] == '\0') continue;
                
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

                    // **Fix:** Copy the group name as well
                    strncpy(groupName, uniqueGroups[i][1], 49);
                    groupName[49] = '\0';

                    assigned = true;
                    std::cout << "Assigned to incomplete group: " << groupName << " [" << groupID << "] (" 
                              << groupSizes[i] + 1 << "/" << MAX_MEMBERS_PER_GROUP << " members)\n";
                    break;
                }
            }
            
            if (!assigned && uniqueGroupCount < MAX_GROUPS) { // Ensure we don't exceed max groups
                std::cout << "No available incomplete group. Creating new group.\n";
                // Use sequential ID generator for group ID
                do {
                    generateSequentialGroupID(groupID);
                } while (!is_group_id_unique(groupID));
                
                std::cout << "Auto-generated Group ID: " << groupID << "\n";
                
                std::cout << "Enter group name: ";
                std::cin.getline(groupName, 50);
                
                // Player will be assigned to this new group later in the function
                assigned = true; // Mark as assigned to prevent creating another new group
            } else if (!assigned) { // All groups full and cannot create new one
                 std::cout << "No available incomplete group and cannot create a new one. Please try again later.\n";
            }
        }
        
        // Set group information
        p.priority = priority;
        // Only assign group info if assigned to a group
        if (assigned) {
            strncpy(p.groupID, groupID, 9);
            p.groupID[9] = '\0';
            strncpy(p.groupName, groupName, 49);
            p.groupName[49] = '\0';
        } else { // If not assigned, set group info to empty
            p.groupID[0] = '\0';
            p.groupName[0] = '\0';
        }
        
        // Insert player maintaining priority order
        int i = rear;
        while (i >= front && arr[i].priority > p.priority) {
            arr[i + 1] = arr[i];
            i--;
        }
        arr[i + 1] = p;
        rear++;
        
        std::cout << "[FIFO] Player '" << p.name << "' registered";
        if (p.groupID[0] != '\0') {
             std::cout << " in group '" << p.groupName << " [" << p.groupID << "]";
        }
        std::cout << " with priority " << p.priority << ".\n";
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
    
    // Use the player ID directly without modification
    char uniquePlayerID[20];
    strcpy(uniquePlayerID, arr[front].PlayerID);
    
    // Check if adding this player would exceed the group limit in the CSV
    int currentGroupSize = 0;
    if (arr[front].groupID[0] != '\0') {
        currentGroupSize = count_players_in_group(arr[front].groupID);
    }
    
    bool assignedToGroup = (arr[front].groupID[0] != '\0' && currentGroupSize < MAX_MEMBERS_PER_GROUP);
    
    if (file_exists("CheckedIn.csv")) {
        // If CheckedIn.csv exists, add player to it
        std::ofstream outfile("CheckedIn.csv", std::ios::app);
        outfile << uniquePlayerID << ","
                << arr[front].name << ","
                << priorityStr << ","
                << timeStr << ","
                << "Checked-in,"
                << "No,";
        
        // Only write group ID and name if the player was assigned to a group
        if (arr[front].groupID[0] != '\0') {
            outfile << arr[front].groupID << ","
                    << arr[front].groupName << "\n";
        } else {
            outfile << ",\n"; // Write empty group ID and name
        }
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
                << "No,";
        
        // Only write group ID and name if the player was assigned to a group
        if (arr[front].groupID[0] != '\0') {
            outfile << arr[front].groupID << ","
                    << arr[front].groupName << "\n";
        } else {
            outfile << ",\n"; // Write empty group ID and name
        }
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
