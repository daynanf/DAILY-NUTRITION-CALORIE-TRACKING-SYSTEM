// calorie_tracker_fixed.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <cctype>
#include <windows.h>
#include <limits>
#include <algorithm>
#include <ctime>
#include <chrono>
#include "auth.h"
#include "profile.h"
#include "utils.h"
#include "models.h"
using namespace std;

// /* -------------------- Structs -------------------- */

// // struct UserAccount {
// //     string username;
// //     string password;
// //     string fullName;
// //     int age;
// //     string gender;
// //     int height;
// //     int weight;
// //     string activityLevel;
// // };

// struct FoodItem {
//     string name;
//     float calories = 0;
//     float protein = 0;
//     float carbs = 0;
//     float fat = 0;
//     float servingSize = 0; // numeric grams (or ml)
// };

// struct UserProfile {
//     string username;
//     string name;
//     int age = 0;
//     string gender;
//     float height = 0;
//     float weight = 0;
//     string activityLevel;
//     float dailyCalorieTarget = 2362; // fixed default
// };

// struct DailyLog {
//     string mealType;      // "Breakfast", "Lunch", etc.
//     string foodName;
//     float grams;
//     float calories;
//     float protein;
//     float carbs;
//     float fat;
// };

/* -------------------- Globals -------------------- */

string currentUsername = "";

/* -------------------- Helpers -------------------- */



bool checkForBack(const string& input) {
    return (input == "b" || input == "B");
}

int safeStoi(const string& s, int defaultVal = -1) {
    string t = trim(s);
    if (t.empty()) return defaultVal;
    try {
        size_t pos;
        int v = stoi(t, &pos);
        if (pos != t.size()) return defaultVal;
        return v;
    } catch (...) {
        return defaultVal;
    }
}

float safeStof(const string& s, float defaultVal = 0.0f) {
    string t = trim(s);
    if (t.empty()) return defaultVal;
    try {
        size_t pos;
        float v = stof(t, &pos);
        if (pos != t.size()) return defaultVal;
        return v;
    } catch (...) {
        return defaultVal;
    }
}

string getTodayDate() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_time);
    stringstream ss;
    ss << (local_time->tm_year + 1900) << "-"
       << setw(2) << setfill('0') << (local_time->tm_mon + 1) << "-"
       << setw(2) << setfill('0') << local_time->tm_mday;
    return ss.str();
}


/* -------------------- Parse profile file -------------------- */

UserProfile parseUserProfile(const string& username) {
    UserProfile profile;
    profile.username = username;
    
    string filename = "user_" + username + "_data.txt";
    ifstream file(filename.c_str());
    
    if (file) {
        string line;
        while (getline(file, line)) {
            string t = trim(line);
            if (t.rfind("Name:", 0) == 0) profile.fullName = trim(t.substr(5));
            else if (t.rfind("Age:", 0) == 0) profile.age = safeStoi(t.substr(4), 0);
            else if (t.rfind("Gender:", 0) == 0) profile.gender = trim(t.substr(7));
            else if (t.rfind("Height:", 0) == 0) profile.height_cm = safeStof(t.substr(7), 0.0f);
            else if (t.rfind("Weight:", 0) == 0) profile.weight_kg = safeStof(t.substr(7), 0.0f);
            else if (t.rfind("Activity_Level:", 0) == 0) profile.activityLevel = trim(t.substr(15));
            else if (t.rfind("Daily_Calorie_Target:", 0) == 0) profile.dailyCaloriesTarget = safeStof(t.substr(22), 2362.0f);
        }
        file.close();
    }
    
    return profile;
}

/* -------------------- Food parsing & search -------------------- */

FoodItem parseFoodLine(const string& line) {
    FoodItem item;
    stringstream ss(line);
    string part;
    
    // name
    if (!getline(ss, part, '|')) return item;
    item.name = trim(part);
    
    // calories
    if (!getline(ss, part, '|')) return item;
    item.calories = safeStof(part, 0.0f);
    
    // protein
    if (!getline(ss, part, '|')) return item;
    item.protein = safeStof(part, 0.0f);
    
    // carbs
    if (!getline(ss, part, '|')) return item;
    item.carbs = safeStof(part, 0.0f);
    
    // fat
    if (!getline(ss, part, '|')) return item;
    item.fat = safeStof(part, 0.0f);
    
    // serving size (last token may include 'g' or '100g' etc.)
    if (!getline(ss, part)) {
        part = ""; // nothing
    }
    // extract digits+dot
    string num;
    for (char c : part) if ((c >= '0' && c <= '9') || c == '.') num.push_back(c);
    if (!num.empty()) item.servingSize = safeStof(num, 0.0f);
    else item.servingSize = 0.0f;
    
    return item;
}

void searchInFile(const string& filename, const string& searchWord, vector<FoodItem>& results) {
    ifstream file(filename.c_str());
    if (!file) return;
    
    string line;
    while (getline(file, line)) {
        string t = trim(line);
        if (t.empty()) continue;
        FoodItem item = parseFoodLine(line);
        string lw = item.name;
        for (char &c : lw) c = tolower((unsigned char)c);
        string sw = searchWord;
        for (char &c : sw) c = tolower((unsigned char)c);
        if (lw.find(sw) != string::npos) {
            results.push_back(item);
        }
    }
    file.close();
}

/* -------------------- Daily log writer -------------------- */

void saveToDailyLog(const string& filename, const DailyLog& entry) {
    // Read current file lines
    vector<string> lines;
    ifstream inFile(filename);
    string line;
    while (getline(inFile, line)) lines.push_back(line);
    inFile.close();

    string today = getTodayDate();

    // Find today's DAILY_LOG section
    bool inDailyLog = false;
    int currentStart = -1;
    int todayStart = -1, todayEnd = -1;
    for (int i = 0; i < (int)lines.size(); ++i) {
        string t = trim(lines[i]);
        if (t == "DAILY_LOG") { inDailyLog = true; currentStart = i; }
        else if (inDailyLog && t.rfind("Date:", 0) == 0) {
            string fileDate = trim(t.substr(5));
            if (fileDate == today) {
                todayStart = currentStart;
                // find corresponding END_DAILY_LOG
                for (int j = i + 1; j < (int)lines.size(); ++j) {
                    if (trim(lines[j]) == "END_DAILY_LOG") { todayEnd = j; break; }
                }
                break;
            } else {
                inDailyLog = false;
            }
        }
    }

    // Build new two-line entry (Type: ... and Food | ...)
    vector<string> newEntries;
    newEntries.push_back("Type: " + entry.mealType);

    stringstream fs;
    fs << fixed << setprecision(1);
    fs << entry.foodName << " | " << entry.calories << " | "
       << entry.protein << " | " << entry.carbs << " | "
       << entry.fat << " | " << entry.grams;
    newEntries.push_back(fs.str());

    if (todayEnd != -1) {
        // insert before END_DAILY_LOG
        lines.insert(lines.begin() + todayEnd, newEntries.begin(), newEntries.end());
    } else {
        if (!lines.empty()) lines.push_back("");
        lines.push_back("DAILY_LOG");
        lines.push_back("Date: " + today);
        lines.insert(lines.end(), newEntries.begin(), newEntries.end());
        lines.push_back("END_DAILY_LOG");
    }

    ofstream out(filename.c_str());
    for (const string& l : lines) out << l << endl;
    out.close();
}

/* -------------------- Add Custom Food -------------------- */

void runAddCustomFood(const UserProfile& p) {
    printHeader("ADD CUSTOM FOOD");
    FoodItem newFood;
    
    string input;
    cout << "Enter food name (or 'b' to go back): ";
    getline(cin, input);
    input = trim(input);
    if (checkForBack(input)) return;
    newFood.name = input;
    
    cout << "Serving size in grams (usually 100): ";
    getline(cin, input);
    input = trim(input);
    if (checkForBack(input)) return;
    newFood.servingSize = safeStof(input, 100.0f);
    
    cout << "Calories per " << newFood.servingSize << "g: ";
    getline(cin, input);
    input = trim(input);
    if (checkForBack(input)) return;
    newFood.calories = safeStof(input, 0.0f);
    
    cout << "Protein (g): ";
    getline(cin, input);
    input = trim(input);
    if (checkForBack(input)) return;
    newFood.protein = safeStof(input, 0.0f);
    
    cout << "Carbs (g): ";
    getline(cin, input);
    input = trim(input);
    if (checkForBack(input)) return;
    newFood.carbs = safeStof(input, 0.0f);
    
    cout << "Fat (g): ";
    getline(cin, input);
    input = trim(input);
    if (checkForBack(input)) return;
    newFood.fat = safeStof(input, 0.0f);
    
    string customFile = "user_" + p.username + "_custom_foods.txt";
    ofstream file(customFile, ios::app);
    
    if (file) {
        file << newFood.name << " | " << newFood.calories << " | " 
             << newFood.protein << " | " << newFood.carbs << " | " 
             << newFood.fat << " | " << newFood.servingSize << endl;
        file.close();
        cout << "\n✓ Custom food saved to " << customFile << "!" << endl;
    } else {
        cout << "Error saving custom food!" << endl;
    }
    
    cout << "\nAdd this food to a meal now?\n1. Yes\n2. No\nChoice: ";
    string choice;
    getline(cin, choice);
    choice = trim(choice);
    if (choice == "1") {
        // re-parse profile and call add food
        UserProfile up = parseUserProfile(p.username);
        // call runAddFood below
        // avoid forward reference by calling via function pointer? we have direct call below
        // but compile-time it's fine (function declared earlier)
    }
}

/* -------------------- Add Food (fixed) -------------------- */

void runAddFood(const UserProfile& p) {
    system("cls");
    string dailyLogFile = "user_" + p.username + "_data.txt";
    
    printHeader("ADD FOOD TO MEAL");
    cout << "Select meal type:\n"
         << "1. Breakfast\n"
         << "2. Lunch\n"
         << "3. Dinner\n"
         << "4. Snack\n"
         << "Choice (or 'b' to go back): ";

    string choice;
    getline(cin, choice);
    choice = trim(choice);
    if (checkForBack(choice)) return;

    int mealChoice = safeStoi(choice, -1);
    if (mealChoice < 1 || mealChoice > 4) { cout << "Invalid meal choice.\n"; return; }

    string mealType;
    switch (mealChoice) {
        case 1: mealType = "Breakfast"; break;
        case 2: mealType = "Lunch"; break;
        case 3: mealType = "Dinner"; break;
        case 4: mealType = "Snack"; break;
        default: mealType = "Unknown"; break;
    }

    cout << "\nEnter food name to search (or 'b' to go back): ";
    string searchFood;
    getline(cin, searchFood);
    searchFood = trim(searchFood);
    if (checkForBack(searchFood)) return;
    if (searchFood.empty()) { cout << "No search term provided.\n"; return; }

    vector<FoodItem> allResults;
    searchInFile("foods.txt", searchFood, allResults);
    searchInFile("user_" + p.username + "_custom_foods.txt", searchFood, allResults);

    if (allResults.empty()) {
        cout << "\nNo matches found!\n1. Add as custom food first\n2. Search again\nChoice: ";
        string opt; getline(cin, opt); opt = trim(opt);
        if (opt == "1") { runAddCustomFood(p); }
        return;
    }

    cout << "\nFound " << allResults.size() << " match(es):\n";
    for (size_t i = 0; i < allResults.size(); ++i) {
        cout << i+1 << ". " << allResults[i].name 
             << " (" << allResults[i].calories << " cal per " 
             << (allResults[i].servingSize > 0 ? to_string((int)allResults[i].servingSize) + "g" : "serving") << ")\n";
    }
    cout << "0. None of the above / Back\n";

    cout << "\nSelect food (0-" << allResults.size() << "): ";
    string sel; getline(cin, sel); sel = trim(sel);
    if (checkForBack(sel) || sel == "0") return;
    int foodChoice = safeStoi(sel, -1);
    if (foodChoice < 1 || foodChoice > (int)allResults.size()) { cout << "Invalid selection.\n"; return; }

    FoodItem selected = allResults[foodChoice - 1];
    cout << "\nSelected: " << selected.name << "\n";

    // Ask grams consumed; guard for servingSize==0
    float grams = 0.0f;
    while (true) {
        cout << "How many grams did you consume? (or 'b' to go back): ";
        string gramsStr; getline(cin, gramsStr); gramsStr = trim(gramsStr);
        if (checkForBack(gramsStr)) return;
        if (gramsStr.empty()) { cout << "Please enter grams or 'b' to cancel.\n"; continue; }
        float g = safeStof(gramsStr, -1.0f);
        if (g <= 0) { cout << "Enter a positive number for grams.\n"; continue; }
        grams = g; break;
    }

    float ratio = 1.0f;
    if (selected.servingSize > 0.0f) ratio = grams / selected.servingSize;
    else {
        ratio = 1.0f;
        cout << "Warning: serving size unknown for this item -> assuming the cal/protein values correspond to the grams you entered.\n";
    }

    float adjustedCal = selected.calories * ratio;
    float adjustedProt = selected.protein * ratio;
    float adjustedCarbs = selected.carbs * ratio;
    float adjustedFat = selected.fat * ratio;

    DailyLog entry;
    entry.mealType = mealType;
    entry.foodName = selected.name;
    entry.grams = grams;
    entry.calories = adjustedCal;
    entry.protein = adjustedProt;
    entry.carbs = adjustedCarbs;
    entry.fat = adjustedFat;

    saveToDailyLog(dailyLogFile, entry);

    cout << "\n✓ Added to " << mealType << "!\n";
    cout << "  " << grams << "g of " << selected.name 
         << " (" << adjustedCal << " calories)\n";
    cout << "  Nutrition: " << adjustedProt << "g protein, " 
         << adjustedCarbs << "g carbs, " << adjustedFat << "g fat\n";
}

void runViewConsumption(const UserProfile& up){
    printHeader("TODAY'S CONSUMPTION");
                string logfile = "user_" + up.username + "_data.txt";
                ifstream f(logfile);
                if (!f) {
                    cout << "No data found (file missing).\n";
                } else {
                    string l;
                    while (getline(f, l)) {
                        string t = trim(l);
                        if (t.find("DAILY_LOG") != string::npos ||
                            t.find("Date:") != string::npos ||
                            t.find("Type:") != string::npos ||
                            t.find("|") != string::npos ||
                            t.find("END_DAILY_LOG") != string::npos) {
                            cout << l << "\n";
                        }
                    }
                    f.close();
                }
                pauseConsole();
}