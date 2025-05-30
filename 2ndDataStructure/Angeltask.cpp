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
#include "AngelTask.hpp" // Include the new header file for Angel's Task
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

    // Handle overflow: process remaining players interactively and write to separate files
    if (numPlayers > 40) {
        std::cout << "\n=== Processing Overflow Players ===\n";

        // Open output files once before the loop
        // Use truncate mode for all files to ensure clean start
        std::ofstream vipFile("VIP.csv", std::ios::trunc);
        std::ofstream normalFile("Normal.csv", std::ios::trunc);
        std::ofstream influencersFile("influencers.csv", std::ios::trunc);

        // Write headers for all files
        if (vipFile.is_open()) {
            vipFile << "PlayerID,PlayerName,PlayerEmail,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
        }
        if (normalFile.is_open() && normalFile.tellp() == 0) {
            normalFile << "Name,Email,Time\n";
        }
        if (influencersFile.is_open() && influencersFile.tellp() == 0) {
            influencersFile << "Name,Email,Time,InfluenceType\n";
        }

        // Check if all necessary files opened successfully
        if (!vipFile.is_open() || !normalFile.is_open() || !influencersFile.is_open()) {
            std::cerr << "Error: Could not open one or more overflow files for writing.\n";
            // Close any files that were opened before returning
            if (vipFile.is_open()) vipFile.close();
            if (normalFile.is_open()) normalFile.close();
            if (influencersFile.is_open()) influencersFile.close();
            // Clean up allocated memory for players loaded from CSV
            delete[] players;
            return; // Exit function if files can't be opened
        }

        for (int i = 40; i < numPlayers; i++) {
            PlayerCSV& p = players[i]; // Use non-const reference to allow modification
            // Keep this output to show which overflow player is being processed
            std::cout << "Processing Overflow Player: " << p.PlayerName << " (ID: " << p.PlayerID << ", Original Priority: " << p.PriorityType << ")\n";

            // Declare matchedPlayerIndex within the loop scope for each player
            int matchedPlayerIndex = -1;

            const char* filename = nullptr; // Declare filename here within the loop

            // --- Ask about Player ID first ---
            char hasID_input;
            std::cout << "Do you have a Player ID from CheckedIn.csv? (y/n): ";
            std::cin >> hasID_input;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume newline

            // Convert input to uppercase (using toupper from <cctype>)
            hasID_input = toupper(hasID_input);

            char influenceTypeBuffer[50] = {0}; // Buffer for influence type, initialized to empty

            if (hasID_input == 'Y') {
                // --- If user has ID, ONLY ask for Player ID and validate ---
                char enteredID[10]; // Buffer for entered Player ID
                bool idFound = false;

                // Loop for ID re-entry or leaving
                while (true) {
                    std::cout << "Enter the last 3 digits of your Player ID (e.g., 050 for PLY050): ";
                    std::cin.getline(enteredID, sizeof(enteredID)); // Read input into the buffer

                    // Convert enteredID to uppercase for case-insensitive comparison
                    for (int k = 0; enteredID[k]; k++) {
                        enteredID[k] = toupper(enteredID[k]);
                    }

                    // --- Parse entered ID as a 3-digit number ---
                    int enteredID_numeric = atoi(enteredID); // Convert the entered 3 digits to int

                    // Search for the entered ID in the loaded players array
                    idFound = false;
                    matchedPlayerIndex = -1; // Reset matchedPlayerIndex for this search attempt within the loop
                    if (enteredID_numeric >= 0) { // Only search if parsing was successful and ID is valid
                        // Format the full player ID for comparison (PLY followed by 3 digits)
                        char fullPlayerID[10];
                        sprintf(fullPlayerID, "PLY%03d", enteredID_numeric);
                        
                        for (int j = 0; j < numPlayers; j++) {
                            // Compare the full player IDs
                            if (strcmp(players[j].PlayerID, fullPlayerID) == 0) {
                                idFound = true;
                                matchedPlayerIndex = j; // Store the index of the matched player
                                break; // Exit inner search loop
                            }
                        }
                    }

                    if (idFound) {
                        std::cout << "Player ID found in CheckedIn.csv. Categorizing as VIP...\n";
                        filename = "VIP.csv"; // Categorize as VIP if they have an ID and it matches
                        break; // Exit re-entry loop
                    } else {
                        char reenterOrLeave;
                        std::cout << "Player ID not found in CheckedIn.csv. Re-enter (R) or Leave (L)? (R/L): ";
                        std::cin >> reenterOrLeave;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume newline
                        reenterOrLeave = toupper(reenterOrLeave);

                        if (reenterOrLeave == 'L') {
                            std::cout << "Leaving player " << p.PlayerName << " uncategorized.\n";
                            filename = nullptr; // Indicate skip (leave)
                            break; // Exit re-entry loop
                        } else if (reenterOrLeave == 'R') {
                            std::cout << "Please re-enter the Player ID.\n";
                            // Continue the while loop for re-entry
                        } else {
                            std::cout << "Invalid input. Please re-enter the Player ID or Leave.\n";
                            // Continue the while loop for re-entry
                        }
                    }
                }

            } else { 
                 // --- If user does NOT have ID, ask for Name/Email and viewer type prompt for Name and Email ---
                 std::cout << "Enter your Name: ";
                 std::cin.getline(p.PlayerName, sizeof(p.PlayerName)); // Read Name

                 std::cout << "Enter your Email: ";
                 std::cin.getline(p.PlayerEmail, sizeof(p.PlayerEmail)); // Read Email

                 char viewerType_input;
                 std::cout << "Are you an (I)nfluencer, (N)ormal Viewer, (S)kip categorization, or (L)eave? (I/N/S/L): ";
                 std::cin >> viewerType_input;
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume newline

                 // Convert input to uppercase (using toupper from <cctype>)
                 viewerType_input = toupper(viewerType_input);

                 // Determine which file to write to based on new input
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
                if (strcmp(filename, "VIP.csv") == 0) {
                    // Use data from the matched player (players[matchedPlayerIndex])
                    // Add a check to ensure matchedPlayerIndex is valid
                    if (matchedPlayerIndex != -1) {
                        const PlayerCSV& matchedP = players[matchedPlayerIndex];
                        vipFile << matchedP.PlayerID << ","
                                 << matchedP.PlayerName << ","
                                 << matchedP.PlayerEmail << ","
                                 << matchedP.PriorityType << ","
                                 << matchedP.RegistrationTime << ","
                                 << matchedP.CheckInStatus << ","
                                 << matchedP.Withdrawn << ","
                                 << matchedP.GroupID << ","
                                 << matchedP.GroupName << "\n";
                        vipFile.flush(); // Ensure data is written to disk immediately
                    } else {
                        // This case should ideally not be reached if filename is VIP.csv
                        // but matchedPlayerIndex is -1. It indicates a logic error.
                        std::cerr << "Error: Logic error in VIP writing for player " << p.PlayerName << ". matchedPlayerIndex is -1.\n";
                    }
                } else if (strcmp(filename, "Normal.csv") == 0) { // For Normal.csv
                     normalFile << p.PlayerName << "," << p.PlayerEmail << "," << p.RegistrationTime << "\n";
                 } else if (strcmp(filename, "influencers.csv") == 0) { // For influencers.csv
                      influencersFile << p.PlayerName << "," << p.PlayerEmail << "," << p.RegistrationTime << "," << influenceTypeBuffer << "\n";
                 }
                std::cout << "Player " << p.PlayerName << " written to " << filename << "\n";
            }
        } // End of for loop processing overflow players

        // Close files after processing all overflow players
        if (vipFile.is_open()) vipFile.close();
        if (normalFile.is_open()) normalFile.close();
        if (influencersFile.is_open()) influencersFile.close();

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
        // Display the PlayerID directly as it's already in the correct format
        std::cout << players[i].PlayerID << " | " 
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
