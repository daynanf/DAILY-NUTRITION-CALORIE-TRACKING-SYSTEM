#ifndef AUTH_H
#define AUTH_H

#include "models.h"
#include <string>
using namespace std;

// struct UserAccount {
//     string username;
//     string password;
//     string fullName;
//     int age;
//     string gender;
//     int height;
//     int weight;
//     string activityLevel;
// };

//First Welcome Page
void firstWellcomePage();

// Input-validation helpers
void getValidUsername(string &username, char choice);
void getValidPassword(string &password, const string &username, char choice);
void getValidFullname(string &fullname);
void getValidAge(int &age);
void getValidGender(string &gender);
void getValidHeight(int &height);
void getValidWeight(int &weight);
void getValidActivityLevel(string &activityLevel);

// User flow functions
void registerUser(char choice);
bool loginUser(char choice);
bool authentication(UserProfile &profile);

#endif
