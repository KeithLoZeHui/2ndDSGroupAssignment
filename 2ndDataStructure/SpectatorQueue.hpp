#ifndef SPECTATOR_QUEUE_HPP
#define SPECTATOR_QUEUE_HPP

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <cstring> // Required for C-style string functions

// Define maximum lengths for string fields
#define MAX_NAME_LEN 100
#define MAX_ID_LEN 50
#define MAX_EMAIL_LEN 100
#define MAX_PLAYER_TYPE_LEN 50
#define MAX_TIME_LEN 30 // YYYY-MM-DD HH:MM:SS + null terminator

// Stores spectator information including personal details and status
struct SpectatorInfo {
    char name[MAX_NAME_LEN];
    char id[MAX_ID_LEN];
    char email[MAX_EMAIL_LEN];
    char playerType[MAX_PLAYER_TYPE_LEN];
    char timeJoined[MAX_TIME_LEN];
    bool isVIP;
    bool isActive;

    SpectatorInfo() : isActive(false), isVIP(false) {
        // Initialize char arrays to empty strings
        name[0] = '\0';
        id[0] = '\0';
        email[0] = '\0';
        playerType[0] = '\0';
        timeJoined[0] = '\0';
    }
};

// Node structure for the spectator queue linked list
class Spectator {
public:
    char name[MAX_NAME_LEN];
    bool isVIP;
    int arrayIndex;
    Spectator* next;

    Spectator(const char* n, bool vip, int index)
        : isVIP(vip), arrayIndex(index), next(nullptr) {
        strncpy(name, n, MAX_NAME_LEN - 1);
        name[MAX_NAME_LEN - 1] = '\0'; // Ensure null termination
    }

    // Default constructor to allow array initialization
    Spectator() : isVIP(false), arrayIndex(-1), next(nullptr) {
         name[0] = '\0';
    }
};

// Manages VIP and regular spectator queues using linked lists and array storage
class StreamQueue {
private:
    static const int MAX_SPECTATORS = 100;
    SpectatorInfo spectatorArray[MAX_SPECTATORS];
    int currentArraySize;

    Spectator* vipFront;
    Spectator* regularFront;
    Spectator* vipRear;
    Spectator* regularRear;
    int vipCount;
    int regularCount;
    int maxCapacity;

public:
    // Initialize queue with given capacity
    StreamQueue(int capacity = 100) : currentArraySize(0), vipFront(nullptr), regularFront(nullptr),
                                     vipRear(nullptr), regularRear(nullptr),
                                     vipCount(0), regularCount(0), maxCapacity(capacity) {}

    // Clean up all allocated memory
    ~StreamQueue() {
        while (vipFront != nullptr) {
            Spectator* temp = vipFront;
            vipFront = vipFront->next;
            delete temp;
        }
        while (regularFront != nullptr) {
            Spectator* temp = regularFront;
            regularFront = regularFront->next;
            delete temp;
        }
    }

    // Get current time in YYYY-MM-DD HH:MM:SS format
    char* getCurrentTime();

    // Add a new spectator to the queue with their details
    bool addSpectator(const char* name, const char* id, 
                      const char* email, const char* playerType, bool isVIP);

    // Remove a spectator from the queue (VIPs have priority)
    bool removeSpectator();

    // Display current state of VIP and regular queues
    void displayQueue();

    // Display complete history of all spectators (active and inactive)
    void displaySpectatorHistory();

    // Get number of VIP spectators
    int getVIPCount() const { return vipCount; }
    // Get number of regular spectators
    int getRegularCount() const { return regularCount; }
    // Get total number of spectators
    int getTotalCount() const { return vipCount + regularCount; }
};

#endif // SPECTATOR_QUEUE_HPP 