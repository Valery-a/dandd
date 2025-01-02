#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

struct Player {
    int x, y;
};

struct GameState {
    char map[HEIGHT][WIDTH];
    Player player;
};

GameState state;

const int WIDTH = 10;
const int HEIGHT = 10;
const int MAX_LIVES = 3;

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
}
