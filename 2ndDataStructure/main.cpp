#include <iostream>
#include <cstdlib>
#include <limits>

using namespace std;

int main() {
    int choice;
    do {
        cout << "\nTask Selection Menu\n";
        cout << "1. Run Keith's Task 2\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 0:
                cout << "Exiting program. Goodbye!\n";
                break;
            case 1:
                system("KeithLoZeHuiTask2.exe");
                break;
            default:
                cout << "Invalid choice! Please enter 0 or 1.\n";
        }
    } while (choice != 0);
    
    return 0;
} 