#include "recommendations.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

// --- Helper Structures ---

struct RecFoodItem {
    string name;
    double calories;
    double protein;
    double carbs;
    double fat;
    string servingUnitStr; 
    double servingSizeVal; // numeric value of serving size
};

struct NutrientStatus {
    double calories = 0;
    double protein = 0;
    double carbs = 0;
    double fat = 0;
};

struct Recommendation {
    RecFoodItem food;
    double recommendedAmount;
    double nutrientContributed;
};

// --- Helper Functions ---

// Get current date string YYYY-MM-DD
string getCurrentDateRecs() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return string(buf);
}

// Parse serving size string like "100g", "200ml", "1pc" => {value: 100, unit: "g"}
pair<double, string> parseServingSize(string s) {
    string numPart = "";
    string unitPart = "";
    bool readingNum = true;
    for (char c : s) {
        if (readingNum && (isdigit(c) || c == '.')) {
            numPart += c;
        } else {
            readingNum = false;
            unitPart += c;
        }
    }
    double val = 1.0;
    if (!numPart.empty()) {
        try {
            val = stod(numPart);
        } catch (...) {
            val = 1.0;
        }
    }
    // Clean up unit part (remove spaces)
    unitPart = trim(unitPart);
    if (unitPart.empty()) unitPart = "unit";
    return {val, unitPart};
}

// Load foods from foods.txt
vector<RecFoodItem> loadRecFoodDatabase() {
    vector<RecFoodItem> foods;
    ifstream file("foods.txt");
    if (!file.is_open()) {
        cout << "Error opening foods.txt" << endl;
        return foods;
    }

    string line;
    // skip header
    if(getline(file, line)) {
        if(line.size() > 0 && tolower(line[0]) == 'f' && line.find('|') != string::npos) {
            // it's header
        }
    }

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string segment;
        vector<string> parts;
        while (getline(ss, segment, '|')) {
            parts.push_back(trim(segment));
        }

        if (parts.size() >= 6) {
            RecFoodItem item;
            item.name = parts[0];
            try {
                item.calories = stod(parts[1]);
                item.protein = stod(parts[2]);
                item.carbs = stod(parts[3]);
                item.fat = stod(parts[4]);
                
                pair<double, string> serving = parseServingSize(parts[5]);
                item.servingSizeVal = serving.first;
                item.servingUnitStr = serving.second;
                
                foods.push_back(item);
            } catch (...) {
                continue;
            }
        }
    }
    file.close();
    return foods;
}

// Load user's consumed totals for today
NutrientStatus getConsumedToday(const string& username) {
    NutrientStatus consumed;
    string filename = "user_" + username + "_data.txt";
    ifstream file(filename);
    if (!file.is_open()) return consumed;

    string line;
    string today = getCurrentDateRecs();
    bool isInTodayLog = false;

    // Scan the file
    while (getline(file, line)) {
        line = trim(line);
        if (line == "DAILY_LOG") {
            isInTodayLog = false; // reset until date confirmed
            continue;
        }
        if (line.rfind("Date:", 0) == 0) { // starts with Date:
            string dateStr = trim(line.substr(5));
            if (dateStr == today) {
                isInTodayLog = true;
            }
            continue;
        }
        if (line == "END_DAILY_LOG") {
            isInTodayLog = false;
            continue;
        }

        if (isInTodayLog && !line.empty()) {
            if (line.find('|') != string::npos && line.find("Type:") == string::npos) {
                vector<string> parts;
                stringstream ss(line);
                string segment;
                while (getline(ss, segment, '|')) {
                    parts.push_back(trim(segment));
                }
                if (parts.size() >= 5) { 
                    try {
                        consumed.calories += stod(parts[1]);
                        consumed.protein += stod(parts[2]);
                        consumed.carbs += stod(parts[3]);
                        consumed.fat += stod(parts[4]);
                    } catch (...) {}
                }
            }
        }
    }
    file.close();
    return consumed;
}

// Helper to save selected recommendation
void autoAddFood(const string& username, const RecFoodItem& food, double amount) {
    string filename = "user_" + username + "_data.txt";
    string today = getCurrentDateRecs();
    
    ofstream outfile(filename, ios::app);
    if (outfile.is_open()) {
        outfile << "\nDAILY_LOG\n";
        outfile << "Date: " << today << "\n";
        outfile << "Type: Auto-Added Recommendation\n";
        
        // Calculate nutrients for the specific amount
        double ratio = amount / food.servingSizeVal;
        
        double cals = food.calories * ratio;
        double prot = food.protein * ratio;
        double carb = food.carbs * ratio;
        double fat = food.fat * ratio;
        
        outfile << food.name << " | " 
                << fixed << setprecision(1) << cals << " | "
                << prot << " | "
                << carb << " | "
                << fat << " | "
                << amount << food.servingUnitStr << "\n";
        
        outfile << "END_DAILY_LOG\n";
        outfile.close();
        cout << "\n✅ Food auto-added to today's log!\n";
    } else {
        cout << "\n❌ Error writing to log file.\n";
    }
}

// Helper to print summary line (replaces lambda)
void printSummaryLine(string label, double target, double actual, string unit) {
    cout << left << setw(16) << label; 
    
    string tStr = to_string((int)target) + unit;
    string aStr = to_string((int)actual) + unit;
    
    cout << left << setw(11) << tStr;
    cout << left << setw(12) << aStr;
    
    string status;
    if (actual > target) status = "Over";
    else status = "Under";
    
    cout << status << endl;
}


// --- Main Functions ---

void runMealRecommendations(const UserProfile& p) {
    if (p.username.empty()) {
        cout << "Error: User profile not loaded correctly (missing username).\n";
    }
    
    while(true) {
        printHeader("REAL-TIME MEAL RECOMMENDATIONS");

        // 1. Calculate Status
        NutrientStatus consumed = getConsumedToday(p.username);
        
        double remCal = p.dailyCaloriesTarget - consumed.calories;
        double remProt = p.targetProtein_g - consumed.protein;
        double remCarb = p.targetCarbs_g - consumed.carbs;
        double remFat = p.targetFat_g - consumed.fat;

        cout << left << setw(20) << "Remaining Today:";
        cout << "\n";
        cout << left << setw(12) << "Calories:" << right << setw(6) << (int)max(0.0, remCal) << " kcal\n";
        cout << left << setw(12) << "Protein:" << right << setw(6) << (int)max(0.0, remProt) << " g\n";
        cout << left << setw(12) << "Carbs:" << right << setw(6) << (int)max(0.0, remCarb) << " g\n";
        cout << left << setw(12) << "Fat:" << right << setw(6) << (int)max(0.0, remFat) << " g\n\n";

        cout << "Choose a recommendation:\n";
        cout << "A) Protein-focused meal\n";
        cout << "B) Carb-focused meal\n";
        cout << "C) Fat-focused meal\n";
        cout << "D) Back to Dashboard\n";
        cout << "\nYour choice: ";

        char choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        choice = toupper(choice);

        if (choice == 'D') return;

        if (choice != 'A' && choice != 'B' && choice != 'C') {
            cout << "Invalid choice.\n";
            pauseConsole();
            continue;
        }

        // 2. Load Foods
        vector<RecFoodItem> allFoods = loadRecFoodDatabase();
        vector<RecFoodItem> filteredFoods;

        // 3. Filter/Categorize
        for (size_t i = 0; i < allFoods.size(); i++) {
            RecFoodItem f = allFoods[i];
            double norm = (f.servingSizeVal > 0) ? (100.0 / f.servingSizeVal) : 1.0;
            double p100 = f.protein * norm;
            double c100 = f.carbs * norm;
            double f100 = f.fat * norm;

            bool keep = false;
            // Criteria from prompt: "High Protein foods (protein >= 15g / 100g)"
            if (choice == 'A') { 
                if (p100 >= 15.0) keep = true; 
            } else if (choice == 'B') { 
                 if (c100 >= 20.0) keep = true;
            } else if (choice == 'C') { 
                 if (f100 >= 10.0) keep = true;
            }
            if (keep) filteredFoods.push_back(f);
        }

        if (filteredFoods.empty()) filteredFoods = allFoods;

        // Sort by amount per serving (descending) using simple Bubble Sort
        // (Avoiding lambdas for beginner friendliness)
        for (size_t i = 0; i < filteredFoods.size(); ++i) {
            for (size_t j = 0; j < filteredFoods.size() - 1; ++j) {
                bool swapNeeded = false;
                if (choice == 'A') {
                    if (filteredFoods[j].protein < filteredFoods[j+1].protein) swapNeeded = true;
                } else if (choice == 'B') {
                    if (filteredFoods[j].carbs < filteredFoods[j+1].carbs) swapNeeded = true;
                } else {
                    if (filteredFoods[j].fat < filteredFoods[j+1].fat) swapNeeded = true;
                }
                
                if (swapNeeded) {
                    RecFoodItem temp = filteredFoods[j];
                    filteredFoods[j] = filteredFoods[j+1];
                    filteredFoods[j+1] = temp;
                }
            }
        }

        // 4. Top 3
        vector<Recommendation> topOptions;

        double targetNutrientVal = 0;
        string nutName = "";
        
        if (choice == 'A') { targetNutrientVal = remProt; nutName = "protein"; }
        else if (choice == 'B') { targetNutrientVal = remCarb; nutName = "carbs"; }
        else { targetNutrientVal = remFat; nutName = "fat"; }

        if (targetNutrientVal <= 0) targetNutrientVal = 20.0; // Minimal default if on target

        double goal = targetNutrientVal * 0.80; 

        int maxShow = 3;
        if (filteredFoods.size() < 3) maxShow = filteredFoods.size();

        for (int i = 0; i < maxShow; ++i) {
            RecFoodItem f = filteredFoods[i];
            double nutrientPerServing = 0;
            if (choice == 'A') nutrientPerServing = f.protein;
            else if (choice == 'B') nutrientPerServing = f.carbs;
            else nutrientPerServing = f.fat;
            
            if (nutrientPerServing < 0.1) nutrientPerServing = 0.1;

            double requiredServings = goal / nutrientPerServing;
            double finalAmount = requiredServings * f.servingSizeVal;
            
            if (f.servingUnitStr == "g" || f.servingUnitStr == "ml") {
                finalAmount = round(finalAmount);
            } else {
                 finalAmount = round(finalAmount * 10) / 10.0;
            }

            double finalNutrient = (finalAmount / f.servingSizeVal) * nutrientPerServing;
            Recommendation rec;
            rec.food = f;
            rec.recommendedAmount = finalAmount;
            rec.nutrientContributed = finalNutrient;
            topOptions.push_back(rec);
        }

        cout << "\nSuggested Options:\n";
        for (size_t i = 0; i < topOptions.size(); ++i) {
            cout << (i+1) << ") " << topOptions[i].food.name 
                 << " - " << topOptions[i].recommendedAmount << topOptions[i].food.servingUnitStr 
                 << " (" << (int)topOptions[i].nutrientContributed << "g " << nutName << ")\n";
        }

        cout << "\nSelect option to auto-add (0 to cancel): ";
        int sel;
        getValidInt(sel, 0, 3);

        if (sel > 0 && sel <= (int)topOptions.size()) {
            Recommendation chosen = topOptions[sel-1];
            autoAddFood(p.username, chosen.food, chosen.recommendedAmount);
            pauseConsole();
        } 
    }
}

void runEndDaySummary(const UserProfile& p) {
    clearScreen();
    // 1. Calculate Totals
    NutrientStatus consumed = getConsumedToday(p.username);

    // 2. Display
    cout << "=== END OF DAY SUMMARY ===\n\n";
    cout << left << setw(16) << "Nutrient" 
         << setw(11) << "Target" 
         << setw(12) << "Consumed" 
         << setw(10) << "Status" << endl;
    cout << "-----------------------------------------------" << endl;

    printSummaryLine("Calories", p.dailyCaloriesTarget, consumed.calories, "");
    printSummaryLine("Protein", p.targetProtein_g, consumed.protein, "g");
    printSummaryLine("Carbs", p.targetCarbs_g, consumed.carbs, "g");
    printSummaryLine("Fat", p.targetFat_g, consumed.fat, "g");

    cout << "\nReflection:\n";
    
    if (consumed.protein >= p.targetProtein_g) {
        cout << "✔ Great protein intake today!\n";
    } else {
        cout << "⚠ Protein is under target.\n";
    }

    // Use a small tolerance to avoid floating-point noise and provide clearer guidance
    double tol = 0.5; // grams
    if (consumed.fat > p.targetFat_g + tol) {
        cout << "⚠ Slightly high fat — consider leaner meals tomorrow.\n";
    } else if (consumed.fat + tol < p.targetFat_g) {
        cout << "⚠ Fat intake is below target — include some healthy fats.\n";
    } else {
        cout << "✔ Fat intake is within limits.\n";
    }

    cout << "\nDAILY_LOG written successfully.\n";
    pauseConsole();
}
