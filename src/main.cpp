#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void saveLevel(const char* username, int level, int coins, int lives) {
    char filename[100];
    strcpy(filename, username);
    strcat(filename, "_progress.txt");

    ofstream file(filename);
    if (file.is_open()) {
        file << level << endl;
        file << coins << endl;
        file << lives << endl;
        file.close();
        cout << "Your tarnished legacy has been inscribed upon the annals of fate, " << username << "." << endl;
    } else {
        cout << "The fingers tremble... the record cannot be written." << endl;
    }
}

bool loadLevel(const char* username, int &level, int &coins, int &lives) {
    char filename[100];
    strcpy(filename, username);
    strcat(filename, "_progress.txt");

    ifstream file(filename);
    if (file.is_open()) {
        file >> level;
        file >> coins;
        file >> lives;
        file.close();
        cout << "The remnants of your journey have been unearthed, Tarnished " << username << "." << endl;
        return true;
    } else {
        cout << "No memory remains of your tarnished path, " << username << ". Begin anew." << endl;
        return false;
    }
}

void resetProgress(const char* username) {
    char filename[100];
    strcpy(filename, username);
    strcat(filename, "_progress.txt");

    ofstream file(filename, ios::trunc);
    if (file.is_open()) {
        file.close();
        cout << "Your legacy has been erased, forgotten by the lands between." << endl;
    } else {
        cout << "A curse lingers, preventing the erasure of your deeds." << endl;
    }
}

void exitGame(const char* username, int level, int coins, int lives) {
    cout << "The tarnished one rests, for now. Your journey has been recorded." << endl;
    saveLevel(username, level, coins, lives);
    cout << "Goodbye, " << username << "!" << endl;
}

int main() {
    char username[50];
    int level = 1, coins = 0, lives = 3;
    int choice;

    cout << "Speak thy name, Tarnished one: ";
    cin >> username;

    level = 5;
    coins = 600;

    while (true) {
        menu();
        cin >> choice;

    exitGame(username, level, coins, lives);

    return 0;
}
