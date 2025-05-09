#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <random>
#include <limits>
using namespace std;

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

// Group struct to hold up to 5 players
struct Group {
    char groupID[10];
    Player players[5];
    int playerCount;
    Group() : playerCount(0) { groupID[0] = '\0'; }
};

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

std::string generateRandomID(int length = 4) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string id;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    for (int i = 0; i < length; ++i)
        id += alphanum[dis(gen)];
    return id;
}

class PlayerQueue {
    Player* arr;
    int front, rear, capacity;
public:
    PlayerQueue(int initial_capacity = 10) : front(0), rear(-1), capacity(initial_capacity) {
        arr = new Player[capacity];
    }
    ~PlayerQueue() { delete[] arr; }

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
    bool file_exists(const char* filename) {
        std::ifstream infile(filename);
        return infile.good();
    }
    bool is_valid_id(const std::string& s) {
        if (s.empty()) return false;
        for (char c : s) if (!isdigit(c)) return false;
        return true;
    }
    bool is_valid_email(const std::string& s) {
        return s.find('@') != std::string::npos;
    }
    bool is_duplicate_in_csv(const char* filename, const char* playerID, const char* email) {
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
    void enqueue() {
        if (isFull()) resize();
        int regType;
        while (true) {
            cout << "Register as: 1. Group  2. Individual\nEnter choice: ";
            if (cin >> regType && (regType == 1 || regType == 2)) {
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            } else {
                cout << "Invalid registration type. Please enter 1 for Group or 2 for Individual.\n";
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        if (regType == 1) { // Group registration
            char groupID[10];
            cout << "Enter new group ID: ";
            cin.getline(groupID, 10);
            int groupSize;
            while (true) {
                cout << "Enter group size (2-5): ";
                if (cin >> groupSize && groupSize >= 2 && groupSize <= 5) {
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                } else {
                    cout << "Invalid group size. Please enter a value between 2 and 5.\n";
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
            int groupPriority;
            while (true) {
                cout << "Enter group priority (1=Early-bird, 2=Wildcard, 3=Normal): ";
                if (cin >> groupPriority && groupPriority >= 1 && groupPriority <= 3) {
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                } else {
                    cout << "Invalid priority. Please enter 1, 2, or 3.\n";
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
            for (int g = 0; g < groupSize; ++g) {
                Player p;
                cout << "Enter player " << (g+1) << " name: ";
                cin.getline(p.name, 50);
                string randID = generateRandomID();
                strncpy(p.PlayerID, randID.c_str(), 4);
                p.PlayerID[4] = '\0';
                cout << "Generated PlayerID: " << p.PlayerID << endl;
                string email_str;
                while (true) {
                    cout << "Enter player email: ";
                    getline(cin, email_str);
                    if (!is_valid_email(email_str)) {
                        cout << "Invalid email format. Please include '@'.\n";
                        continue;
                    }
                    strncpy(p.PlayerEmail, email_str.c_str(), 99);
                    break;
                }
                p.priority = groupPriority;
                strncpy(p.groupID, groupID, 9);
                p.groupID[9] = '\0';
                int i = rear;
                while (i >= front && arr[i].priority > p.priority) {
                    arr[i + 1] = arr[i];
                    i--;
                }
                arr[i + 1] = p;
                rear++;
                cout << "[FIFO] Player '" << p.name << "' registered in group '" << p.groupID << "' with priority " << p.priority << ".\n";
            }
        } else if (regType == 2) { // Individual registration
            struct GroupInfo {
                char groupID[10];
                int count;
                int maxSize;
            } groupInfos[100];
            int groupCount = 0;
            for (int i = front; i <= rear; ++i) {
                bool found = false;
                for (int j = 0; j < groupCount; ++j) {
                    if (strcmp(arr[i].groupID, groupInfos[j].groupID) == 0) {
                        groupInfos[j].count++;
                        found = true;
                        break;
                    }
                }
                if (!found && strlen(arr[i].groupID) > 0) {
                    strncpy(groupInfos[groupCount].groupID, arr[i].groupID, 9);
                    groupInfos[groupCount].groupID[9] = '\0';
                    groupInfos[groupCount].count = 1;
                    groupInfos[groupCount].maxSize = 5;
                    groupCount++;
                }
            }
            for (int j = 0; j < groupCount; ++j) {
                groupInfos[j].maxSize = 0;
                for (int i = front; i <= rear; ++i) {
                    if (strcmp(arr[i].groupID, groupInfos[j].groupID) == 0) {
                        groupInfos[j].maxSize++;
                    }
                }
            }
            int incompleteIndices[100];
            int incompleteCount = 0;
            for (int j = 0; j < groupCount; ++j) {
                if (groupInfos[j].count < groupInfos[j].maxSize && groupInfos[j].count > 0) {
                    incompleteIndices[incompleteCount++] = j;
                }
            }
            char groupID[10] = "";
            bool assigned = false;
            if (incompleteCount > 0) {
                int idx = rand() % incompleteCount;
                strncpy(groupID, groupInfos[incompleteIndices[idx]].groupID, 9);
                groupID[9] = '\0';
                assigned = true;
                cout << "Assigned to incomplete group: " << groupID << endl;
            }
            if (!assigned) {
                cout << "No available incomplete group. Creating new group.\n";
                cout << "Enter new group ID: ";
                cin.getline(groupID, 10);
            }
            Player p;
            cout << "Enter player name: ";
            cin.getline(p.name, 50);
            string randID = generateRandomID();
            strncpy(p.PlayerID, randID.c_str(), 4);
            p.PlayerID[4] = '\0';
            cout << "Generated PlayerID: " << p.PlayerID << endl;
            string email_str;
            while (true) {
                cout << "Enter player email: ";
                getline(cin, email_str);
                if (!is_valid_email(email_str)) {
                    cout << "Invalid email format. Please include '@'.\n";
                    continue;
                }
                strncpy(p.PlayerEmail, email_str.c_str(), 99);
                break;
            }
            int priority;
            while (true) {
                cout << "Enter priority (1=Early-bird, 2=Wildcard, 3=Normal): ";
                if (cin >> priority && priority >= 1 && priority <= 3) {
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                } else {
                    cout << "Invalid priority. Please enter 1, 2, or 3.\n";
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
            p.priority = priority;
            strncpy(p.groupID, groupID, 9);
            p.groupID[9] = '\0';
            int i = rear;
            while (i >= front && arr[i].priority > p.priority) {
                arr[i + 1] = arr[i];
                i--;
            }
            arr[i + 1] = p;
            rear++;
            cout << "[FIFO] Player '" << p.name << "' registered in group '" << p.groupID << "' with priority " << p.priority << ".\n";
        }
    }
    void dequeue() {
        if (isEmpty()) {
            cout << "Queue is empty!\n";
            return;
        }
        cout << "[FIFO] Player '" << arr[front].name << "' checked in and removed from the front of the queue.\n";
        bool exists = file_exists("CheckedIn.csv");
        std::ofstream file("CheckedIn.csv", std::ios::app);
        if (!file.is_open()) {
            cout << "Error opening CheckedIn.csv for writing.\n";
        } else {
            if (!exists) {
                file << "Name,ID Number,Email,GroupID,Player Type,Time\n";
            }
            file << arr[front].name << "," << arr[front].PlayerID << "," << arr[front].PlayerEmail << "," << arr[front].groupID << ",";
            switch (arr[front].priority) {
                case 1: file << "Early-bird"; break;
                case 2: file << "Wildcard"; break;
                case 3: file << "Normal"; break;
            }
            std::time_t now = std::time(nullptr);
            char buf[30];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            file << "," << buf << "\n";
            file.close();
        }
        front++;
    }
    void withdraw(const char* name) {
        if (isEmpty()) {
            cout << "Queue is empty!\n";
            return;
        }
        int idx = -1;
        for (int i = front; i <= rear; ++i) {
            if (strcmp(arr[i].name, name) == 0) {
                idx = i;
                break;
            }
        }
        if (idx == -1) {
            cout << "Player not found!\n";
            return;
        }
        for (int i = idx; i < rear; ++i) {
            arr[i] = arr[i + 1];
        }
        rear--;
        cout << "Player '" << name << "' withdrawn from queue.\n";
    }
    void replace(const char* oldName, const char* newName, int priority) {
        withdraw(oldName);
        enqueue();
    }
    void display() {
        if (isEmpty()) {
            cout << "Queue is empty!\n";
            return;
        }
        cout << "Current Queue:\n";
        for (int i = front; i <= rear; ++i) {
            cout << i - front + 1 << ". " << arr[i].name << " (ID: " << arr[i].PlayerID << ", Group: " << arr[i].groupID << ", Priority: " << arr[i].priority << ")\n";
        }
    }
};

void display_checkedin_csv() {
    std::ifstream file("CheckedIn.csv");
    if (!file.is_open()) {
        std::cout << "CheckedIn.csv not found.\n";
        return;
    }
    std::string line;
    int row = 0;
    std::getline(file, line); // header
    std::cout << "\nRow | Name           | ID   | Email                | GroupID | Player Type | Time\n";
    std::cout << "--------------------------------------------------------------------------------------\n";
    while (std::getline(file, line)) {
        std::string cols[6];
        int n = split(line, ',', cols, 6);
        if (n < 6) continue;
        std::cout << std::setw(3) << row+1 << " | "
                  << std::setw(14) << cols[0] << " | "
                  << std::setw(4) << cols[1] << " | "
                  << std::setw(20) << cols[2] << " | "
                  << std::setw(7) << cols[3] << " | "
                  << std::setw(11) << cols[4] << " | "
                  << cols[5] << "\n";
        row++;
    }
    if (row == 0) std::cout << "No entries found.\n";
    std::cout << "\n";
}

int count_checkedin_csv_rows() {
    std::ifstream file("CheckedIn.csv");
    if (!file.is_open()) return 0;
    std::string line;
    int count = 0;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) count++;
    return count;
}

void load_checkedin_csv(char arr[][6][120], int& n) {
    std::ifstream file("CheckedIn.csv");
    n = 0;
    if (!file.is_open()) return;
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::string cols[6];
        int c = split(line, ',', cols, 6);
        if (c < 6) continue;
        for (int j = 0; j < 6; ++j) strncpy(arr[n][j], cols[j].c_str(), 119);
        n++;
    }
}

void write_checkedin_csv(char arr[][6][120], int n) {
    std::ofstream file("CheckedIn.csv");
    file << "Name,ID Number,Email,GroupID,Player Type,Time\n";
    for (int i = 0; i < n; ++i) {
        file << arr[i][0] << "," << arr[i][1] << "," << arr[i][2] << "," << arr[i][3] << "," << arr[i][4] << "," << arr[i][5] << "\n";
    }
}

void withdraw_from_csv() {
    int n = count_checkedin_csv_rows();
    if (n == 0) {
        std::cout << "No entries to withdraw.\n";
        return;
    }
    char arr[100][6][120];
    load_checkedin_csv(arr, n);
    display_checkedin_csv();
    int row;
    std::cout << "Enter row number to withdraw: ";
    std::cin >> row;
    std::cin.ignore();
    if (row < 1 || row > n) {
        std::cout << "Invalid row number.\n";
        return;
    }
    for (int i = row - 1; i < n - 1; ++i) {
        for (int j = 0; j < 6; ++j) strcpy(arr[i][j], arr[i + 1][j]);
    }
    n--;
    write_checkedin_csv(arr, n);
    std::cout << "Entry withdrawn.\n";
}

void replace_in_csv() {
    int n = count_checkedin_csv_rows();
    if (n == 0) {
        std::cout << "No entries to replace.\n";
        return;
    }
    char arr[100][6][120];
    load_checkedin_csv(arr, n);
    display_checkedin_csv();
    int row;
    std::cout << "Enter row number to replace: ";
    std::cin >> row;
    std::cin.ignore();
    if (row < 1 || row > n) {
        std::cout << "Invalid row number.\n";
        return;
    }
    char name[50], id[5], email[100], groupid[10], type[20], timebuf[30];
    std::cout << "Enter new player name: ";
    std::cin.getline(name, 50);
    std::cout << "Enter new player ID: ";
    std::cin.getline(id, 5);
    std::cout << "Enter new player email: ";
    std::cin.getline(email, 100);
    std::cout << "Enter new group ID: ";
    std::cin.getline(groupid, 10);
    std::cout << "Enter new player type (Early-bird/Wildcard/Normal): ";
    std::cin.getline(type, 20);
    std::time_t now = std::time(nullptr);
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    strncpy(arr[row - 1][0], name, 119);
    strncpy(arr[row - 1][1], id, 4);
    strncpy(arr[row - 1][2], email, 119);
    strncpy(arr[row - 1][3], groupid, 9);
    strncpy(arr[row - 1][4], type, 19);
    strncpy(arr[row - 1][5], timebuf, 29);
    write_checkedin_csv(arr, n);
    std::cout << "Entry replaced.\n";
}

int main() {
    PlayerQueue pq;
    int choice;
    char name[50], oldName[50], newName[50];
    int priority;
    cout << "Tournament Registration & Player Queueing System\n";
    cout << "1. Register Player\n2. Check-in Player\n3. Withdraw Player\n4. Replace Player\n5. Display Queue\n6. Display All Checked-in Data\n0. Exit\n";
    do {
        cout << "\nEnter choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
            case 1:
                pq.enqueue();
                break;
            case 2:
                pq.dequeue();
                break;
            case 3:
                withdraw_from_csv();
                break;
            case 4:
                replace_in_csv();
                break;
            case 5:
                pq.display();
                break;
            case 6:
                display_checkedin_csv();
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
    return 0;
}
