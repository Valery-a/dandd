#include <iostream>
#include <fstream>

using namespace std;

const int MAX_LEVELS = 3;
const int MAP_HEIGHT = 10;
const int MAP_WIDTH  = 15;

struct PlayerProfile {
    char username[50];
    int level;
    int mapChoice;
    int lives;
    int totalCoins;
    bool hasKey;
    int playerX;
    int playerY;
};

struct PairInt {
    int rows;
    int columns;
};

struct MapData {
    char map[MAP_HEIGHT][MAP_WIDTH];
    PairInt portals[10];
    int portalCount;
};

MapData currentMap;

char level1_map1[MAP_HEIGHT][MAP_WIDTH+1] = {
    "###############",
    "#@    C % C % #",
    "###  ## #  C  #",
    "##   #  ##  ###",
    "#   %   ### ###",
    "# CC  C  C  C #",
    "#  C CCC # # C#",
    "# CC C   # & C#",
    "#  C C  #  # C#",
    "####%##X %#####"
};

char level1_map2[MAP_HEIGHT][MAP_WIDTH+1] = {
    "###############",
    "#@C  & %  C X #",
    "#  #########  #",
    "#  C    %  C  #",
    "#######  ######",
    "#  %  ##  ##  #",
    "#   C   C    C#",
    "#  ######  ####",
    "#    C  C  C  #",
    "###############"
};

char level2_map1[MAP_HEIGHT][MAP_WIDTH+1] = {
    "###############",
    "#@  #  C  %  &#",
    "# # ### ### # #",
    "#  C  #  X  % #",
    "# ###  ###  # #",
    "#  #  C  #  # #",
    "#   #%  #   # #",
    "#  ######  #  #",
    "#      C   #  #",
    "###############"
};

char level2_map2[MAP_HEIGHT][MAP_WIDTH+1] = {
    "###############",
    "#@C  C  %  #  #",
    "## # ## ## ## #",
    "#  %  C  #  X #",
    "#   ###### ## #",
    "# &  # C   #  #",
    "#   # % # ##  #",
    "#   #  ##     #",
    "#      C   ####",
    "###############"
};

char level3_map1[MAP_HEIGHT][MAP_WIDTH+1] = {
    "###############",
    "#@  C &  #  % #",
    "## # #### # # #",
    "#  %  # C  # X#",
    "# ###  ## # # #",
    "#   #   # % # #",
    "#  C# #  #  # #",
    "#   ###  #### #",
    "#  C C  %   C #",
    "###############"
};

char level3_map2[MAP_HEIGHT][MAP_WIDTH+1] = {
    "###############",
    "#@C # &   X % #",
    "# # # ## %% # #",
    "#  C  #   C # #",
    "## ##### # #  #",
    "#   #  #   #  #",
    "#   #   # ##  #",
    "#   ####   #  #",
    "#     C     C #",
    "###############"
};

char (*allMaps[MAX_LEVELS][2])[MAP_WIDTH+1] = {
    { level1_map1, level1_map2 },
    { level2_map1, level2_map2 },
    { level3_map1, level3_map2 }
};

void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

bool loadPlayerProfile(const char* uname, PlayerProfile &profile) {
    char filename[50];
    snprintf(filename, sizeof(filename), "%s.txt", uname);
    ifstream fin(filename);
    if(!fin.is_open()) {
        return false;
    }
    fin >> profile.username
        >> profile.level
        >> profile.mapChoice
        >> profile.lives
        >> profile.totalCoins
        >> profile.hasKey
        >> profile.playerX
        >> profile.playerY;
    fin.close();
    return true;
}

void savePlayerProfile(const PlayerProfile &profile) {
    char filename[50];
    snprintf(filename, sizeof(filename), "%s.txt", profile.username);

    ofstream fout(filename);
    if(!fout.is_open()) {
        return;
    }

    fout << profile.username << endl
        << profile.level << endl
        << profile.mapChoice << endl
        << profile.lives << endl
        << profile.totalCoins << endl
        << profile.hasKey << endl
        << profile.playerX << endl
        << profile.playerY << endl;
    fout.close();
}

void loadMapForLevel(PlayerProfile &prof) {
    int levelIndex = prof.level - 1;
    if(levelIndex < 0) levelIndex = 0;
    if(levelIndex >= MAX_LEVELS) levelIndex = MAX_LEVELS - 1;

    if(prof.mapChoice < 0 || prof.mapChoice >= 2) {
        prof.mapChoice = rand() % 2;
    }

    for(int i=0; i<MAP_HEIGHT; i++) {
        for(int j=0; j<MAP_WIDTH; j++) {
            currentMap.map[i][j] = allMaps[levelIndex][prof.mapChoice][i][j];
        }
    }

    currentMap.portalCount = 0;
    for(int i=0; i<MAP_HEIGHT; i++) {
        for(int j=0; j<MAP_WIDTH; j++) {
            if(currentMap.map[i][j] == '%') {
                currentMap.portals[currentMap.portalCount].rows = i;
                currentMap.portals[currentMap.portalCount].columns = j;
                currentMap.portalCount++;
            }
        }
    }

    if(prof.playerX < 0 || prof.playerX >= MAP_HEIGHT || prof.playerY < 0 || prof.playerY >= MAP_WIDTH)
    {
        for(int i=0; i<MAP_HEIGHT; i++) {
            for(int j=0; j<MAP_WIDTH; j++) {
                if(currentMap.map[i][j] == '@') {
                    prof.playerX = i;
                    prof.playerY = j;
                    break;
                }
            }
        }
    } else {
        currentMap.map[prof.playerX][prof.playerY] = '@';
    }
}

void printMap(const PlayerProfile &prof) {
    clearScreen();
    cout << "[Q] изход" << endl;
    cout << "User playing: " << prof.username << endl;
    cout << "Level chosen: " << prof.level << " (Map# " << prof.mapChoice << ")" << endl;
    cout << "Lives: " << prof.lives << endl;
    cout << "Coins: " << prof.totalCoins << endl;
    cout << "Key: " << (prof.hasKey ? "✓" : "⨯") << endl;
    cout << "-------------------------------" << endl;
    for(int i=0; i<MAP_HEIGHT; i++){
        for(int j=0; j<MAP_WIDTH; j++){
            cout << currentMap.map[i][j];
        }
        cout << endl;
    }
    cout << "-------------------------------" << endl;
}

void processMove(PlayerProfile &profile, char move) {
    int playerx = profile.playerX;
    int playery = profile.playerY;
    int newx = playerx;
    int newy = playery;

    switch(move) {
        case 'W': case 'w': newx--; break;
        case 'S': case 's': newx++; break;
        case 'A': case 'a': newy--; break;
        case 'D': case 'd': newy++; break;
        default:
            return;
    }

    char dest = currentMap.map[newx][newy];
    if(dest == '#') {
        profile.lives--;
        return;
    }

    currentMap.map[playerx][playery] = ' ';

    switch(dest) {
        case 'C':
            profile.totalCoins++;
            break;
        case '&':
            profile.hasKey = true;
            break;
        case 'X':
            if(profile.hasKey) {
                profile.level++;
                profile.playerX = -1;
                profile.playerY = -1;
                profile.mapChoice = -1;
            }
            break;
        case '%':
            for(int i=0; i<currentMap.portalCount; i++){
                if(currentMap.portals[i].rows == newx && currentMap.portals[i].columns == newy) {
                    int next = (i + 1) % currentMap.portalCount;
                    newx = currentMap.portals[next].rows;
                    newy = currentMap.portals[next].columns;
                    break;
                }
            }
            break;
        default:
            break;
    }

    currentMap.map[newx][newy] = '@';
    profile.playerX = newx;
    profile.playerY = newy;
}

int main() {
    PlayerProfile profile;
    if(!mainMenu(profile)) {
        return 0;
    }

    bool running = true;
    loadMapForLevel(profile);
    int lastLoadedLevel = profile.level;

    while(running) {
        if(profile.level > MAX_LEVELS) {
            clearScreen();
            break;
        }
        if(profile.lives <= 0) {
            clearScreen();
            break;
        }

        if(profile.level != lastLoadedLevel && profile.level <= MAX_LEVELS) {
            loadMapForLevel(profile);
            lastLoadedLevel = profile.level;
        }
        
        printMap(profile);

        cout << "Ход (W/A/S/D/Q): ";
        char command;
        cin >> command;
        cin.ignore(1000, '\n');

        if(command == 'q' || command == 'Q') {
            savePlayerProfile(profile);
            running = false;
        }
        else if(command == 'w' || command == 'W' ||
                command == 'a' || command == 'A' ||
                command == 's' || command == 'S' ||
                command == 'd' || command == 'D')
        {
            processMove(profile, command);
        }
        else {
            //invalid command
        }
    }

    savePlayerProfile(profile);
    clearScreen();
    return 0;
}
