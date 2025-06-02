#ifndef BRACKET_HPP
#define BRACKET_HPP

#include <string>
#include <iostream>
#include "Array.hpp"

struct Team {
    std::string name;
    int wins = 0;
    int losses = 0;
    int rank = 0;
    int group = 0;
};

struct MatchNode {
    Team* team1 = nullptr;
    Team* team2 = nullptr;
    Team* winner = nullptr;

    MatchNode* left = nullptr;
    MatchNode* right = nullptr;
};

// Create a match with two teams
inline MatchNode* createLeafMatch(Team* t1, Team* t2) {
    MatchNode* match = new MatchNode();
    match->team1 = t1;
    match->team2 = t2;
    return match;
}

// Build a full bracket with 8 teams
inline MatchNode* buildTournamentTree(Team* teams[8]) {
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

#endif
