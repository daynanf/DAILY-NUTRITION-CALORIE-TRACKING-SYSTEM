#include "profile.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

using namespace std;

// BMR Calculation (Mifflin-St Jeor)
double calcBMR(const UserProfile& p) {
    if (p.gender == "Male") {
        return 10.0 * p.weight_kg + 6.25 * p.height_cm - 5.0 * p.age + 5.0;
    } else {
        return 10.0 * p.weight_kg + 6.25 * p.height_cm - 5.0 * p.age - 161.0;
    }
}

double activityMultiplier(const string& level) {
    if (level == "Sedentary") return 1.2;
    if (level == "Lightly Active") return 1.375;
    if (level == "Moderately Active") return 1.55;
    if (level == "Very Active") return 1.725;
    return 1.2;
}

void calcMacroTargets(UserProfile& p) {
    double bmr = calcBMR(p);
    double tdee = bmr * activityMultiplier(p.activityLevel);

    p.dailyCaloriesTarget = round(tdee);
    p.targetProtein_g = round(p.weight_kg * 2.0); // 2g per kg
    p.targetCarbs_g = round((p.dailyCaloriesTarget * 0.55) / 4.0);
    p.targetFat_g = round((p.dailyCaloriesTarget * 0.25) / 9.0);
}

void saveUserProfile(const UserProfile& p) {
    string filename = "user_" + p.username + "_data.txt";
    ofstream file(filename); // Overwrite mode for profile block at start (simplified)
    
    if (!file) {
        cerr << "Error saving profile to " << filename << endl;
        return;
    }

    file << "USER_PROFILE\n";
    file << "Name: " << p.fullName << "\n";
    file << "Age: " << p.age << "\n";
    file << "Gender: " << p.gender << "\n";
    file << "Height: " << p.height_cm << "\n";
    file << "Weight: " << p.weight_kg << "\n";
    file << "Activity_Level: " << p.activityLevel << "\n";
    file << "Daily_Calorie_Target: " << p.dailyCaloriesTarget << "\n";
    file << "\n"; // End of profile block

    // Note: In a real append scenario, we'd need to be careful not to lose Daily Logs.
    // user_data file structure: Profile Block + Daily Logs.
    // For this implementation, if we are just creating/updating profile, we assume we might be rewriting or we handle it carefully.
    // Since the original code had distinct create/update, let's just use this for Creation.
    // Ideally, for update, we would read the whole file and rewrite the top. 
    // But for now, let's assume this is mostly for creation.
    
    file.close();
}

bool loadUserProfile(const string& username, UserProfile& p) {
    string filename = "user_" + username + "_data.txt";
    ifstream file(filename);
    
    if (!file) return false;

    p.username = username;
    string line;
    bool inProfile = false;

    while (getline(file, line)) {
        if (line == "USER_PROFILE") {
            inProfile = true;
            continue;
        }
        if (inProfile && line.empty()) break; // End of profile block
        if (!inProfile) continue;

        size_t colon = line.find(": ");
        if (colon == string::npos) continue;

        string key = line.substr(0, colon);
        string val = line.substr(colon + 2);

        if (key == "Name") p.fullName = val;
        else if (key == "Age") p.age = stoi(val);
        else if (key == "Gender") p.gender = val;
        else if (key == "Height") p.height_cm = stoi(val);
        else if (key == "Weight") p.weight_kg = stod(val);
        else if (key == "Activity_Level") p.activityLevel = val;
        else if (key == "Daily_Calorie_Target") p.dailyCaloriesTarget = stod(val);
    }
    
    calcMacroTargets(p); // Recalculate based on loaded data to be safe
    return true;
}

void displayNutritionTargets(const UserProfile& p) {
    cout << "\n------------------ DAILY TARGETS ------------------\n";
    cout << "Calories : " << p.dailyCaloriesTarget << " kcal\n";
    cout << "Protein  : " << p.targetProtein_g << " g\n";
    cout << "Carbs    : " << p.targetCarbs_g << " g\n";
    cout << "Fat      : " << p.targetFat_g << " g\n"; 
}

void displayProfile(const UserProfile& p) {
    cout << "\n------------------ MY PROFILE ------------------\n";
    cout << "Name      : " << p.fullName << "\n";
    cout << "Age       : " << p.age << "\n";
    cout << "Gender    : " << p.gender << "\n";
    cout << "Height    : " << p.height_cm << " cm\n";
    cout << "Weight    : " << p.weight_kg << " kg\n";
    cout << "Activity  : " << p.activityLevel << "\n";
}