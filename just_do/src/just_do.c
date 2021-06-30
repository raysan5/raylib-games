/*******************************************************************************************
*
*   JUST DO [GLOBAL GAME JAM 2015]
*
*   Experimental puzzle game that lets the user try to find a logic
*   solution to different shape-color-based situations.
*
*   This game has been created using raylib 1.6 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
//----------------------------------------------------------------------------------
GameScreen currentScreen = LOGO;
Sound levelWin = { 0 };

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static const int screenWidth = 1280;     // Moved to screens.h
static const int screenHeight = 720;     // Moved to screens.h

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static int transToScreen = -1;
static int framesCounter = 0;

static Music music = { 0 };

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
    InitWindow(screenWidth, screenHeight, "JUST DO [GGJ15]");

    // Load global data here (assets that must be available in all screens, i.e. fonts)
    InitAudioDevice();

    levelWin = LoadSound("resources/win.wav");
    music = LoadMusicStream("resources/ambient.ogg");

    // Setup and Init first screen
    currentScreen = LOGO;
    InitLogoScreen();

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

    // Unload all global loaded data (i.e. fonts) here!
    UnloadSound(levelWin);
    UnloadMusicStream(music);

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
        case LEVEL00: UnloadLevel00Screen(); break;
        case LEVEL01: UnloadLevel01Screen(); break;
        default: break;
    }

    switch (screen)
    {
        case LOGO: InitLogoScreen(); break;
        case LEVEL00: InitLevel00Screen(); break;
        case LEVEL01: InitLevel01Screen(); break;
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
        transAlpha += 0.02f;

        if (transAlpha >= 1.0)
        {
            transAlpha = 1.0;
            currentScreen = transToScreen;
            transFadeOut = true;
            framesCounter = 0;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.02f;

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
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, transAlpha));
}

// Update and draw one frame
static void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    if (currentScreen != LOGO) UpdateMusicStream(music);

    if (!onTransition)
    {
        if (IsKeyPressed('0'))
        {
            TransitionToScreen(LEVEL00);
            InitLevel00Screen();
        }
        else if (IsKeyPressed('1'))
        {
            TransitionToScreen(LEVEL01);
            InitLevel01Screen();
        }
        else if (IsKeyPressed('2'))
        {
            TransitionToScreen(LEVEL02);
            InitLevel02Screen();
        }
        else if (IsKeyPressed('3'))
        {
            TransitionToScreen(LEVEL03);
            InitLevel03Screen();
        }
        else if (IsKeyPressed('4'))
        {
            TransitionToScreen(LEVEL04);
            InitLevel04Screen();
        }
        else if (IsKeyPressed('5'))
        {
            TransitionToScreen(LEVEL05);
            InitLevel05Screen();
        }
        else if (IsKeyPressed('6'))
        {
            TransitionToScreen(LEVEL06);
            InitLevel06Screen();
        }
        else if (IsKeyPressed('7'))
        {
            TransitionToScreen(LEVEL07);
            InitLevel07Screen();
        }
        else if (IsKeyPressed('8'))
        {
            TransitionToScreen(LEVEL08);
            InitLevel08Screen();
        }
        else if (IsKeyPressed('9'))
        {
            TransitionToScreen(LEVEL09);
            InitLevel08Screen();
        }

        switch(currentScreen)
        {
            case LOGO:
            {
                UpdateLogoScreen();

                if (FinishLogoScreen())
                {
                    UnloadLogoScreen();
                    TransitionToScreen(LEVEL00);
                    InitLevel00Screen();

                    PlayMusicStream(music);
                    SetMusicVolume(music, 0.6f);
                }
            } break;
            case LEVEL00:
            {
                UpdateLevel00Screen();

                if (FinishLevel00Screen())
                {
                    UnloadLevel00Screen();
                    TransitionToScreen(LEVEL01);
                    InitLevel01Screen();
                }
            } break;
            case LEVEL01:
            {
                UpdateLevel01Screen();

                if (FinishLevel01Screen())
                {
                    UnloadLevel01Screen();
                    TransitionToScreen(LEVEL02);
                    InitLevel02Screen();
                }
            } break;
            case LEVEL02:
            {
                UpdateLevel02Screen();

                if (FinishLevel02Screen())
                {
                    UnloadLevel02Screen();
                    TransitionToScreen(LEVEL03);
                    InitLevel03Screen();
                }
            } break;
            case LEVEL03:
            {
                UpdateLevel03Screen();

                if (FinishLevel03Screen())
                {
                    UnloadLevel03Screen();
                    TransitionToScreen(LEVEL04);
                    InitLevel04Screen();
                }
            } break;
            case LEVEL04:
            {
                UpdateLevel04Screen();

                if (FinishLevel04Screen())
                {
                    UnloadLevel04Screen();
                    TransitionToScreen(LEVEL05);
                    InitLevel05Screen();
                }
            } break;
            case LEVEL05:
            {
                UpdateLevel05Screen();

                if (FinishLevel05Screen())
                {
                    UnloadLevel05Screen();
                    TransitionToScreen(LEVEL06);
                    InitLevel06Screen();
                }
            } break;
            case LEVEL06:
            {
                UpdateLevel06Screen();

                if (FinishLevel06Screen())
                {
                    UnloadLevel06Screen();
                    TransitionToScreen(LEVEL07);
                    InitLevel07Screen();
                }
            } break;
            case LEVEL07:
            {
                UpdateLevel07Screen();

                if (FinishLevel07Screen())
                {
                    UnloadLevel07Screen();
                    TransitionToScreen(LEVEL08);
                    InitLevel08Screen();
                }
            } break;
            case LEVEL08:
            {
                UpdateLevel08Screen();

                if (FinishLevel08Screen())
                {
                    UnloadLevel08Screen();
                    TransitionToScreen(LEVEL09);
                    InitLevel09Screen();
                }
            } break;
            case LEVEL09:
            {
                UpdateLevel09Screen();

                if (FinishLevel09Screen())
                {
                    UnloadLevel09Screen();
                    TransitionToScreen(LEVEL00);
                    InitLevel00Screen();
                }
            } break;
            default: break;
        }
    }
    else UpdateTransition(); // Update transition (fade-in, fade-out)
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);

        switch(currentScreen)
        {
            case LOGO: DrawLogoScreen(); break;
            case LEVEL00: DrawLevel00Screen(); break;
            case LEVEL01: DrawLevel01Screen(); break;
            case LEVEL02: DrawLevel02Screen(); break;
            case LEVEL03: DrawLevel03Screen(); break;
            case LEVEL04: DrawLevel04Screen(); break;
            case LEVEL05: DrawLevel05Screen(); break;
            case LEVEL06: DrawLevel06Screen(); break;
            case LEVEL07: DrawLevel07Screen(); break;
            case LEVEL08: DrawLevel08Screen(); break;
            case LEVEL09: DrawLevel09Screen(); break;
            default: break;
        }

        if (onTransition) DrawTransition();

    EndDrawing();
    //----------------------------------------------------------------------------------
}
