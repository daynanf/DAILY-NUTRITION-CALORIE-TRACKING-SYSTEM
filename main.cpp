#include <iostream>
#include <limits>
#include "auth.h"
#include "profile.h"
#include "food.h"
#include "recommendations.h"
#include "utils.h"

using namespace std;




void userDashboardMenu()
{
    clearScreen();
    cout << "\n"
     << "╔═════════════════════════════════════════════════════════════════════════════╗\n"
     << "║ 🥗💪  WELCOME TO THE DAILY NUTRITION & CALORIE TRACKER  💪🥗              ║\n"
     << "╚═════════════════════════════════════════════════════════════════════════════╝\n\n";

    cout << "╔═════════════════════════════════════════════════════════════════════════════╗\n"
         << "║                              🏠 USER DASHBOARD 🏠                          ║\n"
         << "╠═════════════════════════════════════════════════════════════════════════════╣\n"
         << "║ 1️⃣  View My Profile                         👤                             ║\n"
         << "║ 2️⃣  View Daily Nutrition Targets           📊                              ║\n"
         << "║ 3️⃣  Add Food to Today's Log                📝                              ║\n"
         << "║ 4️⃣  View Today's Consumption               🍽️                              ║\n"
         << "║ 5️⃣  Add Custom Food                        🥘                              ║\n"
         << "║ 6️⃣  Get Meal Recommendations               🍱                              ║\n"
         << "║ 7️⃣  End Day & View Summary                 📅                              ║\n"
         << "║ 8️⃣  Logout                                 🚪                              ║\n"
         << "╚═════════════════════════════════════════════════════════════════════════════╝\n\n"
         << " Enter your choice: ";
}

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
                char dashChoice;
                cin >> dashChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                switch(dashChoice) {
                    case '1':
                        printHeader("MY PROFILE");
                        displayProfile(currentUser);
                        pauseConsole();
                        break;
                    case '2':
                        printHeader("DAILY TARGETS");
                        displayNutritionTargets(currentUser);
                        pauseConsole();
                        break;
                    case '3':
                        runAddFood(currentUser);
                        break;
                    case '4':
                        runViewConsumption(currentUser);
                        break;
                    case '5':
                        runAddCustomFood(currentUser);
                        break;
                    case '6':
                        runMealRecommendations(currentUser);
                        break;
                    case '7':
                        runEndDaySummary(currentUser);
                        break;
                    case '8':
                        loggedIn = false;
                        cout << "Logging out...\n";
                        system("cls");
                        break;
                    default:
                        cout << "Invalid Option!\n";
                        pauseConsole();
                }
                
                if (!loggedIn) break; // Break dashboard loop, back to main
            }
        }
    }
    return 0;
}