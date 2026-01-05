#include "profile.h"
#include "utils.h"      // getValidInt, getValidDouble, pauseConsole
#include "models.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <cctype>       // tolower

// Helper: write only the USER_PROFILE block
static void writeProfileBlock(std::ostream& out, const UserProfile& p) {
    out << "USER_PROFILE\n";
    out << "Name: " << p.fullName << "\n";
    out << "Age: " << p.age << "\n";
    out << "Gender: " << p.gender << "\n";
    out << "Height: " << p.height_cm << "\n";
    out << "Weight: " << p.weight_kg << "\n";
    out << "Activity_Level: " << p.activityLevel << "\n";
    out << "Daily_Calorie_Target: " << static_cast<int>(p.dailyCaloriesTarget) << "\n";
    out << "\n";
}

// Calculate targets (unchanged)
double calcBMR(const UserProfile& p) {
    if (p.gender == "Male" || p.gender == "male") {
        return 10.0 * p.weight_kg + 6.25 * p.height_cm - 5.0 * p.age + 5.0;
    } else {
        return 10.0 * p.weight_kg + 6.25 * p.height_cm - 5.0 * p.age - 161.0;
    }
}

double activityMultiplier(const std::string& level) {
    std::string l = level;
    if (l.find("Sedentary") != std::string::npos) return 1.2;
    if (l.find("Lightly") != std::string::npos) return 1.375;
    if (l.find("Moderately") != std::string::npos) return 1.55;
    if (l.find("Very") != std::string::npos) return 1.725;
    return 1.2;
}

void calcMacroTargets(UserProfile& p) {
    double bmr = calcBMR(p);
    double tdee = bmr * activityMultiplier(p.activityLevel);
    p.dailyCaloriesTarget = std::round(tdee);
    p.targetProtein_g = std::round(p.weight_kg * 2.0);
    p.targetCarbs_g = std::round((p.dailyCaloriesTarget * 0.55) / 4.0);
    p.targetFat_g = std::round((p.dailyCaloriesTarget * 0.25) / 9.0);
}

//  Display targets
void displayNutritionTargets(const UserProfile& p) {
    std::cout << "\nYour Daily Nutrition Targets:\n";
    std::cout << "Calories:     " << static_cast<int>(p.dailyCaloriesTarget) << " kcal\n";
    std::cout << "Protein:      " << static_cast<int>(p.targetProtein_g) << " g\n";
    std::cout << "Carbohydrates:" << static_cast<int>(p.targetCarbs_g) << " g\n";
    std::cout << "Fat:          " << static_cast<int>(p.targetFat_g) << " g\n\n";
    pauseConsole();
}

//  Display full profile (menu case '1')
void displayProfile(const UserProfile& p) {
    if (p.fullName.empty()) {
        std::cout << "No profile loaded yet.\n";
        pauseConsole();
        return;
    }

    std::cout << "\n--- My Profile ---\n";
    std::cout << "Username      : " << p.username << "\n";
    std::cout << "Full Name     : " << p.fullName << "\n";
    std::cout << "Age           : " << p.age << " years\n";
    std::cout << "Gender        : " << p.gender << "\n";
    std::cout << "Height        : " << p.height_cm << " cm\n";
    std::cout << "Weight        : " << p.weight_kg << " kg\n";
    std::cout << "Activity Level: " << p.activityLevel << "\n\n";

    pauseConsole();
}

//  Create per-user file & write profile
void createUserDataFile(const std::string& filename, const UserProfile& p) {
    std::ofstream fout(filename);
    if (!fout) {
        std::cerr << "Error creating file: " << filename << "\n";
        pauseConsole();
        return;
    }

    writeProfileBlock(fout, p);
    fout.close();
    std::cout << "Profile saved to " << filename << "\n";
    pauseConsole();
}

//  Load profile from file
void loadProfileFromFile(const std::string& filename, UserProfile& p) {
    std::ifstream fin(filename);
    if (!fin) {
        std::cerr << "Error opening file: " << filename << "\n";
        pauseConsole();
        return;
    }

    std::string line;
    bool inProfile = false;

    while (std::getline(fin, line)) {
        if (line == "USER_PROFILE") {
            inProfile = true;
            continue;
        }
        if (inProfile && line.empty()) break;

        size_t colon = line.find(": ");
        if (colon == std::string::npos) continue;

        std::string key = line.substr(0, colon);
        std::string val = line.substr(colon + 2);

        if (key == "Name") p.fullName = val;
        else if (key == "Age") p.age = std::stoi(val);
        else if (key == "Gender") p.gender = val;
        else if (key == "Height") p.height_cm = std::stod(val);
        else if (key == "Weight") p.weight_kg = std::stod(val);
        else if (key == "Activity_Level") p.activityLevel = val;
        else if (key == "Daily_Calorie_Target") p.dailyCaloriesTarget = std::stod(val);
    }

    fin.close();
    calcMacroTargets(p);
}

//  Update weight & save – back option added
void updateWeightAndTargets(const std::string& filename, UserProfile& p) {
    std::cout << "\n--- Update Weight ---\n";
    std::cout << "Current weight: " << p.weight_kg << " kg\n";
    std::cout << "Enter new weight (kg) or 'b' to back/cancel: ";

    std::string input;
    std::getline(std::cin, input);

    std::string lower = input;
    for (char& c : lower) c = std::tolower(c);

    if (lower == "b" || lower == "back" || lower == "cancel") {
        std::cout << "Update cancelled — returning to menu.\n";
        pauseConsole();
        return;
    }

    double newWeight;
    try {
        newWeight = std::stod(input);
    } catch (...) {
        std::cout << "Invalid input. Update cancelled.\n";
        pauseConsole();
        return;
    }

    if (newWeight <= 0) {
        std::cout << "Invalid weight. Update cancelled.\n";
        pauseConsole();
        return;
    }

    p.weight_kg = newWeight;
    calcMacroTargets(p);

    std::cout << "Weight updated to " << newWeight << " kg.\n";
    displayNutritionTargets(p);

    std::fstream file(filename, std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Error opening file: " << filename << "\n";
        pauseConsole();
        return;
    }

    file.seekp(0);
    writeProfileBlock(file, p);

    file.seekp(0, std::ios::end);
    time_t now = time(nullptr);
    tm* lt = localtime(&now);
    file << "WEIGHT_UPDATE\n";
    file << "Date: " << (1900 + lt->tm_year) << "-" << (1 + lt->tm_mon) << "-" << lt->tm_mday << "\n";
    file << "Weight: " << newWeight << "\n\n";

    file.close();
    std::cout << "Profile updated and weight history recorded.\n";
    pauseConsole();
}