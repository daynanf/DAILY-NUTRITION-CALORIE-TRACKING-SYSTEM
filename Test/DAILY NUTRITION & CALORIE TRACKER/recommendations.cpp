#include "recommendations.h"
#include "food.h"
#include "utils.h"
#include <iostream>

using namespace std;

void provideProteinRecommendations(double shortfall) {
    cout << "\n[Protein Tip]: You need " << shortfall << "g more protein.\n";
    cout << "  - Chicken Breast (High effeciency): " << (shortfall * 3) << "g weight approx.\n";
    cout << "  - Greek Yogurt: Good for snacks.\n";
}

void provideCarbRecommendations(double shortfall) {
    cout << "\n[Carb Tip]: You need " << shortfall << "g more carbs.\n";
    cout << "  - Oats or Rice are great sources.\n";
    cout << "  - " << shortfall * 1.5 << "g of Rice approx.\n";
}

void provideFatRecommendations(double shortfall) {
    cout << "\n[Fat Tip]: You need " << shortfall << "g more fat.\n";
    cout << "  - Almonds or Peanut Butter.\n";
}

void runMealRecommendations(const UserProfile& p) {
    DailyLog log = getTodayLog(p);
    
    double remCal = p.dailyCaloriesTarget - log.totalCalories;
    double remProt = p.targetProtein_g - log.totalProtein;
    double remCarb = p.targetCarbs_g - log.totalCarbs;
    double remFat = p.targetFat_g - log.totalFat;
    
    printHeader("MEAL RECOMMENDATIONS");
    
    cout << "Current Totals: " << log.totalCalories << " cal | " << log.totalProtein << "g P | " << log.totalCarbs << "g C\n";
    cout << "Remaining: " << remCal << " cal | " << remProt << "g P | " << remCarb << "g C\n\n";
    
    if (remCal <= 0) {
        cout << "You have met or exceeded your calorie target for today!\n";
    } else {
        if (remProt > 20) provideProteinRecommendations(remProt);
        if (remCarb > 30) provideCarbRecommendations(remCarb);
        if (remFat > 10) provideFatRecommendations(remFat);
        
        if (remProt <= 20 && remCarb <= 30 && remFat <= 10) {
            cout << "You are mostly on track! Just a small snack will do.\n";
        }
    }
    pauseConsole();
}

void runEndDaySummary(const UserProfile& p) {
    DailyLog log = getTodayLog(p);
    
    printHeader("DAILY SUMMARY");
    cout << "Date: " << log.date << "\n\n";
    
    cout << "Target vs Consumption:\n";
    cout << "Calories: " << p.dailyCaloriesTarget << " / " << log.totalCalories << "\n";
    cout << "Protein : " << p.targetProtein_g << " / " << log.totalProtein << "\n";
    cout << "Carbs   : " << p.targetCarbs_g << " / " << log.totalCarbs << "\n";
    cout << "Fat     : " << p.targetFat_g << " / " << log.totalFat << "\n";
    
    cout << "\nEnd-of-Day Insight:\n";
    if (log.totalCalories < p.dailyCaloriesTarget * 0.8) {
        cout << "Your intake was significantly below recommended levels.\n";
    } else if (log.totalCalories > p.dailyCaloriesTarget * 1.1) {
        cout << "Your intake was higher than recommended.\n";
    } else {
        cout << "Great job! You hit close to your calorie targets.\n";
    }
    
    cout << "\nDay ended. Data is saved.\n";
    pauseConsole();
}
