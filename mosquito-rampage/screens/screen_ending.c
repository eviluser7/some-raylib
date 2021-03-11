#include "raylib.h"
#include "screens.h"

static Texture2D lose;
static Texture2D win;

static int finishScreen;

// Initialize ending screen
void InitEndingScreen(void)
{
    finishScreen = 0;
    lose = LoadTexture("resources/ending_lose.png");
    win = LoadTexture("resources/ending_win.png"); 
}

// Update ending screen
void UpdateEndingScreen(void)
{
    StopMusicStream(ambient);

    if (IsKeyPressed(KEY_L) || IsGestureDetected(GESTURE_TAP)) finishScreen = 1;
}

// Draw ending screen
void DrawEndingScreen(void)
{
    ClearBackground(GetColor(0x57374cff));

    // Check if user won or lost
    if (result == 0)
    {
        DrawTexture(lose, 0, 0, WHITE);
        DrawText("You lost...", SCREEN_WIDTH/2-140, 30, 72, WHITE);
    }
    else
    {
        DrawTexture(win, 0, 0, WHITE);
        DrawText("You won!!!!", SCREEN_WIDTH/2-140, 30, 72, WHITE);
    }

    // General texts
    if (!phoneMode) DrawText("Press L to leave.", SCREEN_WIDTH/2-140, 650, 36, WHITE);
    else DrawText("Tap anywhere to leave.", SCREEN_WIDTH/2-180, 650, 36, WHITE);
}

// Unload ending screen
void UnloadEndingScreen(void)
{
    UnloadTexture(lose);
    UnloadTexture(win);
}

// Should ending screen finish?
int FinishEndingScreen(void)
{
    return finishScreen;
}