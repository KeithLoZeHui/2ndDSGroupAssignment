#ifndef BRACKET_HPP
#define BRACKET_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "Array.hpp"

struct Team {
    std::string name;
};

struct MatchNode {
    Team* team1 = nullptr;
    Team* team2 = nullptr;
    Team* winner = nullptr;

    MatchNode* left = nullptr;
    MatchNode* right = nullptr;
};

MatchNode* createLeafMatch(Team* t1, Team* t2) {
    MatchNode* match = new MatchNode();
    match->team1 = t1;
    match->team2 = t2;
    return match;
}

MatchNode* buildTournamentTree(Team* teams[8]) {
    MatchNode* qf1 = createLeafMatch(teams[0], teams[1]);
    MatchNode* qf2 = createLeafMatch(teams[2], teams[3]);
    MatchNode* qf3 = createLeafMatch(teams[4], teams[5]);
    MatchNode* qf4 = createLeafMatch(teams[6], teams[7]);

    MatchNode* sf1 = new MatchNode();
    sf1->left = qf1;
    sf1->right = qf2;

    MatchNode* sf2 = new MatchNode();
    sf2->left = qf3;
    sf2->right = qf4;

    MatchNode* final = new MatchNode();
    final->left = sf1;
    final->right = sf2;

    return final;
}

std::string currentTimeStamp(){
    time_t now = time(0);
    tm* ltm = localtime(&now);

    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ltm);
    return std::string(buf);
}

std::string getStageLabel(int depth){
    if(depth == 0) return "Final";
    else if(depth == 1) return "Semifinal";
    else if(depth == 2) return "Quarterfinal";
    else return "Unknown";
}

void printBracket(MatchNode* node, int depth = 0) {
    if (node == nullptr) return;

    std::string indent(depth * 2, ' ');
    std::string t1 = node->team1 ? node->team1->name : "???";
    std::string t2 = node->team2 ? node->team2->name : "???";

    std::cout << indent << "[" << t1 << " vs " << t2 << "]" << std::endl;

    printBracket(node->left, depth + 1);
    printBracket(node->right, depth + 1);
}



bool teamExists(const Array<Team>& teams, const std::string& name) {
    for (int i = 0; i < teams.getSize(); i++) {
        if (teams.get(i).name == name) return true;
    }
    return false;
}

void loadTeamsFromCSV(const std::string& filename, Array<Team>& teams) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string temp, teamName;
        for (int i = 0; i < 8; ++i) getline(ss, temp, ','); // Skip to GroupName
        getline(ss, teamName, ',');

        if (!teamExists(teams, teamName) && teams.getSize() < 8) {
            Team t;
            t.name = teamName;
            teams.add(t);
        }
    }

    file.close();
}

std::ofstream resultLog("MatchResults.csv", std::ios::app); // global or pass it

int matchCounter = 1;

void playMatchAndLog(MatchNode* match, std::ofstream& log, int depth) {
    if (!match || !match->team1 || !match->team2) return;

    std::cout << "\nMatch: " << match->team1->name << " vs " << match->team2->name << std::endl;
    std::cout << "Enter winner (1 or 2): ";
    int winner;
    std::cin >> winner;

    if (winner == 1) {
        match->winner = match->team1;
    } else if (winner == 2) {
        match->winner = match->team2;
    } else {
        std::cout << "Invalid input. Try again.\n";
        playMatchAndLog(match, log, depth);
        return;
    }

    log << "Match" << std::setw(3) << std::setfill('0') << matchCounter++ << ","
        << getStageLabel(depth) << ","
        << match->team1->name << ","
        << match->team2->name << ","
        << match->winner->name << ","
        << currentTimeStamp() << "\n";
}

void simulateTournament(MatchNode* node, std::ofstream& log, int depth = 0) {
    if (!node) return;

    if (node->team1 && node->team2 && !node->left && !node->right) {
        playMatchAndLog(node, log, depth);
        return;
    }

    simulateTournament(node->left, log, depth + 1);
    simulateTournament(node->right, log, depth + 1);

    if (node->left && node->right && node->left->winner && node->right->winner) {
        node->team1 = node->left->winner;
        node->team2 = node->right->winner;
        playMatchAndLog(node, log, depth);
    }
}

void simulateTournamentAndLog(Array<Team>& teams) {
    Team* teamPtrs[8];
    for (int i = 0; i < teams.getSize(); ++i) {
        teamPtrs[i] = new Team();
        teamPtrs[i]->name = teams.get(i).name;
    }

    MatchNode* root = buildTournamentTree(teamPtrs);

    std::ofstream log("MatchResults.csv");
    log << "MatchID,Stage,Group1,Group2,Winner,Timestamp\n";

    std::cout << "\n--- Tournament Simulation ---\n";
    simulateTournament(root, log);

    std::cout << "\nFinal Winner: " << root->winner->name << "\n";
    log << "Champion,,," << root->winner->name << "\n";
    log.close();
}

void printBlankBracket(const Array<Team>& teams) {
    std::cout << "\n--- Bracket Preview ---\n";
    for (int i = 0; i < 8; i += 2) {
        std::string t1 = (i < teams.getSize()) ? teams.get(i).name : "???";
        std::string t2 = (i + 1 < teams.getSize()) ? teams.get(i + 1).name : "???";
        std::cout << "Match " << (i / 2 + 1) << ": " << t1 << " vs " << t2 << "\n";
    }
}


#endif
