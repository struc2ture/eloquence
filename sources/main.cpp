#include <raylib.h>
#include <cstdlib>

#define ARRAY_COUNT(array) (sizeof((array))/sizeof((array)[0]))

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
};

static Vector2 atlasTileDim{ 24.0f, 36.0f };
static int atlasCols{ 16 };
static Color colors[] = {
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

void generateTiles(Tile *tiles, int tileCount)
{
    for (int i = 0; i < tileCount; i++)
    {
        tiles[i] = Tile{
            Vector2i{ GetRandomValue(0, 16), GetRandomValue(0, 16) },
            colors[GetRandomValue(0, ARRAY_COUNT(colors))],
            colors[GetRandomValue(0, ARRAY_COUNT(colors))]
        };
    }
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "ELOQ");
    SetTargetFPS(60);

    Texture2D texture = LoadTexture("assets/curses.png");

    int mapDim = 20;
    int tileCount = mapDim * mapDim;
    Tile* tiles = (Tile*)malloc(tileCount * sizeof(Tile));
    generateTiles(tiles, tileCount);

    float accum = 0.0f;

    while (!WindowShouldClose())
    {
        accum += GetFrameTime();
        if (accum >= 1.0f)
        {
            generateTiles(tiles, tileCount);
            accum -= 1.0f;
        }

        BeginDrawing();

            ClearBackground(Color{30, 20, 20, 255});

            DrawText("Congrats! You created your first window!", 190, 200, 20, Color{200, 210, 200, 255});

            for (int x = 0; x < mapDim; x++)
            {
                for (int y = 0; y < mapDim; y++)
                {
                    Tile *tile = &tiles[y * mapDim + x];
                    auto pos = getPixelPos(Vector2i{x, y});
                    Rectangle bgRect{ pos.x, pos.y, atlasTileDim.x, atlasTileDim.y };
                    DrawRectangleRec(bgRect, tile->bgColor);
                    DrawTextureRec(texture, getAtlasRect(tile->glyphCoord), pos, tile->fgColor);
                }
            }

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
