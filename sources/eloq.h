#pragma once

#include <raylib.h>

#define ARRAY_COUNT(array) (sizeof((array))/sizeof((array)[0]))

#define GROUND_GLYPH Vector2i{14, 2}
#define WALL_GLYPH Vector2i{3, 2}
#define BACKGROUND_COLOR Color{30, 20, 20, 255}
#define FOREGROUND_COLOR Color{120, 100, 100, 255}
#define FOREGROUND_ACTIVE_COLOR Color{220, 200, 200, 255}
#define PLAYER_GLYPH Vector2i{0, 4}

struct Vector2i
{
    int x;
    int y;
};

struct Tile
{
    Vector2i glyphCoord;
    Color bgColor;
    Color fgColor;
    bool blocksMovement;
};

struct GameState
{
    Texture2D cursesAtlas;
    Vector2i playerPosition;
    Tile* tiles;
    Vector2i mapSize;
};

void init(GameState *gs);
void update(GameState *gs);
