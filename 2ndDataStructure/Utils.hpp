#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <random>

// Implementation of split string by delimiter
int split(const char* s, char delimiter, char tokens[][100], int max_tokens) {
    int count = 0;
    int len = strlen(s);
    int start = 0;
    
    for (int i = 0; i <= len; i++) {
        if (s[i] == delimiter || s[i] == '\0') {
            if (count < max_tokens) {
                int tokenLen = i - start;
                if (tokenLen > 99) tokenLen = 99; // Ensure we don't overflow
                strncpy(tokens[count], s + start, tokenLen);
                tokens[count][tokenLen] = '\0';
                count++;
            }
            start = i + 1;
        }
    }
    return count;
}

// Implementation of Generate random ID
void generateRandomID(char* output, int length) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, strlen(alphanum) - 1);
    
    // Generate 'length' characters
    for (int i = 0; i < length; ++i)
        output[i] = alphanum[dis(gen)];
    output[length] = '\0';
}

// Implementation of is_valid_email
bool is_valid_email(const char* email) {
    int len = strlen(email);
    bool hasAt = false;
    bool hasDot = false;
    int atPos = -1;
    
    // Basic validation: check for @ and . after @
    for (int i = 0; i < len; i++) {
        if (email[i] == '@') {
            hasAt = true;
            atPos = i;
        } else if (hasAt && email[i] == '.') {
            hasDot = true;
        }
    }
    
    // Must have @ and . after @, and can't start with @ or end with .
    return hasAt && hasDot && atPos > 0 && atPos < len - 1 && email[len - 1] != '.';
}

// Add this function at the top of the file, after the includes
bool is_id_unique(const char* id, Player* arr, int front, int rear) {
    for (int i = front; i <= rear; i++) {
        if (strcmp(arr[i].PlayerID, id) == 0) {
            return false;
        }
    }
    return true;
}

// Function to check if a group is full
bool is_group_full(const char* groupID, char data[][8][120], int rowCount) {
    int count = 0;
    for (int i = 0; i < rowCount; i++) {
        // Skip withdrawn players
        if (strcmp(data[i][5], "Yes") == 0) continue;
        
        // Count players in this group
        if (strcmp(data[i][6], groupID) == 0) {
            count++;
            if (count >= MAX_MEMBERS_PER_GROUP) {
                return true;
            }
        }
    }
    return false;
}

// Function to generate sequential group IDs (e.g., GRP000, GRP001)
void generateSequentialGroupID(char* output) {
    int highestID = -1;
    std::ifstream file("CheckedIn.csv");
    char line[500];

    if (file.is_open()) {
        file.getline(line, 500); // Skip header
        while (file.getline(line, 500)) {
            char cols[8][100];
            int num_cols = split(line, ',', cols, 8);

            if (num_cols >= 7 && strncmp(cols[6], "GRP", 3) == 0) {
                int idNum = atoi(cols[6] + 3);
                if (idNum > highestID) {
                    highestID = idNum;
                }
            }
        }
        file.close();
    }

    int nextID = highestID + 1;
    sprintf(output, "GRP%03d", nextID);
}

// Function to generate sequential player IDs (e.g., PLY000, PLY001)
void generateSequentialPlayerID(char* output) {
    int highestID = -1;
    std::ifstream file("CheckedIn.csv");
    char line[500];

    if (file.is_open()) {
        file.getline(line, 500); // Skip header
        while (file.getline(line, 500)) {
            char cols[8][100];
            int num_cols = split(line, ',', cols, 8);

            // Assuming PlayerID is in column 0
            if (num_cols >= 1 && strncmp(cols[0], "PLY", 3) == 0) {
                int idNum = atoi(cols[0] + 3);
                if (idNum > highestID) {
                    highestID = idNum;
                }
            }
        }
        file.close();
    }

    int nextID = highestID + 1;
    sprintf(output, "PLY%03d", nextID);
}

#endif // UTILS_HPP
