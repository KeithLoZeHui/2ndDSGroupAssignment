#include "Task4.hpp" 
#include <fstream>   
#include <sstream>   
#include <limits>    
#include <random>    
#include <chrono>    

// --- Implementations of constructors and destructors for structs defined in Task4.hpp ---

// MatchNode constructor implementation
MatchNode::MatchNode(std::string id, std::string winner, std::string loser, std::string sc, std::string ts)
    : matchID(id), winnerTeam(winner), loserTeam(loser), score(sc), timestamp(ts), next(nullptr) {}

// RosterPlayerNameNode constructor implementation
RosterPlayerNameNode::RosterPlayerNameNode(std::string name)
    : playerName(name), next(nullptr) {}

// TeamRosterNode constructor and destructor implementations
TeamRosterNode::TeamRosterNode(std::string name)
    : teamName(name), playersHead(nullptr), nextTeam(nullptr) {}

TeamRosterNode::~TeamRosterNode() {
    RosterPlayerNameNode* current = playersHead;
    while (current) {
        RosterPlayerNameNode* temp = current;
        current = current->next;
        delete temp;
    }
}

//  DEFINITION FOR TeamRosterNode::addPlayerToRoster 
void TeamRosterNode::addPlayerToRoster(std::string playerName) {
    RosterPlayerNameNode* newNode = new RosterPlayerNameNode(playerName);
    newNode->next = playersHead;
    playersHead = newNode;
}

// PlayerMatchHistoryNode constructor implementation
PlayerMatchHistoryNode::PlayerMatchHistoryNode(std::string id, std::string opp, std::string out, std::string sc, std::string ts, int k, int d, int a)
    : matchID(id), opponentName(opp), outcome(out), score(sc), timestamp(ts), kills(k), deaths(d), assists(a), next(nullptr) {}

// PlayerNode constructor and destructor implementations
PlayerNode::PlayerNode(std::string name)
    : playerName(name), totalWins(0), totalLosses(0), totalKills(0), totalDeaths(0), totalAssists(0), matchHistoryHead(nullptr), nextPlayer(nullptr) {}

PlayerNode::~PlayerNode() {
    PlayerMatchHistoryNode* current = matchHistoryHead;
    while (current) {
        PlayerMatchHistoryNode* temp = current;
        current = current->next;
        delete temp;
    }
}


// --- Function Prototypes  ---
void processCheckedInPlayerLine(const std::string& line, RosterDirectory& roster);
void processMatchResultLine(const std::string& line, RecentMatchStack& recentMatches, PlayerDirectory& playerDirectory, RosterDirectory& roster);
void displayMenu();


// --- RecentMatchStack Class Method Implementations ---
RecentMatchStack::RecentMatchStack() : top(nullptr) {}

RecentMatchStack::~RecentMatchStack() {
    while (top) {
        MatchNode* temp = top;
        top = top->next;
        delete temp;
    }
}

void RecentMatchStack::push(std::string matchID, std::string winner, std::string loser, std::string score, std::string timestamp) {
    MatchNode* newNode = new MatchNode(matchID, winner, loser, score, timestamp);
    newNode->next = top;
    top = newNode;
}

MatchNode* RecentMatchStack::pop() {
    if (top == nullptr) {
        return nullptr;
    }
    MatchNode* temp = top;
    top = top->next;
    return temp;
}

void RecentMatchStack::displayRecent(int count) const {
    if (top == nullptr) {
        std::cout << "No recent matches to display." << std::endl;
        return;
    }
    std::cout << "\n--- Recent Matches (" << count << " newest) ---" << std::endl;
    MatchNode* current = top;
    int displayed = 0;
    while (current != nullptr && displayed < count) {
        std::cout << "  ID: " << current->matchID
                  << ", Winner: " << current->winnerTeam
                  << ", Loser: " << current->loserTeam
                  << ", Score: " << current->score
                  << ", Time: " << current->timestamp << std::endl;
        current = current->next;
        displayed++;
    }
}

// --- RosterDirectory Class Method Implementations ---
RosterDirectory::RosterDirectory() : head(nullptr) {}

RosterDirectory::~RosterDirectory() {
    while (head) {
        TeamRosterNode* temp = head;
        head = head->nextTeam;
        delete temp;
    }
}

TeamRosterNode* RosterDirectory::findTeamRoster(const std::string& teamName) const {
    TeamRosterNode* current = head;
    while (current != nullptr) {
        if (current->teamName == teamName) {
            return current;
        }
        current = current->nextTeam;
    }
    return nullptr;
}

void RosterDirectory::addPlayerToTeam(const std::string& teamName, const std::string& playerName) {
    TeamRosterNode* team = findTeamRoster(teamName);
    if (team == nullptr) {
        team = new TeamRosterNode(teamName);
        team->nextTeam = head; // Add new team to the beginning of the list
        head = team;
    }
    team->addPlayerToRoster(playerName); // This calls the TeamRosterNode's method
}

void RosterDirectory::displayAllRosters() const {
    if (head == nullptr) {
        std::cout << "No teams in roster directory." << std::endl;
        return;
    }
    std::cout << "\n--- Team Rosters ---" << std::endl;
    TeamRosterNode* currentTeam = head;
    while (currentTeam != nullptr) {
        std::cout << "Team: " << currentTeam->teamName << std::endl;
        RosterPlayerNameNode* currentPlayer = currentTeam->playersHead;
        while (currentPlayer != nullptr) {
            std::cout << "  - " << currentPlayer->playerName << std::endl;
            currentPlayer = currentPlayer->next;
        }
        currentTeam = currentTeam->nextTeam;
    }
}

// --- PlayerNode Method Implementations ---
void PlayerNode::addMatchToHistory(std::string matchID, std::string opp, std::string out, std::string sc, std::string ts, int k, int d, int a) {
    PlayerMatchHistoryNode* newNode = new PlayerMatchHistoryNode(matchID, opp, out, sc, ts, k, d, a);
    newNode->next = matchHistoryHead;
    matchHistoryHead = newNode;

    if (out == "Win") {
        totalWins++;
    } else {
        totalLosses++;
    }
    totalKills += k;
    totalDeaths += d;
    totalAssists += a;
}

void PlayerNode::displayHistory() const {
    std::cout << "\n--- " << playerName << "'s Full Match History ---" << std::endl;
    displayOverallStats(); 
    PlayerMatchHistoryNode* current = matchHistoryHead;
    if (current == nullptr) {
        std::cout << "  No matches recorded for " << playerName << "." << std::endl;
        return;
    }
    while (current != nullptr) {
        std::cout << "  Match ID: " << current->matchID
                  << ", Opponent: " << current->opponentName
                  << ", Outcome: " << current->outcome
                  << ", Score: " << current->score
                  << ", K/D/A: " << current->kills << "/" << current->deaths << "/" << current->assists
                  << ", Time: " << current->timestamp << std::endl;
        current = current->next;
    }
}

void PlayerNode::displayOverallStats() const {
    std::cout << "  Player: " << playerName
              << ", Wins: " << totalWins
              << ", Losses: " << totalLosses
              << ", Kills: " << totalKills
              << ", Deaths:  " << totalDeaths
              << ", Assists: " << totalAssists;

    if ((totalWins + totalLosses) > 0) {
        double winRate = static_cast<double>(totalWins) / (totalWins + totalLosses) * 100.0;
        std::cout << ", Win Rate: " << winRate << "%";
    } else {
        std::cout << ", Win Rate: N/A";
    }

    if (totalDeaths > 0) {
        double kdaRatio = static_cast<double>(totalKills + totalAssists) / totalDeaths;
        std::cout << ", KDA Ratio: " << kdaRatio;
    } else if (totalKills > 0 || totalAssists > 0) {
        std::cout << ", KDA Ratio: INF"; // Infinite KDA if no deaths but kills/assists
    } else {
        std::cout << ", KDA Ratio: 0.0";
    }
    std::cout << std::endl;
}

// --- PlayerDirectory Class Method Implementations ---
PlayerDirectory::PlayerDirectory() : head(nullptr) {}

PlayerDirectory::~PlayerDirectory() {
    while (head) {
        PlayerNode* temp = head;
        head = head->nextPlayer;
        delete temp;
    }
}

PlayerNode* PlayerDirectory::findPlayer(const std::string& playerName) const {
    PlayerNode* current = head;
    while (current != nullptr) {
        if (current->playerName == playerName) {
            return current;
        }
        current = current->nextPlayer;
    }
    return nullptr;
}

void PlayerDirectory::addOrUpdatePlayerMatch(const std::string& playerName, const std::string& matchID,
                                            const std::string& opponentName, const std::string& outcome,
                                            const std::string& score, const std::string& timestamp,
                                            int kills, int deaths, int assists) {
    PlayerNode* player = findPlayer(playerName);
    if (player == nullptr) {
        player = new PlayerNode(playerName);
        player->nextPlayer = head; // Add to the beginning of the master list
        head = player;
    }
    player->addMatchToHistory(matchID, opponentName, outcome, score, timestamp, kills, deaths, assists);
}

void PlayerDirectory::displayAllPlayerOverallStats() const {
    if (head == nullptr) {
        std::cout << "No players in the directory." << std::endl;
        return;
    }
    std::cout << "\n--- All Players' Overall Stats ---" << std::endl;
    PlayerNode* current = head;
    while (current != nullptr) {
        current->displayOverallStats();
        current = current->nextPlayer;
    }
}

// --- Tournament Analysis Functions ---
void PlayerDirectory::displayPlayersSortedByWins() const {
    if (head == nullptr) {
        std::cout << "No players to display." << std::endl;
        return;
    }

    std::cout << "\n--- Top Players by Wins ---" << std::endl;
    PlayerNode* topPlayers[3] = {nullptr, nullptr, nullptr}; // Store top 3 players
    int topWins[3] = {-1, -1, -1};

    PlayerNode* current = head;
    while (current != nullptr) {
        if (current->totalWins > topWins[0]) {
            topWins[2] = topWins[1];
            topPlayers[2] = topPlayers[1];
            topWins[1] = topWins[0];
            topPlayers[1] = topPlayers[0];
            topWins[0] = current->totalWins;
            topPlayers[0] = current;
        } else if (current->totalWins > topWins[1]) {
            topWins[2] = topWins[1];
            topPlayers[2] = topPlayers[1];
            topWins[1] = current->totalWins;
            topPlayers[1] = current;
        } else if (current->totalWins > topWins[2]) {
            topWins[2] = current->totalWins;
            topPlayers[2] = current;
        }
        current = current->nextPlayer;
    }

    for (int i = 0; i < 3; ++i) {
        if (topPlayers[i]) {
            std::cout << (i + 1) << ". ";
            topPlayers[i]->displayOverallStats();
        }
    }
    if (!topPlayers[0] && !topPlayers[1] && !topPlayers[2]) {
        std::cout << "No players with wins found." << std::endl;
    }
}

// Win Rate Sort 
void PlayerDirectory::displayPlayersSortedByWinRate() const {
    if (head == nullptr) {
        std::cout << "No players to display." << std::endl;
        return;
    }

    std::cout << "\n--- Top Players by Win Rate ---" << std::endl;
    PlayerNode* topPlayers[3] = {nullptr, nullptr, nullptr};
    double topWinRates[3] = {-1.0, -1.0, -1.0};

    PlayerNode* current = head;
    while (current != nullptr) {
        int totalGames = current->totalWins + current->totalLosses;
        if (totalGames == 0) { // Skip players who haven't played
            current = current->nextPlayer;
            continue;
        }
        double currentWinRate = static_cast<double>(current->totalWins) / totalGames;

        if (currentWinRate > topWinRates[0]) {
            topWinRates[2] = topWinRates[1]; topPlayers[2] = topPlayers[1];
            topWinRates[1] = topWinRates[0]; topPlayers[1] = topPlayers[0];
            topWinRates[0] = currentWinRate; topPlayers[0] = current;
        } else if (currentWinRate > topWinRates[1]) {
            topWinRates[2] = topWinRates[1]; topPlayers[2] = topPlayers[1];
            topWinRates[1] = currentWinRate; topPlayers[1] = current;
        } else if (currentWinRate > topWinRates[2]) {
            topWinRates[2] = currentWinRate; topPlayers[2] = current;
        }
        current = current->nextPlayer;
    }

    for (int i = 0; i < 3; ++i) {
        if (topPlayers[i]) {
            std::cout << (i + 1) << ". ";
            topPlayers[i]->displayOverallStats();
        }
    }
    if (!topPlayers[0] && !topPlayers[1] && !topPlayers[2]) {
        std::cout << "No players with recorded games found." << std::endl;
    }
}

// KDA Sort 
void PlayerDirectory::displayPlayersSortedByKDA() const {
    if (head == nullptr) {
        std::cout << "No players to display." << std::endl;
        return;
    }

    std::cout << "\n--- Top Players by KDA Ratio ---" << std::endl;
    PlayerNode* topPlayers[3] = {nullptr, nullptr, nullptr};
    double topKDARatios[3] = {-1.0, -1.0, -1.0}; // -1.0 to ensure any valid KDA is higher

    PlayerNode* current = head;
    while (current != nullptr) {
        double currentKDA = 0.0;
        if (current->totalDeaths > 0) {
            currentKDA = static_cast<double>(current->totalKills + current->totalAssists) / current->totalDeaths;
        } else if (current->totalKills > 0 || current->totalAssists > 0) {
            currentKDA = std::numeric_limits<double>::infinity(); // Represent infinite KDA
        }

        if (currentKDA > topKDARatios[0]) {
            topKDARatios[2] = topKDARatios[1]; topPlayers[2] = topPlayers[1];
            topKDARatios[1] = topKDARatios[0]; topPlayers[1] = topPlayers[0];
            topKDARatios[0] = currentKDA; topPlayers[0] = current;
        } else if (currentKDA > topKDARatios[1]) {
            topKDARatios[2] = topKDARatios[1]; topPlayers[2] = topPlayers[1];
            topKDARatios[1] = currentKDA; topPlayers[1] = current;
        } else if (currentKDA > topKDARatios[2]) {
            topKDARatios[2] = currentKDA; topPlayers[2] = current;
        }
        current = current->nextPlayer;
    }

    for (int i = 0; i < 3; ++i) {
        if (topPlayers[i]) {
            std::cout << (i + 1) << ". ";
            topPlayers[i]->displayOverallStats();
        }
    }
    if (!topPlayers[0] && !topPlayers[1] && !topPlayers[2]) {
        std::cout << "No players with KDA data found." << std::endl;
    }
}

// Function to display the main menu options (definition)
void displayMenu() {
    std::cout << "\n--- Task 4: Esports Championship Management System Menu ---" << std::endl;
    std::cout << "1. Display Recent Match Results" << std::endl;
    std::cout << "2. Display All Players' Overall Stats" << std::endl;
    std::cout << "3. Display Specific Player's Full History" << std::endl;
    std::cout << "4. Display Top Players by Wins" << std::endl;
    std::cout << "5. Display Top Players by Win Rate" << std::endl;
    std::cout << "6. Display Top Players by KDA Ratio" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}


// Function to parse a single line from checkedin.csv (definition)
void processCheckedInPlayerLine(const std::string& line, RosterDirectory& roster) {
    std::stringstream ss(line);
    std::string segment;
    std::string playerID, playerName, playerEmail, priorityType, regTime, checkInStatus, withdrawn, groupID, groupName; //

    std::getline(ss, playerID, ','); // PLY000
    std::getline(ss, playerName, ','); // player1
    std::getline(ss, playerEmail, ','); // NEW: player1@example.com
    std::getline(ss, priorityType, ','); // Early-bird
    std::getline(ss, regTime, ','); // 2025-05-29 18:15:57
    std::getline(ss, checkInStatus, ','); // Checked-in
    std::getline(ss, withdrawn, ','); // No
    std::getline(ss, groupID, ','); // GRP000
    std::getline(ss, groupName); // Playertesters

    if (playerID == "PlayerID") { // Skip header
        return;
    }

    // Only process active, checked-in players
    if (checkInStatus == "Checked-in" && withdrawn == "No") {
        roster.addPlayerToTeam(groupName, playerName);
    }
}

// Function to parse a single line from matchresult.csv 
void processMatchResultLine(const std::string& line, RecentMatchStack& recentMatches, PlayerDirectory& playerDirectory, RosterDirectory& roster) {
    std::stringstream ss(line);
    std::string segment;
    std::string matchID, stage, group1, group2, winnerTeamName, timestamp; 

    std::getline(ss, matchID, ','); // Match001
    std::getline(ss, stage, ','); // Quarterfinal
    std::getline(ss, group1, ','); // Playertesters
    std::getline(ss, group2, ','); // AlphaTeam
    std::getline(ss, winnerTeamName, ','); // Playertesters
    std::getline(ss, timestamp); // 2025-05-30 15:36:24

    if (matchID == "MatchID" || matchID == "Champion") { 
        return;
    }

    std::string loserTeamName = (winnerTeamName == group1) ? group2 : group1; 
    std::string score = "1-0"; 

    // --- 1. Store recent match results for quick review ---
    recentMatches.push(matchID, winnerTeamName, loserTeamName, score, timestamp);

    // --- 2. Maintain structured history for player performance tracking ---
    // Generate random KDA for players
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> k_dist_win(5, 10);
    std::uniform_int_distribution<> d_dist_win(0, 3);
    std::uniform_int_distribution<> a_dist_win(2, 7);

    std::uniform_int_distribution<> k_dist_loss(1, 5);
    std::uniform_int_distribution<> d_dist_loss(3, 8);
    std::uniform_int_distribution<> a_dist_loss(0, 4);

    // Get players from the winning team and update their history
    TeamRosterNode* winnerRoster = roster.findTeamRoster(winnerTeamName);
    if (winnerRoster) {
        RosterPlayerNameNode* currentPlayer = winnerRoster->playersHead;
        while (currentPlayer != nullptr) {
            int k = k_dist_win(gen);
            int d = d_dist_win(gen);
            int a = a_dist_win(gen);
            playerDirectory.addOrUpdatePlayerMatch(currentPlayer->playerName, matchID, loserTeamName, "Win", score, timestamp, k, d, a);
            currentPlayer = currentPlayer->next;
        }
    } 
        
    

    // Get players from the losing team and update their history
    TeamRosterNode* loserRoster = roster.findTeamRoster(loserTeamName);
    if (loserRoster) {
        RosterPlayerNameNode* currentPlayer = loserRoster->playersHead;
        while (currentPlayer != nullptr) {
            int k = k_dist_loss(gen);
            int d = d_dist_loss(gen);
            int a = a_dist_loss(gen);
            playerDirectory.addOrUpdatePlayerMatch(currentPlayer->playerName, matchID, winnerTeamName, "Loss", score, timestamp, k, d, a);
            currentPlayer = currentPlayer->next;
        }
    } 
}


int main() {
    // --- Initialize Data Structures ---
    RecentMatchStack recentMatches;
    RosterDirectory roster;         
    PlayerDirectory playerDirectory; 

    // --- Step 1: Populate RosterDirectory from CheckedIn.csv ---
    std::cout << "--- Loading Checked-in Players ---" << std::endl;
    std::ifstream playersFile("CheckedIn.csv"); //
    if (playersFile.is_open()) {
        std::string line;
        while (std::getline(playersFile, line)) {
            processCheckedInPlayerLine(line, roster);
        }
        playersFile.close();
        std::cout << "Finished loading checked-in players." << std::endl;
    } else {
        std::cerr << "Error: Could not open 'CheckedIn.csv'. Please ensure the file is in the same directory." << std::endl;
        return 1;
    }

    // --- Step 2: Process Match History from MatchResults.csv ---
    std::cout << "\n--- Processing Match History ---" << std::endl;
    std::ifstream matchesFile("MatchResults.csv"); //
    if (matchesFile.is_open()) {
        std::string line;
        while (std::getline(matchesFile, line)) {
            processMatchResultLine(line, recentMatches, playerDirectory, roster);
        }
        matchesFile.close();
        std::cout << "Finished processing match history." << std::endl;
    } else {
        std::cerr << "Error: Could not open 'MatchResults.csv'. Please ensure the file is in the same directory." << std::endl;
        return 1;
    }

    // --- Menu Loop ---
    int choice;
    do {
        displayMenu(); 
        std::cin >> choice;

        // Input validation
        while (std::cin.fail() || choice < 0 || choice > 6) {
            std::cout << "Invalid input. Please enter a number between 0 and 6: ";
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin >> choice;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        switch (choice) {
            case 1: {
                std::cout << "How many recent matches would you like to display? (e.g., 5): ";
                int num;
                std::cin >> num;
                while (std::cin.fail() || num < 1) {
                    std::cout << "Invalid input. Please enter a positive number: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cin >> num;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                recentMatches.displayRecent(num);
                break;
            }
            case 2:
                playerDirectory.displayAllPlayerOverallStats();
                break;
            case 3: {
                std::cout << "Enter player name to display history: ";
                std::string playerName;
                std::getline(std::cin, playerName);
                PlayerNode* player = playerDirectory.findPlayer(playerName);
                if (player) {
                    player->displayHistory();
                } else {
                    std::cout << "Player '" << playerName << "' not found." << std::endl;
                }
                break;
            }
            case 4:
                playerDirectory.displayPlayersSortedByWins();
                break;
            case 5:
                playerDirectory.displayPlayersSortedByWinRate();
                break;
            case 6:
                playerDirectory.displayPlayersSortedByKDA();
                break;
            case 0:
                std::cout << "Exiting program. Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get(); 
        }
    } while (choice != 0);

    return 0;
}
