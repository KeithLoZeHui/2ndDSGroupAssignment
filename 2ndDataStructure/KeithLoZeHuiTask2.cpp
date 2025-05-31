#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include "KeithTask2.hpp"
#include "AngelTask.hpp"
#include "CSVOperations.hpp"
#include "Utils.hpp"
#include "PlayerQueue.hpp"
#include "GroupManager.hpp"

// Function to demonstrate Keith's Task 2
void demonstrateKeithsTask2() {
    std::cout << "\n=== Task 2: Tournament Registration & Player Queueing ===\n";
    
    // Create player queue
    PlayerQueue queue(10);
    
    // Create group manager
    GroupManager groupManager;
    
    while (true) {
        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. Register player\n";
        std::cout << "2. Check in player\n";
        std::cout << "3. Display queue\n";
        std::cout << "4. Withdraw player\n";
        std::cout << "5. Replace player\n";
        std::cout << "6. Group Management\n";
        std::cout << "7. Exit\n";
        std::cout << "Enter your choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline
        
        switch (choice) {
            case 1:
                queue.enqueue();
                break;
            case 2:
                queue.dequeue();
                break;
            case 3:
                queue.display();
                break;
            case 4: {
                char name[50];
                std::cout << "Enter player name to withdraw: ";
                std::cin.getline(name, 50);
                queue.withdraw(name);
                break;
            }
            case 5: {
                char oldName[50], newName[50];
                int priority;
                std::cout << "Enter old player name: ";
                std::cin.getline(oldName, 50);
                std::cout << "Enter new player name: ";
                std::cin.getline(newName, 50);
                std::cout << "Enter priority (1=Early-bird, 2=Wildcard, 3=Normal): ";
                std::cin >> priority;
                std::cin.ignore(); // Clear the newline
                queue.replace(oldName, newName, priority);
                break;
            }
            case 6:
                groupManager.run();
                break;
            case 7:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {
    // Demonstrate Keith's Task 2
    demonstrateKeithsTask2();
    return 0;
}
