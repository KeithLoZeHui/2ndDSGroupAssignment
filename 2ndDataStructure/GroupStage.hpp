#ifndef GROUPSTAGE_HPP
#define GROUPSTAGE_HPP

#include <cstdlib>
#include <ctime>
#include "Bracket.hpp"

struct TeamStats
{
    Team* team;
    int points = 0;
};

struct Group{
    std::string name;
    Array<TeamStats> standings;
};

void seedRandom(){
    srand(time(0));
}

void splitIntoGroups(const Array<Team>& teams, Group& groupA, Group& groupB){
    // Shuffle teams
    Array<Team> shuffled = teams;
    for(int i = 0; i < shuffled.getSize(); ++i){
        int j = rand() % shuffled.getSize();
        std::swap(shuffled[i], shuffled[j]);
    }

    groupA.name = "A";
    groupB.name = "B";

    for(int i = 0; i < shuffled.getSize(); ++i){
        TeamStats ts;
        ts.team = new Team();
        ts.team->name = shuffled[i].name;

        if(i < 4)
            groupA.standings.add(ts);
        else
            groupB.standings.add(ts);
    }
}

void playGroupMatches(Group& group){
    int numTeams = group.standings.getSize();
    for(int i = 0; i < numTeams; ++i){
        for(int j = i + 1; j < numTeams; ++j){
            std::cout << "\nGroup" << group.name << " Match: "
                      << group.standings[i].team->name << " vs "
                      << group.standings[j].team->name << std::endl;
            std::cout << "Enter winner (1 or 2): ";
            int winner;
            std::cin >> winner;

            if(winner == 1)
                group.standings[i].points += 3;
            else if(winner == 2)
                group.standings[j].points += 3;
            else
                std::cout << "Invalid. No points awarded.\n";
        }
    }
}

void sortGroupStandings(Group& group){
    for(int i = 0; i < group.standings.getSize() - 1; ++i){
        for(int j = 0; j < group.standings.getSize() - i - 1; ++j){
            if(group.standings[j].points < group.standings[j + 1].points){
                std::swap(group.standings[j], group.standings[j + 1]);
            }
        }
    }
}

void buildKnockoutBracket(Group& groupA, Group& groupB, Team* knockoutTeams[8]){
    for(int i = 0; i < 4; ++i){
        knockoutTeams[i * 2] = groupA.standings[i].team; // A placement
        knockoutTeams[i * 2 + 1] = groupB.standings[i].team; // B placement
    }
}

#endif