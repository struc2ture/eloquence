#include "eloq.h"
#include "raylib.h"

#include <assert.h>
#include <cstdlib>
#include <cstdio>

#define ATLAS_PATH "assets/curses.png"
#define PLAYER_ENTITY_I 0

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

void pushEntity(GameState *gs, Entity entity)
{
    assert(gs->entityCount < gs->maxEntityCount);
    gs->entities[gs->entityCount++] = entity;
}

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

void generateMap(GameState *gs, Tile *tiles, int width, int height)
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

    int maxHerbsCount = 1;
    for (int i = 0; i < maxHerbsCount; i++)
    {
        pushEntity(gs, Entity{
            Vector2i{ GetRandomValue(1, width - 2), GetRandomValue(1, height - 2) },
            HERB_GLYPH,
            BACKGROUND_COLOR,
            GREEN,
            false,
            false,
            false,
            true,
            true,
            3,
            "Jibrus"
        });
    }
}

inline bool checkValidPosition(GameState *gs, Vector2i p)
{
    bool valid = (p.x >= 0 && p.x < gs->mapSize.x &&
                  p.y >= 0 && p.y < gs->mapSize.y);
    return valid;
}

inline bool checkTileWalkable(GameState *gs, Tile *tiles, Vector2i p)
{
    return !tiles[p.y * gs->mapSize.x + p.x].blocksMovement;
}

inline bool v2iEqual(Vector2i a, Vector2i b)
{
    return a.x == b.x && a.y == b.y;
}

bool checkEntitiesWalkable(GameState *gs, Vector2i p)
{
    for (uint32_t entityI = 1; entityI < gs->entityCount; entityI++)
    {
        Entity *entity = &gs->entities[entityI];
        if (entity->isActive && entity->blocksMovement && v2iEqual(entity->pos, p))
        {
            return false;
        }
    }

    return true;
}

inline bool checkCanWalk(GameState *gs, Tile *tiles, Vector2i p)
{
    bool canWalk = (checkValidPosition(gs, p) &&
                    checkTileWalkable(gs, tiles, p) &&
                    checkEntitiesWalkable(gs, p));
    return canWalk;
}

inline bool isKeyPressedOrRepeat(int key)
{
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

bool checkEntityMovement(GameState *gs, Entity *entity)
{
    Vector2i testPosition = entity->pos;
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
            entity->pos = testPosition;
            return true;
        }
    }

    return false;
}

void processEntityTurns(GameState *gs)
{
    if (!gs->pendingEntityProcessed)
    {
        if (checkEntityMovement(gs, &gs->entities[gs->pendingEntityI]))
        {
            gs->pendingEntityProcessed = true;
        }
    }

    if (gs->pendingEntityProcessed)
    {
        uint32_t startI = gs->pendingEntityI + 1;
        if (startI >= gs->entityCount - 1)
        {
            startI = 0;
            // update turn counter if pending entity was the last entity in the list
            gs->currentTurn++;
        }

        gs->pendingEntityI = 0;
        
        for (uint32_t i = startI; i < gs->entityCount; i++)
        {
            Entity *entity = &gs->entities[i];
            if (entity->isActive)
            {
                if (entity->isPlayerControlled)
                {
                    gs->pendingEntityI = i;
                    gs->pendingEntityProcessed = false;
                    return;
                }
                else
                {
                    Vector2i randomMove = Vector2i{ GetRandomValue(-1, 1), 0 };
                    if (randomMove.x == 0)
                    {
                        randomMove.y = GetRandomValue(-1, 1);
                    }

                    if (randomMove.x != 0 || randomMove.y != 0)
                    {
                        Vector2i testPosition{ entity->pos.x + randomMove.x, entity->pos.y + randomMove.y };
                        if (checkCanWalk(gs, gs->tiles, testPosition))
                        {
                            entity->pos = testPosition;
                        }
                    }
                }
            }
        }

        // if there was a pending entity, there's an early return.
        gs->currentTurn++;
    }
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

EntitySearchResult getEntitiesAtPos(GameState *gs, Vector2i pos)
{
    EntitySearchResult result;
    result.count = 0;
    for (uint32_t i = 1; i < gs->entityCount; i++)
    {
        if (v2iEqual(gs->entities[i].pos, pos))
        {
            result.entities[result.count++] = &gs->entities[i];
        }
    }
    return result;
}

void init(GameState *gs)
{
    gs->cursesAtlas = LoadTexture(ATLAS_PATH);
    gs->mapSize = defaultMapSize;
    int tileCount = gs->mapSize.x * gs->mapSize.y;
    gs->tiles = (Tile*)malloc(tileCount * sizeof(Tile));
    gs->maxEntityCount = MAX_ENTITY_COUNT;

    gs->entities = (Entity*)malloc(gs->maxEntityCount * sizeof(Entity));
    pushEntity(gs, Entity{});
    pushEntity(gs, Entity{
        Vector2i{ gs->mapSize.x / 2, gs->mapSize.y / 2 },
        PLAYER_GLYPH,
        BACKGROUND_COLOR,
        FOREGROUND_ACTIVE_COLOR,
        true,
        true,
        true,
        true,
        false,
        5,
        "Player"
    });

    generateMap(gs, gs->tiles, gs->mapSize.x, gs->mapSize.y);

    pushEntity(gs, Entity{
        Vector2i{ gs->mapSize.x / 2 + 2, gs->mapSize.y / 2 },
        PLAYER_GLYPH,
        BACKGROUND_COLOR,
        RED,
        true,
        false,
        true,
        true,
        false,
        5,
        "NPC A"
    });
    pushEntity(gs, Entity{
        Vector2i{ gs->mapSize.x / 2 + 4, gs->mapSize.y / 2 },
        NPC_GLYPH,
        BACKGROUND_COLOR,
        GREEN,
        true,
        false,
        true,
        true,
        false,
        5,
        "NPC B"
    });

    gs->pendingEntityI = 0;
    gs->pendingEntityProcessed = true;
}

void update(GameState *gs)
{
    Entity *standingOverEntity = nullptr;
    {
        EntitySearchResult entities = getEntitiesAtPos(gs, gs->entities[1].pos);
        for (int i = 0; i < entities.count; i++)
        {
            Entity *e = entities.entities[i];
            if (e->canPickUp)
            {
                standingOverEntity = e;
            }
        }
    }

    if (!gs->pendingEntityProcessed && standingOverEntity)
    {
        if (IsKeyPressed(KEY_E))
        {
            printf("Picking up %s\n", standingOverEntity->name);
            gs->pendingEntityProcessed = true;
        }
    }

    processEntityTurns(gs);

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

        for (int drawLayer = 1; drawLayer <= 5; drawLayer++)
        {
            for (uint32_t i = 0; i < gs->entityCount; i++)
            {
                Entity *entity = &gs->entities[i];
                if (entity->isVisible && entity->drawLayer == drawLayer)
                {
                    drawGlyph(gs, entity->pos, entity->glyphCoord, entity->bgColor, entity->fgColor);
                }
            }
        }

        float width = GetScreenWidth();
        DrawText(TextFormat("Turn: %d", gs->currentTurn), width - 100, 10, 24, FOREGROUND_ACTIVE_COLOR);

        DrawText(TextFormat("Player pos: %d, %d", gs->entities[1].pos.x, gs->entities[1].pos.y), width - 200, 40, 24, FOREGROUND_ACTIVE_COLOR);
        DrawText(TextFormat("Plant pos: %d, %d", gs->entities[2].pos.x, gs->entities[2].pos.y), width - 200, 70, 24, FOREGROUND_ACTIVE_COLOR);

        EntitySearchResult entities = getEntitiesAtPos(gs, gs->entities[1].pos);
        for (int i = 0; i < entities.count; i++)
        {
            Entity *e = entities.entities[i];
            if (e->canPickUp)
            {
                float height = GetScreenHeight();
                DrawText(TextFormat("Pick up %s", e->name), 10, height - 30, 24, FOREGROUND_ACTIVE_COLOR);
            }
        }

        if (standingOverEntity)
        {
            float height = GetScreenHeight();
            DrawText(TextFormat("Pick up %s", standingOverEntity->name), 10, height - 30, 24, FOREGROUND_ACTIVE_COLOR);
        }

    EndDrawing();
}
