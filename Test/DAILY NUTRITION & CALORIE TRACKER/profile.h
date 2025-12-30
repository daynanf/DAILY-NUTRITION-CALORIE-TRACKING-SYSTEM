#ifndef PROFILE_H
#define PROFILE_H

#include "models.h"
#include <string>

// Core Calculations
double calcBMR(const UserProfile& p);
double activityMultiplier(const std::string& level);
void calcMacroTargets(UserProfile& p);

// File Operations
void saveUserProfile(const UserProfile& p);
bool loadUserProfile(const std::string& username, UserProfile& p);

// Display
void displayNutritionTargets(const UserProfile& p);
void displayProfile(const UserProfile& p);

// Updates
void updateWeight(UserProfile& p, double newWeight);

#endif
