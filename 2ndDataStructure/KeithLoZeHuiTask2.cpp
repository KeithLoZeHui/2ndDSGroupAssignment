#include "KeithTask2.hpp"

using namespace std;

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
    
    for (int i = 0; i < length; ++i)
        output[i] = alphanum[dis(gen)];
    output[length] = '\0';
}

// Implementation of Count rows in checked-in CSV
int count_checkedin_csv_rows() {
    std::ifstream file("checkedin.csv");
    if (!file.is_open()) return 0;
    char line[500];
    int count = 0;
    while (file.getline(line, 500)) count++;
    return count > 0 ? count - 1 : 0; // Subtract header if file not empty
}

// Implementation of Load checked-in CSV data into array
void load_checkedin_csv(char arr[][8][120], int& n) {
    std::ifstream file("checkedin.csv");
    if (!file.is_open()) return;
    char line[500];
    n = 0;
    file.getline(line, 500); // Skip header
    while (file.getline(line, 500) && n < 100) {
        char cols[8][100]; // Updated to include GroupID and GroupName
        int num_cols = split(line, ',', cols, 8);
        for (int i = 0; i < 8; i++) {
            // If we don't have enough columns (old format), set empty string
            if (i >= num_cols) {
                arr[n][i][0] = '\0';
            } else {
                strncpy(arr[n][i], cols[i], 119);
                arr[n][i][119] = '\0';
            }
        }
        n++;
    }
    file.close();
}

// Implementation of Write array data to checked-in CSV
void write_checkedin_csv(char arr[][8][120], int n) {
    std::ofstream file("checkedin.csv");
    file << "PlayerID,PlayerName,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 8; j++) { // Updated to include GroupID and GroupName
            file << arr[i][j] << (j < 7 ? "," : "\n"); // Updated condition
        }
    }
    file.close();
}

// Implementation of Ensure same group ID for all members
void ensure_same_group_id(const char* groupID, const char* groupName, char arr[][8][120], int n) {
    // If group ID is empty, nothing to do
    if (groupID[0] == '\0') return;
    
    for (int i = 0; i < n; i++) {
        // Skip this entry if it's withdrawn
        if (strcmp(arr[i][5], "Yes") == 0) continue;
        
        // If this player has the same group ID, make sure it's exactly the same string
        if (arr[i][6][0] != '\0' && strcmp(arr[i][6], groupID) != 0) {
            // Check if this player is already in another group
            bool found_match = false;
            for (int j = 0; j < n; j++) {
                if (i != j && strcmp(arr[j][5], "Yes") != 0 && strcmp(arr[i][6], arr[j][6]) == 0) {
                    found_match = true;
                    break;
                }
            }
            
            // If not in another group, update to the new group ID and group name
            if (!found_match) {
                strncpy(arr[i][6], groupID, 119);
                arr[i][6][119] = '\0';
                strncpy(arr[i][7], groupName, 119);
                arr[i][7][119] = '\0';
            }
        }
    }
}

// Implementation of Display checked-in players from CSV
void display_checkedin_csv() {
    std::ifstream file("checkedin.csv");
    if (!file.is_open()) {
        std::cout << "checkedin.csv not found.\n";
        return;
    }
    char line[500];
    int row = 0;
    std::cout << "\nChecked-in Players:\n";
    std::cout << "------------------------------------------\n";
    while (file.getline(line, 500)) {
        if (row == 0) {
            row++;
            continue; // Skip header
        }
        char cols[8][100]; // Updated to include GroupID and GroupName
        int num_cols = split(line, ',', cols, 8);
        
        // Map columns to their correct positions
        const char* playerID = cols[0];      // Column 0: PlayerID
        const char* playerName = cols[1];    // Column 1: PlayerName
        const char* priorityType = cols[2];  // Column 2: PriorityType
        const char* regTime = cols[3];       // Column 3: RegistrationTime
        const char* status = cols[4];        // Column 4: CheckInStatus
        const char* withdrawn = cols[5];     // Column 5: Withdrawn
        const char* groupID = num_cols > 6 ? cols[6] : "";    // Column 6: GroupID
        const char* groupName = num_cols > 7 ? cols[7] : "";  // Column 7: GroupName
        
        std::cout << row << ". "
                  << "ID: " << playerID << ", "
                  << "Name: " << playerName << ", "
                  << "Priority: " << priorityType << ", "
                  << "Time: " << regTime << ", "
                  << "Status: " << status << ", "
                  << "Withdrawn: " << withdrawn;
        
        // Display Group information if available
        if (groupID[0] != '\0') {
            std::cout << ", Group: ";
            if (groupName[0] != '\0') {
                std::cout << groupName << " [" << groupID << "]";
            } else {
                std::cout << groupID;
            }
        }
        std::cout << "\n";
        row++;
    }
    std::cout << "------------------------------------------\n";
    file.close();
}

// Load players from CSV file
PlayerCSV* loadPlayersFromCSV(const char* filename, int& num_players) {
    std::ifstream file(filename);
    char line[500];
    int capacity = 10;
    num_players = 0;
    PlayerCSV* players = new PlayerCSV[capacity];
    
    // Skip header
    if (file.getline(line, 500)) {
        while (file.getline(line, 500)) {
            char cols[8][100]; // Updated to include GroupID and GroupName
            int n = split(line, ',', cols, 8);
            if (n < 6) continue; // Still allow old format without GroupID/GroupName
            if (num_players == capacity) {
                capacity *= 2;
                PlayerCSV* new_players = new PlayerCSV[capacity];
                for (int i = 0; i < num_players; ++i) new_players[i] = players[i];
                delete[] players;
                players = new_players;
            }
            players[num_players].PlayerID = atoi(cols[0]);
            strncpy(players[num_players].PlayerName, cols[1], 49);
            strncpy(players[num_players].PriorityType, cols[2], 19);
            strncpy(players[num_players].RegistrationTime, cols[3], 24);
            strncpy(players[num_players].CheckInStatus, cols[4], 19);
            strncpy(players[num_players].Withdrawn, cols[5], 4);
            
            // Handle GroupID - use empty string if not present in CSV
            if (n >= 7) {
                strncpy(players[num_players].GroupID, cols[6], 9);
            } else {
                players[num_players].GroupID[0] = '\0';
            }
            
            // Handle GroupName - use empty string if not present in CSV
            if (n >= 8) {
                strncpy(players[num_players].GroupName, cols[7], 49);
            } else {
                players[num_players].GroupName[0] = '\0';
            }
            
            num_players++;
        }
    }
    return players;
}

// Save players to CSV file
void savePlayersToCSV(const char* filename, PlayerCSV* players, int num_players) {
    std::ofstream file(filename);
    file << "PlayerID,PlayerName,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
    for (int i = 0; i < num_players; ++i) {
        file << players[i].PlayerID << ',' << players[i].PlayerName << ',' << players[i].PriorityType << ',' 
             << players[i].RegistrationTime << ',' << players[i].CheckInStatus << ',' << players[i].Withdrawn << ',' 
             << players[i].GroupID << ',' << players[i].GroupName << "\n";
    }
}

// Withdraw a player from CSV
void withdraw_from_csv(const char* name) {
    char arr[100][8][120]; // Updated to include GroupID and GroupName
    int n = 0;
    load_checkedin_csv(arr, n);
    
    bool found = false;
    for (int i = 0; i < n; i++) {
        if (strcmp(arr[i][1], name) == 0 && strcmp(arr[i][5], "Yes") != 0) {
            strcpy(arr[i][5], "Yes");
            found = true;
            break;
        }
    }
    
    if (found) {
        write_checkedin_csv(arr, n);
        std::cout << "Player " << name << " withdrawn.\n";
    } else {
        std::cout << "Player " << name << " not found or already withdrawn.\n";
    }
}

// Replace a player in CSV
void replace_in_csv(const char* oldName, const char* newName, int priority) {
    int n = count_checkedin_csv_rows();
    if (n == 0) {
        std::cout << "No entries to replace.\n";
        return;
    }
    char arr[100][8][120]; // Updated to include GroupID and GroupName
    load_checkedin_csv(arr, n);
    
    bool found = false;
    int row = -1;
    for (int i = 0; i < n; i++) {
        if (strcmp(arr[i][1], oldName) == 0) {
            row = i;
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::cout << "Player " << oldName << " not found.\n";
        return;
    }
    
    // Generate timestamp
    char timebuf[30];
    std::time_t now = std::time(nullptr);
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    
    // Update player info
    strncpy(arr[row][1], newName, 119);
    
    // Convert priority to string
    char priorityStr[20];
    switch(priority) {
        case 1: strcpy(priorityStr, "Early-bird"); break;
        case 2: strcpy(priorityStr, "Wildcard"); break;
        case 3: strcpy(priorityStr, "Normal"); break;
        default: strcpy(priorityStr, "Normal");
    }
    strncpy(arr[row][2], priorityStr, 119);
    
    write_checkedin_csv(arr, n);
    std::cout << "Player " << oldName << " replaced with " << newName << ".\n";
}

// Check if file exists
bool PlayerQueue::file_exists(const char* filename) {
    std::ifstream infile(filename);
    return infile.good();
}

// Check if string is a valid ID
bool PlayerQueue::is_valid_id(const char* s) {
    if (s == nullptr || s[0] == '\0') return false;
    for (int i = 0; s[i] != '\0'; i++) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

// Check if email is valid (basic check for @)
bool PlayerQueue::is_valid_email(const char* s) {
    if (s == nullptr) return false;
    const char* at = strchr(s, '@');
    return at != nullptr;
}

// Check for duplicates in CSV
bool PlayerQueue::is_duplicate_in_csv(const char* filename, const char* playerID, const char* email) {
    std::ifstream file(filename);
    char line[500];
    file.getline(line, 500); // skip header
    while (file.getline(line, 500)) {
        char cols[5][100];
        int n = split(line, ',', cols, 5);
        if (n < 5) continue;
        if ((playerID[0] != '\0' && strcmp(cols[1], playerID) == 0) || (email[0] != '\0' && strcmp(cols[2], email) == 0)) {
            return true;
        }
    }
    return false;
}

// Assign individuals to existing groups
void GroupManager::assignToGroups() {
    int groupCount;
    int groupSizes[MAX_GROUPS];
    char groupIDs[MAX_GROUPS][10];
    
    // Count existing groups and their sizes
    countGroups(groupCount, groupSizes);
    
    // Collect group IDs
    int groupIndex = 0;
    for (int i = 0; i < rowCount && groupIndex < groupCount; i++) {
        if (strcmp(data[i][5], "Yes") != 0 && data[i][6][0] != '\0') {
            bool found = false;
            for (int j = 0; j < groupIndex; j++) {
                if (strcmp(groupIDs[j], data[i][6]) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                strncpy(groupIDs[groupIndex], data[i][6], 9);
                groupIDs[groupIndex][9] = '\0';
                groupIndex++;
            }
        }
    }
    
    // Create a random seed
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Assign individuals to groups that have space
    for (int i = 0; i < rowCount; i++) {
        // Skip withdrawn players or players already in a group
        if (strcmp(data[i][5], "Yes") == 0 || data[i][6][0] != '\0') continue;
        
        // Find groups with space
        int availableGroups[MAX_GROUPS];
        int availableCount = 0;
        for (int j = 0; j < groupCount; j++) {
            if (groupSizes[j] < MAX_MEMBERS_PER_GROUP) {
                availableGroups[availableCount++] = j;
            }
        }
        
        // If no groups with space and we can create a new group
        if (availableCount == 0 && groupCount < MAX_GROUPS) {
            // Generate a new group ID
            char newGroupID[10];
            generateRandomID(newGroupID, 4);
            strncpy(groupIDs[groupCount], newGroupID, 9);
            groupIDs[groupCount][9] = '\0';
            groupSizes[groupCount] = 0;
            availableGroups[0] = groupCount;
            availableCount = 1;
            groupCount++;
        }
        
        // Randomly assign to an available group
        if (availableCount > 0) {
            std::uniform_int_distribution<> distrib(0, availableCount - 1);
            int randomGroupIndex = availableGroups[distrib(gen)];
            
            // Assign to this group
            strncpy(data[i][6], groupIDs[randomGroupIndex], 119);
            data[i][6][119] = '\0';
            groupSizes[randomGroupIndex]++;
        }
    }
}

// Merge small groups together
void GroupManager::mergeSmallGroups() {
    int groupCount;
    int groupSizes[MAX_GROUPS];
    char groupIDs[MAX_GROUPS][10];
    
    // Count existing groups and their sizes
    countGroups(groupCount, groupSizes);
    
    // Collect group IDs
    int groupIndex = 0;
    for (int i = 0; i < rowCount && groupIndex < groupCount; i++) {
        if (strcmp(data[i][5], "Yes") != 0 && data[i][6][0] != '\0') {
            bool found = false;
            for (int j = 0; j < groupIndex; j++) {
                if (strcmp(groupIDs[j], data[i][6]) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                strncpy(groupIDs[groupIndex], data[i][6], 9);
                groupIDs[groupIndex][9] = '\0';
                groupIndex++;
            }
        }
    }
    
    // Sort groups by size (ascending)
    for (int i = 0; i < groupCount - 1; i++) {
        for (int j = 0; j < groupCount - i - 1; j++) {
            if (groupSizes[j] > groupSizes[j + 1]) {
                // Swap sizes
                int tempSize = groupSizes[j];
                groupSizes[j] = groupSizes[j + 1];
                groupSizes[j + 1] = tempSize;
                
                // Swap IDs
                char tempID[10];
                strncpy(tempID, groupIDs[j], 9);
                tempID[9] = '\0';
                strncpy(groupIDs[j], groupIDs[j + 1], 9);
                groupIDs[j][9] = '\0';
                strncpy(groupIDs[j + 1], tempID, 9);
                groupIDs[j + 1][9] = '\0';
            }
        }
    }
    
    // Try to merge small groups
    for (int i = 0; i < groupCount - 1; i++) {
        // Skip if this group is already at max capacity
        if (groupSizes[i] >= MAX_MEMBERS_PER_GROUP) continue;
        
        for (int j = i + 1; j < groupCount; j++) {
            // Skip if merging would exceed max capacity
            if (groupSizes[i] + groupSizes[j] > MAX_MEMBERS_PER_GROUP) continue;
            
            // Merge group j into group i
            for (int k = 0; k < rowCount; k++) {
                if (strcmp(data[k][5], "Yes") != 0 && strcmp(data[k][6], groupIDs[j]) == 0) {
                    strncpy(data[k][6], groupIDs[i], 119);
                    data[k][6][119] = '\0';
                }
            }
            
            // Update group sizes
            groupSizes[i] += groupSizes[j];
            groupSizes[j] = 0;
            
            // Mark group j as empty
            groupIDs[j][0] = '\0';
        }
    }
}

// Function to display statistics about groups
void GroupManager::displayGroupStatistics() {
    // Display group statistics
    int groupCount;
    int groupSizes[MAX_GROUPS];
    countGroups(groupCount, groupSizes);
    
    std::cout << "\nGroup Statistics:\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Total Groups: " << groupCount << " (Maximum: " << MAX_GROUPS << ")\n";
    
    // Count individuals without a group
    int individualsCount = 0;
    for (int i = 0; i < rowCount; i++) {
        if (strcmp(data[i][5], "Yes") != 0 && data[i][6][0] == '\0') {
            individualsCount++;
        }
    }
    
    if (individualsCount > 0) {
        std::cout << "Players without a group: " << individualsCount << "\n";
    }
    
    std::cout << "------------------------------------------\n";
}

// Function to organize groups (wrapper for the class method)
void organize_groups() {
    GroupManager manager;
    manager.organizeGroups();
}

// Implementation of GroupManager::organizeGroups()
void GroupManager::organizeGroups() {
    // Load current data
    loadData();
    
    if (rowCount == 0) {
        std::cout << "No player data found." << std::endl;
        return;
    }
    
    // Assign individual players to groups
    assignToGroups();
    
    // Try to merge small groups
    mergeSmallGroups();
    
    // Save changes
    saveData();
    
    // Display the results
    std::cout << "Players have been organized into groups." << std::endl;
    display_checkedin_csv();
    displayGroupStatistics();
}

// Implementation of GroupManager::countGroups()
void GroupManager::countGroups(int& groupCount, int groupSizes[]) {
    char groupIDs[MAX_GROUPS][10];
    groupCount = 0;
    
    // Initialize group sizes
    for (int i = 0; i < MAX_GROUPS; i++) {
        groupSizes[i] = 0;
        groupIDs[i][0] = '\0';
    }
    
    // Count groups and their sizes
    for (int i = 0; i < rowCount; i++) {
        // Skip withdrawn players or players without a group
        if (strcmp(data[i][5], "Yes") == 0 || data[i][6][0] == '\0') continue;
        
        // Check if we've seen this group before
        bool found = false;
        for (int j = 0; j < groupCount; j++) {
            if (strcmp(groupIDs[j], data[i][6]) == 0) {
                groupSizes[j]++;
                found = true;
                break;
            }
        }
        
        // New group found
        if (!found && groupCount < MAX_GROUPS) {
            strncpy(groupIDs[groupCount], data[i][6], 9);
            groupIDs[groupCount][9] = '\0';
            groupSizes[groupCount] = 1;
            groupCount++;
        }
    }
}

// Implementation of is_valid_email
bool is_valid_email(const char* email) {
    if (!email) return false;
    
    // Check for @ symbol
    bool has_at = false;
    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') {
            has_at = true;
            break;
        }
    }
    
    return has_at;
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

int main() {
    PlayerQueue pq;
    int choice;
    do {
        cout << "\nPlayer Queue Management System\n";
        cout << "1. Register Player\n";
        cout << "2. Check-in Player\n";
        cout << "3. Withdraw Player\n";
        cout << "4. Replace Player\n";
        cout << "5. Display Queue\n";
        cout << "6. Display Checked-in Players\n";
        cout << "7. Organize Groups\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 0: // Exit option
                cout << "Exiting program. Goodbye!\n";
                break;
            case 1:
                pq.enqueue();
                break;
            case 2:
                pq.dequeue();
                break;
            case 3: {
                char name[50];
                cout << "Enter player name to withdraw: ";
                cin.getline(name, 50);
                withdraw_from_csv(name);
                break;
            }
            case 4: {
                char oldName[50], newName[50];
                int priority;
                cout << "Enter player name to replace: ";
                cin.getline(oldName, 50);
                cout << "Enter new player name: ";
                cin.getline(newName, 50);
                cout << "Enter new player priority (1=Early-bird, 2=Wildcard, 3=Normal): ";
                cin >> priority;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                replace_in_csv(oldName, newName, priority);
                break;
            }
            case 5:
                pq.display();
                break;
            case 6:
                display_checkedin_csv();
                break;
            case 7:
                organize_groups();
                break;
            default:
                cout << "Invalid choice! Please enter a number between 0 and 7.\n";
        }
    } while (choice != 0);
    return 0;
}

// Constructor
PlayerQueue::PlayerQueue(int initial_capacity) : front(0), rear(-1), capacity(initial_capacity) {
    arr = new Player[capacity];
}

// Destructor
PlayerQueue::~PlayerQueue() { 
    delete[] arr; 
}

void PlayerQueue::enqueue() {
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
        // First collect all information
        char groupName[50];
        cout << "Enter group name: ";
        cin.getline(groupName, 50);
        
        int groupSize;
        while (true) {
            cout << "Enter group size (2-5): ";
            if (cin >> groupSize && groupSize >= 2 && groupSize <= 5) {
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer after group size
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
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer after priority
                break;
            } else {
                cout << "Invalid priority. Please enter 1, 2, or 3.\n";
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }

        // Create temporary storage for all player information
        struct TempPlayer {
            char name[50];
            char email[100];
            char playerID[5];
        };
        
        TempPlayer players[5]; // Max 5 players in a group
        
        // Collect information for all players
        for (int g = 0; g < groupSize; ++g) {
            cout << "Enter player " << (g+1) << " name: ";
            cin.getline(players[g].name, 50);
            
            while (true) {
                cout << "Enter player " << (g+1) << " email: ";
                cin.getline(players[g].email, 100);
                if (!is_valid_email(players[g].email)) {
                    cout << "Invalid email format. Please include '@'.\n";
                    continue;
                }
                break;
            }
            
            // Generate a unique player ID
            generateRandomID(players[g].playerID, 4);
            
            // Ensure ID is unique
            bool idUnique = true;
            for (int i = front; i <= rear; i++) {
                if (strcmp(arr[i].PlayerID, players[g].playerID) == 0) {
                    idUnique = false;
                    break;
                }
            }
            
            // If not unique, generate a new one
            if (!idUnique) {
                do {
                    generateRandomID(players[g].playerID, 4);
                    idUnique = true;
                    for (int i = front; i <= rear; i++) {
                        if (strcmp(arr[i].PlayerID, players[g].playerID) == 0) {
                            idUnique = false;
                            break;
                        }
                    }
                } while (!idUnique);
            }
        }
        
        // Now generate the group ID
        char groupID[10];
        generateRandomID(groupID, 6);
        cout << "Auto-generated Group ID: " << groupID << "\n";
        
        // Now add all players to the queue
        for (int g = 0; g < groupSize; ++g) {
            Player p;
            
            // Copy collected information
            strncpy(p.name, players[g].name, 49);
            p.name[49] = '\0';
            
            strncpy(p.PlayerID, players[g].playerID, 4);
            p.PlayerID[4] = '\0';
            cout << "Player " << (g+1) << " ID: " << p.PlayerID << "\n";
            
            strncpy(p.PlayerEmail, players[g].email, 99);
            p.PlayerEmail[99] = '\0';
            
            p.priority = groupPriority;
            strncpy(p.groupID, groupID, 9);
            p.groupID[9] = '\0';
            strncpy(p.groupName, groupName, 49);
            p.groupName[49] = '\0';
            
            // Insert player maintaining priority order
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
    else if (regType == 2) { // Individual registration
        // First collect player information
        Player p;
        cout << "Enter player name: ";
        cin.getline(p.name, 50);
        
        while (true) {
            cout << "Enter player email: ";
            cin.getline(p.PlayerEmail, 100);
            if (!is_valid_email(p.PlayerEmail)) {
                cout << "Invalid email format. Please include '@'.\n";
                continue;
            }
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
        
        // Generate player ID
        generateRandomID(p.PlayerID, 4);
        bool idUnique = true;
        for (int i = front; i <= rear; i++) {
            if (strcmp(arr[i].PlayerID, p.PlayerID) == 0) {
                idUnique = false;
                break;
            }
        }
        if (!idUnique) {
            do {
                generateRandomID(p.PlayerID, 4);
                idUnique = true;
                for (int i = front; i <= rear; i++) {
                    if (strcmp(arr[i].PlayerID, p.PlayerID) == 0) {
                        idUnique = false;
                        break;
                    }
                }
            } while (!idUnique);
        }
        cout << "Generated PlayerID: " << p.PlayerID << endl;
        
        // Now determine group assignment
        char groupID[10] = "";
        char groupName[50] = "";
        bool assigned = false;
        
        // Find incomplete groups
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
        
        if (incompleteCount > 0) {
            int idx = rand() % incompleteCount;
            strncpy(groupID, groupInfos[incompleteIndices[idx]].groupID, 9);
            groupID[9] = '\0';
            
            // Find the group name for this group ID
            for (int i = front; i <= rear; ++i) {
                if (strcmp(arr[i].groupID, groupID) == 0) {
                    strncpy(groupName, arr[i].groupName, 49);
                    groupName[49] = '\0';
                    break;
                }
            }
            
            assigned = true;
            cout << "Assigned to incomplete group: " << groupID << " (" << groupName << ")" << endl;
        }
        
        if (!assigned) {
            cout << "No available incomplete group. Creating new group.\n";
            generateRandomID(groupID, 6); // Auto-generate group ID
            cout << "Auto-generated Group ID: " << groupID << "\n";
            
            cout << "Enter group name: ";
            cin.getline(groupName, 50);
        }
        
        // Set group information
        p.priority = priority;
        strncpy(p.groupID, groupID, 9);
        p.groupID[9] = '\0';
        strncpy(p.groupName, groupName, 49);
        p.groupName[49] = '\0';
        
        // Insert player maintaining priority order
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

void PlayerQueue::dequeue() {
    if (isEmpty()) {
        cout << "Queue is empty!\n";
        return;
    }
    cout << "Dequeued player: " << arr[front].name << " (ID: " << arr[front].PlayerID << ")\n";
    
    // Convert priority number to string
    char priorityStr[20];
    switch(arr[front].priority) {
        case 1: strcpy(priorityStr, "Early-bird"); break;
        case 2: strcpy(priorityStr, "Wildcard"); break;
        case 3: strcpy(priorityStr, "Normal"); break;
        default: strcpy(priorityStr, "Normal");
    }
    
    // Get current timestamp
    char timeStr[25];
    std::time_t now = std::time(nullptr);
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    
    // Create a unique player ID for the CSV
    char uniquePlayerID[20];
    // Ensure we have a truly unique ID by combining the player ID with a timestamp suffix
    sprintf(uniquePlayerID, "%s_%d", arr[front].PlayerID, static_cast<int>(now % 10000));
    
    if (file_exists("checkedin.csv")) {
        // If checkedin.csv exists, add player to it
        std::ofstream outfile("checkedin.csv", std::ios::app);
        outfile << uniquePlayerID << ","
                << arr[front].name << ","
                << priorityStr << ","
                << timeStr << ","
                << "Checked-in,"
                << "No,"
                << arr[front].groupID << ","
                << arr[front].groupName << "\n";
        outfile.close();
    } else {
        // Create new checkedin.csv file with header
        std::ofstream outfile("checkedin.csv");
        outfile << "PlayerID,PlayerName,PriorityType,RegistrationTime,CheckInStatus,Withdrawn,GroupID,GroupName\n";
        outfile << uniquePlayerID << ","
                << arr[front].name << ","
                << priorityStr << ","
                << timeStr << ","
                << "Checked-in,"
                << "No,"
                << arr[front].groupID << ","
                << arr[front].groupName << "\n";
        outfile.close();
    }
    front++;
}

void PlayerQueue::withdraw(const char* name) {
    if (isEmpty()) {
        cout << "Queue is empty!\n";
        return;
    }
    for (int i = front; i <= rear; ++i) {
        if (strcmp(arr[i].name, name) == 0) {
            cout << "Player " << name << " withdrawn from queue.\n";
            // Shift all elements after this one
            for (int j = i; j < rear; ++j) {
                arr[j] = arr[j + 1];
            }
            rear--;
            return;
        }
    }
    cout << "Player " << name << " not found in queue.\n";
}

void PlayerQueue::replace(const char* oldName, const char* newName, int priority) {
    withdraw(oldName);
    enqueue();
}

