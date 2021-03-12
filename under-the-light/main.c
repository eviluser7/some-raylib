/* *******************************************************************************
*
*   Under the Light
*
*   "You're afraid of the dark. Find the light and eat to stay alive"
*
*   Made with raylib v3.5.0
*   Made by eviluser7 (c) 2021
*
*********************************************************************************/

#include <stdio.h>
#include "raylib.h"
#define NEZ_TILEMAP_IMPLEMENTATION
#include "libs/tilemap.h"
#define NEZ_TILESET_IMPLEMENTATION
#include "libs/tileset.h"

#define TILE_SIZE 16
#define TILEMAP_WIDTH   16
#define TILEMAP_HEIGHT  10

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
    #include <emscripten.h>
#endif

// ----------------------
// Structures definition
typedef enum {
    TITLE = 0,
    GAME,
    ENDING
} GameScreen;

typedef struct {
    Vector2 position;
    int score;
    int state; // 0 - lose, 1 - alive
    int justStarted;
    int onTile;
    int hunger;
    int hungerCounter;
} Player;

typedef struct {
    int scene;
    int timer;
} Game;

// ----------------------
// Functions declaration
void InitGame(void);
void UpdateGame(void);
void DrawGame(void);
void UnloadGame(void);
void EndGame(void);
void UpdateDrawFrame(void); // for webpages

void UpdatePlayer(void);
void DrawPlayer(void);
void GetRandomLight(void);
void GetRandomApple(void);

// ----------------------
// Variables declaration
static Camera2D camera = { 0 };

static int screenWidth;
static int screenHeight;
static int screenScale;
static int selectedBackground = 0;
static int framesLightCounter = 0;
static int framesAppleCounter = 0;
static int timerReduce = 0;
static int playerFrames = 0;
static int framesSpeed = 8;
static int currentFrame = 0;
static int tileDetector = 0;

Texture2D background1;
Texture2D background2;
Texture2D character;
Texture2D logo;
Texture2D rlLogo;
Rectangle frameRec;
Sound defeat = { 0 };
Sound footsteps = { 0 };
Sound initialize = { 0 };
Sound pickupApple = { 0 };
Sound point = { 0 };
Sound startup = { 0 };
Player player;
Game game;

TileSet *tileSet;
TileMap *tileMap;
int map[TILEMAP_HEIGHT][TILEMAP_WIDTH] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static int randomLightX = -1;
static int randomLightY = -1;
static int previousLightTile = -1;
static int randomAppleX = -1;
static int randomAppleY = -1;
static int previousAppleTile = -1;

// Main entry point
int main(void)
{
    screenScale = 5.0;
    screenWidth = TILE_SIZE * TILEMAP_WIDTH * screenScale;
    screenHeight = TILE_SIZE * TILEMAP_HEIGHT * screenScale + 128;
    InitWindow(screenWidth, screenHeight, "Under the Light");
    InitAudioDevice();
    InitGame();
    PlaySound(initialize);
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    TileSetDestroy(tileSet);
    TileMapDestroy(tileMap);
    UnloadGame();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

// Initialize game variables
void InitGame(void)
{
    // Tilemap initialization
    tileSet = TileSetNewInitFromFile("resources/tileset.png", 16, 16, NULL, 0);
    tileMap = TileMapNew();
    tileMap->tileSet = tileSet;
    tileMap->x = 0;
    tileMap->y = 0;
    TileMapInitSize(tileMap, TILEMAP_WIDTH, TILEMAP_HEIGHT);
    TileMapSetGridData(tileMap, map, TILEMAP_WIDTH*TILEMAP_HEIGHT);

    // Resources initialization
    background1 = LoadTexture("resources/bg1.png");
    background2 = LoadTexture("resources/bg2.png");
    character = LoadTexture("resources/character.png");
    logo = LoadTexture("resources/logo.png");
    rlLogo = LoadTexture("resources/raylib_32x32.png");
    defeat = LoadSound("resources/defeat.wav");
    footsteps = LoadSound("resources/footsteps.wav");
    initialize = LoadSound("resources/init.wav");
    pickupApple = LoadSound("resources/pickup_apple.wav");
    point = LoadSound("resources/point.wav");
    startup = LoadSound("resources/startup.wav");

    // Non-texture values

    // Player values
    player.position.x = 128;
    player.position.y = 80;
    player.score = 0;
    player.state = 1;
    player.justStarted = 1;
    player.onTile = 0;
    player.hunger = 100;

    // Game values
    game.scene = TITLE;
    game.timer = 3;

    // Camera values
    camera.offset = (Vector2){0.0, 0.0};
    camera.target = (Vector2){0.0, 0.0};
    camera.zoom = screenScale;

    // Timing things
    framesLightCounter = 0;
    framesAppleCounter = 0;
    timerReduce = 0;
    playerFrames = 0;
    framesSpeed = 8;
    frameRec = (Rectangle){ 0.0f, 0.0f, (float)character.width/4, (float)character.height };
    currentFrame = 0;
    tileDetector = 0;
    
    // Tiles
    randomLightX = -1;
    randomLightY = -1;
    previousLightTile = -1;
    randomAppleX = -1;
    randomAppleY = -1;
}

// Update game logic
void UpdateGame(void)
{
    if (game.scene == TITLE) {
        if (IsKeyPressed(KEY_ENTER)) {
            InitGame();
            PlaySound(startup);
            game.scene = GAME;
        }
    }

    if (game.scene == GAME) {
        if (player.state == 1) UpdatePlayer();
        GetRandomLight();
        GetRandomApple();
        timerReduce++;

        if (timerReduce == 60) {
            game.timer--;
            timerReduce = 0;

            if (game.timer < 1) {
                game.timer = 3;
            }
        }
    }
    if (tileDetector == 180) tileDetector = 0;

    // Detect input
    if (game.scene == ENDING) {
        if (IsKeyPressed(KEY_R)) {
            InitGame();
            PlaySound(startup);
            game.scene = GAME;
        }
        if (IsKeyPressed(KEY_L)) game.scene = TITLE;
    }
}

// Draw game frame
void DrawGame(void)
{
    if (game.scene == TITLE) {
        DrawTexture(background2, 0, -30, WHITE);
        DrawTexture(logo, 63, 0, WHITE);
        DrawText("Press ENTER to begin", 10, 120, 8, WHITE);
        DrawText("Arrow keys to move", 10, 135, 8, WHITE);
        DrawText("Tip: don't leave the light", 10, 150, 8, WHITE);
        DrawText("v1.0", 5, 5, 8, WHITE);
        DrawText("Made by eviluser7 (c) 2021", 118, 172, 8, WHITE);

        DrawTexture(rlLogo, 220, 135, WHITE);
        DrawText("powered by", 192, 120, 8, WHITE);
    }

    if (game.scene == GAME) {
        DrawTexture(background1, 0, 0, WHITE);
        TileMapDrawExWorld(tileMap, 0, 0, screenWidth, screenHeight);
        if (player.state == 1) {
            DrawPlayer();
            DrawText(FormatText("Hunger: %d", player.hunger), 5, 175, 8, WHITE);
            DrawText(FormatText("Next light tile in: %ds", game.timer), 5, 165, 8, WHITE);
            DrawText(FormatText("  :::  Score: %d", player.score), 105, 165, 8, WHITE);
        }
    }

    if (game.scene == ENDING) {
        DrawText("You lost! :(", 75, 20, 20, WHITE);
        DrawText("Press R to try again", 25, 140, 20, WHITE);
        DrawText("Press L to leave", 37, 160, 20, WHITE);
        player.state = 0;
    }
}

// Unload game variables
void UnloadGame(void)
{
    UnloadTexture(background1);
    UnloadTexture(background2);
    UnloadTexture(character);
    UnloadTexture(logo);
    UnloadSound(defeat);
    UnloadSound(footsteps);
    UnloadSound(initialize);
    UnloadSound(pickupApple);
    UnloadSound(point);
    UnloadSound(startup);
}

// End the game
void EndGame(void)
{
    game.scene = ENDING;
}

// Update and draw
void UpdateDrawFrame(void)
{
    UpdateGame();

    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(BLACK);
    DrawGame();
    EndMode2D();
    EndDrawing();
}

// -----------------
// Player functions
void UpdatePlayer(void)
{
    // Updating hunger
    player.hungerCounter++;
    if (player.hungerCounter == 30) {
        player.hunger--;
        player.hungerCounter = 0;
    }

    if (player.hunger > 100) player.hunger = 100; // set a limit

    // Calculate player animation
    playerFrames++;

    if (playerFrames >= (60/framesSpeed)) {
        playerFrames = 0;
        currentFrame++;

        if (currentFrame > 3) currentFrame = 0;
        frameRec.x = (float)currentFrame*(float)character.width/4;
    }

    // Calculate player input
    if (IsKeyPressed(KEY_UP) && player.position.y > 0.0) {
        player.position.y -= 16.0;
        PlaySound(footsteps);
    }
    if (IsKeyPressed(KEY_LEFT) && player.position.x > 0.0) {
        player.position.x -= 16.0;
        PlaySound(footsteps);
    }
    if (IsKeyPressed(KEY_DOWN) && player.position.y < 144.0) {
        player.position.y += 16.0;
        PlaySound(footsteps);
    }
    if (IsKeyPressed(KEY_RIGHT) && player.position.x < 240.0) {
        player.position.x += 16.0;
        PlaySound(footsteps);
    }

    // Check if player is on certain tile
    int playerTileX = player.position.x/TILE_SIZE;
    int playerTileY = player.position.y/TILE_SIZE;

    if (playerTileX == randomLightX && playerTileY == randomLightY && player.justStarted < 1) {
        player.onTile = 1;
    } else if (playerTileX != randomLightX && playerTileY != randomLightY && player.justStarted < 1) {
        player.onTile = 0;
    }
    
    // Check if player is on apple tile
    if (playerTileX == randomAppleX && playerTileY == randomAppleY && player.justStarted < 1) {
        PlaySound(pickupApple);
        player.onTile = 2;
        player.score += 5;
        player.hunger += 10;
        TileMapSetTile(tileMap, randomAppleX, randomAppleY, 0);
        randomAppleX = -1;
        randomAppleY = -1;
        previousAppleTile = -1;
    }

    // Make player win points if they're standing on the light tile when the timer resets
    // else, lose the game
    tileDetector++;
    if (tileDetector == 180 && player.justStarted < 1 &&
        player.onTile == 1) {
            PlaySound(point);
            player.score += 10;
            tileDetector = 0;
    } else if (tileDetector == 180 && player.justStarted < 1 &&
        player.onTile == 0 || player.hunger < 1) {
            PlaySound(defeat);
            EndGame();
            player.state = 0;
            tileDetector = 0;
    }
}

void DrawPlayer(void)
{
    DrawTextureRec(character, frameRec, player.position, WHITE);
}

void GetRandomLight(void)
{
    framesLightCounter++;
    if (framesLightCounter >= 180) {  // Change every 3 seconds
        player.justStarted = 0;
        // Restore original tile value

        // Calculate new tile with random positions
        int randomX = GetRandomValue(0, TILEMAP_WIDTH-1);
        int randomY = GetRandomValue(0, TILEMAP_HEIGHT-1);
        int tile = TileMapGetTile(tileMap, randomX, randomY);
        if (tile != 0) {
            return;
        }

        if (previousLightTile > -1) TileMapSetTile(tileMap, randomLightX, randomLightY, previousLightTile);

        // Change tile value to new one
        randomLightX = randomX;
        randomLightY = randomY;
        previousLightTile = tile;
        TileMapSetTile(tileMap, randomLightX, randomLightY, 1);
        framesLightCounter = 0;
    }
}

void GetRandomApple(void)
{
    framesAppleCounter++;
    if (framesAppleCounter >= 200) {

        // Store old tile value
        int randomX = GetRandomValue(0, TILEMAP_WIDTH-1);
        int randomY = GetRandomValue(0, TILEMAP_HEIGHT-1);
        int tile = TileMapGetTile(tileMap, randomX, randomY);
        if (tile != 0) {
            return;
        }
        if (previousAppleTile > -1) TileMapSetTile(tileMap, randomAppleX, randomAppleY, previousAppleTile);

        // Change tile value to a new one
        randomAppleX = randomX;
        randomAppleY = randomY;
        previousAppleTile = tile;
        TileMapSetTile(tileMap, randomAppleX, randomAppleY, 2);
        framesAppleCounter = 0;
    }
}