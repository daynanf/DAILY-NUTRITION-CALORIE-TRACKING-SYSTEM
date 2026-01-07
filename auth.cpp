#include "auth.h"
#include "profile.h"
#include "utils.h"
#include "models.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void firstWellcomePage()
{
    system("cls");
	cout << "\n"
     << "╔═══════════════════════════════════════════════════════════════════════╗\n"
     << "║        WELCOME TO THE DAILY NUTRITION & CALORIE TRACKER               ║\n"
     << "╚═══════════════════════════════════════════════════════════════════════╝\n\n"
     << "   1. Register New User\n"
     << "   2. Login\n"
     << "   3. Exit\n\n"
     << "   Please Enter Your Choice: ";
}

// getValidUsername: for choice == '1' (register) ensure username NOT present.
//                    for choice == '2' (login) ensure username IS present.
bool getValidUsername(string &username, char choice) 
{
    while (true) 
    {
        cout << "\nEnter Username (or 'b' to back): ";
        getline(cin, username);

        if (checkForBack(username)) return false;

        if (username.empty()) 
        {
            cout << "\nUsername cannot be empty! Please try again.\n";
            continue;
        }
        
        
        // Check if first character is a letter (A-Z or a-z)
        if (!isalpha(username[0]))  
        {
            cout << "\nInvalid username! Username must start with a letter. Try again.\n";
            continue;
        }
        
        // All other characters must be letters or digits
        bool validChars = true;
        for (char c:username)
        {
            if (!isalnum(c)) // not letter or digit
            {
                validChars = false;
                break;
            }
        }

        if (!validChars)
        {
            cout << "\nInvalid username! Username can only contain letters and numbers.\n";
            continue;
        }

        bool allDigits = true;
        for (char c : username) 
        {
            if (!isdigit(c)) 
            {
                allDigits = false;
                break;
            }
        }

        if (allDigits) 
        {
            cout << "\nInvalid username! Username cannot be only numbers. Please try again.\n";
            continue;
        } 

        ifstream f("users.txt");
        if (!f)
        {
            // If file does not exist:
            if (choice == '1') {
                // registering — it's fine, file will be created when appending
                break;
            } else {
                // login — no accounts yet
                cout << "No users found (users.txt missing). Please register first.\n";
                continue;
            }
        }
        else
        {
            string line;
            bool found = false;
            while (getline(f, line))
            {
                size_t pos = line.find("|");
                string fileusername = line.substr(0, pos);
                if (username == fileusername)
                {
                    found = true;
                    if (choice == '1') {
                        cout << "The username \"" << username << "\" already exists, go for another one!\n";
                    } else if (choice == '2') {
                        cout << "Hey, 👤" << username << "!\n";
                    }
                    break;
                }
            }
            f.close();

            if (choice == '1' && found) continue;   // registration: can't use existing username
            if (choice == '2' && !found) {          // login: username must exist
                cout << "Username not found. Please try again or register.\n";
                continue;
            }
        }

        // all checks passed
        return true;
    }

    return true; // Should be unreachable given the while(true)
}


// Passing the username by reference (even if not modified) is more efficient,
// as it avoids copying the string, which can be costly if the string is long.
bool getValidPassword(string &password, const string &username, char choice) 
{
    if (choice == '1') // Registration
    {
        while (true)
        {
            cout << "\nEnter Password (or 'b' to back): ";
            getline(cin, password);

            if (checkForBack(password)) return false;

            if (password.empty())
            {
                cout << "\nPassword cannot be empty! Please try again.\n";
                continue;
            }

            break; // valid password
        }
        return true;
    }
    else if (choice == '2') // Login
    {
        int chances = 3;
        bool matched = false;

        while (chances > 0)
        {
            cout << "\nEnter Password (or 'b' to back): ";
            getline(cin, password);

            if (checkForBack(password)) return false;

            if (password.empty())
            {
                cout << "Password cannot be empty. Try again.\n";
                continue;
            }

            ifstream file("users.txt");
            if (!file)
            {
                cout << "Error: Could not open users file.\n";
                exit(1);
            }

            string line;
            matched = false;

            while (getline(file, line))
            {
                size_t pos1 = line.find("|");
                if (pos1 == string::npos) continue;

                string fileUsername = line.substr(0, pos1);
                if (fileUsername != username) continue;

                size_t pos2 = line.find("|", pos1 + 1);
                string filePassword;
                if (pos2 == string::npos)
                    filePassword = line.substr(pos1 + 1);
                else
                    filePassword = line.substr(pos1 + 1, pos2 - pos1 - 1);

                if (password == filePassword)
                    matched = true;

                break;
            }

            file.close();

            file.close();

            if (matched)
                return true; // password correct
            else
            {
                chances--;
                if (chances > 0)
                    cout << "Wrong password! You have " << chances << " chances left.\n";
                else
                {
                    cout << "😒 Sorry, you have run out of chances. Try some other time.\n";
                    exit(0);
                }
            }
        }
    }

    return false; // Should not reach here if choice is valid
}


bool getValidFullname(string &fullname)  
{
    while (true) 
    {
        cout << "\n\nEnter Your Fullname (or 'b' to back): ";
        getline(cin, fullname);

        if (checkForBack(fullname)) return false;

        // Trim whitespace

        // Trim whitespace
        fullname = trim(fullname);

        // Check if empty
        if (fullname.empty()) 
        {
            cout << "\nHey! Everyone has a name 😄. Yours can't be empty—try again!\n";
            continue; 
        }

        // Limit length to avoid extremely large inputs
        if (fullname.size() > 1024) {
            cout << "Name is too long. Please enter a shorter name (max 1024 chars).\n";
            continue;
        }

        // Validate characters: allow letters (including UTF-8 bytes), spaces, hyphens and apostrophes.
        // Reject digits, pipe character '|' (used as delimiter) and control characters.
        bool invalid = false;
        for (unsigned char uc : fullname) {
            if (uc == '|' || uc == '\n' || uc == '\r' || uc == '\t') {
                invalid = true; break;
            }
            if (uc >= '0' && uc <= '9') { // reject ASCII digits
                invalid = true; break;
            }
            // allow space, hyphen, apostrophe explicitly
            if (uc == ' ' || uc == '-' || uc == '\'' ) continue;
            // allow ASCII letters
            if ((uc >= 'A' && uc <= 'Z') || (uc >= 'a' && uc <= 'z')) continue;
            // allow non-ASCII bytes (part of UTF-8 multi-byte sequences)
            if (uc >= 0x80) continue;
            // anything else is invalid
            // (this will still permit many valid Unicode letters because their bytes are >=0x80)
            invalid = true; break;
        }

        if (invalid) {
            cout << "\nInvalid name. Use letters, spaces, hyphens or apostrophes only (no digits or '|'). Please try again.\n";
            continue;
        }

        // Valid name, exit loop
        return true;
    }
}


bool getValidAge(int &age)
{
    string input;
    while (true)
    {
        cout << "\n\nEnter Age (or 'b' to back): ";
        cin >> input;

        if (checkForBack(input)) return false;

        // Try to parse integer
        try {
            size_t pos;
            age = stoi(input, &pos);
            if (pos < input.size()) {
                 // Trailing characters found (e.g. "25abc")
                 throw invalid_argument("Trailing characters");
            }
        } catch (...) {
            cout << "Oops! That’s not a valid number. Please enter a whole number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // Clear leftover newline so subsequent getline() calls wait for user input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Check age range with funny/friendly messages
        if (age < 1)
		{
            cout << "Oops! Age can't be negative or zero 😅. Please enter a real age.\n";
            continue;
        }

        if (age > 200) 
		{
            cout << "Hmm… haven't met anyone above 200 yet 😲. Please enter your real age.\n";
            continue;
        }

        break; // valid age
    }
    return true;
}

// Check whether the provided age, height (cm) and weight (kg) are roughly proportional
bool isProportional(int age, double height, double weight) {
    if (age < 1) { // Infants (0-1 years)
        if (height < 50 || height > 150 || weight < 2 || weight > 20) {
            return false;
        }
    } else if (age <= 12) { // Children (1-12 years)
        if (height < 70.0 || height > 165.0 || weight < 5.0 || weight > 80.0) {
            return false;
        }
    } else if (age <= 18) { // Teenagers (13-18 years)
        if (height < 140.0 || height > 195.0 || weight < 40.0 || weight > 100.0) {
            return false;
        }
    } else { // Adults (19+ years)
        if (height < 140.0 || height > 280.0 || weight < 35.0 || weight > 650.0) {
            return false;
        }
    }

    return true;
}

bool getValidGender(string &gender) 
{
    while (true) 
	{
        cout << "\n\nEnter Gender (M/F) (or 'b' to back): ";
        getline(cin, gender);

        if (checkForBack(gender)) return false;

        // Convert input to lowercase for simple comparison
        for (char &c : gender) c = tolower(c);
        
        bool isnum=false;
        for(char c: gender)
        {
        	if(isdigit(c))
        	{
        		cout<<"Oops! Gender can't contain a number.\n";
        		isnum=true;
        		break;
			}
		}
		
		if(isnum) continue;  //if it gets a number in the input gender

        if (gender == "m" || gender == "male") 
		{
            gender = "Male";  // store standardized
            break;
        } 
		else if (gender == "f" || gender == "female") 
		{
            gender = "Female"; // store standardized
            break;
        } 
		else 
		{
            cout << "In Ethiopia, genders are only Male or Female 😅. Please try again.\n";
        }
    }
    return true;
}

// Function to safely get a valid height (in cm)
bool getValidHeight(int &height) {
    string input;
    while (true) {
        cout << "\n\nEnter Height (cm) (or 'b' to back): ";
        cin >> input;

        if (checkForBack(input)) return false;

        try {
            size_t pos;
            height = stoi(input, &pos);
            if (pos < input.size()) throw invalid_argument("Trailing characters");
        } catch (...) {
            cout << "Oops! That's not a valid number. Please enter a whole number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (height < 50) {
            cout << "Hmm… that seems too short 😅. Please enter a realistic height in cm.\n";
            continue;
        }

        if (height > 300) {
            cout << "Wow! Haven't seen anyone that tall 😲. Enter a realistic height.\n";
            continue;
        }

        break; // valid height
    }
    return true;
}

// Function to safely get a valid weight (in kg)
bool getValidWeight( double &weight) {
    string input;
    while (true) {
        cout << "\n\nEnter Weight (kg) (or 'b' to back): ";
        cin >> input;

        if (checkForBack(input)) return false;

        try {
            size_t pos;
            weight = stod(input, &pos);
            if (pos < input.size()) throw invalid_argument("Trailing characters");
        } catch (...) {
            cout << "Oops! That's not a valid number. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (weight < 2) {
            cout << "Hmm… that weight is too low 😄. Enter a realistic weight.\n";
            continue;
        }

        if (weight > 650) {
            cout << "Whoa! Haven't seen anyone that heavy 😲. Enter a realistic weight.\n";
            continue;
        }

        break; // valid weight
    }
    return true;
}

bool getValidActivityLevel(string &activityLevel)
{
    const string allActivityLevels[] = {
        "Sedentary",
        "Lightly Active",
        "Moderately Active",
        "Very Active"
    };

    string input;
    int choice;

    while (true)
    {
        cout << "\nSelect Activity Level (or 'b' to back):\n"
             << "1. Sedentary\n"
             << "2. Lightly Active\n"
             << "3. Moderately Active\n"
             << "4. Very Active\n"
             << "Enter choice: ";

        cin >> input;

        if (checkForBack(input)) return false;

        try {
            size_t pos;
            choice = stoi(input, &pos);
            if (pos < input.size()) throw invalid_argument("Trailing characters");
        } catch (...) {
             cout << "Invalid input. Please enter a number.\n";
             cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
             continue;
        }

        if (choice < 1 || choice > 4)
        {
            cout << "Choice out of range. Enter 1 to 4.\n";
            // cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // cin.ignore(numeric_limits<streamsize>::max(), '\n');
        activityLevel = allActivityLevels[choice - 1];
        break;
    }
    return true;
}

void registerUser(char choice)
{
    UserProfile ua;

    printHeader("REGISTER NEW USER");
    if (!getValidUsername(ua.username, choice)) return;

    if (!getValidPassword(ua.password, ua.username, choice)) return;

    if (!getValidFullname(ua.fullName)) return;

    // Loop label for re-entering age/height/weight when proportions are invalid
start_age:
    if (!getValidAge(ua.age)) return;

    if (!getValidGender(ua.gender)) return;

    if (!getValidHeight(ua.height_cm)) return;

    if (!getValidWeight(ua.weight_kg)) return;

    // Verify proportionality before moving to activity level
    if (!isProportional(ua.age, static_cast<double>(ua.height_cm), ua.weight_kg)) {
        cout << "\nThe age, height and weight you entered are not proportional. Please re-enter them.\n";
        goto start_age;
    }

    if (!getValidActivityLevel(ua.activityLevel)) return;

    calcMacroTargets(ua);

    // Append mode and raw string literal for file path
    ofstream file("users.txt", ios::app);
    if (!file) {
        cout << "Error: Could not open file. Make sure the directory exists!" << endl;
        return;
    }

    file << ua.username << "|"
            << ua.password
            << "| user_" << ua.username << "_data.txt"
            << ua.fullName << "|"
            << ua.age << "|"
            << ua.gender << "|"
            << ua.height_cm << "|"
            << ua.weight_kg << "|"
            <<ua.activityLevel<<"\n";
    file.close();

    string filename = "user_" + ua.username + "_data.txt";

        createUserDataFile(filename, ua);
        cout << "\n--------------------------------------------\n"
             << "    Profile created successfully!   \n"
             << "--------------------------------------------\n\n";
}

bool loginUser( UserProfile& profile,char choice)
{   
    system("cls");
    printHeader("LOGIN");
    string inputUsername, inputPassword;

    if (!getValidUsername(inputUsername, choice)) return false;
    if (!getValidPassword(inputPassword, inputUsername, choice)) return false;

    cout << "\n\nLogin successful! \n";
    cout << "Welcome back, 👤 " << inputUsername << "!\n\n";
    
    string filename = "user_" + inputUsername + "_data.txt";
    // Ensure profile.username is set so subsequent operations use correct per-user files
    profile.username = inputUsername;
    loadProfileFromFile(filename, profile);
    cout << "👉 Press Enter to continue.....";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    // Clear screen and show welcome
    system("cls"); 
    return true;
}                         
bool authentication(UserProfile& profile)
{
	firstWellcomePage();
	char choice;
    cin >> choice;
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear leftover newline
    bool isLoggedIn = false;
    while(true)
    {
    	switch (choice) 
		{
	        case '1': 
			{
                system("cls");
				registerUser(choice);
				
				// cout << "👉 Press Enter to login to your account...";
		        // cin.ignore(numeric_limits<streamsize>::max(), '\n');
				// cin.get();
				// system("cls");
				
				// // Call loginUser with choice = '2'
    			// loginUser('2');
    			
	            return false; // exit main, back to welcome screen
	        }
	
		    case '2': 
			{
				isLoggedIn = loginUser(profile,choice);
			
			    return isLoggedIn;  // exits this module and goes to main
			}

	        case '3':
	        {
        		cout << "\nThank you! Goodbye 👋\n";
            	exit(0);	
			}
	           	
	        default:
	        {
        		cout << "\nInvalid choice!\n";
        		
        		//asking again for another value of choice and go back to the while loop
        		cout << "\nEnter Your Choice: ";
			    cin >> choice;
			    cin.ignore(numeric_limits<streamsize>::max(), '\n');
			    continue;
			}
	    }	
	}
    return isLoggedIn;
}