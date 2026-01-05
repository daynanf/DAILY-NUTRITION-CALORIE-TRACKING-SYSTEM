#ifndef PROFILE_H
#define PROFILE_H

#include "models.h"     // Shared UserProfile struct (with username, fullName, etc.)
#include <string>

/**
 * Collects full user profile from input during registration.
 * Returns filled UserProfile (or empty if cancelled with 'b').
 */
UserProfile collectProfileFromUser();

/**
 * Calculates BMR, TDEE, and macro targets (protein/carbs/fat) for the given profile.
 * Updates the profile struct directly.
 */
void calcMacroTargets(UserProfile& p);

/**
 * Displays the user's daily nutrition targets (calories, protein, carbs, fat).
 * Used in dashboard menu option 2.
 */
void displayNutritionTargets(const UserProfile& p);

/**
 * Displays the full user profile (personal fields only).
 * Used in dashboard menu option 1.
 */
void displayProfile(const UserProfile& p);

/**
 * Creates (or overwrites) the per-user data file and writes the USER_PROFILE block.
 * Called during registration after collecting profile.
 */
void createUserDataFile(const std::string& filename, const UserProfile& p);

/**
 * Loads the USER_PROFILE block from the per-user data file into the profile struct.
 * Called after successful login.
 */
void loadProfileFromFile(const std::string& filename, UserProfile& p);

/**
 * Prompts for new weight, recalculates targets, rewrites USER_PROFILE block,
 * and appends WEIGHT_UPDATE block to the file.
 * Called from dashboard menu (e.g. option 9).
 */
void updateWeightAndTargets(const std::string& filename, UserProfile& p);

#endif // PROFILE_H