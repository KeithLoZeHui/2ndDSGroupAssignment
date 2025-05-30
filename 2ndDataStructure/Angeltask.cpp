#include <iostream>
// #include <string> // Removed as std::string is not allowed and definitions moved to SpectatorQueue.hpp
#include <ctime>
#include <fstream>
#include <cstring> // Required for C-style string functions
#include <cstdio>  // Required for sprintf

#include "KeithTask2.hpp"
#include "CSVOperations.hpp"
#include "Utils.hpp"
#include "PlayerQueue.hpp"
#include "GroupManager.hpp"
#include "SpectatorQueue.hpp" // Include the new header file

// Angel's Part: Live Stream & Spectator Queue Management

// Implementations of StreamQueue methods

// Get current time in YYYY-MM-DD HH:MM:SS format
char* StreamQueue::getCurrentTime() {
    static char buffer[MAX_TIME_LEN]; // Use a static buffer
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

// Add a new spectator to the queue with their details
bool StreamQueue::addSpectator(const char* name, const char* id, 
                               const char* email, const char* playerType, bool isVIP) {
    if (vipCount + regularCount >= maxCapacity || currentArraySize >= MAX_SPECTATORS) {
        std::cout << "Queue is full!\n";
        return false;
    }

    // Store in array first
    int arrayIndex = currentArraySize;
    strncpy(spectatorArray[arrayIndex].name, name, MAX_NAME_LEN - 1);
    spectatorArray[arrayIndex].name[MAX_NAME_LEN - 1] = '\0';
    strncpy(spectatorArray[arrayIndex].id, id, MAX_ID_LEN - 1);
    spectatorArray[arrayIndex].id[MAX_ID_LEN - 1] = '\0';
    strncpy(spectatorArray[arrayIndex].email, email, MAX_EMAIL_LEN - 1);
    spectatorArray[arrayIndex].email[MAX_EMAIL_LEN - 1] = '\0';
    strncpy(spectatorArray[arrayIndex].playerType, playerType, MAX_PLAYER_TYPE_LEN - 1);
    spectatorArray[arrayIndex].playerType[MAX_PLAYER_TYPE_LEN - 1] = '\0';
    spectatorArray[arrayIndex].isVIP = isVIP;
    spectatorArray[arrayIndex].isActive = true;
    
    char* currentTimeStr = getCurrentTime();
    strncpy(spectatorArray[arrayIndex].timeJoined, currentTimeStr, MAX_TIME_LEN - 1);
    spectatorArray[arrayIndex].timeJoined[MAX_TIME_LEN - 1] = '\0';

    currentArraySize++;

    // Create linked list node
    Spectator* newSpectator = new Spectator(name, isVIP, arrayIndex);

    if (isVIP) {
        if (vipFront == nullptr) {
            vipFront = vipRear = newSpectator;
        } else {
            vipRear->next = newSpectator;
            vipRear = newSpectator;
        }
        vipCount++;
    } else {
        if (regularFront == nullptr) {
            regularFront = regularRear = newSpectator;
        } else {
            regularRear->next = newSpectator;
            regularRear = newSpectator;
        }
        regularCount++;
    }
    return true;
}

// Remove a spectator from the queue (VIPs have priority)
bool StreamQueue::removeSpectator() {
    if (vipFront == nullptr && regularFront == nullptr) {
        std::cout << "Queue is empty!\n";
        return false;
    }

    Spectator* temp;
    if (vipFront != nullptr) {
        temp = vipFront;
        vipFront = vipFront->next;
        if (vipFront == nullptr) vipRear = nullptr;
        vipCount--;
    } else {
        temp = regularFront;
        regularFront = regularFront->next;
        if (regularFront == nullptr) regularRear = nullptr;
        regularCount--;
    }

    // Mark as inactive in array
    spectatorArray[temp->arrayIndex].isActive = false;
    
    std::cout << "Removed spectator: " << temp->name << std::endl;
    delete temp;
    return true;
}

// Display current state of VIP and regular queues
void StreamQueue::displayQueue() {
    std::cout << "\nVIP Queue (" << vipCount << " spectators):\n";
    Spectator* current = vipFront;
    while (current != nullptr) {
        const SpectatorInfo& info = spectatorArray[current->arrayIndex];
        std::cout << "Name: " << info.name
                  << ", ID: " << info.id
                  << ", Type: " << info.playerType
                  << ", Joined: " << info.timeJoined << std::endl;
        current = current->next;
    }

    std::cout << "\nRegular Queue (" << regularCount << " spectators):\n";
    current = regularFront;
    while (current != nullptr) {
        const SpectatorInfo& info = spectatorArray[current->arrayIndex];
        std::cout << "Name: " << info.name
                  << ", ID: " << info.id
                  << ", Type: " << info.playerType
                  << ", Joined: " << info.timeJoined << std::endl;
        current = current->next;
    }
}

// Display complete history of all spectators (active and inactive)
void StreamQueue::displaySpectatorHistory() {
    std::cout << "\nComplete Spectator History:\n";
    for (int i = 0; i < currentArraySize; i++) {
        const SpectatorInfo& info = spectatorArray[i];
        std::cout << "Name: " << info.name
                  << ", ID: " << info.id
                  << ", Email: " << info.email
                  << ", Type: " << info.playerType
                  << ", VIP: " << (info.isVIP ? "Yes" : "No")
                  << ", Status: " << (info.isActive ? "Active" : "Inactive")
                  << ", Joined: " << info.timeJoined << std::endl;
    }
}

// Helper function to format player ID in PLY000 format
char* formatPlayerID(int index, char* buffer) {
    sprintf(buffer, "PLY%03d", index);
    return buffer;
}

// Function to demonstrate Task 2 functionality (Keith's Part)
void demonstrateKeithsTask2() {
    // std::cout << "\n=== Task 2: Tournament Registration & Player Queueing (Keith's Part) ===\n"; // Removed explicit mention of Keith's Part
    
    // Load players from CheckedIn.csv
    int numPlayers = 0;
    PlayerCSV* players = loadPlayersFromCSV("CheckedIn.csv", numPlayers);
    
    if (numPlayers == 0) {
        std::cout << "No players found in any CheckedIn.csv file.\n";
        return;
    }
    
    // Display all players from CSV
    // std::cout << "\n--- Loaded Players from CSV ---\n"; // Removed section header
    std::cout << "Total players loaded: " << numPlayers << "\n";
    std::cout << "PlayerID | PlayerName | PriorityType | Group\n";
    std::cout << "-------------------------------------------\n";
    
    // Count players by priority type
    int earlyBirdCount = 0;
    int wildcardCount = 0;
    int normalCount = 0;
    
    for (int i = 0; i < numPlayers; i++) {
        // Format the PlayerID as PLY followed by a 3-digit number
        char formattedID[10];
        sprintf(formattedID, "PLY%03d", i);
        
        std::cout << formattedID << " | " 
                  << players[i].PlayerName << " | "
                  << players[i].PriorityType << " | ";
        
        if (players[i].GroupID[0] != '\0') {
            std::cout << players[i].GroupID << " (" << players[i].GroupName << ")";
        } else {
            std::cout << "No Group";
        }
        std::cout << "\n";
        
        // Count by priority type
        if (strcmp(players[i].PriorityType, "Early-bird") == 0) {
            earlyBirdCount++;
        } else if (strcmp(players[i].PriorityType, "Wildcard") == 0) {
            wildcardCount++;
        } else if (strcmp(players[i].PriorityType, "Normal") == 0) {
            normalCount++;
        }
    }
    
    // Display priority type statistics
    // std::cout << "\n--- Priority Type Statistics ---\n"; // Removed section header
    std::cout << "Early-bird players: " << earlyBirdCount << "\n";
    std::cout << "Wildcard players: " << wildcardCount << "\n";
    std::cout << "Normal players: " << normalCount << "\n";
    
    // Display group statistics
    // std::cout << "\n--- Group Statistics ---\n"; // Removed section header
    char processedGroups[MAX_GROUPS][10] = {{0}};
    int groupCount = 0;
    
    for (int i = 0; i < numPlayers; i++) {
        if (players[i].GroupID[0] == '\0') continue;
        
        bool groupProcessed = false;
        for (int j = 0; j < groupCount; j++) {
            if (strcmp(processedGroups[j], players[i].GroupID) == 0) {
                groupProcessed = true;
                break;
            }
        }
        
        if (!groupProcessed && groupCount < MAX_GROUPS) {
            strcpy(processedGroups[groupCount], players[i].GroupID);
            
            // Count members in this group
            int membersCount = 0;
            for (int k = 0; k < numPlayers; k++) {
                if (strcmp(players[k].GroupID, players[i].GroupID) == 0) {
                    membersCount++;
                }
            }
            
            std::cout << "Group " << players[i].GroupID << " (" << players[i].GroupName 
                      << "): " << membersCount << " members\n";
            
            groupCount++;
        }
    }
    
    // Clean up allocated memory
    delete[] players;
}

// Angel's Part: Live Stream & Spectator Queue Management
void demonstrateAngelsTask3() {
    std::cout << "\n=== Task 3: Live Stream & Spectator Queue Management (Angel's Part) ===\n";
    // Initialize queue with capacity of 10 spectators
    StreamQueue queue(10);

    // Demonstrate adding VIP spectators (Early-bird type)
    queue.addSpectator("John", "KQCW", "John@gmail.com", "Early-bird", true);
    queue.addSpectator("Jane", "0LVI", "Jane@gmail.com", "Early-bird", true);

    // Demonstrate adding regular spectators
    queue.addSpectator("Jess", "YZ6N", "Jess@gmail.com", "Regular", false);
    queue.addSpectator("Mike", "P2M8", "Mike@gmail.com", "Regular", false);
    queue.addSpectator("Sarah", "R4K9", "Sarah@gmail.com", "Regular", false);

    // Show current state of VIP and regular queues
    std::cout << "\n=== Initial Queue State ===";
    queue.displayQueue();

    // Demonstrate spectator removal with VIP priority
    std::cout << "\n=== Removing Spectators ===";
    queue.removeSpectator(); // Removes John (VIP)
    queue.removeSpectator(); // Removes Jane (VIP)
    queue.removeSpectator(); // Removes Jess (Regular)

    // Show updated queue state
    std::cout << "\n=== Updated Queue State ===";
    queue.displayQueue();

    // Demonstrate historical tracking of all spectators
    std::cout << "\n=== Complete Spectator History ===";
    queue.displaySpectatorHistory();
}

// Main function to run demonstrations
int main() {
    // Demonstrate Angel's Task 3
    demonstrateAngelsTask3();

    // Demonstrate Keith's Task 2
    demonstrateKeithsTask2();

    return 0;
}
