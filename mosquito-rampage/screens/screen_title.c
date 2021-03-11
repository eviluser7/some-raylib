#include "raylib.h"
#include "screens.h"
#include <stdio.h>

Texture2D protagonist;
Texture2D background;
static int finishScreen;

// Initialize title screen
void InitTitleScreen(void)
{
    finishScreen = 0;
    protagonist = LoadTexture("resources/title_man.png");
    background = LoadTexture("resources/title_background.png");
}

// Update title screen
void UpdateTitleScreen(void)
{
    if (IsKeyPressed(KEY_ENTER) && !phoneMode) 
    {
        finishScreen = 1;
        PlaySound(point);
    }

    PlayMusicStream(ambient);
}

// Draw title screen
void DrawTitleScreen(void)
{
    DrawTexture(background, 0, 0, WHITE);
    DrawTexture(protagonist, 750, 220, WHITE);
    DrawText("Mosquito Rampage", GetScreenWidth() - 960, GetScreenHeight() - 600, 72, WHITE);
    if (phoneMode)
    {
        if (ClickGuiButton((Rectangle){ GetScreenWidth()/2 - 210, GetScreenHeight()/2, 400, 100 }, "Tap here to begin"))
        {
            finishScreen = 1;
            PlaySound(point);
        }
    }

    else DrawText("Press Enter to begin", GetScreenWidth() - 890, GetScreenHeight()/2+40, 48, WHITE);

    // Toggle to phone mode
    if (!phoneMode)
    {
        if (ClickGuiButton((Rectangle){ 20, 620, 340, 80 }, "Phone Mode: Off")) phoneMode = true;
    }
    else
    {
        if (ClickGuiButton((Rectangle){ 20, 620, 340, 80 }, "Phone Mode: On")) phoneMode = false;
    }

    DrawText("2.1.1", 30, 10, 48, WHITE);
}

// Unload title screen
void UnloadTitleScreen(void)
{
    UnloadTexture(background);
    UnloadTexture(protagonist);
}

// Should title screen finish?
int FinishTitleScreen(void)
{
    return finishScreen;
}