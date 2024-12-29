#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void saveLevel(const char* username, int level, int coins, int lives) {
    char filename[100];
    strcpy(filename, username);
    strcat(filename, "progress.txt");

    ofstream file(filename);
    if (file.is_open()) {
        file << "Level: " << level << endl;
        file << "Coins: " << coins << endl;
        file << "Lives: " << lives << endl;
        file.close();
    } else {
    }
}

void loadLevel(const char* username) {
    char filename[100];
    strcpy(filename, username);
    strcat(filename, "progress.txt");

    ifstream file(filename);
    if (file.is_open()) {
        char line[100];
        while (file.getline(line, 100)) {
            cout << line << endl;
        }
        file.close();
    } else {
    }
}

void checkProgress(int level, int coins) {
    char message[100];
    if (level >= 5 && coins >= 500) {
    } else if (level >= 3) {
    } else {
    }
    cout << message << endl;
}

void exitGame(const char* username, int level, int coins, int lives) {
    cout << "Exiting game and saving progress..." << endl;
    saveLevel(username, level, coins, lives);
    cout << "Goodbye, " << username << "!" << endl;
}

int main() {
    char username[50] = "player1";
    int level = 2;
    int coins = 150;
    int lives = 3;

    saveLevel(username, level, coins, lives);
    loadLevel(username);
    checkProgress(level, coins);

    level = 5;
    coins = 600;

    saveLevel(username, level, coins, lives);
    loadLevel(username);
    checkProgress(level, coins);

    exitGame(username, level, coins, lives);

    return 0;
}
