#ifndef KEITH_TASK2_HPP
#define KEITH_TASK2_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <random>
#include <limits>
#include <cstdlib>

// Forward declarations
class PlayerQueue;
class GroupManager;

// Player structure
struct Player {
    char name[50];
    char PlayerID[10]; // Changed to accommodate PLY000 format (6 chars + null)
    char PlayerEmail[100]; // Restored email field
    int priority; // 1 = Early-bird, 2 = Wildcard, 3 = Normal
    char groupID[10]; // Auto-generated group ID
    char groupName[50]; // Group name
};

// Group struct to hold up to 5 players
struct Group {
    char groupID[10];
    Player players[5];
    int playerCount;
    Group() : playerCount(0) { groupID[0] = '\0'; }
};

// Constants for group management
const int MAX_GROUPS = 8;
const int MAX_MEMBERS_PER_GROUP = 5;

// Function declarations for CSV operations - implemented in CSVOperations.hpp
void load_checkedin_csv(char data[][9][120], int& rowCount);
void write_checkedin_csv(char data[][9][120], int rowCount);
void display_checkedin_csv();
int count_checkedin_csv_rows();

// Custom FIFO Queue (First In, First Out) - Class declaration
class PlayerQueue {
private:
    Player* arr;
    int front, rear, capacity;
public:
    // Constructor and destructor
    PlayerQueue(int initial_capacity = 10);
    ~PlayerQueue();
    
    // Member function declarations
    bool isEmpty() { return front > rear; }
    bool isFull() { return rear == capacity - 1; }
    void resize() {
        int new_capacity = capacity * 2;
        Player* new_arr = new Player[new_capacity];
        for (int i = front; i <= rear; ++i) {
            new_arr[i - front] = arr[i];
        }
        rear = rear - front;
        front = 0;
        delete[] arr;
        arr = new_arr;
        capacity = new_capacity;
    }
    bool file_exists(const char* filename);
    bool is_valid_id(const char* s);
    bool is_valid_email(const char* s);
    bool is_duplicate_in_csv(const char* filename, const char* playerID, const char* email);
    
    // Updated function signatures to use 9-column array
    int count_players_in_group(const char* groupID); // This one reads CSV
    bool can_add_to_group(const char* groupID, int numNewPlayers); // This one calls count_players_in_group
    bool is_group_id_unique(const char* groupID); // This one reads CSV

    void enqueue();
    void dequeue();
    void withdraw(const char* name);
    void replace(const char* oldName, const char* newName, int priority);
    void display() {
        if (isEmpty()) {
            std::cout << "Queue is empty!\n";
            return;
        }
        std::cout << "\nCurrent Queue:\n";
        std::cout << "----------------------------\n";
        for (int i = front; i <= rear; ++i) {
            std::cout << i - front + 1 << ". " << arr[i].name << " (ID: " << arr[i].PlayerID << ", Priority: ";
            switch (arr[i].priority) {
                case 1: std::cout << "Early-bird"; break;
                case 2: std::cout << "Wildcard"; break;
                case 3: std::cout << "Normal"; break;
                default: std::cout << "Unknown";
            }
            
            // Display group information if available
            if (arr[i].groupID[0] != '\0') {
                std::cout << ", Group: " << arr[i].groupName << " [" << arr[i].groupID << "]";
            }
            
            std::cout << ")\n";
        }
        std::cout << "----------------------------\n";
    }
};

// Function declarations - Keep this declaration
bool is_valid_email(const char* email);

#endif // KEITH_TASK2_HPP

// Include all implementation files
#include "Utils.hpp"
#include "CSVOperations.hpp"
#include "PlayerQueue.hpp"
#include "GroupManager.hpp"