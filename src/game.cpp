#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

const int WIDTH = 10;
const int HEIGHT = 10;

struct Player {
    int x, y;
};

struct GameState {
    char map[HEIGHT][WIDTH];
    Player player;
    int coins;
};

GameState state;

bool isValidMove(int x, int y) {
    return x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH && state.map[x][y] != '#';
}

void initializeMap() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            state.map[i][j] = ' ';
        }
    }
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (rand() % 40 < 2) {
                state.map[i][j] = '#';
            }
        }
    }
    for (int i = 0; i < 5; i++) {
        int x, y;
        do {
            x = rand() % HEIGHT;
            y = rand() % WIDTH;
        } while (state.map[x][y] != ' ');
        state.map[x][y] = 'C';
    }
    do {
        state.player.x = rand() % HEIGHT;
        state.player.y = rand() % WIDTH;
    } while (state.map[state.player.x][state.player.y] != ' ');
    state.map[state.player.x][state.player.y] = '@';
    state.coins = 0;
}

void printMap() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            cout << state.map[i][j] << ' ';
        }
        cout << endl;
    }
    cout << "Coins: " << state.coins << endl << endl;
}

void movePlayer(char direction) {
    int newX = state.player.x;
    int newY = state.player.y;
    switch (direction) {
        case 'w': newX--; break;
        case 's': newX++; break;
        case 'a': newY--; break;
        case 'd': newY++; break;
    }
    if (isValidMove(newX, newY)) {
        if (state.map[newX][newY] == 'C') state.coins++;
        state.map[state.player.x][state.player.y] = ' ';
        state.player.x = newX;
        state.player.y = newY;
        state.map[state.player.x][state.player.y] = '@';
    }
}

int main() {
    srand(time(0));
    initializeMap();
    while (true) {
        printMap();
        char input;
        cin >> input;
        movePlayer(tolower(input));
    }
    return 0;
}
