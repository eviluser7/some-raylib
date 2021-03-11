#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   720

// Screens to be listed
typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

// Define global variables
extern GameScreen currentScreen;
extern int result; // 0 - Lose, 1 - Win
extern Sound point;
extern Sound spraySound;
extern Sound hitSound;
extern Music ambient;
extern bool phoneMode;

#ifdef __cplusplus
extern "C" {
#endif

// Buttons
bool ClickGuiButton(Rectangle bounds, char *text);
bool HoldGuiButton(Rectangle bounds, char *text);

// Logo screen function declarations
void InitLogoScreen(void);
void UpdateLogoScreen(void);
void DrawLogoScreen(void);
void UnloadLogoScreen(void);
int FinishLogoScreen(void);

// Title screen function declarations
void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
void UnloadTitleScreen(void);
int FinishTitleScreen(void);

// Gameplay function declarations
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
int FinishGameplayScreen(void);

// Ending screen function declarations
void InitEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(void);
void UnloadEndingScreen(void);
int FinishEndingScreen(void);

#ifdef __cplusplus
}
#endif

#endif // SCREENS_H