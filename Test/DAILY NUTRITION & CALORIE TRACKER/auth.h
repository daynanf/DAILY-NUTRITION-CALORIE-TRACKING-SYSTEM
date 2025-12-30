#ifndef AUTH_H
#define AUTH_H

#include "models.h"
#include <string>

// Returns true if login successful, popupulates the profile
bool loginUser(UserProfile& profile);

// data flow for registration
void registerUser();

#endif
