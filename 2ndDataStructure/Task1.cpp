#include <fstream>
#include <sstream>
#include <iostream>
#include "Array.hpp"
#include "Bracket.hpp"


int main() {
    int choice;
    Array<Team> teams; // Declare once and persist across loop

    do {
        std::cout << "\n=== Tournament Management Menu ===\n";
        std::cout << "1. View Teams & Bracket\n";
        std::cout << "2. Simulate Tournament & Save Results\n";
        std::cout << "3. Clear Bracket and Restart\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            choice = -1;
        }

        if (choice == 1) {
            loadTeamsFromCSV("CheckedIn.csv", teams);
            std::cout << "\nRegistered Teams:\n";
            for (int i = 0; i < teams.getSize(); i++) {
                std::cout << "- " << teams.get(i).name << "\n";
            }
            printBlankBracket(teams);
        }
        else if (choice == 2) {
            if (teams.getSize() < 2) {
                std::cout << "Not enough teams to run tournament.\n";
            } else {
                matchCounter = 1;
                simulateTournamentAndLog(teams);
            }
        }
        else if (choice == 3) {
            std::ofstream clear("MatchResults.csv", std::ios::trunc);
            clear << "MatchID,Stage,Team1,Team2,Winner,Timestamp\n";
            std::cout << "Match results cleared.\n";
            teams.clear(); // optional: clear loaded teams
        }
        else if (choice == 4) {
            std::cout << "Exiting system. Goodbye!\n";
        }
        else {
            std::cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 4);

    return 0;
}

