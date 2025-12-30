#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <algorithm>
#include <limits>       // For numeric_limits
#include <windows.h>    // For console settings

// Function declarations
void setupConsole();
void clearScreen();
void pauseConsole();
void printHeader(const std::string& title, const std::string& subtitle = "");
std::string toLower(const std::string& str);
bool isSimilar(std::string searchWord, std::string text);
std::string trim(const std::string& str);

// Input validation helpers
void getValidInt(int& value, int min = -1, int max = -1);
void getValidDouble(double& value, double min = -1.0, double max = -1.0);

#endif
