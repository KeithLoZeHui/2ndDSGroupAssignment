#ifndef KEITH_TASK2_HPP
#define KEITH_TASK2_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// Player structure
struct Player {
    char name[50];
    char PlayerID[5]; // 4 chars + null terminator
    char PlayerEmail[100];
    int priority; // 1 = Early-bird, 2 = Wildcard, 3 = Normal
    char groupID[10]; // New: group ID
};

// Add a struct for minimal CSV row for Task 2
struct PlayerCSV {
    int PlayerID;
    char PlayerName[50];
    char PriorityType[20];
    char RegistrationTime[25];
    char CheckInStatus[20];
    char Withdrawn[5];
};
//testing
// Group struct to hold up to 5 players
struct Group {
    char groupID[10];
    Player players[5];
    int playerCount;
    Group() : playerCount(0) { groupID[0] = '\0'; }
};

// Function declarations - only declare what they do, not how they do it
int split(const std::string& s, char delimiter, std::string tokens[], int max_tokens);
PlayerCSV* loadPlayersFromCSV(const char* filename, int& num_players);
void savePlayersToCSV(const char* filename, PlayerCSV* players, int num_players);
std::string generateRandomID(int length = 4);
void display_checkedin_csv();
int count_checkedin_csv_rows();
void load_checkedin_csv(char arr[][6][120], int& n);
void write_checkedin_csv(char arr[][6][120], int n);
void withdraw_from_csv();
void replace_in_csv();

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
    bool isEmpty();
    bool isFull();
    void resize();
    bool file_exists(const char* filename);
    bool is_valid_id(const std::string& s);
    bool is_valid_email(const std::string& s);
    bool is_duplicate_in_csv(const char* filename, const char* playerID, const char* email);
    void enqueue();
    void dequeue();
    void withdraw(const char* name);
    void replace(const char* oldName, const char* newName, int priority);
    void display();
};

#endif // KEITH_TASK2_HPP