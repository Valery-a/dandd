#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

struct PlayerProfile {
    char username[64];
    int mapChoice;
    int lives;
    int totalCoins;
    int playerX;
    int playerY;
};

void savePlayerProfile(const PlayerProfile &profile) {
    char filename[128];
    sprintf(filename, "%s.txt", profile.username);

    ofstream fout(filename);
    if(!fout.is_open()) {
        return;
    }

    fout << profile.username << endl
        << profile.mapChoice << endl
        << profile.lives << endl
        << profile.totalCoins << endl
        << profile.playerX << endl
        << profile.playerY << endl;
    fout.close();
}

bool loadPlayerProfile(const char* uname, PlayerProfile &profile) {
    char filename[128];
    sprintf(filename, "%s.txt", uname);

    ifstream fin(filename);
    if(!fin.is_open()) {
        return false;
    }
    fin >> profile.username
        >> profile.mapChoice
        >> profile.lives
        >> profile.totalCoins
        >> profile.playerX
        >> profile.playerY;
    fin.close();
    return true;
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
    //saveLevel(username, level, coins, lives);
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

    // if (loadLevel(username, level, coins, lives)) {
    //     cout << "Welcome back, " << username << ". Thou standest at Level " << level << ", with " << coins << " runes and " << lives << " vestiges of life remaining." << endl;
    // } else {
    //     cout << "A new journey begins. Bear the burden of grace, " << username << "." << endl;
    // }

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
        //saveLevel(username, level, coins, lives);
    }

    return 0;
}
