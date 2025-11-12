#include <iostream> 

#include <iomanip>

#include <vector> 

#include <cstdlib> 

#include <ctime> 

using namespace std; 

 

const int WIDTH = 10; 

const int HEIGHT = 10; 

char map[HEIGHT][WIDTH]; 

 

// Player position 

int playerX = 0; 

int playerY = 0; 

 

void generateMap(int zombieCount, int buildingCount) { 

    for (int i = 0; i < HEIGHT; i++) { 

        for (int j = 0; j < WIDTH; j++) { 

            map[i][j] = '.'; 

        } 

    } 

    map[0][0] = 'P'; // Player start 

    map[HEIGHT - 1][WIDTH - 1] = 'E'; // Exit 
    

    int placedZombies = 0;
    while (placedZombies < zombieCount)
    {
        int x = rand() % HEIGHT;
        int y = rand() % WIDTH;
        if (map[x][y] == '.')
        {
            map[x][y] = 'Z';
            placedZombies++;
        }
    }

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

    // Remove player from current position 

    map[playerX][playerY] = '.'; 

 

    // Update position based on input 

    if (move == 'W' || move == 'w') playerX = max(0, playerX - 1); 

    if (move == 'S' || move == 's') playerX = min(HEIGHT - 1, playerX + 1); 

    if (move == 'A' || move == 'a') playerY = max(0, playerY - 1); 

    if (move == 'D' || move == 'd') playerY = min(WIDTH - 1, playerY + 1); 

 

    // Place player in new position 

    map[playerX][playerY] = 'P'; 

} 

 

int main() { 

    srand(time(0)); 

    generateMap(10, 6);// tells it to generate 10 zombies and 6 buildings  

 

    while (true) { 

        printMap(); 

        cout << "Use W/A/S/D to move. Reach E to escape!" << endl; 

 

        // Check win condition 

        if (playerX == HEIGHT - 1 && playerY == WIDTH - 1) { 

            cout << "You reached the exit. Game Over!" << endl; 

            break; 

        } 

 

        char move; 

        cin >> move; 

        movePlayer(move); 

 

        system("cls"); // For Linux/Mac to clear screen; use "cls" on Windows 

    } 

 

    return 0; 

} 