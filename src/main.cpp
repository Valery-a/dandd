#include <iostream>
#include <fstream>

using namespace std;

const int MAX_LEVELS = 3;
const int MAP_HEIGHT = 10;
const int MAP_WIDTH = 15;
const int MAX_COLLECTED_COINS = 100;

struct PairInt 
{
	int rows;
	int columns;
};

struct PlayerProfile 
{
	char username[50];
	int level;
	int mapChoice;
	int lives;
	int totalCoins;
	bool hasKey;
	int playerX;
	int playerY;
	PairInt collectedCoins[MAX_COLLECTED_COINS];
	int collectedCoinCount;
	int maxUnlockedLevel;
};

struct MapData 
{
	char map[MAP_HEIGHT][MAP_WIDTH];
	PairInt portals[10];
	int portalCount;
};

MapData currentMap;

void selectLevel (PlayerProfile &profile);

void clearScreen () 
{
	cout << "\033[2J\033[1;1H";
}

bool userExists (const char *username) 
{
	char filename[55];
	snprintf (filename, sizeof (filename), "%s.txt", username);
	ifstream file (filename);
	return file.is_open();
}

void removeCollectedCoinsFromMap (PlayerProfile &profile) 
{
	for (int i = 0; i < profile.collectedCoinCount; i++) 
	{
		PairInt &pos = profile.collectedCoins[i];
		if (currentMap.map[pos.rows][pos.columns] == 'C') 
		{
			currentMap.map[pos.rows][pos.columns] = ' ';
		}
	}
}

bool loadPlayerProfile (const char *uname, PlayerProfile &profile) 
{
	char filename[50];
	snprintf (filename, sizeof (filename), "%s.txt", uname);
	ifstream fin (filename);
	if (!fin.is_open()) 
	{
		return false;
	}

	fin >> profile.username
	    >> profile.level
	    >> profile.mapChoice
	    >> profile.lives
	    >> profile.totalCoins
	    >> profile.hasKey
	    >> profile.playerX
	    >> profile.playerY
	    >> profile.collectedCoinCount;

	for (int i = 0; i < profile.collectedCoinCount; i++) 
	{
		fin >> profile.collectedCoins[i].rows 
		    >> profile.collectedCoins[i].columns;
	}

	if (!(fin >> profile.maxUnlockedLevel)) 
	{
		profile.maxUnlockedLevel = (profile.level > 0) ? profile.level : 1;
	}

	fin.close();
	return true;
}

void savePlayerProfile (const PlayerProfile &profile) 
{
	char filename[50];
	snprintf (filename, sizeof (filename), "%s.txt", profile.username);
	ofstream fout (filename);

	if (!fout.is_open()) 
	{
		return;
	}

	fout << profile.username << endl
	     << profile.level << endl
	     << profile.mapChoice << endl
	     << profile.lives << endl
	     << profile.totalCoins << endl
	     << profile.hasKey << endl
	     << profile.playerX << endl
	     << profile.playerY << endl
	     << profile.collectedCoinCount << endl;

	for (int i = 0; i < profile.collectedCoinCount; i++) 
	{
		fout << profile.collectedCoins[i].rows 
		     << " " 
		     << profile.collectedCoins[i].columns 
		     << endl;
	}

	fout << profile.maxUnlockedLevel << endl;
	fout.close();
}

void myStrCpy (char *dest, const char *src, size_t n) 
{
	size_t i = 0;
	for (; i < n && src[i] != '\0'; i++) 
	{
		dest[i] = src[i];
	}

	for (; i < n; i++) 
	{
		dest[i] = '\0';
	}
}

bool loadMapFromFile (const char *fileName, MapData &mapData) 
{
	ifstream file (fileName);

	if (!file.is_open()) 
	{
		cerr << "Failed to open map file: " << fileName << endl;
		return false;
	}

	for (int i = 0; i < MAP_HEIGHT; i++) 
	{
		char line[MAP_WIDTH + 1] = { 0 };
		file.getline (line, sizeof (line));

		if (strlen (line) < MAP_WIDTH) 
		{
			cerr << "Error reading map file: " << fileName << endl;
			return false;
		}

		myStrCpy (mapData.map[i], line, MAP_WIDTH);
	}

	file.close();
	mapData.portalCount = 0;

	for (int i = 0; i < MAP_HEIGHT; i++) 
	{
		for (int j = 0; j < MAP_WIDTH; j++) 
		{
			if (mapData.map[i][j] == '%') 
			{
				if (mapData.portalCount < 10) 
				{
					mapData.portals[mapData.portalCount].rows = i;
					mapData.portals[mapData.portalCount].columns = j;
					mapData.portalCount++;
				}
			}
		}
	}

	return true;
}

void loadMapForLevel (PlayerProfile &profile) 
{
	int levelIndex = profile.level - 1;

	if (levelIndex < 0 || levelIndex >= MAX_LEVELS) 
	{
		cerr << "Invalid level index: " << levelIndex << endl;
		return;
	}

	if (profile.mapChoice < 0 || profile.mapChoice >= 2) 
	{
		srand (static_cast<unsigned> (time (0)));
		profile.mapChoice = rand() % 2;
	}

	char fileName[100];
	snprintf (fileName, sizeof (fileName), "level%d_map%d.txt", profile.level, profile.mapChoice + 1);
	cout << "Loading map file: " << fileName << endl;
	ifstream fileCheck (fileName);

	if (!fileCheck.good()) 
	{
		cerr << "Map file not found: " << fileName << endl;
		memset (currentMap.map, ' ', sizeof (currentMap.map));
		return;
	}

	fileCheck.close();

	if (!loadMapFromFile (fileName, currentMap)) 
	{
		cerr << "Error loading map. Defaulting to blank map." << endl;
		memset (currentMap.map, ' ', sizeof (currentMap.map));
	}

	removeCollectedCoinsFromMap (profile);
	profile.playerX = 1;
	profile.playerY = 1;
	currentMap.map[profile.playerX][profile.playerY] = '@';
}

void printMap (const PlayerProfile &prof) 
{
	clearScreen();
	cout << "[Q] EXIT" << endl;
	cout << "[P] CHANGE PROFILE" << endl;
	cout << "User playing: " << prof.username << endl;
	cout << "Level chosen: " << prof.level << endl;
	cout << "Lives: " << prof.lives << endl;
	cout << "Coins: " << prof.totalCoins << endl;
	cout << "Key: " << (prof.hasKey ? "✓" : "⨯") << endl;
	cout << "-------------------------------" << endl;

	for (int i = 0; i < MAP_HEIGHT; i++) 
	{
		for (int j = 0; j < MAP_WIDTH; j++) 
		{
			cout << currentMap.map[i][j] << ' ';
		}
		cout << endl;
	}

	cout << "-------------------------------" << endl;
}

void processMove (PlayerProfile &profile, char move) 
{
	int playerx = profile.playerX;
	int playery = profile.playerY;
	int newx = playerx;
	int newy = playery;

	switch (move) 
	{
		case 'W':
		case 'w':
			newx--;
			break;
		case 'S':
		case 's':
			newx++;
			break;
		case 'A':
		case 'a':
			newy--;
			break;
		case 'D':
		case 'd':
			newy++;
			break;
		default:
			return;
	}

	char dest = currentMap.map[newx][newy];

	if (dest == '#') 
	{
		profile.lives--;

		if (profile.lives <= 0) 
		{
			cout << "You ran out of lives! Restarting level..." << endl;
			cin.ignore (1000, '\n');
			cin.get ();
			profile.lives = 3;
			loadMapForLevel (profile);
		}
		return;
	}

	if (dest == 'X' && profile.hasKey) 
	{
		profile.hasKey = false;
		profile.level++;

		if (profile.level > profile.maxUnlockedLevel) 
		{
			profile.maxUnlockedLevel = profile.level;
		}

		profile.playerX = -1;
		profile.playerY = -1;
		profile.mapChoice = -1;
	}
	else if (dest == 'X' && !profile.hasKey) 
	{
		cout << "You don't have a key yet, go find it. [ENTER] (ok)";
		cin.get();
		return;
	}

	if (dest == 'C') 
	{
		profile.totalCoins++;

		if (profile.collectedCoinCount < MAX_COLLECTED_COINS) 
		{
			profile.collectedCoins[profile.collectedCoinCount++] = { newx, newy };
		}
	}

	if (currentMap.map[playerx][playery] == '@') 
	{
		bool wasOnTeleporter = false;

		for (int i = 0; i < currentMap.portalCount; i++) 
		{
			if (currentMap.portals[i].rows == playerx && currentMap.portals[i].columns == playery) 
			{
				wasOnTeleporter = true;
				break;
			}
		}

		if (wasOnTeleporter) 
		{
			currentMap.map[playerx][playery] = '%';
		}
		else 
		{
			currentMap.map[playerx][playery] = ' ';
		}
	}

	switch (dest) 
	{
		case '&':
			profile.hasKey = true;
			break;
		case '%':
		{
			for (int i = 0; i < currentMap.portalCount; i++) 
			{
				if (currentMap.portals[i].rows == newx && currentMap.portals[i].columns == newy) 
				{
					int next = (i + 1) % currentMap.portalCount;
					newx = currentMap.portals[next].rows;
					newy = currentMap.portals[next].columns;
					break;
				}
			}
			break;
		}
		default:
			break;
	}

	currentMap.map[newx][newy] = '@';
	profile.playerX = newx;
	profile.playerY = newy;
}

void mainMenu (PlayerProfile &profile) 
{
	int choice;

	while (true) 
	{
		clearScreen();
		cout << "=== Main Menu ===" << endl;
		cout << "1. Login" << endl;
		cout << "2. Register" << endl;
		cout << "3. Exit" << endl;
		cout << "Enter your choice: ";

		if (cin.fail() || !(cin >> choice)) 
		{
			cin.clear();
			cin.ignore (1000, '\n');
			cout << "Invalid input. Please enter a number. [ENTER]" << endl;
			cin.get();
			continue;
		}

		switch (choice) 
		{
			case 1:
			{
				cout << "Enter username: ";
				cin >> profile.username;

				if (loadPlayerProfile (profile.username, profile)) 
				{
					cout << "Welcome back, " << profile.username << "!" << endl;
					cin.ignore (1000, '\n');
					cin.get ();
					selectLevel (profile);
					return;
				}
				else 
				{
					cout << "Username not found. [ENTER]" << endl;
					cin.ignore (1000, '\n');
					cin.get();
				}
				break;
			}
			case 2:
			{
				cout << "Enter a new username (50 characters): ";

				while (true) 
				{
					cin >> profile.username;

					if (strlen (profile.username) > 50) 
					{
						cout << "Username cannot exceed 50 characters.";
					}
					else if (userExists (profile.username)) 
					{
						cout << "Username already in use.";
					}
					else 
					{
						cout << "Successfuly registered. [ENTER]" << endl;
						cin.ignore (1000, '\n');
						cin.get();
						break;
					}
				}

				profile.level = 1;
				profile.maxUnlockedLevel = 1;
				profile.mapChoice = -1;
				profile.lives = 3;
				profile.totalCoins = 0;
				profile.collectedCoinCount = 0;
				profile.hasKey = false;
				profile.playerX = -1;
				profile.playerY = -1;
				savePlayerProfile (profile);
				cout << "Registration successful! Welcome, " << profile.username << "." << endl;
				selectLevel (profile);
				return;
			}
			case 3:
				cout << "Exiting the game" << endl;
				exit (0);
			default:
				cout << "Invalid choice. [ENTER]" << endl;
				cin.ignore (1000, '\n');
				cin.get();
				break;
		}
	}
}

void changeProfile (PlayerProfile &profile) 
{
	char newUsername[50];
	cout << "Enter the username of the profile you want to switch to: ";
	cin >> newUsername;

	if (loadPlayerProfile (newUsername, profile)) 
	{
		cout << "Switched to profile: " << newUsername << endl;
		savePlayerProfile (profile);
		cin.ignore (1000, '\n');
		cin.get();
		selectLevel (profile);
	}
	else 
	{
		cout << "Profile not found. Please ensure the username is correct. [ENTER]" << endl;
		cin.ignore (1000, '\n');
		cin.get();
	}
}

void selectLevel (PlayerProfile &profile) 
{
	while (true) 
	{
		clearScreen();
		cout << "=== Select Level ===" << endl;

		for (int i = 1; i <= MAX_LEVELS; i++) 
		{
			cout << i << ". Level " << i;
			if (i > profile.maxUnlockedLevel) 
			{
				cout << " (Locked)";
			}
			cout << endl;
		}

		cout << "4. Back to main menu" << endl;
		cout << "5. Reset progress for the last played level" << endl;
		cout << "Enter your choice: ";

		int choice;
		cin >> choice;

		if (cin.fail()) 
		{
			cin.clear();
			cin.ignore (1000, '\n');
			cout << "Invalid input. Please enter a number. [ENTER]" << endl;
			cin.get();
			continue;
		}

		if (choice >= 1 && choice <= MAX_LEVELS) 
		{
			if (choice > profile.maxUnlockedLevel) 
			{
				cout << "Level " << choice << " is locked. Complete Level " 
				     << (choice - 1) << " first. [ENTER]" << endl;
				cin.ignore (1000, '\n');
				cin.get();
			}
			else 
			{
				profile.level = choice;
				loadMapForLevel (profile);
				return;
			}
		}
		else if (choice == 4) 
		{
			mainMenu (profile);
			return;
		}
		else if (choice == 5) 
		{
			cout << "Are you sure you want to reset progress for the last played level? (Y/N): ";
			char confirmation;
			cin >> confirmation;

			if (confirmation == 'y' || confirmation == 'Y') 
			{
				if (profile.level <= 1) 
				{
					cout << "You cannot reset Level 1. [ENTER]" << endl;
					cin.ignore (1000, '\n');
					cin.get();
					continue;
				}

				int coinsToRemove = 0;

				for (int i = 0; i < profile.collectedCoinCount; i++) 
				{
					PairInt &coin = profile.collectedCoins[i];
					if (currentMap.map[coin.rows][coin.columns] == ' ') 
					{
						coinsToRemove++;
					}
				}

				profile.totalCoins -= coinsToRemove;
				if (profile.totalCoins < 0) 
				{
					profile.totalCoins = 0;
				}

				profile.collectedCoinCount = 0;
				profile.mapChoice = -1;
				profile.playerX = -1;
				profile.playerY = -1;
				profile.hasKey = false;

				if (profile.level == profile.maxUnlockedLevel && profile.maxUnlockedLevel > 1) 
				{
					profile.maxUnlockedLevel--;
				}

				profile.level--;
				if (profile.level < 1) 
				{
					profile.level = 1;
				}

				loadMapForLevel (profile);
				cout << "Progress for the last played level has been reset. [ENTER]" << endl;
				cin.ignore (1000, '\n');
				cin.get();
			}
			else 
			{
				cout << "Reset canceled. [ENTER]" << endl;
				cin.ignore (1000, '\n');
				cin.get();
			}
		}
		else 
		{
			cout << "Invalid choice. Try again. [ENTER]" << endl;
			cin.ignore (1000, '\n');
			cin.get();
		}
	}
}

int main () 
{
	PlayerProfile profile;
	srand (time (0));
	mainMenu (profile);
	bool running = true;
	loadMapForLevel (profile);
	int lastLoadedLevel = profile.level;

	while (running) 
	{
		if (profile.level > MAX_LEVELS) 
		{
			savePlayerProfile (profile);
			clearScreen();
			cout << "Congratulations " << profile.username 
			     << ", you completed all levels!" << endl;
			cin.ignore (1000, '\n');
			mainMenu (profile);
		}

		if (profile.lives <= 0) 
		{
			clearScreen();
			cout << "Game Over! You ran out of lives. Restarting the level..." << endl;
			cin.ignore (1000, '\n');
			profile.lives = 3;
			loadMapForLevel (profile);
			continue;
		}

		if (profile.level != lastLoadedLevel && profile.level <= MAX_LEVELS) 
		{
			loadMapForLevel (profile);
			lastLoadedLevel = profile.level;
		}

		printMap (profile);
		char command;
		cout << "(W/A/S/D to move, Q to quit, P to change profile): ";
		cin >> command;
		cin.ignore (1000, '\n');

		if (command == 'q' || command == 'Q') 
		{
			savePlayerProfile (profile);
			cout << "Level saved. Returning to the main menu... [ENTER]" << endl;
			cin.ignore (1000, '\n');
			mainMenu (profile);
			loadMapForLevel (profile);
			lastLoadedLevel = profile.level;
		}
		else if (command == 'p' || command == 'P') 
		{
			savePlayerProfile (profile);
			changeProfile (profile);
		}
		else if (command == 'w' || command == 'W' 
		      || command == 'a' || command == 'A' 
		      || command == 's' || command == 'S' 
		      || command == 'd' || command == 'D') 
		{
			processMove (profile, command);
		}
		else 
		{
			cout << "Invalid command. [ENTER]" << endl;
			cin.ignore (1000, '\n');
		}
	}

	savePlayerProfile (profile);
	clearScreen();
	return 0;
}
