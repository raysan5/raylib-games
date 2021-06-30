/*******************************************************************************************
*
*   LIGHT MY RITUAL [GLOBAL GAME JAM 2016]
*
*   Preparing a ritual session is not that easy.
*   You must light all the candles before the astral alignment finishes...
*   but dark creatures move in the shadows to put out all your lights!
*   Be fast! Be smart! Light my ritual!
*
*   This game has been created using raylib 1.6 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2016 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions

#include <stdlib.h>     // Required for:

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
//----------------------------------------------------------------------------------
GameScreen currentScreen = LOGO;
Font font = { 0 };
Color *lightsMap = NULL;
int lightsMapWidth = 0;
int lightsMapHeight = 0;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static int transToScreen = -1;

static Music music;

//----------------------------------------------------------------------------------
// Module Functions Declaration (local)
//----------------------------------------------------------------------------------
static void ChangeToScreen(int screen);     // Change to screen, no transition effect

static void TransitionToScreen(int screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)

static void UpdateDrawFrame(void);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "LIGHT MY RITUAL! [GGJ16]");

    // Global data loading (assets that must be available in all screens, i.e. fonts)
    InitAudioDevice();

    Image image = LoadImage("resources/lights_map.png");  // Load image in CPU memory (RAM)

    lightsMap = LoadImageColors(image);         // Get image pixels data as an array of Color
    lightsMapWidth = image.width;
    lightsMapHeight = image.height;

    UnloadImage(image);                         // Unload image from CPU memory (RAM)

    font = LoadFont("resources/font_arcadian.png");
    //doors = LoadTexture("resources/textures/doors.png");
    //sndDoor = LoadSound("resources/audio/door.ogg");

    music = LoadMusicStream("resources/audio/ambient.ogg");
    PlayMusicStream(music);
    SetMusicVolume(music, 1.0f);

    // Setup and Init first screen
    currentScreen = LOGO;
    //InitTitleScreen();
    //InitGameplayScreen();
    rlInitLogoScreen();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    switch (currentScreen)
    {
        case LOGO_RL: UnloadLogoScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        default: break;
    }

    // Unload all global loaded data (i.e. fonts) here!
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadImageColors(lightsMap);

    CloseAudioDevice();

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition (local)
//----------------------------------------------------------------------------------
// Change to screen, no transition effect
static void ChangeToScreen(int screen)
{
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        default: break;
    }

    switch (screen)
    {
        case LOGO_RL: InitLogoScreen(); break;
        case TITLE: InitTitleScreen(); break;
        case GAMEPLAY: InitGameplayScreen(); break;
        default: break;
    }

    currentScreen = screen;
}

// Request transition to next screen
static void TransitionToScreen(int screen)
{
    onTransition = true;
    transFromScreen = currentScreen;
    transToScreen = screen;
}

// Update transition effect
static void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 0.05f;

        if (transAlpha >= 1.0)
        {
            transAlpha = 1.0;

            switch (transFromScreen)
            {
                case LOGO: UnloadLogoScreen(); break;
                case TITLE: UnloadTitleScreen(); break;
                case GAMEPLAY: UnloadGameplayScreen(); break;
                default: break;
            }

            switch (transToScreen)
            {
                case LOGO:
                {
                    InitLogoScreen();
                    currentScreen = LOGO_RL;
                } break;
                case TITLE:
                {
                    InitTitleScreen();
                    currentScreen = TITLE;
                } break;
                case GAMEPLAY:
                {
                    InitGameplayScreen();
                    currentScreen = GAMEPLAY;
                } break;
                default: break;
            }

            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.05f;

        if (transAlpha <= 0)
        {
            transAlpha = 0;
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

                if (FinishTitleScreen() == 1)
                {
                    StopMusicStream(music);
                    TransitionToScreen(GAMEPLAY);
                }

            } break;
            case GAMEPLAY:
            {
                UpdateGameplayScreen();

                if (FinishGameplayScreen() == 1) ChangeToScreen(LOGO);
                else if (FinishGameplayScreen() == 2) TransitionToScreen(TITLE);

            } break;
            default: break;
        }
    }
    else
    {
        // Update transition (fade-in, fade-out)
        UpdateTransition();
    }

    if (currentScreen != GAMEPLAY) UpdateMusicStream(music);
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
            default: break;
        }

        if (onTransition) DrawTransition();

    EndDrawing();
    //----------------------------------------------------------------------------------
}
