#include <iostream>
// #include <string> // Removed as std::string is not allowed and definitions moved to SpectatorQueue.hpp
#include <ctime>
#include <fstream>
#include <cstring> // Required for C-style string functions
#include <cstdio>  // Required for sprintf
#include <limits> // Required for numeric_limits
#include <algorithm>  // Required for std::min
#include <cctype>  // Required for toupper

#include "KeithTask2.hpp"
#include "CSVOperations.hpp"
#include "Utils.hpp"
#include "PlayerQueue.hpp"
#include "GroupManager.hpp"
// #include "SpectatorQueue.hpp" // Include the new header file (Removed as StreamQueue is removed)

// Helper function to get integer priority for sorting
int getPriorityOrder(const char* priorityType) {
    if (strcmp(priorityType, "Early-bird") == 0) {
        return 1; // Highest priority
    } else if (strcmp(priorityType, "Normal") == 0) {
        return 2;
    } else if (strcmp(priorityType, "Wildcard") == 0) {
        return 3; // Lowest priority
    }
    return 4; // Should not happen with valid data
}

// Angel's Part: Live Stream & Spectator Queue Management (New Implementation)
void processLiveStreamAndOverflow() {
    std::cout << "\n=== Task 3: Live Stream & Spectator Queue Management ===\n";
    
    // Load players from CheckedIn.csv
    int numPlayers = 0;
    PlayerCSV* players = loadPlayersFromCSV("CheckedIn.csv", numPlayers);
    
    if (numPlayers == 0) {
        std::cout << "No players found in CheckedIn.csv file.\n";
        // Clean up allocated memory
        delete[] players; // Ensure players is deleted even if numPlayers is 0
        return;
    }

    // Implement Selection Sort to sort players by priority
    // Priority order: Early-bird (1), Normal (2), Wildcard (3)
    for (int i = 0; i < numPlayers - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < numPlayers; j++) {
            if (getPriorityOrder(players[j].PriorityType) < getPriorityOrder(players[min_idx].PriorityType)) {
                min_idx = j;
            }
        }
        // Swap the found minimum element with the first element
        if (min_idx != i) {
            PlayerCSV temp = players[i];
            players[i] = players[min_idx];
            players[min_idx] = temp;
        }
    }

    // Determine the number of players for the live stream (top 40)
    int liveStreamCount = std::min(numPlayers, 40);

    // Identify players in the Live Stream
    // Removed console output for players in Live Stream as requested
    // std::cout << "\n=== Players in Live Stream (Top " << liveStreamCount << " by Priority) ===\n";
    // for (int i = 0; i < liveStreamCount; i++) {
    //     const PlayerCSV& p = players[i];
    //     std::cout << "- PLY" << p.PlayerID << ": " << p.PlayerName << " (Priority: " << p.PriorityType << ")\n";
    // }

    // Handle overflow: process remaining players interactively and write to separate files
    if (numPlayers > 40) {
        std::cout << "\n=== Processing Overflow Players ===\n";
        for (int i = 40; i < numPlayers; i++) {
            PlayerCSV& p = players[i]; // Use non-const reference to allow modification
            // Keep this output to show which overflow player is being processed
            std::cout << "Processing Overflow Player: " << p.PlayerName << " (ID: PLY" << p.PlayerID << ", Original Priority: " << p.PriorityType << ")\n";

            // --- Interactive Categorization for Overflow ---
            char hasID_input;
            std::cout << "Do you have a Player ID from CheckedIn.csv? (y/n): ";
            std::cin >> hasID_input;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume newline

            // Convert input to uppercase (using toupper from <cctype>)
            hasID_input = toupper(hasID_input);

            std::ofstream outputFile;
            const char* filename = nullptr;

            // --- Prompt for Name and Email (and PlayerID if applicable) ---
            std::cout << "Enter your Name: ";
            std::cin.getline(p.PlayerName, sizeof(p.PlayerName)); // Read Name

            std::cout << "Enter your Email: ";
            std::cin.getline(p.PlayerEmail, sizeof(p.PlayerEmail)); // Read Email

            char influenceTypeBuffer[50] = {0}; // Buffer for influence type, initialized to empty

            if (hasID_input == 'Y') {
                char enteredID[10]; // This input is not used for categorization per requirements
                std::cout << "Enter Player ID (from CheckedIn.csv): ";
                std::cin.getline(enteredID, sizeof(enteredID)); // Read input into a temporary buffer
                std::cout << "Categorizing as VIP based on having Player ID...\n";
                filename = "VIP.csv"; // Categorize as VIP if they have an ID
            } else { 
                 char viewerType_input;
                 std::cout << "Are you an (I)nfluencer, (N)ormal Viewer, (S)kip categorization, or (L)eave? (I/N/S/L): ";
                 std::cin >> viewerType_input;
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume newline

                 // Convert input to uppercase (using toupper from <cctype>)
                 viewerType_input = toupper(viewerType_input);

                 // Determine which file to write to based on new input (original priority ignored in this branch)
                 if (viewerType_input == 'I') {
                      filename = "influencers.csv";
                      // Prompt for Influence Type if categorised as Influencer
                      std::cout << "Enter Influence Type (e.g., youtuber, facebook, instagram, tiktok, others): ";
                      std::cin.getline(influenceTypeBuffer, sizeof(influenceTypeBuffer)); // Read into the shared buffer
                 } else if (viewerType_input == 'N' || viewerType_input == 'S') { // Includes 'N', 'S' - Route to Normal.csv
                      filename = "Normal.csv";
                 } else if (viewerType_input == 'L') {
                      std::cout << "Leaving player " << p.PlayerName << " uncategorized.\n";
                      filename = nullptr; // Indicate skip (leave) by setting filename to null
                 } else {
                      std::cout << "Invalid input for viewer type. Categorizing as Normal.\n";
                      filename = "Normal.csv"; // Default to Normal on invalid input (or handle differently if desired)
                 }
            }

            // --- Write player data to the determined file (if filename is not null) ---
            if (filename) {
                 outputFile.open(filename, std::ios::app);
                 if (outputFile.is_open()) {
                      // Write header if file is empty
                      if (outputFile.tellp() == 0) {
                          if (strcmp(filename, "VIP.csv") == 0) {
                              outputFile << "PlayerID,PlayerName,PlayerEmail,PriorityType,RegistrationTime,GroupID,GroupName\n";
                          } else if (strcmp(filename, "Normal.csv") == 0) { // For Normal.csv
                              outputFile << "Name,Email,Time\n";
                          } else { // For influencers.csv (retain previous format or clarify)
                              outputFile << "Name,Email,Time,InfluenceType\n";
                          }
                      }
                      // Write player data based on filename
                      if (strcmp(filename, "VIP.csv") == 0) {
                          outputFile << p.PlayerID << "," << p.PlayerName << "," << p.PlayerEmail << "," << p.PriorityType << "," << p.RegistrationTime << "," << p.GroupID << "," << p.GroupName << "\n";
                      } else if (strcmp(filename, "Normal.csv") == 0) { // For Normal.csv
                           outputFile << p.PlayerName << "," << p.PlayerEmail << "," << p.RegistrationTime << "\n";
                       } else if (strcmp(filename, "influencers.csv") == 0) { // For influencers.csv
                            outputFile << p.PlayerName << "," << p.PlayerEmail << "," << p.RegistrationTime << "," << influenceTypeBuffer << "\n";
                       }
                      outputFile.close();
                      std::cout << "Player " << p.PlayerName << " written to " << filename << "\n";
                 } else {
                      std::cerr << "Error: Could not open " << filename << " for writing overflow player " << p.PlayerName << ".\n";
                 }
            } else {
                 // (Optional: Add a log or message if the player was skipped due to 'L' or other reasons)
                 // std::cout << "Player " << p.PlayerName << " was not written to any file (skipped or left).\n";
            }

            // --- (Optional) Remove the player from the overflow queue (e.g., pop) if processing is complete ---
            // (If you have a queue structure, remove the player here after processing, regardless of whether they were written or left.)

            // --- (Optional) Prompt to continue processing the next overflow player (if any) ---
            // (You could add a prompt here if you want the user to confirm before moving to the next player, e.g., "Process next overflow player? (y/n): ")

            // --- (Optional) Break the loop if the user chooses to leave (e.g., if a global flag is set) ---
            // (If you want the entire overflow processing to stop if the user leaves, you could set a flag (e.g., bool leaveOverflow = false; at the start) and break the loop here if (leaveOverflow) { ... }.)

            // --- (End of Overflow Processing Loop) ---

            // (Optional: Add a final message after the loop, e.g., "Overflow processing complete.")
        }
    } else {
        std::cout << "\nNo overflow players to process.\n";
    }

    // Clean up allocated memory for players loaded from CSV
    delete[] players;
}

// Keep the demonstration function for Keith's Task 2
void demonstrateKeithsTask2() {
    // std::cout << "\n=== Task 2: Tournament Registration & Player Queueing (Keith's Part) ===\n"; // Removed explicit mention of Keith's Part
    
    // Load players from CheckedIn.csv
    int numPlayers = 0;
    PlayerCSV* players = loadPlayersFromCSV("CheckedIn.csv", numPlayers);
    
    if (numPlayers == 0) {
        std::cout << "No players found in any CheckedIn.csv file.\n";
        // Clean up allocated memory
        delete[] players; // Ensure players is deleted even if numPlayers is 0
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
        sprintf(formattedID, "PLY%03d", players[i].PlayerID);
        
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

// Main function to run demonstrations
int main() {
    // Demonstrate Angel's Task 3 (Live Stream & Spectator Queue Management)
    processLiveStreamAndOverflow();

    // Demonstrate Keith's Task 2 (Tournament Registration & Player Queueing)
    demonstrateKeithsTask2(); // Keeping Keith's demonstration for now

    return 0;
}
