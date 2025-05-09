#include "KeithTask2.hpp"

int split(const std::string& s, char delimiter, std::string tokens[], int max_tokens) {
    int count = 0;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter) && count < max_tokens) {
        tokens[count++] = token;
    }
    return count;
}

PlayerCSV* loadPlayersFromCSV(const char* filename, int& num_players) {
    std::ifstream file(filename);
    std::string line;
    int capacity = 10;
    num_players = 0;
    PlayerCSV* players = new PlayerCSV[capacity];
    getline(file, line); // skip header
    while (getline(file, line)) {
        std::string cols[6];
        int n = split(line, ',', cols, 6);
        if (n < 6) continue;
        if (num_players == capacity) {
            capacity *= 2;
            PlayerCSV* new_players = new PlayerCSV[capacity];
            for (int i = 0; i < num_players; ++i) new_players[i] = players[i];
            delete[] players;
            players = new_players;
        }
        players[num_players].PlayerID = std::stoi(cols[0]);
        strncpy(players[num_players].PlayerName, cols[1].c_str(), 49);
        strncpy(players[num_players].PriorityType, cols[2].c_str(), 19);
        strncpy(players[num_players].RegistrationTime, cols[3].c_str(), 24);
        strncpy(players[num_players].CheckInStatus, cols[4].c_str(), 19);
        strncpy(players[num_players].Withdrawn, cols[5].c_str(), 4);
        num_players++;
    }
    return players;
}

void savePlayersToCSV(const char* filename, PlayerCSV* players, int num_players) {
    std::ofstream file(filename);
    file << "PlayerID,PlayerName,PriorityType,RegistrationTime,CheckInStatus,Withdrawn\n";
    for (int i = 0; i < num_players; ++i) {
        file << players[i].PlayerID << ',' << players[i].PlayerName << ',' << players[i].PriorityType << ',' << players[i].RegistrationTime << ',' << players[i].CheckInStatus << ',' << players[i].Withdrawn << "\n";
    }
}

std::string generateRandomID(int length) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string id;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    for (int i = 0; i < length; ++i)
        id += alphanum[dis(gen)];
    return id;
}

PlayerQueue::PlayerQueue(int initial_capacity) : front(0), rear(-1), capacity(initial_capacity) {
    arr = new Player[capacity];
}
PlayerQueue::~PlayerQueue() { delete[] arr; }
bool PlayerQueue::isEmpty() { return front > rear; }
bool PlayerQueue::isFull() { return rear == capacity - 1; }
void PlayerQueue::resize() {
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
bool PlayerQueue::file_exists(const char* filename) {
    std::ifstream infile(filename);
    return infile.good();
}
bool PlayerQueue::is_valid_id(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit(c)) return false;
    return true;
}
bool PlayerQueue::is_valid_email(const std::string& s) {
    return s.find('@') != std::string::npos;
}
bool PlayerQueue::is_duplicate_in_csv(const char* filename, const char* playerID, const char* email) {
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        std::string cols[5];
        int n = split(line, ',', cols, 5);
        if (n < 5) continue;
        if ((playerID[0] != '\0' && cols[1] == playerID) || (email[0] != '\0' && strcmp(cols[2].c_str(), email) == 0)) {
            return true;
        }
    }
    return false;
}
// ... (copy the rest of the PlayerQueue methods and all other functions from your previous single-file version here, except main) ... 