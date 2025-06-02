#ifndef TASK4_HPP
#define TASK4_HPP

#include <string>
#include <iostream>
#include <limits>   // For numeric_limits in KDA calculation
#include <random>   // For random number generation
#include <chrono>   // For seeding random number generator

// --- MatchNode for Recent Match Stack ---
struct MatchNode {
    std::string matchID;
    std::string winnerTeam;
    std::string loserTeam;
    std::string score;
    std::string timestamp;
    MatchNode* next;

    MatchNode(std::string id, std::string winner, std::string loser, std::string sc, std::string ts);
};

// --- RecentMatchStack Class ---
class RecentMatchStack {
private:
    MatchNode* top;

public:
    RecentMatchStack();
    ~RecentMatchStack();

    void push(std::string matchID, std::string winner, std::string loser, std::string score, std::string timestamp);
    MatchNode* pop();
    void displayRecent(int count) const;
};

// --- Roster Player name node for a team's roster ---
struct RosterPlayerNameNode {
    std::string playerName;
    RosterPlayerNameNode* next;
    RosterPlayerNameNode(std::string name);
};

// --- Team Roster Node for the Roster Directory ---
struct TeamRosterNode {
    std::string teamName;
    RosterPlayerNameNode* playersHead;
    TeamRosterNode* nextTeam;

    TeamRosterNode(std::string name);
    ~TeamRosterNode(); // Destructor declared here
    void addPlayerToRoster(std::string playerName);
};

// --- Roster Directory (Master Linked List of Teams) ---
class RosterDirectory {
private:
    TeamRosterNode* head;

public:
    RosterDirectory();
    ~RosterDirectory();

    TeamRosterNode* findTeamRoster(const std::string& teamName) const;
    void addPlayerToTeam(const std::string& teamName, const std::string& playerName);
    void displayAllRosters() const;
};


// --- Player-specific Match History Node ---
struct PlayerMatchHistoryNode {
    std::string matchID;
    std::string opponentName;
    std::string outcome;
    std::string score;
    std::string timestamp;
    int kills;
    int deaths;
    int assists;

    PlayerMatchHistoryNode* next;

    PlayerMatchHistoryNode(std::string id, std::string opp, std::string out, std::string sc, std::string ts, int k, int d, int a);
};

// --- PlayerNode for the master Player Directory ---
struct PlayerNode {
    std::string playerName;
    int totalWins;
    int totalLosses;
    int totalKills;
    int totalDeaths;
    int totalAssists;

    PlayerMatchHistoryNode* matchHistoryHead;
    PlayerNode* nextPlayer;

    PlayerNode(std::string name);
    ~PlayerNode(); // Destructor declared here

    void addMatchToHistory(std::string matchID, std::string opp, std::string out, std::string sc, std::string ts, int k, int d, int a);
    void displayHistory() const;
    void displayOverallStats() const;
};

// --- PlayerDirectory Class ---
class PlayerDirectory {
private:
    PlayerNode* head;

public:
    PlayerDirectory();
    ~PlayerDirectory();

    PlayerNode* findPlayer(const std::string& playerName) const;
    void addOrUpdatePlayerMatch(const std::string& playerName, const std::string& matchID,
                                const std::string& opponentName, const std::string& outcome,
                                const std::string& score, const std::string& timestamp,
                                int kills, int deaths, int assists);
    void displayAllPlayerOverallStats() const;

    // Functions for tournament analysis
    void displayPlayersSortedByWins() const;
    void displayPlayersSortedByWinRate() const;
    void displayPlayersSortedByKDA() const;
};

#endif // TASK4_HPP