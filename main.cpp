#include <iostream>
#include <limits>
#include "auth.h"
#include "profile.h"
#include "food.h"
#include "recommendations.h"
#include "utils.h"

using namespace std;

// Displays the main user dashboard menu options.
// This function clears the screen and prints the available actions for the logged-in user.
void userDashboardMenu()
{
    clearScreen();
    cout << "\n"
     << "╔═════════════════════════════════════════════════════════════════════════════╗\n"
     << "║           WELCOME TO THE DAILY NUTRITION & CALORIE TRACKER                  ║\n"
     << "╚═════════════════════════════════════════════════════════════════════════════╝\n\n";

    cout << "╔══════════════════════════════════════════════════════════════════════════╗\n"
         << "║                               USER DASHBOARD                             ║\n"
         << "╠══════════════════════════════════════════════════════════════════════════╣\n"
         << "║ 1️⃣  View My Profile                                                      ║\n"
         << "║ 2️⃣  View Daily Nutrition Targets                                         ║\n"
         << "║ 3️⃣  Add Food to Today's Log                                              ║\n"
         << "║ 4️⃣  View Today's Consumption                                             ║\n"
         << "║ 5️⃣  Add Custom Food                                                      ║\n"
         << "║ 6️⃣  Get Meal Recommendations                                             ║\n"
         << "║ 7️⃣  End Day & View Summary                                               ║\n"
         << "║ 8️⃣  Update Weight & Recalculate Targets                                  ║\n"
         << "║ 9️⃣  Logout                                                               ║\n"
         << "╚══════════════════════════════════════════════════════════════════════════╝\n\n"
         << " Enter your choice: ";
}

// Application Entry Point.
// 1. Sets up the console environment.
// 2. Enters the main application loop.
// 3. Handles User Authentication (Login/Register).
// 4. If authenticated, enters the Dashboard Loop where users can access features.
// 5. Handles Logout to return to the main authentication screen.
int main() {
    setupConsole();
    
    while(true) {
        
        UserProfile currentUser;
        bool loggedIn = false;
        
        loggedIn = authentication(currentUser);
        
        if (loggedIn) {
            // Dashboard Loop
            while(true) {
                userDashboardMenu();
                int dashChoice = 0;
                cin >> dashChoice;
                
                // Handle invalid input (non-numeric, out of range, etc.)
                if (cin.fail() || dashChoice < 1 || dashChoice > 9) {
                    cin.clear(); 
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                    cout << "Invalid Option! Please enter a number from 1-9.\n";
                    pauseConsole();
                    continue; 
                }
                
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                switch(dashChoice) {
                    case 1:
                        printHeader("MY PROFILE");
                        displayProfile(currentUser);
                        break;
                    case 2:
                        printHeader("DAILY TARGETS");
                        displayNutritionTargets(currentUser);
                        break;
                    case 3:
                        runAddFood(currentUser);
                        break;
                    case 4:
                        runViewConsumption(currentUser);
                        break;
                    case 5:
                        runAddCustomFood(currentUser);
                        break;
                    case 6:
                        runMealRecommendations(currentUser);
                        break;
                    case 7:
                        runEndDaySummary(currentUser);
                        break;
                    case 8: {
                        string filename = "user_" + currentUser.username + "_data.txt";
                        updateWeightAndTargets(filename, currentUser);
                        break;
                    }
                    case 9:
                        loggedIn = false;
                        cout << "Logging out...\n";
                        clearScreen();
                        break;
                    default:
                        cout << "Invalid Option!\n";
                        pauseConsole();
                
                }
                
                if (!loggedIn) break;
            }
        }
    }
    return 0;
}