#include "utils.h"
#include <iostream>
#include <windows.h>

using namespace std;

void setupConsole() {
    SetConsoleOutputCP(CP_UTF8); // enable UTF-8 output
}

void clearScreen() {
    system("cls");
}

void pauseConsole() {
    cout << "\n👉 Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

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

string toLower(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

bool isSimilar(string searchWord, string text) {
    return toLower(text).find(toLower(searchWord)) != string::npos;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

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
