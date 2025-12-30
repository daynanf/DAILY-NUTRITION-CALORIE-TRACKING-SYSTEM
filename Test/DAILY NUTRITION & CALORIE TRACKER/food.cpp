#include "food.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

// --- Helper: Get Current Date ---
string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-" 
       << 1 + ltm->tm_mon << "-" 
       << ltm->tm_mday;
    return ss.str();
}

// --- Helper: Parse Food Line ---
FoodItem parseFoodLine(string line) {
    FoodItem item;
    // Format: Name|Calories|Protein|Carbs|Fat|ServingSize
    // Note: Parsing pipe delimited
    stringstream ss(line);
    string segment;
    vector<string> seglist;
    
    while(getline(ss, segment, '|')) {
        seglist.push_back(segment);
    }
    
    if (seglist.size() >= 6) {
        item.name = trim(seglist[0]);
        // Handle potential "g" in serving size if read from file? 
        // Spec says "100g" in some places, checking parse logic.
        // Spec said: Apple|52|0.3...|182g
        try {
            item.calories = stod(seglist[1]);
            item.protein = stod(seglist[2]);
            item.carbs = stod(seglist[3]);
            item.fat = stod(seglist[4]);
            
            string sizeStr = seglist[5];
            // Remove 'g' if present
            size_t gPos = sizeStr.find('g');
            if (gPos != string::npos) sizeStr = sizeStr.substr(0, gPos);
            item.servingSize = stod(sizeStr);
        } catch (...) {
            // Error parsing, return empty or safe default
        }
    }
    return item;
}

// --- Data Access: Search ---
vector<FoodItem> searchFoodDatabase(const string& query, const string& username) {
    vector<FoodItem> results;
    vector<string> files = {"foods.txt", "user_" + username + "_custom_foods.txt"};
    
    for (const auto& fname : files) {
        ifstream file(fname);
        if (!file) continue;
        
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            // Check match first to avoid parsing everything
            // But we need name.
            size_t piper = line.find('|');
            string foodName = line.substr(0, piper);
            if (isSimilar(query, foodName)) {
                results.push_back(parseFoodLine(line));
            }
        }
    }
    return results;
}

// --- Data Access: Load Log ---
// Loads ALL logs, but we mostly care about today.
// For "Add Food", we will Append to the file.
// For "View Consumption", we will Read the file and aggregate.
DailyLog getTodayLog(const UserProfile& p) {
    DailyLog log;
    log.date = getCurrentDate();
    
    string filename = "user_" + p.username + "_data.txt";
    ifstream file(filename);
    if (!file) return log;
    
    string line;
    bool inDailyLog = false;
    string currentDateBlock = "";
    
    // Simple state machine parser
    while (getline(file, line)) {
        line = trim(line);
        if (line == "DAILY_LOG") {
            inDailyLog = true;
            continue;
        }
        if (line == "END_DAILY_LOG") {
            inDailyLog = false;
            currentDateBlock = "";
            continue;
        }
        
        if (inDailyLog) {
            if (line.find("Date:") == 0) {
                currentDateBlock = trim(line.substr(5));
            }
            if (currentDateBlock == log.date) {
                // This block belongs to today!
                // We look for food lines. Format in file:
                // "FoodName | Cal | Prot | Carb | Fat | Grams"
                // Wait, spec said:
                // Meals:
                //   Breakfast:
                //     Food: Oatmeal
                //     ...
                // That spec is VERY verbose.
                // However, the USER's code in DAILY LOG.cpp wrote: 
                // "Name | Cal | Prot | Carb | Fat | Grams"
                // I will Stick to the COMPACT line format within the DAILY_LOG block for simplicity 
                // and because parsing the verbose one is error prone.
                // I will assume lines with pipes are food entries.
                
                if (line.find('|') != string::npos) {
                    // It's a food line
                    // Name | Cal | Prot | Carb | Fat | GramsConsumed
                    // Note: Cal/Prot etc are already adjusted for the grams in the previous code?
                    // Previous code: "adjustedCal...". 
                    // Let's parse it.
                    stringstream ss(line);
                    string seg;
                    vector<string> parts;
                    while(getline(ss, seg, '|')) parts.push_back(trim(seg));
                    
                    if (parts.size() >= 6) {
                        FoodItem item;
                        item.name = parts[0];
                        double consumedGrams = stod(parts[5]);
                        double cal = stod(parts[1]);
                        double prot = stod(parts[2]);
                        double carb = stod(parts[3]);
                        double fat = stod(parts[4]);
                        
                        // Reconstruct item base stats if needed, or just store totals?
                        // For display, we just need totals.
                        // Storing "consumed" item
                        item.calories = cal;
                        item.protein = prot;
                        item.carbs = carb;
                        item.fat = fat;
                        item.servingSize = consumedGrams; // Store consumed amount here for display
                        
                        log.consumedMeals.push_back({item, consumedGrams});
                        log.totalCalories += cal;
                        log.totalProtein += prot;
                        log.totalCarbs += carb;
                        log.totalFat += fat;
                    }
                }
            }
        }
    }
    return log;
}

// --- Data Access: Append Log ---
void appendFoodToLog(const UserProfile& p, const FoodItem& food, double grams, const string& mealType) {
    string filename = "user_" + p.username + "_data.txt";
    ofstream file(filename, ios::app);
    
    // If file is empty (shouldn't be, profile is there), handle it.
    // We just append a block.
    // To avoid spamming DAILY_LOG headers, we could check if the last line was for today... 
    // but appending a new block is valid and safer for file integrity (no seek/overwrite risk).
    
    double ratio = grams / food.servingSize;
    double cal = food.calories * ratio;
    double prot = food.protein * ratio;
    double carb = food.carbs * ratio;
    double fat = food.fat * ratio;

    file << "DAILY_LOG\n";
    file << "Date: " << getCurrentDate() << "\n";
    file << "Type: " << mealType << "\n"; // Optional metadata
    // Format: Name | Cal | Prot | Carb | Fat | Grams
    file << food.name << " | " << cal << " | " << prot << " | " << carb << " | " << fat << " | " << grams << "\n";
    file << "END_DAILY_LOG\n\n";
    
    file.close();
}

// --- UI Functions ---

void runAddFood(const UserProfile& p) {
    printHeader("ADD FOOD TO LOG");
    
    string mealType;
    cout << "Select Meal Type:\n1. Breakfast\n2. Lunch\n3. Dinner\n4. Snack\nEnter choice: ";
    int m;
    getValidInt(m, 1, 4);
    if (m==1) mealType="Breakfast"; else if (m==2) mealType="Lunch"; else if (m==3) mealType="Dinner"; else mealType="Snack";

    cin.ignore(); // clear buffer
    cout << "\nSearch Food (type part of name): ";
    string query;
    getline(cin, query);
    
    vector<FoodItem> results = searchFoodDatabase(query, p.username);
    
    cout << "\nResults:\n";
    if (results.empty()) {
        cout << "No matches found.\n";
    } else {
        for (size_t i = 0; i < results.size(); ++i) {
            cout << (i+1) << ". " << results[i].name 
                 << " (" << results[i].calories << " kcal/" << results[i].servingSize << "g)\n";
        }
    }
    cout << (results.size() + 1) << ". None of the above / Custom Food\n";
    
    int choice;
    cout << "\nSelect food number: ";
    getValidInt(choice, 1, results.size() + 1);
    
    if (choice == results.size() + 1) {
        // Go to custom food flow
        runAddCustomFood(p); 
        return; 
    }
    
    FoodItem selected = results[choice - 1];
    cout << "You selected: " << selected.name << "\n";
    
    double grams;
    cout << "Enter quantity (g): ";
    getValidDouble(grams, 1, 5000);
    
    appendFoodToLog(p, selected, grams, mealType);
    
    cout << "\nFood added to " << mealType << "!\n";
    pauseConsole();
}

void runViewConsumption(const UserProfile& p) {
    DailyLog log = getTodayLog(p);
    
    printHeader("TODAY'S CONSUMPTION", log.date);
    
    if (log.consumedMeals.empty()) {
        cout << "No meals logged today yet.\n";
        pauseConsole();
        return;
    }
    
    cout << "---------------------------------------------------------------------------------\n";
    printf("%-30s | %-10s | %-10s | %-10s | %-10s\n", "Food", "Calories", "Protein", "Carbs", "Fat");
    cout << "---------------------------------------------------------------------------------\n";
    
    for (auto& pair : log.consumedMeals) {
        FoodItem& item = pair.first;
        // item values are already total for the meal in the parser
        printf("%-30s | %-10.1f | %-10.1f | %-10.1f | %-10.1f\n", 
               item.name.substr(0,29).c_str(), item.calories, item.protein, item.carbs, item.fat);
    }
    cout << "---------------------------------------------------------------------------------\n";
    printf("%-30s | %-10.1f | %-10.1f | %-10.1f | %-10.1f\n", 
           "TOTALS", log.totalCalories, log.totalProtein, log.totalCarbs, log.totalFat);
    cout << "---------------------------------------------------------------------------------\n";
    
    // Remaining
    cout << "\nREMAINING TARGETS:\n";
    cout << "Calories: " << (p.dailyCaloriesTarget - log.totalCalories) << "\n";
    cout << "Protein:  " << (p.targetProtein_g - log.totalProtein) << "\n";
    cout << "Carbs:    " << (p.targetCarbs_g - log.totalCarbs) << "\n";
    cout << "Fat:      " << (p.targetFat_g - log.totalFat) << "\n";
    
    pauseConsole();
}

void runAddCustomFood(const UserProfile& p) {
    printHeader("ADD CUSTOM FOOD");
    FoodItem item;
    cout << "Enter Food Name: ";
    getline(cin, item.name);
    
    // Simple manual entry for now
    cout << "Enter Serving Size (g): ";
    getValidDouble(item.servingSize, 1, 1000);
    cout << "Enter Calories: ";
    getValidDouble(item.calories, 0, 10000);
    cout << "Enter Protein (g): ";
    getValidDouble(item.protein, 0, 1000);
    cout << "Enter Carbs (g): ";
    getValidDouble(item.carbs, 0, 1000);
    cout << "Enter Fat (g): ";
    getValidDouble(item.fat, 0, 1000);
    
    // Save
    string filename = "user_" + p.username + "_custom_foods.txt";
    ofstream file(filename, ios::app);
    // Format: Name|Calories|Protein|Carbs|Fat|ServingSize
    file << item.name << "|" << item.calories << "|" << item.protein << "|" 
         << item.carbs << "|" << item.fat << "|" << item.servingSize << "g\n";
    file.close();
    
    cout << "Custom food saved successfully!\n";
    
    cout << "Add to today's log now? (1. Yes / 2. No): ";
    int c;
    getValidInt(c, 1, 2);
    if (c == 1) {
        cout << "Enter quantity (g): ";
        double g;
        getValidDouble(g, 1, 5000);
        appendFoodToLog(p, item, g, "Custom");
        cout << "Added to log.\n";
    }
    pauseConsole();
}
