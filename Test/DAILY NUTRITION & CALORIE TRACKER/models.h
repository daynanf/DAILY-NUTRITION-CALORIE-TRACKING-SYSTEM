#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

// Structure for User Profile
struct UserProfile {
    std::string username;           // Added for linking to files
    std::string fullName;
    int age = 0;
    std::string gender;             // "Male" or "Female"
    int height_cm = 0;
    double weight_kg = 0.0;
    std::string activityLevel;      // "Sedentary", "Lightly Active", etc.
    
    // Calculated Targets
    double dailyCaloriesTarget = 0.0;
    double targetProtein_g = 0.0;
    double targetCarbs_g = 0.0;
    double targetFat_g = 0.0;
};

// Structure for Food Item
struct FoodItem {
    std::string name;
    double calories = 0.0;
    double protein = 0.0;
    double carbs = 0.0;
    double fat = 0.0;
    double servingSize = 100.0; // Standard serving size in grams
};

// Structure to track daily totals
struct DailyLog {
    std::string date;
    std::vector<std::pair<FoodItem, double>> consumedMeals; // FoodItem + quantity consumed in grams
    
    double totalCalories = 0.0;
    double totalProtein = 0.0;
    double totalCarbs = 0.0;
    double totalFat = 0.0;
};

#endif
