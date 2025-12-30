#ifndef FOOD_H
#define FOOD_H

#include "models.h"
#include <vector>

void runAddFood(const UserProfile& p);
void runViewConsumption(const UserProfile& p);
void runAddCustomFood(const UserProfile& p);

// Helpers reused by Recommendations
DailyLog getTodayLog(const UserProfile& p);
std::vector<FoodItem> searchFoodDatabase(const std::string& query, const std::string& username);

#endif
