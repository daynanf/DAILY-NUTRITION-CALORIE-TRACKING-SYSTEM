#include "utils.h"
#include <iostream>
#include <windows.h>

using namespace std;

// Configure the console output code page to UTF-8
// Ensures that special characters (like emojis or box-drawing chars) display correctly.
void setupConsole() {
    SetConsoleOutputCP(CP_UTF8); // enable UTF-8 output
}

// Clears the console screen.
// Uses a system call "cls", which makes it Windows-specific.
void clearScreen() {
    system("cls");
}

// Pauses program execution until the user presses Enter.
// Used to handle "Press any key to continue" style interactions.
void pauseConsole() {
    cout << "\n👉 Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Prints a decorative header with a centered title (and optional subtitle).
// Provides a consistent visual style for different screens/menus.
void printHeader(const string& title, const string& subtitle) {
    clearScreen();
    const int width = 77;
    string top = "╔═════════════════════════════════════════════════════════════════════════════╗";
    string bot = "╚═════════════════════════════════════════════════════════════════════════════╝";

    cout << "\n" << top << "\n";

    // Helper to center a line
    auto printCentered = [&](const string& s) {
        int pad = (width - (int)s.size()) / 2;
        if (pad < 0) pad = 0;
        cout << "║" << string(pad, ' ') << s << string(max(0, width - pad - (int)s.size()), ' ') << "║\n";
    };

    printCentered(title);
    if (!subtitle.empty()) printCentered("- " + subtitle + " -");

    cout << bot << "\n\n";
}

// Converts a standard string to lowercase.
// Utility used for case-insensitive string comparisons.
string toLower(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Checks if 'searchWord' is occurring inside 'text' (case-insensitive).
// Returns true if found, false otherwise.
bool isSimilar(string searchWord, string text) {
    return toLower(text).find(toLower(searchWord)) != string::npos;
}

// Removes leading and trailing whitespace from a string.
// Important for cleaning up user inputs (e.g. "   hello   " -> "hello").
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Helper to get a validated integer input from the user.
// - loops until a valid integer is entered
// - optionally checks against min/max bounds if they are not -1.
void getValidInt(int& value, int min, int max) {
    while (true) {
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number: ";
            continue;
        }
        if ((min != -1 && value < min) || (max != -1 && value > max)) {
            cout << "Value out of range (" << min << "-" << max << "). Try again: ";
            continue;
        }
        break;
    }
}

// Helper to get a validated double (floating point) input from the user.
// - loops until a valid number is entered
// - optionally checks against min/max bounds if they are not -1.0.
void getValidDouble(double& value, double min, double max) {
    while (true) {
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number: ";
            continue;
        }
        if ((min != -1.0 && value < min) || (max != -1.0 && value > max)) {
            cout << "Value out of range check. Try again: ";
            continue;
        }
        break;
    }
}

// Checks if the user input is "b" or "B" (Back command)
// If triggered, it asks for confirmation ("Are you sure you want to go back?").
// Returns true if the user confirms the back action, false otherwise.
bool checkForBack(const string& input) {
    if (input == "b" || input == "B") {
        cout << "\nAre you sure you want to go back? (y/n): ";
        char confirm;
        cin >> confirm;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (confirm == 'y' || confirm == 'Y') {
            return true;
        }
    }
    return false;
}
