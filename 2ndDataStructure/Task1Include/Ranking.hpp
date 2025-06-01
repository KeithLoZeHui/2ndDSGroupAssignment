#ifndef RANKING_HPP
#define RANKING_HPP

#include <string>
#include <fstream>
#include <sstream>
#include "Array.hpp"
#include "Bracket.hpp"

// Represents a team's ranking in the tournament
struct TeamRanking {
    std::string name;
    int wins = 0;
    int losses = 0;
    int rank = 0;
};

// Initialize rankings file with default values
inline void initializeRankings(const Array<Team>& teams) {
    std::ofstream file("Rankings.csv");
    file << "Team,Wins,Losses,Rank\n";
    for (int i = 0; i < teams.getSize(); ++i) {
        file << teams[i].name << ",0,0,0\n";
    }
    file.close();
}

// Load rankings from file
inline void loadRankings(Array<TeamRanking>& rankings) {
    std::ifstream file("Rankings.csv");
    if (!file.is_open()) return;

    std::string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string name, wins, losses, rank;

        getline(ss, name, ',');
        getline(ss, wins, ',');
        getline(ss, losses, ',');
        getline(ss, rank, ',');

        TeamRanking tr;
        tr.name = name;
        tr.wins = std::stoi(wins);
        tr.losses = std::stoi(losses);
        tr.rank = std::stoi(rank);

        rankings.add(tr);
    }

    file.close();
}

// Save rankings to file
inline void saveRankings(const Array<TeamRanking>& rankings) {
    std::ofstream file("Rankings.csv");
    file << "Team,Wins,Losses,Rank\n";
    for (int i = 0; i < rankings.getSize(); ++i) {
        file << rankings[i].name << ","
             << rankings[i].wins << ","
             << rankings[i].losses << ","
             << rankings[i].rank << "\n";
    }
    file.close();
}

#endif
