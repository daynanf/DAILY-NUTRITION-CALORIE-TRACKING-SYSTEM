#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
using namespace std;
// Structure for User Profile
// Structure for User Profile
// Stores all personal information and calculated nutritional targets.
struct UserProfile {
    string username;           // Used for linking to file names (e.g., user_USERNAME_data.txt)
    string fullName;
    string password;
    int age = 0;
    string gender;             // "Male" or "Female"
    int height_cm = 0;
    double weight_kg = 0.0;
    string activityLevel;      // e.g., "Sedentary", "Lightly Active"
    
    // Calculated Targets (runtime only, re-calculated on load)
    double dailyCaloriesTarget = 0.0;
    double targetProtein_g = 0.0;
    double targetCarbs_g = 0.0;
    double targetFat_g = 0.0;
};

// Structure for Food Item
// Structure for Food Item
// Represents a single food entry from the database (foods.txt) or custom foods.
struct FoodItem {
    string name;
    double calories = 0.0;
    double protein = 0.0;
    double carbs = 0.0;
    double fat = 0.0;
    double servingSize = 100.0; // Standard reference serving size in grams
};

// Structure to track daily totals
// Structure to track daily totals and individual log entries.
// Used primarily during runtime aggregation of daily consumption.
struct DailyLog {
    string date;
    vector<pair<FoodItem, double>> consumedMeals; // List of meals: {FoodItem, GramsConfirmed}
    
    // Individual entry fields used when parsing a log file line-by-line:
    string mealType;      // "Breakfast", "Lunch", etc.
    string foodName;
    double grams;
    double calories;
    double protein;
    double carbs;
    double fat;
    
    // Aggregated daily totals:
    double totalCalories = 0.0;
    double totalProtein = 0.0;
    double totalCarbs = 0.0;
    double totalFat = 0.0;
};

#endif
