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
// Purpose: Serializes the core profile fields into the provided stream
// in a simple key:value text format used by the app's per-user data files.
// Note: This writes only the USER_PROFILE block; weight history is written
// separately elsewhere. Keep format stable because other functions parse it.
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

// Calculate BMR (Basal Metabolic Rate)
// Purpose: Compute basal metabolic rate using the Mifflin-St Jeor
// formula. The gender check normalizes to Male vs Female variants.
// Returns BMR in kcal/day.
double calcBMR(const UserProfile& p) {
    if (p.gender == "Male" || p.gender == "male") {
        return 10.0 * p.weight_kg + 6.25 * p.height_cm - 5.0 * p.age + 5.0;
    } else {
        return 10.0 * p.weight_kg + 6.25 * p.height_cm - 5.0 * p.age - 161.0;
    }
}

// Activity multiplier lookup
// Purpose: Return a multiplier to convert BMR -> TDEE (total daily energy
// expenditure) based on the user's reported activity level string.
// The implementation performs simple substring checks to tolerate
// slightly different level strings.
double activityMultiplier(const std::string& level) {
    std::string l = level;
    if (l.find("Sedentary") != std::string::npos) return 1.2;
    if (l.find("Lightly") != std::string::npos) return 1.375;
    if (l.find("Moderately") != std::string::npos) return 1.55;
    if (l.find("Very") != std::string::npos) return 1.725;
    return 1.2;
}

// Calculate macro and calorie targets
// Purpose: Given a filled profile, compute daily calorie target and
// macronutrient goals (protein, carbs, fat) and store them back in `p`.
// Notes: Uses simple rules: protein = 2g/kg, carbs = 55% calories, fat = 25%.
void calcMacroTargets(UserProfile& p) {
    double bmr = calcBMR(p);
    double tdee = bmr * activityMultiplier(p.activityLevel);
    p.dailyCaloriesTarget = std::round(tdee);
    p.targetProtein_g = std::round(p.weight_kg * 2.0);
    p.targetCarbs_g = std::round((p.dailyCaloriesTarget * 0.55) / 4.0);
    p.targetFat_g = std::round((p.dailyCaloriesTarget * 0.25) / 9.0);
}

// Display daily nutrition targets
// Purpose: Nicely prints calories and macronutrient targets to the console
// and then pauses. Formatting uses fixed column widths for readability.
void displayNutritionTargets(const UserProfile& p) {
    printHeader("DAILY NUTRITION TARGETS");
    std::cout << left << setw(18) << "Calories:" << right << setw(6) << static_cast<int>(p.dailyCaloriesTarget) << " kcal\n";
    std::cout << left << setw(18) << "Protein:" << right << setw(6) << static_cast<int>(p.targetProtein_g) << " g\n";
    std::cout << left << setw(18) << "Carbohydrates:" << right << setw(6) << static_cast<int>(p.targetCarbs_g) << " g\n";
    std::cout << left << setw(18) << "Fat:" << right << setw(6) << static_cast<int>(p.targetFat_g) << " g\n\n";
    pauseConsole();
}

// Display full profile (user-facing)
// Purpose: Prints the loaded profile fields (username, name, age, gender,
// height, weight, activity level). If no profile is loaded, tells the user
// and returns. Lightweight formatting for console display.
void displayProfile(const UserProfile& p) {
    if (p.fullName.empty()) {
        std::cout << "No profile loaded yet.\n";
        pauseConsole();
        return;
    }

    printHeader("MY PROFILE");
    const int labelWidth = 18;
    const int valueStartCol = 2; // single space after label
    std::cout << left << setw(labelWidth) << "Username:" << string(valueStartCol, ' ') << p.username << "\n";
    std::cout << left << setw(labelWidth) << "Full Name:" << string(valueStartCol, ' ') << p.fullName << "\n";
    std::cout << left << setw(labelWidth) << "Age:" << string(valueStartCol, ' ') << p.age << " years\n";
    std::cout << left << setw(labelWidth) << "Gender:" << string(valueStartCol, ' ') << p.gender << "\n";
    std::cout << left << setw(labelWidth) << "Height:" << string(valueStartCol, ' ') << p.height_cm << " cm\n";
    std::cout << left << setw(labelWidth) << "Weight:" << string(valueStartCol, ' ') << p.weight_kg << " kg\n";
    std::cout << left << setw(labelWidth) << "Activity Level:" << string(valueStartCol, ' ') << p.activityLevel << "\n\n";

    pauseConsole();
}

// Create per-user data file and write profile
// Purpose: Creates (or overwrites) the per-user data file and writes the
// USER_PROFILE block using `writeProfileBlock`. This is called during
// registration to initialize a user's data file.
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

// Load profile from per-user file
// Purpose: Reads the USER_PROFILE block from the user's data file and
// populates the `UserProfile` struct fields. Also infers `username` from
// the filename when possible and recalculates macro targets after loading.
// Notes/complexity: Parsing is line-oriented with 'Key: Value' pairs and
// relies on stoi/stod for numeric fields; malformed lines are skipped.
void loadProfileFromFile(const std::string& filename, UserProfile& p) {
    std::ifstream fin(filename);
    if (!fin) {
        std::cerr << "Error opening file: " << filename << "\n";
        pauseConsole();
        return;
    }

    // Infer username from filename pattern "user_<username>_data.txt"
    // If filename matches, set p.username so other modules have it available.
    size_t ustart = filename.find("user_");
    size_t uend = filename.find("_data");
    if (ustart != std::string::npos && uend != std::string::npos && uend > ustart + 5) {
        p.username = filename.substr(ustart + 5, uend - (ustart + 5));
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

// Update weight & save – interactive
// Purpose: Prompt the user to enter a new weight (or 'b' to cancel),
// update the `UserProfile`, recalculate targets, and persist the change.
// Implementation details (non-trivial):
// - Uses std::getline to accept free-form input and allows words like
//   'b', 'back', or 'cancel' to abort the update.
// - After validation, it rewrites the USER_PROFILE block at the start of
//   the file (seeking to the beginning) and appends a WEIGHT_UPDATE block
//   with a timestamp. This approach is simple but assumes the file format
//   is tolerant to overwriting from the start; concurrent access or
//   more complex file layouts would require a safer rewrite strategy.
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

    file.close();
    std::cout << "Profile updated and weight history recorded.\n";
    pauseConsole();
}