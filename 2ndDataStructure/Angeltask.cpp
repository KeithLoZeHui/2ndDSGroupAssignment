#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include "KeithTask2.hpp"
#include "CSVOperations.hpp"
#include "Utils.hpp"
#include "PlayerQueue.hpp"
#include "GroupManager.hpp"

// Stores spectator information including personal details and status
struct SpectatorInfo {
    std::string name;
    std::string id;
    std::string email;
    std::string playerType;
    std::string timeJoined;
    bool isVIP;
    bool isActive;

    SpectatorInfo() : isActive(false), isVIP(false) {}
};

// Node structure for the spectator queue linked list
class Spectator {
public:
    std::string name;
    bool isVIP;
    int arrayIndex;
    Spectator* next;

    Spectator(const std::string& n, bool vip, int index)
        : name(n), isVIP(vip), arrayIndex(index), next(nullptr) {}
};

// Manages VIP and regular spectator queues using linked lists and array storage
class StreamQueue {
private:
    static const int MAX_SPECTATORS = 100;
    SpectatorInfo spectatorArray[MAX_SPECTATORS];
    int currentArraySize;

    Spectator* vipFront;
    Spectator* regularFront;
    Spectator* vipRear;
    Spectator* regularRear;
    int vipCount;
    int regularCount;
    int maxCapacity;

public:
    // Initialize queue with given capacity
    StreamQueue(int capacity = 100) : currentArraySize(0), vipFront(nullptr), regularFront(nullptr),
                                     vipRear(nullptr), regularRear(nullptr),
                                     vipCount(0), regularCount(0), maxCapacity(capacity) {}

    // Clean up all allocated memory
    ~StreamQueue() {
        while (vipFront != nullptr) {
            Spectator* temp = vipFront;
            vipFront = vipFront->next;
            delete temp;
        }
        while (regularFront != nullptr) {
            Spectator* temp = regularFront;
            regularFront = regularFront->next;
            delete temp;
        }
    }

    // Get current time in YYYY-MM-DD HH:MM:SS format
    std::string getCurrentTime() {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

    // Add a new spectator to the queue with their details
    bool addSpectator(const std::string& name, const std::string& id, 
                      const std::string& email, const std::string& playerType, bool isVIP) {
        if (vipCount + regularCount >= maxCapacity || currentArraySize >= MAX_SPECTATORS) {
            std::cout << "Queue is full!\n";
            return false;
        }

        // Store in array first
        int arrayIndex = currentArraySize;
        spectatorArray[arrayIndex].name = name;
        spectatorArray[arrayIndex].id = id;
        spectatorArray[arrayIndex].email = email;
        spectatorArray[arrayIndex].playerType = playerType;
        spectatorArray[arrayIndex].isVIP = isVIP;
        spectatorArray[arrayIndex].isActive = true;
        spectatorArray[arrayIndex].timeJoined = getCurrentTime();
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
    bool removeSpectator() {
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
    void displayQueue() {
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
    void displaySpectatorHistory() {
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

    // Get number of VIP spectators
    int getVIPCount() const { return vipCount; }
    // Get number of regular spectators
    int getRegularCount() const { return regularCount; }
    // Get total number of spectators
    int getTotalCount() const { return vipCount + regularCount; }
};

// Helper function to format player ID in PLY000 format
char* formatPlayerID(int index, char* buffer) {
    sprintf(buffer, "PLY%03d", index);
    return buffer;
}

// Function to demonstrate Task 2 functionality
void demonstrateTask2() {
    std::cout << "\n=== Task 2: Tournament Registration & Player Queueing ===\n";
    
    // Load players from CheckedIn.csv
    int numPlayers = 0;
    PlayerCSV* players = loadPlayersFromCSV("CheckedIn.csv", numPlayers);
    
    if (numPlayers == 0) {
        std::cout << "No players found in any CheckedIn.csv file.\n";
        return;
    }
    
    // Display all players from CSV
    std::cout << "\n--- Loaded Players from CSV ---\n";
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
    std::cout << "\n--- Priority Type Statistics ---\n";
    std::cout << "Early-bird players: " << earlyBirdCount << "\n";
    std::cout << "Wildcard players: " << wildcardCount << "\n";
    std::cout << "Normal players: " << normalCount << "\n";
    
    // Display group statistics
    std::cout << "\n--- Group Statistics ---\n";
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

// Test both the StreamQueue implementation and Task 2 functionality
int main() {
    // First demonstrate the original StreamQueue implementation
    std::cout << "\n=== Original StreamQueue Implementation ===\n";
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
    
    // Now demonstrate Task 2 functionality
    demonstrateTask2();

    return 0;
}
