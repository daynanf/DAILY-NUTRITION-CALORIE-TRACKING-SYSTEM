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
	cout << "\n"
     << "╔═══════════════════════════════════════════════════════════════════════╗\n"
     << "║ 🍏💪  WELCOME TO THE DAILY NUTRITION & CALORIE TRACKER  💪🍏 ║\n"
     << "╚═══════════════════════════════════════════════════════════════════════╝\n\n"
     << " 🆕  1. Register New User\n"
     << " 🔑  2. Login\n"
     << " ❌  3. Exit\n\n"
     << " ✨  Please Enter Your Choice: ";
}

// getValidUsername: for choice == '1' (register) ensure username NOT present.
//                    for choice == '2' (login) ensure username IS present.
void getValidUsername(string &username, char choice) 
{
    while (true) 
    {
        cout << "\nEnter Username: ";
        getline(cin, username);

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
        break;
    }
}


// Passing the username by reference (even if not modified) is more efficient,
// as it avoids copying the string, which can be costly if the string is long.
void getValidPassword(string &password, const string &username, char choice) 
{
    if (choice == '1') // Registration
    {
        while (true)
        {
            cout << "\nEnter Password: ";
            getline(cin, password);

            if (password.empty())
            {
                cout << "\nPassword cannot be empty! Please try again.\n";
                continue;
            }

            break; // valid password
        }
    }
    else if (choice == '2') // Login
    {
        int chances = 3;
        bool matched = false;

        while (chances > 0)
        {
            cout << "\nEnter Password: ";
            getline(cin, password);

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

            if (matched)
                return; // password correct
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
}


void getValidFullname(string &fullname) 
{
    while (true) 
    {
        cout << "\n\nEnter Your Fullname: ";
        getline(cin, fullname);

        // Check if empty
        if (fullname.empty()) 
        {
            cout << "\nHey! Everyone has a name 😄. Yours can't be empty—try again!\n";
            continue; 
        }

        // Check if all characters are letters
        bool allLetters = true;
        for (char c : fullname) 
        {
            if (!isalpha(c))  // if not a letter
            {
                allLetters = false;
                break;
            }
        }

        if (!allLetters) 
        { 
            cout << "\nOops! Names can only contain letters 😎. Please try again.\n";
            continue;
        } 
        
        // Valid name, exit loop
        break;
    }
}


void getValidAge(int &age) 
{
    while (true) 
	{
        cout << "\n\nEnter Age: ";
        cin >> age;

        // Check if input failed (not an integer)
        if (cin.fail()) 
		{
            cout << "Oops! That’s not a valid number. Please enter a whole number.\n";
            cin.clear(); // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            continue;
        }

        // Clear leftover newline
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
}

void getValidGender(string &gender) 
{
    while (true) 
	{
        cout << "\n\nEnter Gender (M/F): ";
        getline(cin, gender);

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
}

// Function to safely get a valid height (in cm)
void getValidHeight(int &height) {
    while (true) {
        cout << "\n\nEnter Height (cm): ";
        cin >> height;

        if (cin.fail()) {
            cout << "Oops! That's not a valid number. Please enter a whole number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
}

// Function to safely get a valid weight (in kg)
void getValidWeight( double &weight) {
    while (true) {
        cout << "\n\nEnter Weight (kg): ";
        cin >> weight;

        if (cin.fail()) {
            cout << "Oops! That's not a valid number. Please enter a whole number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
}

void getValidActivityLevel(string &activityLevel)
{
    const string allActivityLevels[] = {
        "Sedentary",
        "Lightly Active",
        "Moderately Active",
        "Very Active"
    };

    int choice;

    while (true)
    {
        cout << "\nSelect Activity Level:\n"
             << "1. Sedentary\n"
             << "2. Lightly Active\n"
             << "3. Moderately Active\n"
             << "4. Very Active\n"
             << "Enter choice: ";

        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice < 1 || choice > 4)
        {
            cout << "Choice out of range. Enter 1 to 4.\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        activityLevel = allActivityLevels[choice - 1];
        break;
    }
}

void registerUser(char choice)
{
	UserProfile ua;
	cout << "\n--- Register New User ---\n";
	
	    getValidUsername(ua.username,choice);

		getValidPassword(ua.password, ua.username, choice);
		
		getValidFullname(ua.fullName);
		
		getValidAge(ua.age);
		
		getValidGender(ua.gender);
		
		getValidHeight(ua.height_cm);
	
		getValidWeight(ua.weight_kg);
		
		getValidActivityLevel(ua.activityLevel);
        
        calcMacroTargets(ua);

        // Append mode and raw string literal for file path
        ofstream file("users.txt", ios::app);
        if (!file) {
            cout << "Error: Could not open file. Make sure the directory exists!" << endl;
            return;
        }

        file << ua.username << "|"
             << ua.password << "|"
             << "|user_" << ua.username << "_data.txt"
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
             << " 🎉✅ Profile created successfully! 👤 \n"
             << "--------------------------------------------\n\n";
}

bool loginUser( UserProfile& profile,char choice)
{
	cout << "\n--- Login ---\n";
	    string inputUsername, inputPassword;
	
	    getValidUsername(inputUsername, choice);
	    getValidPassword(inputPassword, inputUsername, choice);

	    cout << "\n\nLogin successful! \n";
	    cout << "Welcome back, 👤 " << inputUsername << "!\n\n";
	    cout << "👉 Press Enter to continue.....";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.get();
        string filename = "user_" + inputUsername + "_data.txt";
		loadProfileFromFile(filename,profile);
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
				registerUser(choice);
				
				// cout << "👉 Press Enter to login to your account...";
		        // cin.ignore(numeric_limits<streamsize>::max(), '\n');
				// cin.get();
				// system("cls");
				
				// // Call loginUser with choice = '2'
    			// loginUser('2');
    			
	            return; // exit main
	        }
	
		    case '2': 
			{
				isLoggedIn = loginUser(profile,choice);
			
			    return;  // exits this module and goes to main
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