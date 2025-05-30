#include "KeithTask2.hpp"

int main() {
    PlayerQueue pq;
    int choice;
    do {
        std::cout << "\nPlayer Queue Management System\n";
        std::cout << "1. Register Player\n";
        std::cout << "2. Check-in Player\n";
        std::cout << "3. Withdraw Player\n";
        std::cout << "4. Replace Player\n";
        std::cout << "5. Display Queue\n";
        std::cout << "6. Display Checked-in Players\n";
        std::cout << "7. Organize Groups\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 0: // Exit option
                std::cout << "Exiting program. Goodbye!\n";
                break;
            case 1:
                pq.enqueue();
                break;
            case 2:
                pq.dequeue();
                break;
            case 3: {
                char name[50];
                std::cout << "Enter player name, full PlayerID (e.g., PLY056), or last 3 digits (e.g., 056) to withdraw: ";
                std::cin.getline(name, 50);
                withdraw_from_csv(name);
                break;
            }
            case 4: {
                char oldName[50], newName[50];
                int priority;
                std::cout << "Enter player name to replace: ";
                std::cin.getline(oldName, 50);
                std::cout << "Enter new player name: ";
                std::cin.getline(newName, 50);
                std::cout << "Enter new player priority (1=Early-bird, 2=Wildcard, 3=Normal): ";
                std::cin >> priority;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
                std::cout << "Invalid choice! Please enter a number between 0 and 7.\n";
        }
    } while (choice != 0);
    return 0;
}
