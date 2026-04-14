#pragma once

#include <cstdint>

#include <raylib.h>

#define ARRAY_COUNT(array) (sizeof((array))/sizeof((array)[0]))

#define GROUND_GLYPH Vector2i{14, 2}
#define WALL_GLYPH Vector2i{3, 2}
#define HERB_GLYPH Vector2i{10, 2}
#define BACKGROUND_COLOR Color{30, 20, 20, 255}
#define FOREGROUND_COLOR Color{120, 100, 100, 255}
#define FOREGROUND_ACTIVE_COLOR Color{220, 200, 200, 255}
#define PLAYER_GLYPH Vector2i{0, 4}
#define NPC_GLYPH Vector2i{1, 0}
#define MAX_ENTITY_COUNT 1024

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

#define ENTITY_NAME_SIZE 256
struct Entity
{
    Vector2i pos;
    Vector2i glyphCoord;
    Color bgColor;
    Color fgColor;
    bool blocksMovement;
    bool isPlayerControlled;
    bool isActive;
    bool isVisible;
    bool canPickUp;
    int drawLayer;
    char name[ENTITY_NAME_SIZE];

    Entity* nextEntity;
    Entity* parentEntity;
    Entity* firstInventoryItem;
};

struct GameState
{
    Texture2D cursesAtlas;
    Tile* tiles;
    Vector2i mapSize;
    Entity *entities;
    int entityCount;
    int maxEntityCount;
    int currentTurn;
    uint32_t pendingEntityI;
    bool pendingEntityProcessed;
    Entity *standingOverEntity;
};

#define MAX_ENTITY_SEARCH_COUNT 256
struct EntitySearchResult
{
    Entity *entities[MAX_ENTITY_SEARCH_COUNT];
    int count;
};

void init(GameState *gs);
void update(GameState *gs);
