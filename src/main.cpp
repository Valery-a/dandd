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
        cout << "saved in" << filename << endl;
    } else {
        cout << "error during saving" << endl;
    }
}

void loadLevel(const char* username) {
    char filename[100];
    strcpy(filename, username);
    strcat(filename, "progress.txt");

    ifstream file(filename);
    if (file.is_open()) {
        char line[100];
        cout << "loaded progress for: " << username << ":" << endl;
        while (file.getline(line, 100)) {
            cout << line << endl;
        }
        file.close();
    } else {
        cout << "progress not found" << username << endl;
    }
}

int main() {
    char username[50] = "player1";
    int level = 2;
    int coins = 150;
    int lives = 3;

    saveLevel(username, level, coins, lives);
    loadLevel(username);
}
