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
        char cols[9][100];
        int num_cols = split(line, ',', cols, 9);
        
        // Check for matching ID or email (Indices for ID and Email are now 0 and 2 respectively)
        if (strcmp(cols[0], playerID) == 0 || strcmp(cols[2], email) == 0) {
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
        char cols[9][100];
        int num_cols = split(line, ',', cols, 9);
        
        // Check if player is in this group and not withdrawn (Adjusted indices)
        if (num_cols >= 7 && strcmp(cols[6], "No") == 0 && strcmp(cols[7], groupID) == 0) {
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

    // Get the numeric part of the group ID to check
    int checkID = atoi(groupID + 3);

    while (file.getline(line, 500)) {
        char cols[8][100];
        int num_cols = split(line, ',', cols, 8);
        
        // Check for matching group ID (column 7) and ensure player is not withdrawn
        if (num_cols >= 7 && strcmp(cols[5], "No") == 0 && strncmp(cols[6], "GRP", 3) == 0) {
            int existingID = atoi(cols[6] + 3);
            if (existingID == checkID) {
                std::cout << "Debug: Found matching group ID '" << groupID << "'. Not unique.\n";
                file.close();
                return false;
            }
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
            if (!(std::cin >> groupSize)) {
                // Clear error flags
                std::cin.clear();
                // Clear input buffer
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number between 2 and " << MAX_MEMBERS_PER_GROUP << ".\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear any remaining input
            
            if (groupSize < 2 || groupSize > MAX_MEMBERS_PER_GROUP) {
                std::cout << "Invalid group size. Please enter a number between 2 and " << MAX_MEMBERS_PER_GROUP << ".\n";
            }
        }
        
        // Collect information for all group members
        Player groupMembers[MAX_MEMBERS_PER_GROUP];
        char groupID[10];
        char groupName[50];
        
        // Find the highest PlayerID in both the queue and the CSV
        int highestPlayerID = -1;
        // Check in the queue
        for (int i = front; i <= rear; ++i) {
            if (strncmp(arr[i].PlayerID, "PLY", 3) == 0) {
                int idNum = atoi(arr[i].PlayerID + 3);
                if (idNum > highestPlayerID) highestPlayerID = idNum;
            }
        }
        // Check in the CSV
        std::ifstream fileP("CheckedIn.csv");
        if (fileP.is_open()) {
            char line[500];
            fileP.getline(line, 500); // Skip header
            while (fileP.getline(line, 500)) {
                char cols[9][100];
                int num_cols = split(line, ',', cols, 9);
                if (num_cols >= 1 && strncmp(cols[0], "PLY", 3) == 0) {
                    int idNum = atoi(cols[0] + 3);
                    if (idNum > highestPlayerID) highestPlayerID = idNum;
                }
            }
            fileP.close();
        }
        // Assign unique PlayerIDs to each group member
        for (int memberIndex = 0; memberIndex < groupSize; memberIndex++) {
            std::cout << "\n--- Enter details for group member " << memberIndex + 1 << " ---\n";
            std::cout << "Enter player name: ";
            std::cin.getline(groupMembers[memberIndex].name, 50);
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
            // Generate unique PlayerID for this member
            highestPlayerID++;
            sprintf(groupMembers[memberIndex].PlayerID, "PLY%03d", highestPlayerID);
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
        
        // Find the highest GroupID in both the queue and the CSV
        int highestGroupID = -1;
        // Check in the queue
        for (int i = front; i <= rear; ++i) {
            if (strncmp(arr[i].groupID, "GRP", 3) == 0) {
                int idNum = atoi(arr[i].groupID + 3);
                if (idNum > highestGroupID) highestGroupID = idNum;
            }
        }
        // Check in the CSV
        std::ifstream fileG("CheckedIn.csv");
        if (fileG.is_open()) {
            char line[500];
            fileG.getline(line, 500); // Skip header
            while (fileG.getline(line, 500)) {
                char cols[9][100];
                int num_cols = split(line, ',', cols, 9);
                if (num_cols >= 7 && strncmp(cols[7], "GRP", 3) == 0) {
                    // Only compare the numeric part after "GRP"
                    int idNum = atoi(cols[7] + 3);
                    if (idNum > highestGroupID) highestGroupID = idNum;
                }
            }
            fileG.close();
        }
        
        // Generate new group ID
        highestGroupID++;
        sprintf(groupID, "GRP%03d", highestGroupID);
        
        std::cout << "Auto-generated Group ID: " << groupID << "\n";
        std::cout << "Enter group name: ";
        std::cin.getline(groupName, 50);
        
        std::cout << "\nCreated new group: " << groupName << " [" << groupID << "]\n";
        
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
        
        // Generate a sequential player ID
        generateSequentialPlayerID(p.PlayerID);
        
        // Ensure ID is unique in the queue (Less critical with sequential but keep)
        while (!is_id_unique(p.PlayerID, arr, front, rear)) {
             generateSequentialPlayerID(p.PlayerID);
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
            char arr[100][9][120];
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
                if (strcmp(arr[i][6], "Yes") == 0 || arr[i][7][0] == '\0') continue;
                
                bool found = false;
                for (int j = 0; j < uniqueGroupCount; j++) {
                    if (strcmp(uniqueGroups[j][0], arr[i][7]) == 0) {
                        groupSizes[j]++;
                        found = true;
                        break;
                    }
                }
                
                if (!found && uniqueGroupCount < MAX_GROUPS) {
                    strncpy(uniqueGroups[uniqueGroupCount][0], arr[i][7], 119);
                    uniqueGroups[uniqueGroupCount][0][119] = '\0';
                    
                    strncpy(uniqueGroups[uniqueGroupCount][1], arr[i][8], 119);
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
                sprintf(groupID, "GRP%03d", highestGroupID);
                
                std::cout << "Auto-generated Group ID: " << groupID << "\n";
                std::cout << "Enter group name: ";
                std::cin.getline(groupName, 50);
            }
        } else {
            // Player chose not to join existing group - create a new group
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
            sprintf(groupID, "GRP%03d", highestGroupID);
            
            std::cout << "Auto-generated Group ID: " << groupID << "\n";
            std::cout << "Enter group name: ";
            std::cin.getline(groupName, 50);
            assigned = true;
        }
        
        // Set group information
        p.priority = priority;
        // Only assign group info if assigned to a group
        if (assigned) {
            strncpy(p.groupID, groupID, 9);
            p.groupID[9] = '\0';
            strncpy(p.groupName, groupName, 49);
            p.groupName[49] = '\0';
        } else {
            // If not assigned, set group info to empty
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
    std::cout << "Checked in player: " << arr[front].name << " (ID: " << arr[front].PlayerID << ")\n";
    
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
    
    // Create a temporary file for writing
    std::ofstream tempFile("temp.csv");
    if (!tempFile.is_open()) {
        std::cout << "Error: Could not create temporary file.\n";
        return;
    }
    
    // Write header
    tempFile << "PlayerID,PlayerName,PlayerEmail,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
    
    // Copy existing entries from CheckedIn.csv
    std::ifstream inFile("CheckedIn.csv");
    if (inFile.is_open()) {
        char line[500];
        inFile.getline(line, 500); // Skip header
        
        while (inFile.getline(line, 500)) {
            tempFile << line << "\n";
        }
        inFile.close();
    }
    
    // Write the new player entry
    tempFile << arr[front].PlayerID << ","
             << arr[front].name << ","
             << arr[front].PlayerEmail << ","
             << priorityStr << ","
             << timeStr << ","
             << "Checked-in,"
             << "No,";
    
    // Only write group ID and name if they exist
    if (arr[front].groupID[0] != '\0') {
        tempFile << arr[front].groupID << ","
                 << arr[front].groupName;
    }
    tempFile << "\n";
    
    tempFile.close();
    
    // Replace the original file with the temporary file
    remove("CheckedIn.csv");
    rename("temp.csv", "CheckedIn.csv");
    
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
    if (isEmpty()) {
        std::cout << "Queue is empty!\n";
        return;
    }

    // Find the player to replace
    int replaceIndex = -1;
    for (int i = front; i <= rear; ++i) {
        if (strcmp(arr[i].name, oldName) == 0) {
            replaceIndex = i;
            break;
        }
    }

    if (replaceIndex == -1) {
        std::cout << "Player " << oldName << " not found in queue.\n";
        return;
    }

    // Store the old player's information
    Player oldPlayer = arr[replaceIndex];

    // Create new player with updated information
    Player newPlayer;
    strncpy(newPlayer.name, newName, 49);
    newPlayer.name[49] = '\0';
    strncpy(newPlayer.PlayerID, oldPlayer.PlayerID, 9);
    newPlayer.PlayerID[9] = '\0';
    strncpy(newPlayer.PlayerEmail, oldPlayer.PlayerEmail, 99);
    newPlayer.PlayerEmail[99] = '\0';
    newPlayer.priority = priority;
    strncpy(newPlayer.groupID, oldPlayer.groupID, 9);
    newPlayer.groupID[9] = '\0';
    strncpy(newPlayer.groupName, oldPlayer.groupName, 49);
    newPlayer.groupName[49] = '\0';

    // Remove the old player
    for (int i = replaceIndex; i < rear; ++i) {
        arr[i] = arr[i + 1];
    }
    rear--;

    // Find the correct position to insert the new player based on priority
    int insertPos = front;
    while (insertPos <= rear && arr[insertPos].priority <= newPlayer.priority) {
        insertPos++;
    }

    // Shift elements to make room for the new player
    for (int i = rear; i >= insertPos; --i) {
        arr[i + 1] = arr[i];
    }

    // Insert the new player
    arr[insertPos] = newPlayer;
    rear++;

    std::cout << "Player " << oldName << " replaced with " << newName << " (Priority: " << priority << ")\n";
}

#endif // PLAYER_QUEUE_HPP

