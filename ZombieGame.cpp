#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

const int WIDTH = 10;
const int HEIGHT = 10;
char map[HEIGHT][WIDTH];

char tileUnderPlayer = '.';
// Player position
int playerX = 0;
int playerY = 0;
int playerHealth = 100;
int playerScore = 0;
int zombieDamage = 10;
bool playerHit = false;

struct Zombie {
    int x;
    int y;
    char tileUnder; // tile the zombie currently stands on ('.' or 'R' or 'E' etc)
};

vector<Zombie> zombies;

void initializeMap() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                map[i][j] = '#'; // barrier
            } else {
                map[i][j] = '.'; // empty space
            }
        }
    }
}



void generateObjects(int zombieCount, int buildingCount) {
    // Place player and exit first so other placements avoid them
    map[1][1] = 'P';
    playerX = 1;
    playerY = 1;
    tileUnderPlayer = '.'; // starting underlying tile

    map[HEIGHT - 2][WIDTH - 2] = 'E';

    // Place buildings (only on '.')
    int placedBuildings = 0;
    while (placedBuildings < buildingCount)
    {
        int x = rand() % HEIGHT;
        int y = rand() % WIDTH;
        if (map[x][y] == '.')
        {
            map[x][y] = 'B';
            placedBuildings++;
        }
    }
    //place health packs only on '.' blank spaces
    int healthPackCount = 4;
    int placedHealthPacks = 0;
    while (placedHealthPacks < healthPackCount)
    {
        int x = rand() % HEIGHT;
        int y = rand() % WIDTH;
        if (map[x][y] == '.')
        {
            map[x][y] = 'H';
            placedHealthPacks++;
        }
    }
    // Place zombies (allow spawning on '.' or 'R' but avoid P and E and B)
    int placedZombies = 0;
    while (placedZombies < zombieCount)
    {
        int x = rand() % HEIGHT;
        int y = rand() % WIDTH;
        // allow on '.' or 'R' only (not on P, E, B, or existing Z)
        if (map[x][y] == '.' || map[x][y] == 'R')
        {
            Zombie z;
            z.x = x;
            z.y = y;
            z.tileUnder = map[x][y];
            zombies.push_back(z);
            map[x][y] = 'Z';
            placedZombies++;
        }
    }
}


void printMap() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            cout << setw(2) << map[i][j] << ' ';
        }
        cout << endl;
    }
}

void movePlayer(char move) {
    int newX = playerX;
    int newY = playerY;

    if (move == 'W' || move == 'w') newX = max(0, playerX - 1);
    if (move == 'S' || move == 's') newX = min(HEIGHT - 1, playerX + 1);
    if (move == 'A' || move == 'a') newY = max(0, playerY - 1);
    if (move == 'D' || move == 'd') newY = min(WIDTH - 1, playerY + 1);

    char destination = map[newX][newY];

    if (destination == 'H')
    {
        int healthGain = 20;
        playerHealth += healthGain;
        if (playerHealth > 100)
        {
            playerHealth = 100; // Cap health at 100
        }

        int scoreBonus = 5;
        playerScore += scoreBonus;
        cout << "You picked up a health pack! Health +" << healthGain << ", Score +" << scoreBonus << "!" << endl;

        destination = '.'; // Remove health pack from map
    }


    // If destination is a building, cancel move
    if (destination == 'B') {
        return;
    }

    if (destination == '#') {
        return;//cannot move into barriers
    }
    
    if (destination == 'Z') {
        playerHealth -= zombieDamage;
        cout << "A zombie bites you! Health -" << zombieDamage << "!" << endl;
        if (playerHealth <= 0) {
            cout << "You have been eaten by zombies! Game Over!" << endl;
            exit(0);
        }
    }
    
    if (playerHealth <= 0)
    {
        cout << "You have been eaten by zombies! Game Over!" << endl;
        exit(0);
    }
    

    // If destination is exit -> win
    if (destination == 'E') {
        cout << "You reached the exit. You Escaped!" << endl;
        exit(0);
    }

    // Restore the tile under the player where they left
    map[playerX][playerY] = tileUnderPlayer;

    // Save the tile under the new player position and place the player
    tileUnderPlayer = destination;
    playerX = newX;
    playerY = newY;
    map[playerX][playerY] = 'P';
    playerScore++;

}

void moveZombies()
{
    if (zombies.empty()) return;

    static const int dx[4] = {-1, 1, 0, 0};
    static const int dy[4] = {0, 0, -1, 1};

    // targetTaken prevents multiple zombies moving into same tile
    vector<vector<bool>> targetTaken(HEIGHT, vector<bool>(WIDTH, false));
    for (const Zombie &z : zombies) {
        targetTaken[z.x][z.y] = true;
    }

    // Planned moves
    struct Move { int fromX, fromY, toX, toY, idx; char fromTile; };
    vector<Move> moves;
    moves.reserve(zombies.size());

    for (size_t i = 0; i < zombies.size(); ++i) {
        Zombie &z = zombies[i];

        // collect possible moves (stay + 4 neighbors)
        vector<pair<int,int>> options;
        options.push_back({z.x, z.y}); // stay

        for (int d = 0; d < 4; ++d) {
            int nx = z.x + dx[d];
            int ny = z.y + dy[d];
            if (nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH) continue;
            char c = map[nx][ny];
            // can't move into buildings or other zombies (current map)
            if (c == 'B' || c == 'Z' || c == '#' || c == 'H') continue;
            options.push_back({nx, ny});
        }

        // pick a random starting index among options and try to reserve a tile
        int choiceIndex = rand() % options.size();
        bool moved = false;
        for (int attempt = 0; attempt < (int)options.size(); ++attempt) {
            int idx = (choiceIndex + attempt) % options.size();
            int tx = options[idx].first;
            int ty = options[idx].second;
            // if moving onto player -> attack immediately
            if (tx == playerX && ty == playerY) {
                playerHealth -= zombieDamage;
                cout << "A zombie bites you! Health -" << zombieDamage << "!" << endl;
                if (playerHealth <= 0) {
                    cout << "You have been eaten by zombies! Game Over!" << endl;
                    exit(0);
                }
                // stay in place after attack
                moves.push_back({z.x, z.y, z.x, z.y, (int)i, z.tileUnder});
                moved = true;
                break;
            }
            if (!targetTaken[tx][ty]) {
                // reserve it
                targetTaken[tx][ty] = true;
                moves.push_back({z.x, z.y, tx, ty, (int)i, z.tileUnder});
                moved = true;
                break;
            }
        }
        if (!moved) {
            // stay in place (reserve current tile is already true)
            moves.push_back({z.x, z.y, z.x, z.y, (int)i, z.tileUnder});
        }
    }

    // Apply moves: restore old tiles for zombies that moved away
    for (const Move &m : moves) {
        if (!(m.fromX == m.toX && m.fromY == m.toY)) {
            map[m.fromX][m.fromY] = m.fromTile;
        }
    }

    // Place zombies at new positions and update their stored state
    for (const Move &m : moves) {
        Zombie &z = zombies[m.idx];
        z.tileUnder = map[m.toX][m.toY]; // remember what we overwrite
        z.x = m.toX;
        z.y = m.toY;
        map[z.x][z.y] = 'Z';
    }
}

int main() {
    srand((unsigned)time(0));

    // Timer setup using time()
    time_t startTime = time(nullptr);
    const int durationSeconds = 300; // 5 minutes; change to suit

    initializeMap();
    generateObjects(10, 6);


    while (true) {
        // compute remaining time
        int elapsed = static_cast<int>(time(nullptr) - startTime);
        int remaining = durationSeconds - elapsed;
        if (remaining < 0) remaining = 0;

        cout << "\033[2J\033[1;1H"; // Clear screen and move cursor to top-left

        // print timer header
        int minutes = remaining / 60;
        int seconds = remaining % 60;
        cout << "Time left: " << minutes << ':' << setw(2) << setfill('0') << seconds 
        << setfill(' ') 
        << " | Health: " << playerHealth 
        << " | Score: " << playerScore 
        << '\n';


        printMap();
        cout << "Use W/A/S/D to move. Reach E to escape!" << endl;

        // Check timeout
        if (remaining == 0) {
            cout << "Time's up! Game Over!" << endl;
            break;
        }

        // Check win condition
        if (playerX == HEIGHT - 1 && playerY == WIDTH - 1) {
            cout << "You reached the exit. Game Over!" << endl;
            break;
        }

        char move;
        cin >> move;
        movePlayer(move);

        // Move zombies after player acts
        moveZombies();
    }

    return 0;
}