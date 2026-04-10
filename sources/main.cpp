#include <raylib.h>
#include <cstdlib>

#include "eloq.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "ELOQ");
    SetTargetFPS(60);

    GameState gs{};
    init(&gs);

    while (!WindowShouldClose())
    {
        update(&gs);
    }

    CloseWindow();

    return 0;
}
