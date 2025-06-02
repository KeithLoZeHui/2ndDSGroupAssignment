#ifndef MATCHLOGGER_HPP
#define MATCHLOGGER_HPP

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

// Externally declared in main file
extern int matchCounter;

// Timestamp function prototype
std::string currentTimestamp();

// Logs a match result to MatchResults.csv
inline void logMatchResult(const std::string& stage, const std::string& team1,
                           const std::string& team2, const std::string& winner) {
    std::ofstream log("MatchResults.csv", std::ios::app);
    if (!log.is_open()) {
        std::cout << "Error: Could not open MatchResults.csv to log match.\n";
        return;
    }
    log << "Match" << std::setw(3) << std::setfill('0') << matchCounter++
        << "," << stage
        << "," << team1
        << "," << team2
        << "," << winner
        << "," << currentTimestamp() << "\n";
    log.close();
}

#endif
