#include "KeithTask2.hpp"

using namespace std;

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
