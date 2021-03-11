#include "raylib.h"
#include "screens.h"
#include <stdio.h>

#define SPRAY_HEIGHT_LIMIT  -200

#define MAX_ENEMY_NUM       12
#define TIME_LIMIT_SECONDS  180
#define TARGET_ENEMY_NUM    100
#define BUTTON_HEIGHT       540

// Structures
typedef struct {
    Vector2 position;
    bool isAlive;
} Mosquito;

// Function declarations
void InitMosquito(void);
void SpawnMosquito(void);
void DrawMosquito(void);
void UpdateMosquito(void);

// Variables
static int finishScreen;
static int timeLevelSeconds;
static int framesSprayCounter;
static int framesMosquitoCounter;
static int framesCounter;
Texture2D gameBackground;
Texture2D gameObjective;
Texture2D gameObjectiveMobile;
Texture2D arrowLeft;
Texture2D arrowRight;
Texture2D sprayButton;
Texture2D uiElement;
Texture2D player;
Texture2D spray;
Texture2D mosquitoTexture;
Texture2D sprayBottle;
Texture2D health;

Vector2 playerPosition = { 640, 440 };
Vector2 sprayPosition;
static int playerHealth = 3;
static bool sprayWasShot = false;
static int sprayLimit = 100;
static Mosquito enemies[MAX_ENEMY_NUM] = { 0 };
static int mosquitoCounter = 0;
static bool gameBegan = false; // Check if the user presses enter to show objective

// Initialize gameplay screen
void InitGameplayScreen(void)
{
    finishScreen = 0;
    timeLevelSeconds = TIME_LIMIT_SECONDS;
    framesSprayCounter = 0;
    framesMosquitoCounter = 0;
    framesCounter = 0;
    playerPosition.x = 640;
    playerPosition.y = 440;
    playerHealth = 3;
    sprayWasShot = false;
    sprayLimit = 100;
    InitMosquito();
    mosquitoCounter = 0;
    gameBegan = false;

    gameBackground = LoadTexture("resources/background.png");
    gameObjective = LoadTexture("resources/objective.png");
    gameObjectiveMobile = LoadTexture("resources/objectiveMobile.png");
    player = LoadTexture("resources/player.png");
    spray = LoadTexture("resources/spray.png");
    mosquitoTexture = LoadTexture("resources/mosquito.png");
    uiElement = LoadTexture("resources/ui_element.png");
    sprayBottle = LoadTexture("resources/sprayBottle.png");
    health = LoadTexture("resources/health.png");
    arrowLeft = LoadTexture("resources/arrowLeft.png");
    arrowRight = LoadTexture("resources/arrowRight.png");
    sprayButton = LoadTexture("resources/shootButton.png");
}

// Update gameplay screen
void UpdateGameplayScreen(void)
{
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) gameBegan = true;
    // Do all of the things below if the game started
    if (gameBegan)
    {
        if (!phoneMode)
        {
            // Player movement logic
            if (IsKeyDown(KEY_A) && playerPosition.x > GetScreenWidth() - 1255 && gameBegan) playerPosition.x -= 10;
            if (IsKeyDown(KEY_D) && playerPosition.x < GetScreenWidth() - 205 && gameBegan) playerPosition.x += 10;

            // Spray shooting logic
            if (IsKeyPressed(KEY_L) && !sprayWasShot && sprayPosition.y > SPRAY_HEIGHT_LIMIT &&
                sprayLimit > 0)
            {
                PlaySound(spraySound);
                sprayWasShot = true;
                if (sprayLimit > -1) sprayLimit -= 15;
            }
        }

        // Normal game logic
        if (sprayWasShot) sprayPosition.y -= 20;

        if (sprayPosition.y < SPRAY_HEIGHT_LIMIT) sprayWasShot = false;
        if (!sprayWasShot)
        {
            sprayPosition.x = playerPosition.x - 100;
            sprayPosition.y = playerPosition.y;
        }

        // Spray cooldown logic
        if (sprayLimit < 100)
        {
            framesSprayCounter++;

            if (framesSprayCounter == 6)
            {
                framesSprayCounter = 0;
                sprayLimit += 1;
            }
        }

        // Mosquito logic
        UpdateMosquito();

        framesMosquitoCounter++;

        if (framesMosquitoCounter == 50)
        {
            SpawnMosquito();
            framesMosquitoCounter = 0;
        }

        // Reduce timer
        framesCounter++;

        if (framesCounter == 60)
        {
            timeLevelSeconds--;
            framesCounter = 0;
        }

        // Player loses
        if (playerHealth == 0 || timeLevelSeconds == 0)
        {
            result = 0;
            finishScreen = 1;
        }

        // Player wins
        if (mosquitoCounter == TARGET_ENEMY_NUM)
        {
            result = 1;
            finishScreen = 1;
        }
    }
}

// Draw gameplay screen
void DrawGameplayScreen(void)
{
    // Draw the game
    DrawTexture(gameBackground, 0, 0, WHITE);
    DrawTexture(player, playerPosition.x, playerPosition.y, WHITE);

    if (sprayWasShot) DrawTexture(spray, sprayPosition.x, sprayPosition.y, WHITE);

    // Mosquito drawing
    if (gameBegan) DrawMosquito();

    // Draw UI
    DrawTexture(uiElement, -50, 1, WHITE); // Spray
    DrawTexture(sprayBottle, 10, 5, WHITE);
    DrawTexture(uiElement, 0, 67, WHITE); // Mosquitos
    DrawTexture(mosquitoTexture, 20, 83, WHITE);
    DrawTexture(uiElement, 1065, 1, WHITE); // Timer
    DrawTexture(uiElement, 1065, 67, WHITE); // Health
    switch (playerHealth)
    {
    case 3:
        {
            // Draw all three hearts
            DrawTexture(health, 1226, 75, WHITE);
            DrawTexture(health, 1164, 75, WHITE);
            DrawTexture(health, 1097, 75, WHITE);
        }
    case 2:
        {
            // Draw two hearts
            DrawTexture(health, 1164, 75, WHITE);
            DrawTexture(health, 1097, 75, WHITE);
        }
    case 1:
        {
            DrawTexture(health, 1097, 75, WHITE);
        }
    }

    DrawText(FormatText("%d", sprayLimit), 50, 10, 48, WHITE);
    DrawText(FormatText("%d/%d", mosquitoCounter, TARGET_ENEMY_NUM), 80, 83, 36, WHITE);
    DrawText(FormatText("%i:%02im", timeLevelSeconds/60, timeLevelSeconds%60), 1150, 10, 48, WHITE);


    // Draw mission
    if (!gameBegan)
    {
        if (!phoneMode) DrawTexture(gameObjective, 0, 0, WHITE);
        else DrawTexture(gameObjectiveMobile, 0, 0, WHITE);
    }

    // Mobile controls
    // Player movement logic
    if (phoneMode)
    {
        if (HoldGuiButton((Rectangle){ 20, BUTTON_HEIGHT, 150, 150 }, "") &&
            playerPosition.x > GetScreenWidth() - 1255 && gameBegan) playerPosition.x -= 10;

        if (HoldGuiButton((Rectangle){ 210, BUTTON_HEIGHT, 150, 150 }, "") &&
            playerPosition.x < GetScreenWidth() - 205 && gameBegan) playerPosition.x += 10;

        if (ClickGuiButton((Rectangle){ 1100, BUTTON_HEIGHT, 150, 150 }, "") &&
            !sprayWasShot && sprayPosition.y > SPRAY_HEIGHT_LIMIT && sprayLimit > 0)
        {
            PlaySound(spraySound);
            sprayWasShot = true;
            if (sprayLimit > -1) sprayLimit -= 15;
        }
        DrawTexture(arrowLeft, 20, BUTTON_HEIGHT, WHITE);
        DrawTexture(arrowRight, 210, BUTTON_HEIGHT, WHITE);
        DrawTexture(sprayButton, 1100, BUTTON_HEIGHT, WHITE);
    }

}

// Unload gameplay screen
void UnloadGameplayScreen(void)
{
    UnloadTexture(gameBackground);
    UnloadTexture(gameObjective);
    UnloadTexture(gameObjectiveMobile);
    UnloadTexture(player);
    UnloadTexture(spray);
    UnloadTexture(mosquitoTexture);
    UnloadTexture(uiElement);
    UnloadTexture(sprayBottle);
    UnloadTexture(health);
    UnloadTexture(arrowLeft);
    UnloadTexture(arrowRight);
    UnloadTexture(sprayButton);
}

// Should gameplay screen finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}

// Module functions
void InitMosquito(void)
{
    for (int i = 0; i < MAX_ENEMY_NUM; i++) enemies[i].isAlive = false;
}

void SpawnMosquito(void)
{
    for (int i = 0; i < MAX_ENEMY_NUM; i++)
    {
        if (!enemies[i].isAlive)
        {
            enemies[i].isAlive = true;
            enemies[i].position.x = i%2 ? 100 : 1100; // Odd mosquito number spawn on left, even spawn on right
            enemies[i].position.y = GetRandomValue(0, 250);
            break;
        }
    }
}

void DrawMosquito(void)
{
    for (int i = 0; i < MAX_ENEMY_NUM; i++)
    {
        if (enemies[i].isAlive) DrawTexture(mosquitoTexture, enemies[i].position.x, enemies[i].position.y, WHITE);
    }
}

void UpdateMosquito(void)
{
    for (int i = 0; i < MAX_ENEMY_NUM; i++)
    {
        if (enemies[i].isAlive)
        {
            int direction = 0;
            if (enemies[i].position.x > playerPosition.x + 50) direction = -1;
            else direction = 1;

            // Movement logic
            float velX = 5.0;
            float velY = 4.0;
            enemies[i].position.x = enemies[i].position.x + velX*direction*2;
            enemies[i].position.y = enemies[i].position.y + velY;

        }

        // Check if mosquito still alive
        if (enemies[i].position.y > SCREEN_HEIGHT-120) enemies[i].isAlive = false;

        // Check if mosquito gets hit by spray
        if (enemies[i].position.x < (sprayPosition.x+spray.width) &&
            enemies[i].position.x+mosquitoTexture.width > sprayPosition.x &&
            enemies[i].position.y < (sprayPosition.y+spray.height) &&
            mosquitoTexture.height+enemies[i].position.y > sprayPosition.y &&
            sprayWasShot && enemies[i].isAlive)
            {
                mosquitoCounter++;
                enemies[i].isAlive = false;
                PlaySound(hitSound);
            }

        // Check if player gets hit by mosquito
        if (enemies[i].position.x < (playerPosition.x+player.width) &&
            enemies[i].position.x+player.width > playerPosition.x+132 &&
            enemies[i].position.y < (playerPosition.y+player.height)+235 &&
            player.height+enemies[i].position.y > playerPosition.y+235 &&
            enemies[i].isAlive)
            {
                enemies[i].isAlive = false;
                playerHealth--;
                PlaySound(hitSound);
            }
    }
}