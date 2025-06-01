#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "Task1Include/Array.hpp"
#include "Task1Include/Bracket.hpp"
#include "Task1Include/GroupStage.hpp"
#include "Task1Include/Ranking.hpp"
#include "Task1Include/MatchLogger.hpp"

using namespace std;

// Forward Declarations
void loadTeamsFromCSV(const std::string& filename, Array<Team>& teams);
std::string currentTimestamp();
std::string getStageLabel(int depth);
void playMatchAndLog(MatchNode* match, std::ofstream& log, int depth);
void simulateTournament(MatchNode* node, std::ofstream& log, int depth);
void runQualifiers(Array<Group>& groups);
int loadLastMatchID();
void playGroupMatches(Group& group);

// Global match counter (loaded from MatchResults.csv)
int matchCounter = 1;

// Main Function
int main() {
    seedRandom();

    // Load teams
    Array<Team> teams;
    loadTeamsFromCSV("CheckedIn.csv", teams);

    // Initialize rankings if not present
    std::ifstream rankFile("Rankings.csv");
    if (!rankFile.good()) {
        initializeRankings(teams);
    }
    rankFile.close();

    // Create groups for qualifiers
    Array<Group> groups;
    std::cout << "Creating groups for qualifiers...\n";
    for (int i = 0; i < teams.getSize(); ++i) {
        Group g;
        g.name = teams[i].name;
        groups.add(g);
    }

    // Load current match ID counter
    matchCounter = loadLastMatchID() + 1;

    int choice = -1;
    while (choice != 7) {
        std::cout << "\n=== Tournament Management Menu ===\n";
        std::cout << "1. View Rankings\n";
        std::cout << "2. Play Qualifiers\n";
        std::cout << "3. Play Group Matches\n";
        std::cout << "4. Play Knockout Matches\n";
        std::cout << "5. View Match History\n";
        std::cout << "6. Reset Rankings and Match History\n";
        std::cout << "7. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: {
                Array<TeamRanking> rankings;
                loadRankings(rankings);
                std::cout << "\n--- Rankings ---\n";
                for (int i = 0; i < rankings.getSize(); ++i) {
                    std::cout << i + 1 << ". " << rankings[i].name
                              << " | Wins: " << rankings[i].wins
                              << " | Losses: " << rankings[i].losses
                              << " | Rank: " << rankings[i].rank << "\n";
                }
                break;
            }
            case 2: {
                runQualifiers(groups);
                std::cout << "Qualifiers complete. Groups updated.\n";
                break;
            }
            case 3: {
                Array<Team> qualifiedTeams;
                for (int i = 0; i < groups.getSize(); ++i) {
                    Team t;
                    t.name = groups[i].name;
                    qualifiedTeams.add(t);
                }

                Group groupA, groupB;
                splitIntoGroups(qualifiedTeams, groupA, groupB);

                playGroupMatches(groupA);
                playGroupMatches(groupB);

                // Load & update rankings
                Array<TeamRanking> rankings;
                loadRankings(rankings);

                auto updateRankings = [&](const Group& group) {
                    for (int i = 0; i < group.standings.getSize(); ++i) {
                        std::string teamName = group.standings[i].team->name;
                        for (int j = 0; j < rankings.getSize(); ++j) {
                            if (rankings[j].name == teamName) {
                                rankings[j].wins += group.standings[i].points / 3;
                                rankings[j].losses += 3 - (group.standings[i].points / 3);
                                break;
                            }
                        }
                    }
                };

                updateRankings(groupA);
                updateRankings(groupB);

                // Sort and update ranks
                for (int i = 0; i < rankings.getSize() - 1; ++i) {
                    for (int j = 0; j < rankings.getSize() - i - 1; ++j) {
                        if (rankings[j].wins < rankings[j + 1].wins) {
                            std::swap(rankings[j], rankings[j + 1]);
                        }
                    }
                }
                for (int i = 0; i < rankings.getSize(); ++i) {
                    rankings[i].rank = i + 1;
                }

                saveRankings(rankings);
                std::cout << "Group stage complete. Rankings updated.\n";
                break;
            }
            case 4: {
                Array<TeamRanking> rankings;
                loadRankings(rankings);

                Team* knockoutTeams[8];
                for (int i = 0; i < 8; ++i) {
                    knockoutTeams[i] = new Team();
                    knockoutTeams[i]->name = rankings[i].name;
                }

                MatchNode* root = buildTournamentTree(knockoutTeams);

                std::ofstream log("MatchResults.csv", std::ios::app);
                simulateTournament(root, log, 0);
                log.close();

                std::cout << "\nBracket phase completed.\n";
                break;
            }
            case 5: {
                std::ifstream file("MatchResults.csv");
                if (file.is_open()) {
                    std::string line;
                    getline(file, line); // Skip header
                    std::cout << "\n--- Match History ---\n";
                    while (getline(file, line))
                        std::cout << line << "\n";
                    file.close();
                } else {
                    std::cout << "No match history found.\n";
                }
                break;
            }
            case 6: {
                // Reset rankings
                Array<TeamRanking> rankings;
                loadRankings(rankings);
                for (int i = 0; i < rankings.getSize(); ++i) {
                    rankings[i].wins = 0;
                    rankings[i].losses = 0;
                    rankings[i].rank = 0;
                }
                saveRankings(rankings);
                std::cout << "Rankings cleared (reset to 0).\n";

                // Reset MatchResults.csv
                std::ofstream matchResultsFile("MatchResults.csv");
                if (matchResultsFile.is_open()) {
                    matchResultsFile << "MatchID,Stage,Team1,Team2,Winner,Timestamp\n";
                    matchResultsFile.close();
                    std::cout << "MatchResults.csv cleared (header written).\n";
                } else {
                    std::cout << "Error: Could not open MatchResults.csv to clear.\n";
                }
                break;
            }
            case 7:
                std::cout << "Exiting system. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}

// Load teams from CSV
void loadTeamsFromCSV(const std::string& filename, Array<Team>& teams) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << "\n";
        return;
    }

    std::string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string temp, teamName;

        for (int i = 0; i < 8; ++i)
            getline(ss, temp, ',');
        getline(ss, teamName, ',');

        bool exists = false;
        for (int i = 0; i < teams.getSize(); ++i) {
            if (teams[i].name == teamName) {
                exists = true;
                break;
            }
        }
        if (!exists && !teamName.empty()) {
            Team t;
            t.name = teamName;
            teams.add(t);
        }
    }
    file.close();
}

// Timestamp for match logs
std::string currentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ltm);
    return std::string(buf);
}

// Load last used match ID
int loadLastMatchID() {
    std::ifstream file("MatchResults.csv");
    if (!file.is_open()) return 0;

    std::string line;
    int lastMatchNum = 0;
    while (getline(file, line)) {
        if (line.find("Match") == 0) {
            std::string matchID = line.substr(5, 3);
            try {
                int matchNum = std::stoi(matchID);
                if (matchNum > lastMatchNum)
                    lastMatchNum = matchNum;
            } catch (...) {}
        }
    }
    file.close();
    return lastMatchNum;
}

// Play a group of matches
void playGroupMatches(Group& group) {
    int numTeams = group.standings.getSize();
    for (int i = 0; i < numTeams; ++i) {
        for (int j = i + 1; j < numTeams; ++j) {
            std::cout << "\nGroup " << group.name << " Match: "
                      << group.standings[i].team->name << " vs "
                      << group.standings[j].team->name << "\n";
            std::cout << "Enter winner (1 or 2): ";
            int winner;
            std::cin >> winner;

            if (winner == 1) {
                group.standings[i].points += 3;
                std::cout << group.standings[i].team->name << " wins!\n";
                logMatchResult("Group", group.standings[i].team->name,
                               group.standings[j].team->name,
                               group.standings[i].team->name);
            } else if (winner == 2) {
                group.standings[j].points += 3;
                std::cout << group.standings[j].team->name << " wins!\n";
                logMatchResult("Group", group.standings[i].team->name,
                               group.standings[j].team->name,
                               group.standings[j].team->name);
            } else {
                std::cout << "Invalid input. No points awarded.\n";
            }
        }
    }
}

// Play a match and log it
void playMatchAndLog(MatchNode* match, std::ofstream& log, int depth) {
    if (!match || !match->team1 || !match->team2) return;

    std::cout << "\nMatch: " << match->team1->name << " vs " << match->team2->name << "\n";
    std::cout << "Enter winner (1 or 2): ";
    int winner;
    std::cin >> winner;

    if (winner == 1)
        match->winner = match->team1;
    else if (winner == 2)
        match->winner = match->team2;
    else {
        std::cout << "Invalid input. Try again.\n";
        playMatchAndLog(match, log, depth);
        return;
    }

    logMatchResult("Knockout", match->team1->name,
                    match->team2->name, match->winner->name);
}

// Simulate a knockout tournament
void simulateTournament(MatchNode* node, std::ofstream& log, int depth) {
    if (!node) return;

    if (node->team1 && node->team2 && !node->left && !node->right) {
        playMatchAndLog(node, log, 2); // Quarterfinal
        std::cout << node->winner->name << " moves to the Semifinal!\n";
        return;
    }

    simulateTournament(node->left, log, depth + 1);
    simulateTournament(node->right, log, depth + 1);

    if (node->left && node->left->winner && node->right && node->right->winner) {
        node->team1 = node->left->winner;
        node->team2 = node->right->winner;
        playMatchAndLog(node, log, depth); // Logs the winner

        if (depth == 1) {
            std::cout << node->winner->name << " moves to the Final!\n";
        } else if (depth == 0) {
            std::cout << node->winner->name << " is the Champion!\n";
        }
    }
}

// Play qualifiers
void runQualifiers(Array<Group>& groups) {
    int numGroups = groups.getSize();
    Array<Group> qualifiedGroups;

    std::cout << "\n=== Qualifier Round ===\n";

    if (numGroups % 2 != 0) {
        Group& byeGroup = groups.get(numGroups - 1);
        std::cout << byeGroup.name << " gets a bye and qualifies automatically!\n";
        byeGroup.qualified = true;
        qualifiedGroups.add(byeGroup);
        groups.removeLast();
        numGroups--;
    }

    for (int i = 0; i < numGroups; i += 2) {
        Group& g1 = groups.get(i);
        Group& g2 = groups.get(i + 1);

        std::cout << "Qualifier Match: " << g1.name << " vs " << g2.name << "\n";
        std::cout << "Enter winner (1 for " << g1.name << ", 2 for " << g2.name << "): ";
        int winner;
        std::cin >> winner;

        if (winner == 1) {
            g1.qualified = true;
            g2.qualified = false;
            qualifiedGroups.add(g1);
            std::cout << g1.name << " qualifies.\n";
            logMatchResult("Qualifiers", g1.name, g2.name, g1.name);
        } else {
            g2.qualified = true;
            g1.qualified = false;
            qualifiedGroups.add(g2);
            std::cout << g2.name << " qualifies.\n";
            logMatchResult("Qualifiers", g1.name, g2.name, g2.name);
        }
    }

    if (qualifiedGroups.getSize() < 8) {
        std::cout << "Not enough qualified teams. Adding one more randomly to fill 8 slots.\n";
        for (int i = 0; i < groups.getSize(); ++i) {
            if (!groups[i].qualified) {
                groups[i].qualified = true;
                qualifiedGroups.add(groups[i]);
                std::cout << groups[i].name << " added as a wildcard qualifier.\n";
                break;
            }
        }
    }

    groups = qualifiedGroups;

    std::cout << "\n=== Qualified Groups ===\n";
    for (int i = 0; i < groups.getSize(); i++) {
        std::cout << groups[i].name << " (Qualified)\n";
    }
}
