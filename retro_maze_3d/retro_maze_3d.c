/*******************************************************************************************
*
*   GGJ 2021 - RETRO MAZE 3D
*
*   This game has been created using raylib 3.5 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include "raymath.h"

#include <math.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Define the 4 basic GameBoy green colors
#define GB_GREEN01 (Color){ 155, 188, 15, 255 }
#define GB_GREEN02 (Color){ 110, 150, 27, 255 }
#define GB_GREEN03 (Color){ 48, 98, 48, 255 }
#define GB_GREEN04 (Color){ 15, 56, 15, 255 }

// Define GPi CASE Gamepad controls
#define GPICASE_DPAD_LEFT           0   // GAMEPAD AXIS 6 < 0
#define GPICASE_DPAD_RIGHT          0   // GAMEPAD AXIS 6 > 0
#define GPICASE_DPAD_UP             0   // GAMEPAD AXIS 7 < 0
#define GPICASE_DPAD_DOWN           0   // GAMEPAD AXIS 7 > 0
#define GPICASE_BUTTON_A            0
#define GPICASE_BUTTON_B            1
#define GPICASE_BUTTON_X            2
#define GPICASE_BUTTON_Y            3
#define GPICASE_TRIGGER_LEFT        4
#define GPICASE_TRIGGER_RIGHT       5
#define GPICASE_BUTTON_SELECT       6
#define GPICASE_BUTTON_START        7

// First person camera
#define CAMERA_FIRST_PERSON_MIN_CLAMP       89.0f
#define CAMERA_FIRST_PERSON_MAX_CLAMP      -89.0f

#define PLAYER_MOVEMENT_SENSITIVITY         16.0f

//----------------------------------------------------------------------------------
// Enums and struct data types
//----------------------------------------------------------------------------------
typedef enum { LOGO = 0, TITLE, CREDITS, GAMEPLAY, ENDING } GameScreen;

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
const int screenWidth = 320;
const int screenHeight = 240;

// Game general variables
int currentScreen = LOGO;
int elementPositionY = -128;
int framesCounter = 0; 
int scrollingY = 0;    
int score = 0;         
int hiscore = 0;       
int titleState = 0;
int optionSelect = 0;       // Main menu option selection

Camera camera = { 0 };
static float playerEyesPosition = 0.3f;                 // Player eyes position from ground (in meters)
static Vector2 cameraAngle = { 0.0f, -8.0f*DEG2RAD };   // Camera rotation angle over its axis

Vector3 mapPosition = { 0.0f, 0.0f, 0.0f };     // Set map position
Vector3 playerPosition = { 0 };

int exitCellX = 19;
int exitCellY = 9;

float currentGamepadAxisValue[8] = { 0 };
float previousGamepadAxisValue[8] = { 0 };

bool exitGame = false;

int timeLevelSeconds = 300;

float creditsScrollingPosY = (float)screenHeight + 20.0f;

bool playerMoving = false;
int stepFrameCount = 0;

bool showInGameMenu = false;
bool showMinimapDebug = false;

Color *mapPixels = 0;
int playerCellX = 0;
int playerCellY = 0;

float staminaLevel = 100;

int endingResult = 0;       // 0-Lose, 1-Win

bool gpiCaseMode = false;

// Game resources variables
Texture2D texLogo = { 0 };
Texture2D texTitle = { 0 };
Texture2D texTitleShadow = { 0 };
Texture2D texTitle3d = { 0 };
Texture2D texTitle3dShadow = { 0 };
Texture2D texTitleLogo = { 0 };
Texture2D texMap = { 0 };
Texture2D texMapAtlas = { 0 };

Texture2D texGpiCase = { 0 };

Font font = { 0 };

Sound fxLogo = { 0 };
Sound fxMenuSelect = { 0 };
Sound fxMenuMove = { 0 };
Sound fxPause = { 0 };
Sound fxTitleRocks = { 0 };
Sound fxStep = { 0 };
Sound fxEnding = { 0 };

Music musicTitle = { 0 };
Music musicGameplay = { 0 };
Music musicCredits = { 0 };

Sound fxVoice[4] = { 0 };

Model model = { 0 };

// Render texture to draw full screen, enables screen scaling
RenderTexture2D screenTarget = { 0 };

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);          // Update and Draw one frame
static void UpdateCameraCustom(Camera *camera);    // Update camera custom (first person)

static bool IsGamepadAxisAsButtonPressed(int gamepad, int axis, bool positiveAxis);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_TRANSPARENT);
    InitWindow(screenWidth, screenHeight, "RETRO MAZE 3D [GGJ 2021]");
    
    // Textures loading
    texLogo = LoadTexture("resources/logo.png");
    texTitle = LoadTexture("resources/title.png");
    texTitleShadow = LoadTexture("resources/title_shadow.png");
    texTitle3d = LoadTexture("resources/title_3d.png");
    texTitle3dShadow = LoadTexture("resources/title_3d_shadow.png");
    texTitleLogo = LoadTexture("resources/title_raylib_logo.png");
    
    texGpiCase = LoadTexture("resources/gpi_case_front.png");

    // Text Font loading
    font = LoadFont("resources/alpha_beta.png");
    
    InitAudioDevice();
    
    // Audio loading
    fxLogo = LoadSound("resources/audio/logo.wav");
    fxMenuMove = LoadSound("resources/audio/menu_move.wav");
    fxMenuSelect = LoadSound("resources/audio/menu_select.wav");
    fxPause = LoadSound("resources/audio/pause.wav");
    fxTitleRocks = LoadSound("resources/audio/title_rocks.wav");
    fxStep = LoadSound("resources/audio/step.wav");
    fxEnding = LoadSound("resources/audio/ending.wav");

    musicTitle = LoadMusicStream("resources/audio/music_title.mp3");
    musicGameplay = LoadMusicStream("resources/audio/music_gameplay.mp3");
    musicCredits = LoadMusicStream("resources/audio/music_credits.mp3");
    musicCredits.looping = false;
    
    fxVoice[0] = LoadSound("resources/audio/voice_hello.wav");
    fxVoice[1] = LoadSound("resources/audio/voice_im_lost.wav");
    fxVoice[2] = LoadSound("resources/audio/voice_anyone_there.wav");
    fxVoice[3] = LoadSound("resources/audio/voice_can_you_hear_me.wav");
    
    // Define the camera to look into our 3d world
    // WARNING: Camera target is actually controlled by cameraAngle, so initial value is ignored...
    camera.position = (Vector3){ 2.5f, 0.3f, 23.5f };   // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type
    
    cameraAngle.x = 0.0f;               // Camera rotation angle in Y axis
    cameraAngle.y = -8.0f*DEG2RAD;      // Camera rotation angle in plane XY
    playerPosition = camera.position;   // Initial player position
    
    // Load map data (model anf texture)
    // NOTE: By default each cube is mapped to one part of texture atlas
    Image imMap = LoadImage("resources/game_map.png");  // Load texMap image (RAM)
    Mesh mesh = GenMeshCubicmap(imMap, (Vector3){ 1.0f, 1.0f, 1.0f });
    model = LoadModelFromMesh(mesh);                    // Load generated mesh into a model
    texMapAtlas = LoadTexture("resources/cubicmap_atlas.png");      // Load map texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texMapAtlas;     // Set map diffuse texture  
    mapPixels = LoadImageColors(imMap); // Get map image data to be used for collision detection

    // Convert image to custom GB colors for minimap usage
    ImageColorReplace(&imMap, WHITE, GB_GREEN03);
    ImageColorReplace(&imMap, BLACK, GB_GREEN02);
    texMap = LoadTextureFromImage(imMap);

    UnloadImage(imMap);                 // Unload image from RAM (image not required any more)

    // Load render texture to draw game on it, it could be useful for scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    screenTarget = LoadRenderTexture(640, 480);
    SetTextureFilter(screenTarget.texture, FILTER_BILINEAR);
    
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && !exitGame)    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------  
    UnloadImageColors(mapPixels);   // Unload color array
    
    UnloadRenderTexture(screenTarget);
    
    // Unload textures
    UnloadTexture(texLogo);
    UnloadTexture(texTitle);
    UnloadTexture(texTitleShadow);
    UnloadTexture(texTitle3d);
    UnloadTexture(texTitle3dShadow);
    UnloadTexture(texTitleLogo);
    UnloadTexture(texMap);          // Unload map texture
    UnloadTexture(texMapAtlas);     // Unload map atlas texture
    UnloadTexture(texGpiCase);

    UnloadFont(font);               // Unload fonts
    
    UnloadModel(model);             // Unload map model
    
    // Unload sounds
    UnloadSound(fxLogo);
    UnloadSound(fxMenuMove);
    UnloadSound(fxMenuSelect);
    UnloadSound(fxTitleRocks);
    UnloadSound(fxPause);
    UnloadSound(fxStep);
    UnloadSound(fxEnding);
    
    UnloadMusicStream(musicTitle);
    UnloadMusicStream(musicGameplay);
    UnloadMusicStream(musicCredits);
    
    for (int i = 0; i < 4; i++) UnloadSound(fxVoice[i]);
    
    CloseAudioDevice();
    
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------   
    currentGamepadAxisValue[6] = GetGamepadAxisMovement(0, 6);
    currentGamepadAxisValue[7] = GetGamepadAxisMovement(0, 7);
    
    switch (currentScreen)
    {
        case LOGO:
        {
            // Update LOGO screen
            framesCounter++;
            
            // Logo moving down logic (animation and sound playing)
            elementPositionY++;
            if (elementPositionY == (screenHeight/2 - 64 - 1)) PlaySound(fxLogo);
            if (elementPositionY > (screenHeight/2 - 64)) elementPositionY = screenHeight/2 - 64;

            if (framesCounter > 300)
            {
                framesCounter = 0;
                currentScreen = TITLE;
                
                // NOTE: elementPositionY is reused for title animation
                elementPositionY = -texTitle.width;
                
                // Play game music
                PlayMusicStream(musicTitle);
            }

        } break;
        case TITLE:
        {
            UpdateMusicStream(musicTitle);

            framesCounter++;
            
            if (titleState == 0)        // Title animation (moving down)
            {
                elementPositionY += 2;

                if (elementPositionY > 0) elementPositionY = 0;
                if (framesCounter > 140) titleState = 1;
            }
            else if (titleState == 1)   // Title 3D showing up
            {
                if (framesCounter > 140) 
                {
                    titleState = 2;
                    PlaySound(fxTitleRocks);
                }
            }
            
            if (IsKeyPressed(KEY_DOWN) || IsGamepadAxisAsButtonPressed(0, 7, true)) 
            {
                optionSelect++;
                PlaySound(fxMenuMove);
            }
            else if (IsKeyPressed(KEY_UP) || IsGamepadAxisAsButtonPressed(0, 7, false))
            {
                optionSelect--;
                PlaySound(fxMenuMove);
            }
            
            if (optionSelect < 0) optionSelect = 0;
            else if (optionSelect > 2) optionSelect = 2;
            
            if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GPICASE_BUTTON_A))
            {
                if (optionSelect == 0) 
                {
                    currentScreen = GAMEPLAY;        // Gameplay screen
                    
                    camera.position = (Vector3){ 2.5f, 0.3f, 23.5f };
                    playerPosition = camera.position;
                    
                    StopMusicStream(musicTitle);
                    PlayMusicStream(musicGameplay);
                }
                else if (optionSelect == 1) 
                {
                    currentScreen = CREDITS;    // Credits screen
                    creditsScrollingPosY = (float)screenHeight + 20.0f;
                    
                    StopMusicStream(musicTitle);
                    PlayMusicStream(musicCredits);
                }
                else if (optionSelect == 2) exitGame = true;            // Exit game
                
                framesCounter = 0;    
                PlaySound(fxMenuSelect);
            }
            
        } break;
        case GAMEPLAY:
        {
            UpdateMusicStream(musicGameplay);
            
            // In-game menu logic
            if (IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(0, GPICASE_BUTTON_START)) 
            {
                showInGameMenu = !showInGameMenu;
                PlaySound(fxPause);
            }
            
            // In-game minimap
            if (IsKeyPressed(KEY_M) || IsGamepadButtonPressed(0, GPICASE_BUTTON_SELECT)) showMinimapDebug = !showMinimapDebug;
            
            if (!showInGameMenu)    // Pause menu
            {
                // In game voices playing logic
                if (IsKeyPressed(KEY_V) || IsGamepadButtonPressed(0, GPICASE_BUTTON_X)) PlaySound(fxVoice[GetRandomValue(0, 3)]);

                Vector3 oldCamPos = camera.position;    // Store old camera position (to restore in case of collision)

                UpdateCameraCustom(&camera);      // Update camera

                // Check player collision (we simplify to 2D collision detection)
                Vector2 playerPos = { camera.position.x, camera.position.z };
                float playerRadius = 0.1f;  // Collision radius (player is modelled as a cilinder for collision)

                playerCellX = (int)(playerPos.x - mapPosition.x + 0.5f);
                playerCellY = (int)(playerPos.y - mapPosition.z + 0.5f);

                // Out-of-limits security check
                if (playerCellX < 0) playerCellX = 0;
                else if (playerCellX >= texMap.width) playerCellX = texMap.width - 1;

                if (playerCellY < 0) playerCellY = 0;
                else if (playerCellY >= texMap.height) playerCellY = texMap.height - 1;

                // Check map collisions using image data and player position
                // TODO: Improvement: Just check player surrounding cells for collision
                for (int y = 0; y < texMap.height; y++)
                {
                    for (int x = 0; x < texMap.width; x++)
                    {
                        if ((mapPixels[y*texMap.width + x].r == 255) &&       // Collision: white pixel, only check R channel
                            (CheckCollisionCircleRec(playerPos, playerRadius,
                            (Rectangle){ mapPosition.x - 0.5f + x*1.0f, mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
                        {
                            // Collision detected, reset camera position
                            camera.position = oldCamPos;
                        }
                    }
                }
                
                // Check ending condition: reach destination
                if ((playerCellX == exitCellX) && (playerCellY == exitCellY))
                {
                    currentScreen = ENDING;
                    PlaySound(fxEnding);
                    endingResult = 1;           // Player wins
                    
                    StopMusicStream(musicGameplay);
                }
                
                // Check ending condition: time limit
                framesCounter++;
                if (framesCounter == 60)
                {
                    timeLevelSeconds--;
                    framesCounter = 0;
                    if (timeLevelSeconds == 0)
                    {
                        currentScreen = ENDING;
                        PlaySound(fxEnding);
                        
                        StopMusicStream(musicGameplay);
                    }
                }
                
                // Check ending condition: stamina level
                if (staminaLevel < 0.0f)
                {
                    currentScreen = ENDING;
                    PlaySound(fxEnding);
                    StopMusicStream(musicGameplay);
                }
            }
            else
            {
                if (IsKeyPressed(KEY_DOWN) || IsGamepadAxisAsButtonPressed(0, 7, true)) 
                {
                    optionSelect++;
                    PlaySound(fxMenuMove);
                }
                else if (IsKeyPressed(KEY_UP) || IsGamepadAxisAsButtonPressed(0, 7, false))
                {
                    optionSelect--;
                    PlaySound(fxMenuMove);
                }
                
                if (optionSelect < 0) optionSelect = 0;
                else if (optionSelect > 2) optionSelect = 2;
                
                if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GPICASE_BUTTON_A))
                {
                    if (optionSelect == 0) showInGameMenu = false;      // Resume game
                    else if (optionSelect == 1) 
                    {
                        showInGameMenu = false;
                        currentScreen = TITLE;      // Title screen
                        
                        StopMusicStream(musicGameplay);
                        PlayMusicStream(musicTitle);
                    }
                    else if (optionSelect == 2) exitGame = true;        // Exit game
                    
                    framesCounter = 0;    
                    PlaySound(fxMenuSelect);
                }
            }
            
            // Steps sounds
            if (playerMoving)
            {
                staminaLevel -= 0.01f;
                stepFrameCount++;
                
                if (stepFrameCount%20 == 0) 
                {
                    SetSoundPitch(fxStep, (float)GetRandomValue(80, 120)/100.0f);
                    PlaySound(fxStep);
                }
            }

        } break;
        case CREDITS:
        {
            UpdateMusicStream(musicCredits);
            
            creditsScrollingPosY -= 0.6f;
            if (creditsScrollingPosY < (-screenHeight + 36)) creditsScrollingPosY = -screenHeight + 36;
            
            if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GPICASE_BUTTON_A))
            {
                currentScreen = TITLE;
                titleState = 0;
                framesCounter = 0;
                elementPositionY = -texTitle.height;
                PlaySound(fxMenuSelect);
                
                StopMusicStream(musicCredits);
                PlayMusicStream(musicTitle);
            }
        } break;
        case ENDING:
        {
            if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GPICASE_BUTTON_A))
            {
                currentScreen = CREDITS;
                titleState = 0;
                framesCounter = 0;
                endingResult = 0;
                PlaySound(fxMenuSelect);
                
                creditsScrollingPosY = (float)screenHeight + 20.0f;
                PlayMusicStream(musicCredits);
            }
            
        } break;
        default: break;
    }
    
    if (IsKeyPressed(KEY_G)) 
    {
        gpiCaseMode = !gpiCaseMode;
        Vector2 currentWindowPos = GetWindowPosition();
        
        if (gpiCaseMode)
        {
            SetWindowState(FLAG_WINDOW_UNDECORATED);
            SetWindowSize(480, 800);
            SetWindowPosition(currentWindowPos.x - 79, currentWindowPos.y - 114);
        }
        else
        {
            ClearWindowState(FLAG_WINDOW_UNDECORATED);
            SetWindowSize(screenWidth, screenHeight);
            SetWindowPosition(currentWindowPos.x + 79, currentWindowPos.y + 114);
        }
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
 
    // Render all screen to a texture
    BeginTextureMode(screenTarget);
        ClearBackground(GB_GREEN01);
        
        switch (currentScreen)
        {
            case LOGO:
            {
                // Draw LOGO screen
                DrawTexture(texLogo, screenWidth/2 - 64, elementPositionY, WHITE);
                
            } break;
            case TITLE:
            {
                // Draw title
                if (titleState == 2)   // Title/3D shadow effect
                {
                    DrawTexture(texTitleShadow, 32, 4, WHITE);
                    DrawTexture(texTitle3dShadow, 32, 90 + 4, WHITE);
                }
                DrawTexture(texTitle, 32, elementPositionY, WHITE);
                if (titleState >= 1) DrawTexture(texTitle3d, 32, 90, WHITE);
                
                DrawRectangle(95, 165 + optionSelect*16, 130, 16, GB_GREEN02);
                DrawTextEx(font, "START GAME", (Vector2){ 120, 167 }, font.baseSize, 2, (optionSelect == 0)? GB_GREEN03 : GB_GREEN02);
                DrawTextEx(font, "CREDITS", (Vector2){ 132, 167 + 16 }, font.baseSize, 2, (optionSelect == 1)? GB_GREEN03 : GB_GREEN02);
                DrawTextEx(font, "EXIT GAME", (Vector2){ 124, 167 + 32 }, font.baseSize, 2, (optionSelect == 2)? GB_GREEN03 : GB_GREEN02);
                
                DrawTextEx(font, "GGJ 2021", (Vector2){ 10, 240 - 18 }, font.baseSize, 1, GB_GREEN02);
                DrawTexture(texTitleLogo, 320 - texTitleLogo.width + 5, 240 - texTitleLogo.height + 3, WHITE);
                
                /*
                if (IsGamepadAvailable(GAMEPAD_PLAYER1))
                {
                    if (GetGamepadButtonPressed() != -1) DrawText(TextFormat("DETECTED BUTTON: %i", GetGamepadButtonPressed()), 10, screenHeight - 25, 20, RED);
                    else DrawText("DETECTED BUTTON: NONE", 10, screenHeight - 25, 20, GRAY);
                }
                */

            } break;
            case GAMEPLAY:
            {
                BeginMode3D(camera);
                    DrawModel(model, mapPosition, 1.0f, WHITE);     // Draw maze map
                    
                    // Draw ending cube for reference
                    DrawCube((Vector3){ mapPosition.x + (float)exitCellX, 0.5f, mapPosition.z + (float)exitCellY }, 1.0f, 1.0f, 1.0f, ColorAlpha(GREEN, 0.5f));

                EndMode3D();
                
                DrawRectangle(0, screenHeight - 20, screenWidth, 20, GB_GREEN01);
                DrawTextEx(font, "STAMINA:", (Vector2){ 10, screenHeight - 16 }, font.baseSize, 1, GB_GREEN03);
                DrawRectangleLines(70, screenHeight - 16, 100, 13, GB_GREEN03);
                DrawRectangle(70 + 2, screenHeight - 16 + 2, (int)staminaLevel - 4, 13 - 4, GB_GREEN02);
                DrawTextEx(font, TextFormat("TIME: %i:%02is", timeLevelSeconds/60, timeLevelSeconds%60), (Vector2){ 240, screenHeight - 16 }, font.baseSize, 1, GB_GREEN03);
                
                if (showMinimapDebug)
                {
                    // Draw minimap
                    DrawTextureEx(texMap, (Vector2){ screenWidth/2 - texMap.width*2, screenHeight/2 - texMap.height*2 }, 0.0f, 4.0f, WHITE);
                    DrawRectangle(screenWidth/2 - texMap.width*2 + playerCellX*4,screenHeight/2 - texMap.height*2 + playerCellY*4, 4, 4, GB_GREEN04);   // Draw player
                    DrawRectangle(screenWidth/2 - texMap.width*2 + exitCellX*4,screenHeight/2 - texMap.height*2 + exitCellY*4, 4, 4, LIME);         // Draw exit
                    //DrawTextEx(font, TextFormat("PLAYER POSITION: [%i, %i]", playerCellX, playerCellY), (Vector2){ 10, screenHeight - 16 }, font.baseSize, 1, GB_GREEN03);
                }

                if (showInGameMenu)
                {
                    DrawRectangle(0, 80, screenWidth, 84, GB_GREEN01);
                    DrawRectangle(screenWidth/2 - 65, 100 + optionSelect*16, 130, 16, GB_GREEN02);
                    DrawTextEx(font, "RESUME GAME", (Vector2){ screenWidth/2 - MeasureTextEx(font, "RESUME GAME", font.baseSize, 2).x/2, 102 }, font.baseSize, 2, (optionSelect == 0)? GB_GREEN03 : GB_GREEN02);
                    DrawTextEx(font, "BACK TO TITLE", (Vector2){ screenWidth/2 - MeasureTextEx(font, "BACK TO TITLE", font.baseSize, 2).x/2, 102 + 16 }, font.baseSize, 2, (optionSelect == 1)? GB_GREEN03 : GB_GREEN02);
                    DrawTextEx(font, "EXIT GAME", (Vector2){ screenWidth/2 - MeasureTextEx(font, "EXIT GAME", font.baseSize, 2).x/2, 102 + 32 }, font.baseSize, 2, (optionSelect == 2)? GB_GREEN03 : GB_GREEN02);
                }

            } break;
            case CREDITS:
            {
                // Draw scrolling credits
                DrawTextEx(font, "GAME DESIGN", (Vector2){ screenWidth/2 - MeasureTextEx(font, "GAME DESIGN", font.baseSize, 2).x/2, creditsScrollingPosY }, font.baseSize, 2, GB_GREEN02);
                DrawTextEx(font, "Ramon Santamaria (Ray)", (Vector2){ screenWidth/2 - MeasureTextEx(font, "Ramon Santamaria (Ray)", font.baseSize, 2).x/2, creditsScrollingPosY + 16 }, font.baseSize, 2, GB_GREEN03);
                
                DrawTextEx(font, "GAME PROGRAMMING", (Vector2){ screenWidth/2 - MeasureTextEx(font, "GAME PROGRAMMING", font.baseSize, 2).x/2, creditsScrollingPosY + 50 }, font.baseSize, 2, GB_GREEN02);
                DrawTextEx(font, "Ramon Santamaria (Ray)", (Vector2){ screenWidth/2 - MeasureTextEx(font, "Ramon Santamaria (Ray)", font.baseSize, 2).x/2, creditsScrollingPosY + 66 }, font.baseSize, 2, GB_GREEN03);
                
                DrawTextEx(font, "GAME ART", (Vector2){ screenWidth/2 - MeasureTextEx(font, "GAME ART", font.baseSize, 2).x/2, creditsScrollingPosY + 100 }, font.baseSize, 2, GB_GREEN02);
                DrawTextEx(font, "Ramon Santamaria (Ray)", (Vector2){ screenWidth/2 - MeasureTextEx(font, "Ramon Santamaria (Ray)", font.baseSize, 2).x/2, creditsScrollingPosY + 116 }, font.baseSize, 2, GB_GREEN03);
                
                DrawTextEx(font, "GAME AUDIO", (Vector2){ screenWidth/2 - MeasureTextEx(font, "GAME AUDIO", font.baseSize, 2).x/2, creditsScrollingPosY + 150 }, font.baseSize, 2, GB_GREEN02);
                DrawTextEx(font, "German Sartori", (Vector2){ screenWidth/2 - MeasureTextEx(font, "Germán Sartori", font.baseSize, 2).x/2, creditsScrollingPosY + 166 }, font.baseSize, 2, GB_GREEN03);
                
                DrawTextEx(font, "powered by", (Vector2){ screenWidth/2 - MeasureTextEx(font, "powered by", font.baseSize, 2).x/2, creditsScrollingPosY + 240 }, font.baseSize, 2, GB_GREEN02);
                DrawTexture(texLogo, screenWidth/2 - 64, creditsScrollingPosY + 256, WHITE);
                
                DrawRectangle(screenWidth - 60, screenHeight - 24, 50, 16, GB_GREEN02);
                DrawTextEx(font, "BACK", (Vector2){ screenWidth - 50, screenHeight - 22 }, font.baseSize, 2, GB_GREEN03);
            } break;
            case ENDING:
            {
                if (endingResult == 1) 
                {
                    DrawTextEx(font, "YOU ARE FOUND!", (Vector2){ screenWidth/2 - MeasureTextEx(font, "YOU ARE FOUND!", font.baseSize*2, 2).x/2, 100 }, font.baseSize*2, 2, GB_GREEN03);
                }
                else DrawTextEx(font, "YOU ARE LOST...", (Vector2){ screenWidth/2 - MeasureTextEx(font, "YOU ARE LOST...", font.baseSize*2, 2).x/2, 100 }, font.baseSize*2, 2, GB_GREEN03);
                
                DrawRectangle(screenWidth/2 - 65, 170, 130, 16, GB_GREEN02);
                DrawTextEx(font, "CONTINUE", (Vector2){ screenWidth/2 - MeasureTextEx(font, "CONTINUE", font.baseSize, 2).x/2, 172 }, font.baseSize, 2, GB_GREEN03);
                
            } break;
            default: break;
        }
        
    EndTextureMode();

    BeginDrawing();
    
        ClearBackground(GB_GREEN01);
        
        // Draw render texture to screen
        if (gpiCaseMode)
        {
            ClearBackground(BLANK);
            DrawTexture(texGpiCase, 0, 0, WHITE);
            BeginScissorMode(79, 114, 320, 240);
            DrawTextureRec(screenTarget.texture, (Rectangle){ 0, 0, screenTarget.texture.width, -screenTarget.texture.height }, (Vector2){ 79, 114 }, WHITE);
            EndScissorMode();
        }
        else DrawTextureRec(screenTarget.texture, (Rectangle){ 0, 0, screenTarget.texture.width, -screenTarget.texture.height }, (Vector2){ 0, 0 }, WHITE);

    EndDrawing();
    
    previousGamepadAxisValue[6] = currentGamepadAxisValue[6];
    previousGamepadAxisValue[7] = currentGamepadAxisValue[7];
    //----------------------------------------------------------------------------------
}

// Update camera custom (first person)
void UpdateCameraCustom(Camera *camera)
{
    // Inputs detection
    bool direction[4] = { (IsKeyDown(KEY_W) || (GetGamepadAxisMovement(0, 7) < 0)),
                          (IsKeyDown(KEY_S) || (GetGamepadAxisMovement(0, 7) > 0)),
                          (IsKeyDown(KEY_D) || (GetGamepadAxisMovement(0, 6) > 0)),
                          (IsKeyDown(KEY_A) || (GetGamepadAxisMovement(0, 6) < 0)) };
                          
    if (direction[0] || direction[1] || direction[2] || direction[3]) playerMoving = true;
    else playerMoving = false;

    camera->position.x += (sinf(cameraAngle.x)*direction[1] -
                           sinf(cameraAngle.x)*direction[0] -
                           cosf(cameraAngle.x)*direction[3] +
                           cosf(cameraAngle.x)*direction[2])/PLAYER_MOVEMENT_SENSITIVITY;

    camera->position.y += (sinf(cameraAngle.y)*direction[0] -
                           sinf(cameraAngle.y)*direction[1])/PLAYER_MOVEMENT_SENSITIVITY;

    camera->position.z += (cosf(cameraAngle.x)*direction[1] -
                           cosf(cameraAngle.x)*direction[0] +
                           sinf(cameraAngle.x)*direction[3] -
                           sinf(cameraAngle.x)*direction[2])/PLAYER_MOVEMENT_SENSITIVITY;
                           
    // Camera rotation over its own Y axis
    if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(0, GPICASE_TRIGGER_LEFT)) cameraAngle.x += (1.0f*DEG2RAD);      // GAMEPAD_BUTTON_LEFT_TRIGGER_1    // GAMEPAD_BUTTON_LEFT_FACE_LEFT    // DPAD LEFT
    if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonDown(0, GPICASE_TRIGGER_RIGHT)) cameraAngle.x -= (1.0f*DEG2RAD);     // GAMEPAD_BUTTON_RIGHT_TRIGGER_1   // GAMEPAD_BUTTON_LEFT_FACE_RIGHT   // DPAD RIGHT
    
    // Camera angle clamp to limits
    if (cameraAngle.y > CAMERA_FIRST_PERSON_MIN_CLAMP*DEG2RAD) cameraAngle.y = CAMERA_FIRST_PERSON_MIN_CLAMP*DEG2RAD;
    else if (cameraAngle.y < CAMERA_FIRST_PERSON_MAX_CLAMP*DEG2RAD) cameraAngle.y = CAMERA_FIRST_PERSON_MAX_CLAMP*DEG2RAD;

    // Recalculate camera target considering translation and rotation
    Matrix translation = MatrixTranslate(0.0f, 0.0f, 1.0f);
    Matrix rotation = MatrixRotateXYZ((Vector3){ PI*2 - cameraAngle.y, PI*2 - cameraAngle.x, 0 });
    Matrix transform = MatrixMultiply(translation, rotation);

    camera->target.x = camera->position.x - transform.m12;
    camera->target.y = camera->position.y - transform.m13;
    camera->target.z = camera->position.z - transform.m14;

    // Camera position update
    camera->position.y = playerEyesPosition;
}

static bool IsGamepadAxisAsButtonPressed(int gamepad, int axis, bool positiveAxis)
{
    bool pressed = false;
    
    if ((int)roundf(previousGamepadAxisValue[axis]) == 0)
    {
        if (positiveAxis && (currentGamepadAxisValue[axis] > 0.2f)) pressed = true; // Positive axis pressed
        if (!positiveAxis && (currentGamepadAxisValue[axis] < -0.2f)) pressed = true;            // Negative axis pressed
    }
    
    return pressed;
}