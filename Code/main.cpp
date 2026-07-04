#include "Game.h"
#include <cstdlib>
#include <ctime>

/*
Entry point for Snow Bros.
Assalamualikum!
please run the game.
 */
int main() {
    srand((unsigned)time(nullptr));
    game game;
    game.run();
    return 0;
}
