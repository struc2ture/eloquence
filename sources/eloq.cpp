#include "eloq.h"

#include <cstdlib>

#define ATLAS_PATH "assets/curses.png"

const Vector2 atlasTileDim{ 24.0f, 36.0f };
const int atlasCols{ 16 };
const Color colors[] = {
    LIGHTGRAY,
    GRAY,
    DARKGRAY,
    YELLOW,
    GOLD,
    ORANGE,
    PINK,
    RED,
    MAROON,
    GREEN,
    LIME,
    DARKGREEN,
    SKYBLUE,
    BLUE,
    DARKBLUE,
    PURPLE,
    VIOLET,
    DARKPURPLE,
    BEIGE,
    BROWN,
    DARKBROWN
};
const Vector2i defaultMapSize{ 30, 15 };

void generateRandomTiles(Tile *tiles, int tileCount)
{
    for (int i = 0; i < tileCount; i++)
    {
        tiles[i] = Tile{
            Vector2i{ GetRandomValue(0, 16), GetRandomValue(0, 16) },
            colors[GetRandomValue(0, ARRAY_COUNT(colors))],
            colors[GetRandomValue(0, ARRAY_COUNT(colors))],
            false
        };
    }
}

void generateMap(Tile *tiles, int width, int height)
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            tiles[y * width + x] = Tile{
                GROUND_GLYPH,
                BACKGROUND_COLOR,
                FOREGROUND_COLOR,
                false
            };
        }
    }

    for (int x = 0; x < width; x++)
    {
        int y = 0;
        tiles[y * width + x] = Tile{
            WALL_GLYPH,
            BACKGROUND_COLOR,
            FOREGROUND_COLOR,
            true
        };

        y = height - 1;
        tiles[y * width + x] = Tile{
            WALL_GLYPH,
            BACKGROUND_COLOR,
            FOREGROUND_COLOR,
            true
        };
    }

    for (int y = 0; y < height; y++)
    {
        int x = 0;
        tiles[y * width + x] = Tile{
            WALL_GLYPH,
            BACKGROUND_COLOR,
            FOREGROUND_COLOR,
            true
        };

        x = width - 1;
        tiles[y * width + x] = Tile{
            WALL_GLYPH,
            BACKGROUND_COLOR,
            FOREGROUND_COLOR,
            true
        };
    }
}

bool checkValidPosition(GameState *gs, Vector2i p)
{
    bool valid = (p.x >= 0 && p.x < gs->mapSize.x &&
                  p.y >= 0 && p.y < gs->mapSize.y);
    return valid;
}

bool checkCanWalk(GameState *gs, Tile *tiles, Vector2i p)
{
    bool canWalk = checkValidPosition(gs, p) && !tiles[p.y * gs->mapSize.x + p.x].blocksMovement;
    return canWalk;
}

inline bool isKeyPressedOrRepeat(int key)
{
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

bool checkPlayerMovement(GameState *gs)
{
    Vector2i testPosition = gs->playerPosition;
    bool moveRequested = false;

    if (isKeyPressedOrRepeat(KEY_A))
    {
        testPosition.x--;
        moveRequested = true;
    }
    else if (isKeyPressedOrRepeat(KEY_D))
    {
        testPosition.x++;
        moveRequested = true;
    }
    else if (isKeyPressedOrRepeat(KEY_W))
    {
        testPosition.y--;
        moveRequested = true;
    }
    else if (isKeyPressedOrRepeat(KEY_S))
    {
        testPosition.y++;
        moveRequested = true;
    }

    if (moveRequested)
    {
        if (checkCanWalk(gs, gs->tiles, testPosition))
        {
            gs->playerPosition = testPosition;
            return true;
        }
    }

    return false;
}

Rectangle getAtlasRect(Vector2i coord)
{
    float x = atlasTileDim.x * coord.x;
    float y = atlasTileDim.y * coord.y;
    Rectangle rect{ x, y, atlasTileDim.x, atlasTileDim.y };
    return rect;
}

Vector2 getPixelPos(Vector2i coord)
{
    Vector2 p{ coord.x * atlasTileDim.x, coord.y * atlasTileDim.y };
    return p;
}

void drawGlyph(GameState *gs, Vector2i coord, Vector2i glyphCoord, Color bgColor, Color fgColor)
{
    auto pos = getPixelPos(coord);
    Rectangle bgRect{ pos.x, pos.y, atlasTileDim.x, atlasTileDim.y };
    DrawRectangleRec(bgRect, bgColor);
    DrawTextureRec(gs->cursesAtlas, getAtlasRect(glyphCoord), pos, fgColor);
}

void init(GameState *gs)
{
    gs->cursesAtlas = LoadTexture(ATLAS_PATH);
    gs->mapSize = defaultMapSize;
    int tileCount = gs->mapSize.x * gs->mapSize.y;
    gs->tiles = (Tile*)malloc(tileCount * sizeof(Tile));
    generateMap(gs->tiles, gs->mapSize.x, gs->mapSize.y);
    gs->playerPosition = Vector2i{ gs->mapSize.x / 2, gs->mapSize.y / 2 };
}

void update(GameState *gs)
{
    checkPlayerMovement(gs);

    BeginDrawing();

        ClearBackground(BACKGROUND_COLOR);

        for (int x = 0; x < gs->mapSize.x; x++)
        {
            for (int y = 0; y < gs->mapSize.y; y++)
            {
                Tile *tile = &gs->tiles[y * gs->mapSize.x + x];
                drawGlyph(gs, Vector2i{x, y}, tile->glyphCoord, tile->bgColor, tile->fgColor);
            }
        }

        drawGlyph(gs, gs->playerPosition, PLAYER_GLYPH, BACKGROUND_COLOR, FOREGROUND_ACTIVE_COLOR);

    EndDrawing();
}
