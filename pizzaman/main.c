/* *******************************************************************************
*
*   Pizzaman
*
*   "Feed all the customers! And don't let them leave!"
*
*   Made with raylib v3.5.0
*   Made by eviluser7 (c) 2021
*
*********************************************************************************/

#include "raylib.h"
#include <stdio.h>

#define LOCKED      (Color){ 50, 50, 50, 255 }
#define UNLOCKED    (Color){ 255, 255, 255, 255 }

#define CUSTOMERS_PER_LEVEL     3
#define AMOUNT_OF_PIZZAS        3

#if defined(PLATFORM_WEB)
    #include <emscripten.h>
#endif

// -----------------------
// Types and structures definition
typedef enum {
    TITLE = 0,
    GAME,
    ENDING
} GameScreen;

typedef struct {
    Vector2 position;
    int score;
    int atTable;
    int direction;      // -1 left, 1 right
    bool holdingPizza;
} Player;

typedef struct {
    Vector2 position;
    Vector2 textPosition;
    int timeForNext;
    int state;      // 0 - pizza not taken, 1 - pizza taken
} Furnace;

typedef struct {
    Vector2 position;
    int atTable;
    bool wasThrown;
    bool hasFallen;
} Pizza;

typedef struct {
    Vector2 position;
    int atTable;
    bool isThere;
    int timer;
} Customer;

typedef struct {
    int customersServed;
    int scene;
    int countdownTimer;
    int framesCounter;
} Game;

// -----------------------
// Functions declaration
void InitGame(void);
void UpdateGame(void);
void DrawGame(void);
void UnloadGame(void);
void EndGame(void);
void UpdateDrawFrame(void);

void InitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

void InitPizza(void);
void UpdatePizza(void);
void DrawPizza(void);
void ThrowPizza(void);

void InitCustomer(void);
void SpawnCustomer(void);
void UpdateCustomer(void);
void DrawCustomer(void);

// -----------------------
// Variables definition or declaration
const int screenWidth = 1024;
const int screenHeight = 600;
static int furnace1Counter = 0;
static int furnace2Counter = 0;
static int furnace3Counter = 0;
static int framesCounter;

Texture2D customer1;
Texture2D floorTexture;
Texture2D furnace;
Texture2D logo;
Texture2D pizzaTexture;
Texture2D pizzamanLeft;
Texture2D pizzamanRight;
Texture2D table;
Font font;
Sound defeat = { 0 };
Sound footsteps = { 0 };
Sound getScore = { 0 };
Sound pizzaFell = { 0 };
Sound pizzaGrab = { 0 };
Sound pizzaThrow = { 0 };
Sound startSound = { 0 };

Player player = { 0 };
Game game;
Furnace furnace1;
Furnace furnace2;
Furnace furnace3;

static Customer customers[CUSTOMERS_PER_LEVEL] = { 0 };
static Pizza pizzas[AMOUNT_OF_PIZZAS] = { 0 };

// -----------------------
// Main entry point
int main(void)
{
    InitWindow(screenWidth+200, screenHeight+200, "Pizzaman");
    InitGame();
    InitAudioDevice();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    CloseAudioDevice();
    UnloadGame();
    CloseWindow();
    return 0;
}

// -----------------------
// Functions declaration

// GAME FUNCTIONS
// Initialize game variables
void InitGame(void)
{
    customer1 = LoadTexture("resources/customer1.png");
    floorTexture = LoadTexture("resources/floorTexture.png");
    pizzamanLeft = LoadTexture("resources/pizzamanLeft.png");
    pizzamanRight = LoadTexture("resources/pizzamanRight.png");
    table = LoadTexture("resources/table.png");
    logo = LoadTexture("resources/logo.png");
    furnace = LoadTexture("resources/furnace.png");
    pizzaTexture = LoadTexture("resources/pizza.png");
    font = LoadFontEx("resources/font.ttf", 32, 0, 250);
    defeat = LoadSound("resources/defeat.wav");
    footsteps = LoadSound("resources/footsteps.wav");
    getScore = LoadSound("resources/get_score.wav");
    pizzaFell = LoadSound("resources/pizza_fell.wav");
    pizzaGrab = LoadSound("resources/pizza_grab.wav");
    pizzaThrow = LoadSound("resources/pizza_throw.wav");
    startSound = LoadSound("resources/start.wav");

    InitPlayer();
    InitPizza();
    InitCustomer();
    game.scene = TITLE;
    game.customersServed = 0;

    furnace1.position.x = 990;
    furnace1.position.y = 140;
    furnace1.textPosition.x = 1015;
    furnace1.textPosition.y = 175;
    furnace1.timeForNext = 10;
    furnace1.state = 0;

    furnace2.position.x = 990;
    furnace2.position.y = 345;
    furnace2.textPosition.x = 1015;
    furnace2.textPosition.y = 380;
    furnace2.timeForNext = 10;
    furnace2.state = 0;

    furnace3.position.x = 990;
    furnace3.position.y = 550;
    furnace3.textPosition.x = 1015;
    furnace3.textPosition.y = 585;
    furnace3.timeForNext = 10;
    furnace3.state = 0;

    furnace1Counter = 0;
    furnace2Counter = 0;
    furnace3Counter = 0;

    game.framesCounter = 0;
    framesCounter = 0;
}

void UpdateGame(void)
{
    if (game.scene == TITLE) {
        if (IsKeyPressed(KEY_ENTER)) {
            game.scene = GAME;
            PlaySound(startSound);
        }
    }

    if (game.scene == GAME) {
        UpdatePlayer();
        UpdatePizza();
        UpdateCustomer();

        framesCounter++;
        if (framesCounter == 60) {
            framesCounter = 0;
            SpawnCustomer();
        }

        // Check furnace state to see if it's unlocked or locked
        // Then give it a timer
        if (furnace1.state == 1) {
            furnace1Counter++;

            if (furnace1Counter == 20) {
                furnace1Counter = 0;
                furnace1.timeForNext--;

                if (furnace1.timeForNext <= 0) {
                    furnace1.state = 0;
                    furnace1.timeForNext = 10;
                }
            }
        }

        if (furnace2.state == 1) {
            furnace2Counter++;

            if (furnace2Counter == 20) {
                furnace2Counter = 0;
                furnace2.timeForNext--;

                if (furnace2.timeForNext <= 0) {
                    furnace2.state = 0;
                    furnace2.timeForNext = 10;
                }
            }
        }

        if (furnace3.state == 1) {
            furnace3Counter++;

            if (furnace3Counter == 20) {
                furnace3Counter = 0;
                furnace3.timeForNext--;

                if (furnace3.timeForNext <= 0) {
                    furnace3.state = 0;
                    furnace3.timeForNext = 10;
                }
            }
        }
    }

    if (game.scene == ENDING) {
        if (IsKeyPressed(KEY_L)) game.scene = TITLE;
        if (IsKeyPressed(KEY_R)) {
            InitGame();
            game.scene = GAME;
        }
    }
}

void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (game.scene == TITLE) {
        DrawTexture(floorTexture, 100, 100, WHITE);
        DrawTexture(logo, 230, 50, WHITE);
        DrawTexture(pizzamanLeft, 610, 350, WHITE);
        DrawTextEx(font, "Press Enter to start", (Vector2){ 300.0f, 570.0f }, 36, 2, WHITE);
        DrawTextEx(font, "Mission: Feed your customers!", (Vector2){ 230.0, 630.0 }, 30, 2, WHITE);
        DrawTextEx(font, "W&S to move, A&D to change direction", (Vector2){ 10.0, 710.0 }, 30, 2, WHITE);
        DrawTextEx(font, "Press E to throw the pizza to customers or to pick it up", (Vector2){ 10.0, 770.0 }, 20, 2, WHITE);
        DrawTextEx(font, "Press E to throw the pizza to customers or to pick it up", (Vector2){ 10.0, 770.0 }, 20, 2, WHITE);
        DrawText("powered by raylib", 925.0, 10.0, 30, WHITE);
        DrawText("eviluser7 (c) 2021", 925.0, 55.0, 30, WHITE);
    }

    if (game.scene == GAME) {
        DrawTexture(floorTexture, 100, 100, WHITE);
        DrawTexture(table, 325, 130, WHITE);
        DrawTexture(table, 325, 335, WHITE);
        DrawTexture(table, 325, 540, WHITE);
        if (furnace1.state == 0) DrawTexture(furnace, furnace1.position.x, furnace1.position.y, UNLOCKED);
        else {
            DrawTexture(furnace, furnace1.position.x, furnace1.position.y, LOCKED);
            DrawTextEx(font, FormatText("%d", furnace1.timeForNext), furnace1.textPosition, 30, 2, WHITE);
        }

        if (furnace2.state == 0) DrawTexture(furnace, furnace2.position.x, furnace2.position.y, UNLOCKED);
        else {
            DrawTexture(furnace, furnace2.position.x, furnace2.position.y, LOCKED);
            DrawTextEx(font, FormatText("%d", furnace2.timeForNext), furnace2.textPosition, 30, 2, WHITE);
        }
        
        if (furnace3.state == 0) DrawTexture(furnace, furnace3.position.x, furnace3.position.y, UNLOCKED);
        else {
            DrawTexture(furnace, furnace3.position.x, furnace3.position.y, LOCKED);
            DrawTextEx(font, FormatText("%d", furnace3.timeForNext), furnace3.textPosition, 30, 2, WHITE);
        }

        DrawTextEx(font, FormatText("Customers served: %d", game.customersServed), (Vector2){ 10.0, 10.0 }, 30, 2, WHITE);
        DrawTextEx(font, FormatText("Score: %d", player.score), (Vector2){ 10.0, 50.0 }, 30, 2, WHITE);
        if (!player.holdingPizza) DrawTextEx(font, "Holding pizza: false", (Vector2){ 10.0, 720.0 }, 30, 2, WHITE);
        else DrawTextEx(font, "Holding pizza: true", (Vector2){ 10.0, 720.0 }, 30, 2, WHITE);

        DrawPlayer();
        DrawPizza();
        DrawCustomer();
    }

    if (game.scene == ENDING) {
        DrawTextEx(font, "One of your customers got tired of waiting!", (Vector2){ 45.0f, 100.0f }, 28, 2, WHITE);
        DrawText(FormatText("Score: %d", player.score), 45, 200, 64, WHITE);
        DrawTextEx(font, "Wanna get a better record?", (Vector2){ 230.0f, 400.0f }, 32, 2, WHITE);
        DrawTextEx(font, "Press R to retry", (Vector2){ 350.0f, 500.0f }, 32, 2, WHITE);
        DrawTextEx(font, "Press L to leave", (Vector2){ 350.0f, 550.0f }, 32, 2, WHITE);
    }
    EndDrawing();
}

// Unload all variables here (textures, etc)
void UnloadGame(void)
{
    UnloadTexture(customer1);
    UnloadTexture(pizzamanLeft);
    UnloadTexture(pizzamanRight);
    UnloadTexture(floorTexture);
    UnloadTexture(table);
    UnloadTexture(logo);
    UnloadTexture(furnace);
    UnloadTexture(pizzaTexture);
    UnloadFont(font);
    UnloadSound(defeat);
    UnloadSound(footsteps);
    UnloadSound(getScore);
    UnloadSound(pizzaFell);
    UnloadSound(pizzaGrab);
    UnloadSound(pizzaThrow);
    UnloadSound(startSound);
}

void EndGame(void)
{
    game.scene = ENDING;
}

// Update AND Draw the frame in one function
// Used for browsers
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}

// PLAYER FUNCTIONS
// Initialize player variables
void InitPlayer(void)
{
    player.position.x = 860;
    player.position.y = 325;
    player.score = 0;
    player.atTable = 1;
    player.direction = -1;
    player.holdingPizza = false;
}

void UpdatePlayer(void)
{
    // Detect at what table
    if (player.position.y == 120) player.atTable = 0;
    if (player.position.y == 325) player.atTable = 1;
    if (player.position.y == 530) player.atTable = 2;

    // Check direction / movement input
    if (IsKeyPressed(KEY_W) && player.position.y > 120) {
        player.position.y -= 205;
        PlaySound(footsteps);
    }

    if (IsKeyPressed(KEY_A)) player.direction = -1;

    if (IsKeyPressed(KEY_S) && player.position.y < 530) {
        player.position.y += 205;
        PlaySound(footsteps);
    }

    if (IsKeyPressed(KEY_D)) player.direction = 1;

    // Pizza grabbing on each furnace
    if (IsKeyPressed(KEY_E) && !player.holdingPizza
        && player.direction == 1 && player.atTable == 0
        && furnace1.state == 0) {
            furnace1.state = 1;
            player.holdingPizza = true;
            PlaySound(pizzaGrab);
        }
    
    if (IsKeyPressed(KEY_E) && !player.holdingPizza
        && player.direction == 1 && player.atTable == 1
        && furnace2.state == 0) {
            furnace2.state = 1;
            player.holdingPizza = true;
            PlaySound(pizzaGrab);
        }

    if (IsKeyPressed(KEY_E) && !player.holdingPizza
        && player.direction == 1 && player.atTable == 2
        && furnace3.state == 0) {
            furnace3.state = 1;
            player.holdingPizza = true;
            PlaySound(pizzaGrab);
        }

    // Throwing pizza
    if (IsKeyPressed(KEY_E) && player.holdingPizza
        && player.direction == -1 && player.atTable == 0) {
            ThrowPizza();
            player.holdingPizza = false;
            PlaySound(pizzaThrow);
        }

    if (IsKeyPressed(KEY_E) && player.holdingPizza
        && player.direction == -1 && player.atTable == 1) {
            ThrowPizza();
            player.holdingPizza = false;
            PlaySound(pizzaThrow);
        }

    if (IsKeyPressed(KEY_E) && player.holdingPizza
        && player.direction == -1 && player.atTable == 2) {
            ThrowPizza();
            player.holdingPizza = false;
            PlaySound(pizzaThrow);
        }
}

void DrawPlayer(void)
{
    if (player.direction == -1) DrawTexture(pizzamanLeft, player.position.x, player.position.y, WHITE);
    else if (player.direction == 1) DrawTexture(pizzamanRight, player.position.x, player.position.y, WHITE);
}

// -----------------------
// PIZZA FUNCTIONS
void InitPizza(void)
{
    for (int i = 0; i < AMOUNT_OF_PIZZAS; i++) {
        pizzas[i].wasThrown = false;
        pizzas[i].hasFallen = false;
    }
}

void DrawPizza(void)
{
    for (int i = 0; i < AMOUNT_OF_PIZZAS; i++) {
        if (pizzas[i].wasThrown) DrawTexture(pizzaTexture, pizzas[i].position.x, pizzas[i].position.y, WHITE);
    }
}

void UpdatePizza(void)
{
    for (int i = 0; i < AMOUNT_OF_PIZZAS; i++) {
        // Update y position for each table
        if (!pizzas[i].wasThrown) pizzas[i].position.x = 690;

        if (pizzas[i].atTable == 0) pizzas[i].position.y = 150;
        else if (pizzas[i].atTable == 1) pizzas[i].position.y = 355;
        else if (pizzas[i].atTable == 2) pizzas[i].position.y = 560;

        // Update position once the pizza was thrown
        if (pizzas[i].wasThrown) {
            pizzas[i].position.x -= 15;
        }
    }
}

void ThrowPizza(void)
{
    for (int i = 0; i < AMOUNT_OF_PIZZAS; i++) {
        if (IsKeyPressed(KEY_E) && player.holdingPizza && player.direction == -1
            && !pizzas[i].wasThrown) {
                pizzas[i].wasThrown = true;
                pizzas[i].atTable = player.atTable;
            }
    }
}

// -----------------------
// CUSTOMER FUNCTIONS
void InitCustomer(void)
{
    for (int i = 0; i < CUSTOMERS_PER_LEVEL; i++) {
        customers[i].atTable = -1; // no table set
        customers[i].isThere = false;
        customers[i].timer = GetRandomValue(4, 10);

        customers[i].position.x = 260;
        if (i == 0) {
            customers[i].position.y = 130;
        }
        
        if (i == 1) {
            customers[i].position.y = 335;
        }
        
        if (i == 2) {
            customers[i].position.y = 540;
        }
    }
}

void SpawnCustomer(void)
{
    for (int i = 0; i < CUSTOMERS_PER_LEVEL; i++) {
        if (!customers[i].isThere) {    // TODO: check if customer is on table to avoid overlapping and extra customers
            customers[i].atTable = i;
            customers[i].isThere = true;
            customers[i].timer = GetRandomValue(4, 10);
            break;
        }
    }
}

void UpdateCustomer(void)
{
    for (int i = 0; i < CUSTOMERS_PER_LEVEL; i++) {
        // logic with pizzas
        for (int j = 0; j < AMOUNT_OF_PIZZAS; j++) {
            if (customers[i].atTable != pizzas[j].atTable) continue;
            // Check if customer gets pizza
            if (pizzas[j].position.x < 275 && pizzas[j].wasThrown
                && customers[i].isThere && customers[i].atTable == pizzas[j].atTable) {
                    PlaySound(getScore);
                    pizzas[j].wasThrown = false;
                    customers[i].isThere = false;
                    customers[i].timer = GetRandomValue(4, 10);
                    player.score += 100;
                    game.customersServed++;
            }

            if (pizzas[j].position.x < 275 && pizzas[j].wasThrown
                && !customers[i].isThere) {
                    PlaySound(pizzaFell);
                    player.score -= 25;
                    pizzas[j].wasThrown = false;
                }
        }

        // Timer related
        game.framesCounter++;
        if (game.framesCounter == 180) {
            game.framesCounter = 0;
            customers[0].timer--;
            customers[1].timer--;
            customers[2].timer--;

            if (customers[0].timer == -1) {
                if (customers[0].isThere) {
                    PlaySound(defeat);
                    EndGame();
                }
            }

            if (customers[1].timer == -1) {
                if (customers[1].isThere) {
                    PlaySound(defeat);
                    EndGame();
                }
            }

            if (customers[2].timer == -1) {
                if (customers[2].isThere) {
                    PlaySound(defeat);
                    EndGame();
                }
            }
        }
    }
}

void DrawCustomer(void)
{
    for (int i = 0; i < CUSTOMERS_PER_LEVEL; i++) {
        if (customers[i].isThere) {
            DrawTexture(customer1, customers[i].position.x, customers[i].position.y, WHITE);
            DrawTextEx(font, FormatText("%d", customers[i].timer), (Vector2){ customers[i].position.x-40, customers[i].position.y+60 }, 24, 2, WHITE);
        }
    }
}
