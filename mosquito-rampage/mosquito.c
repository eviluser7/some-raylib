#include "raylib.h"
#include "screens/screens.h"    // NOTE: Defines global variable: currentScreen
#include <stdio.h>

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
    #include <emscripten.h>
#endif

// Variables from screens.h file
GameScreen currentScreen = 0;
Sound point = { 0 };
Sound spraySound = { 0 };
Sound hitSound = { 0 };
Music ambient = { 0 };
Texture2D texNPatch = { 0 };
NPatchInfo npInfo = { 0 };
int result = 0;
bool phoneMode = true;

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
const int screenWidth = SCREEN_WIDTH;
const int screenHeight = SCREEN_HEIGHT;

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0.0f;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static int transToScreen = -1;

// NOTE: Some global variables that require to be visible for all screens,
// are defined in screens.h (i.e. currentScreen)
    
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ChangeToScreen(int screen);     // No transition effect

static void TransitionToScreen(int screen);
static void UpdateTransition(void);
static void DrawTransition(void);

static void UpdateDrawFrame(void);          // Update and Draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "MOSQUITO RAMPAGE");

    // Global data loading (assets that must be available in all screens, i.e. fonts)
    InitAudioDevice();
    point = LoadSound("resources/sfx_point.wav");
    spraySound = LoadSound("resources/sfx_spray.wav");
    hitSound = LoadSound("resources/sfx_hit.wav");
    ambient = LoadMusicStream("resources/music_ambient.ogg");

    texNPatch = LoadTexture("resources/npatch.png");
    npInfo.source = (Rectangle){ 0, 0, texNPatch.width, texNPatch.height};
    npInfo.left = 30;
    npInfo.top = 30;
    npInfo.right = 30;
    npInfo.bottom = 30;

    result = 0;

    SetMusicVolume(ambient, 0.5f);

    // Setup and Init first screen
    currentScreen = LOGO;
    InitLogoScreen();
#if defined(PLATFORM_WEB)
    UpdateMusicStream(ambient);
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
        UpdateMusicStream(ambient);
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
    // Unload current screen data before closing
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }
    
    // Unload all global loaded data (i.e. fonts) here!
    UnloadSound(point);
    UnloadSound(spraySound);
    UnloadSound(hitSound);
    UnloadMusicStream(ambient);
   
    CloseAudioDevice();
    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Previously declared in screens.h
bool ClickGuiButton(Rectangle bounds, char *text)
{
    bool pressed = false;
    
    Vector2 mousePoint = GetMousePosition();

    // Check button state
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP))
        {
            pressed = true;
        }
    }

    // Draw button
    DrawTextureNPatch(texNPatch, npInfo, bounds, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    DrawText(text, bounds.x+bounds.width/2-152, bounds.y+bounds.height/2-15, 34, BLACK);

    return pressed;
}

bool HoldGuiButton(Rectangle bounds, char *text)
{
    bool pressed = false;

    Vector2 mousePoint = GetMousePosition();

    // Check button state
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_HOLD)) pressed = true;
        else pressed = false;
    }

    // Draw button
    DrawTextureNPatch(texNPatch, npInfo, bounds, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    DrawText(text, bounds.x+bounds.width/2-152, bounds.y+bounds.height/2-15, 34, BLACK);

    return pressed;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Change to next screen, no transition
static void ChangeToScreen(int screen)
{
    // Unload current screen
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }
    
    // Init next screen
    switch (screen)
    {
        case LOGO: InitLogoScreen(); break;
        case TITLE: InitTitleScreen(); break;
        case GAMEPLAY: InitGameplayScreen(); break;
        case ENDING: InitEndingScreen(); break;
        default: break;
    }
    
    currentScreen = screen;
}

// Define transition to next screen
static void TransitionToScreen(int screen)
{
    onTransition = true;
    transFadeOut = false;
    transFromScreen = currentScreen;
    transToScreen = screen;
    transAlpha = 0.0f;
}

// Update transition effect
static void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 0.02f;
        
        // NOTE: Due to float internal representation, condition jumps on 1.0f instead of 1.05f
        // For that reason we compare against 1.01f, to avoid last frame loading stop
        if (transAlpha > 1.01f)
        {
            transAlpha = 1.0f;
        
            // Unload current screen
            switch (transFromScreen)
            {
                case LOGO: UnloadLogoScreen(); break;
                case TITLE: UnloadTitleScreen(); break;
                case GAMEPLAY: UnloadGameplayScreen(); break;
                case ENDING: UnloadEndingScreen(); break;
                default: break;
            }
            
            // Load next screen
            switch (transToScreen)
            {
                case LOGO: InitLogoScreen(); break;
                case TITLE: InitTitleScreen(); break;
                case GAMEPLAY: InitGameplayScreen(); break;
                case ENDING: InitEndingScreen(); break;
                default: break;
            }
            
            currentScreen = transToScreen;
            
            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.02f;
        
        if (transAlpha < -0.01f)
        {
            transAlpha = 0.0f;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = -1;
        }
    }
}

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    
    if (!onTransition)
    {
        switch(currentScreen) 
        {
            case LOGO: 
            {
                UpdateLogoScreen();
                
                if (FinishLogoScreen()) TransitionToScreen(TITLE);

            } break;
            case TITLE: 
            {
                UpdateTitleScreen();    
                if (FinishTitleScreen()) TransitionToScreen(GAMEPLAY);

            } break;
            case GAMEPLAY:
            {
                UpdateGameplayScreen();
                if (FinishGameplayScreen()) TransitionToScreen(ENDING);

            } break;
            case ENDING:
            { 
                UpdateEndingScreen();
                if (FinishEndingScreen() == 1) TransitionToScreen(TITLE);

            } break;
            default: break;
        }
    }
    else UpdateTransition();    // Update transition (fade-in, fade-out)
    //----------------------------------------------------------------------------------
    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(currentScreen) 
        {
            case LOGO: DrawLogoScreen(); break;
            case TITLE: DrawTitleScreen(); break;
            case GAMEPLAY: DrawGameplayScreen(); break;
            case ENDING: DrawEndingScreen(); break;
            default: break;
        }
         
        // Draw full screen rectangle in front of everything
        if (onTransition) DrawTransition();
    
    EndDrawing();
    //----------------------------------------------------------------------------------
}