#ifndef GROUPSTAGE_HPP
#define GROUPSTAGE_HPP

#include <cstdlib>
#include <ctime>
#include <algorithm> // for std::swap
#include "Array.hpp"
#include "Bracket.hpp"

// Represents a team's stats within a group
struct TeamStats {
    Team* team;
    int points = 0;
};

// Represents a group of teams
struct Group {
    std::string name;
    Array<TeamStats> standings;
    bool qualified = false;
};

// Initialize random seed
inline void seedRandom() {
    srand(static_cast<unsigned int>(time(0)));
}

// Shuffle teams and split into two groups
inline void splitIntoGroups(const Array<Team>& teams, Group& groupA, Group& groupB) {
    Array<Team> shuffled = teams;
    for (int i = 0; i < shuffled.getSize(); ++i) {
        int j = rand() % shuffled.getSize();
        std::swap(shuffled[i], shuffled[j]);
    }

    groupA.name = "A";
    groupB.name = "B";

    for (int i = 0; i < shuffled.getSize(); ++i) {
        TeamStats ts;
        ts.team = new Team();
        ts.team->name = shuffled[i].name;
        if (i < 4)
            groupA.standings.add(ts);
        else
            groupB.standings.add(ts);
    }
}

// Sort group standings based on points
inline void sortGroupStandings(Group& group) {
    for (int i = 0; i < group.standings.getSize() - 1; ++i) {
        for (int j = 0; j < group.standings.getSize() - i - 1; ++j) {
            if (group.standings[j].points < group.standings[j + 1].points) {
                std::swap(group.standings[j], group.standings[j + 1]);
            }
        }
    }
}

// Build knockout bracket from top 4 of each group
inline void buildKnockoutBracket(Group& groupA, Group& groupB, Team* knockoutTeams[8]) {
    for (int i = 0; i < 4; ++i) {
        knockoutTeams[i * 2] = groupA.standings[i].team;
        knockoutTeams[i * 2 + 1] = groupB.standings[i].team;
    }
}

#endif
