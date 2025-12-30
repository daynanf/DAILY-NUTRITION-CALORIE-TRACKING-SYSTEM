#include "auth.h"
#include "profile.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// Helper to check if username exists
bool usernameExists(const string& username) {
    ifstream file("users.txt");
    if (!file) return false;
    
    string line;
    while (getline(file, line)) {
        size_t pipe = line.find('|');
        if (pipe != string::npos) {
            string fileUser = line.substr(0, pipe);
            if (fileUser == username) return true;
        }
    }
    return false;
}

void registerUser() {
    printHeader("USER REGISTRATION");
    UserProfile p;
    string password;

    // 1. Username
    while (true) {
        cout << "Enter Username: ";
        getline(cin, p.username);
        if (p.username.empty()) {
            cout << "Username cannot be empty!\n"; 
            continue; 
        }
        if (usernameExists(p.username)) {
            cout << "Username \"" << p.username << "\" already exists. Try another!\n";
            continue;
        }
        break;
    }

    // 2. Password
    while (true) {
        cout << "Enter Password: ";
        getline(cin, password);
        if (password.empty()) {
            cout << "Password cannot be empty!\n";
            continue;
        }
        break;
    }

    // 3. Profile Details
    cout << "Enter Full Name: ";
    getline(cin, p.fullName);

    cout << "Enter Age: ";
    getValidInt(p.age, 1, 120);

    while (true) {
        cout << "Enter Gender (Male/Female): ";
        getline(cin, p.gender);
        string g = toLower(p.gender);
        if (g == "male" || g == "m") { p.gender = "Male"; break; }
        if (g == "female" || g == "f") { p.gender = "Female"; break; }
        cout << "Please enter 'Male' or 'Female'.\n";
    }

    cout << "Enter Height (cm): ";
    getValidInt(p.height_cm, 50, 300);

    cout << "Enter Weight (kg): ";
    getValidDouble(p.weight_kg, 10, 500);

    // Activity Level
    cout << "\nSelect Activity Level:\n";
    cout << "1. Sedentary\n2. Lightly Active\n3. Moderately Active\n4. Very Active\n";
    cout << "Enter choice: ";
    int choice;
    getValidInt(choice, 1, 4);
    
    if (choice == 1) p.activityLevel = "Sedentary";
    else if (choice == 2) p.activityLevel = "Lightly Active";
    else if (choice == 3) p.activityLevel = "Moderately Active";
    else p.activityLevel = "Very Active";

    // Calculate initial targets
    calcMacroTargets(p);

    // Save to users.txt
    ofstream users("users.txt", ios::app);
    if (users) {
        users << p.username << "|" << password << "|user_" << p.username << "_data.txt" << endl;
    }
    users.close();

    // Save Profile
    saveUserProfile(p);

    cout << "\n--------------------------------------------\n";
    cout << " Profile created successfully!\n";
    cout << "--------------------------------------------\n";
    displayNutritionTargets(p);
    
    pauseConsole();
}

bool loginUser(UserProfile& profile) {
    printHeader("USER LOGIN");
    string username, password;

    cout << "Enter Username: ";
    getline(cin, username);

    if (!usernameExists(username)) {
        cout << "Username not found. Please register first.\n";
        pauseConsole();
        return false;
    }

    // Check password
    cout << "Enter Password: ";
    getline(cin, password);

    ifstream file("users.txt");
    string line;
    bool authSuccess = false;

    while (getline(file, line)) {
        stringstream ss(line);
        string fileUser, filePass, waste;
        
        getline(ss, fileUser, '|');
        getline(ss, filePass, '|');
        
        if (fileUser == username && filePass == password) {
            authSuccess = true;
            break;
        }
    }

    if (authSuccess) {
        cout << "\nLogin successful! Welcome, " << username << "\n";
        pauseConsole();
        return loadUserProfile(username, profile); // Load the full data
    } else {
        cout << "\nIncorrect password.\n";
        pauseConsole();
        return false;
    }
}
