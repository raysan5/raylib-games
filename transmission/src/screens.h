/**********************************************************************************************
*
*   raylib - transmission mission
*
*   Screens Functions Declarations (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2019 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef SCREENS_H
#define SCREENS_H

#define MAX_CODING_WORDS    12
#define MAX_MISSION_WORDS    8

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { LOGO = 0, TITLE, MISSION, GAMEPLAY, ENDING } GameScreen;

// Words to be coded or coding words
typedef struct Word {
    int id;
    Rectangle rec;
    Rectangle iniRec;
    bool hover;
    bool picked;
    char text[32];          // text
} Word;

// Mission information
typedef struct Mission {
    int id;
    char brief[512];        // Mission briefing
    char key[32];           // Mission keyword
    char msg[256];          // Message to be coded
    int wordsCount;         // Number of words to coded
    int sols[10];            // Solution code, depends on wordsCount
} Mission;

//----------------------------------------------------------------------------------
// Global Variables Declaration  (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;
extern Music music;
extern Sound fxButton;
extern Rectangle recButton;
extern float fadeButton;
extern Color colorButton;
extern Texture2D texButton;
extern Vector2 textPositionButton;
extern int fontSizeButton;
extern Color textColorButton;
extern int currentMission;
extern int totalMissions;
extern Font fontMission;
extern Word messageWords[MAX_MISSION_WORDS];


#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Global Functions Declaration (shared by several modules)
//----------------------------------------------------------------------------------
bool IsButtonPressed();
void DrawButton(const char *text);
Mission *LoadMissions(const char *fileName);

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitLogoScreen(void);
void UpdateLogoScreen(void);
void DrawLogoScreen(void);
void UnloadLogoScreen(void);
int FinishLogoScreen(void);

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
void UnloadTitleScreen(void);
int FinishTitleScreen(void);

//----------------------------------------------------------------------------------
// Mission Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitMissionScreen(void);
void UpdateMissionScreen(void);
void DrawMissionScreen(void);
void UnloadMissionScreen(void);
int FinishMissionScreen(void);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
int FinishGameplayScreen(void);

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(void);
void UnloadEndingScreen(void);
int FinishEndingScreen(void);

#ifdef __cplusplus
}
#endif

#endif // SCREENS_H