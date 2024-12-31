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
    cout << "Fare thee well, " << username << ". May the grace of the Erdtree guide thee henceforth." << endl;
}

void menu() {
    cout << "\nThe Tarnished Path:\n";
    cout << "1. Continue your pilgrimage\n";
    cout << "2. Begin a new quest\n";
    cout << "3. Erase your legacy\n";
    cout << "4. Abandon the lands\n";
    cout << "What is thy will, Tarnished? ";
}

int main() {
    char username[50];
    int level = 1, coins = 0, lives = 3;
    int choice;

    cout << "Speak thy name, Tarnished one: ";
    cin >> username;

    if (loadLevel(username, level, coins, lives)) {
        cout << "Welcome back, " << username << ". Thou standest at Level " << level << ", with " << coins << " runes and " << lives << " vestiges of life remaining." << endl;
    } else {
        cout << "A new journey begins. Bear the burden of grace, " << username << "." << endl;
    }

    while (true) {
        menu();
        cin >> choice;

        if (choice == 1) {
            cout << "The lands between beckon, " << username << ". Press forward." << endl;
        } else if (choice == 2) {
            level = 1;
            coins = 0;
            lives = 3;
            cout << "Your past is ash, and your future lies in shadow. Begin again." << endl;
        } else if (choice == 3) {
            resetProgress(username);
            level = 1;
            coins = 0;
            lives = 3;
        } else if (choice == 4) {
            exitGame(username, level, coins, lives);
            break;
        } else {
            cout << "An unwise choice, Tarnished. Choose again." << endl;
        }

        level++;
        coins += 100;
        if (lives > 0) lives--;

        cout << "Thy current plight: Level " << level << ", Runes " << coins << ", Vestiges of Life " << lives << "." << endl;
        saveLevel(username, level, coins, lives);
    }

    return 0;
}
