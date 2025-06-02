#include <iostream>
#include <cstdlib>
#include <limits>

using namespace std;

int main() {
    int choice;
    do {
        cout << "\nTask Selection Menu\n";
        cout << "1. Run Task 1\n";
        cout << "2. Run Task 2\n";
        cout << "3. Run Task 3\n";
        cout << "4. Run Task 4\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 0:
                cout << "Exiting program. Goodbye!\n";
                break;
            case 1:
                system("Task1.exe");
                break;
            case 2:
                system("KeithLoZeHuiTask2.exe");
                break;
            case 3:
                system("Angeltask.exe");
                break;
            case 4:
                system("SamTask4.exe");
                break;
            default:
                cout << "Invalid choice! Please enter a number between 0 and 4.\n";
        }
    } while (choice != 0);
    
    return 0;
} 