/*******************************************************************************************
*
*   raylib - classic game: platformer
*
*   Sample game developed by Agnis "NeZvers" Aldins (@nezvers)
*
*   This game has been created using raylib v3.7 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Agnis "NeZvers" Aldins (@nezvers) and Ramon Santamaria (@raysan5)

Example showcases:
    * Use of 1D array for a tilemap and using it for collision;
    * Simple movement for a platformer (WASD + space) and top-down (TAB to switch between)
    * Rectangle based collision for movement and collectibles
    * Integer scaling with resizable window
    * Programmer/ procedural art using only built-in shape drawing

Main()
    * GameInit()
        * Reset()
    * GameLoop()
        * GameUpdate()
            * UpdateScreen();
            * UpdatePlayer();
                * RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity)
                * RectangleResize(Rectangle *rect, Vector2 *size)
                * RectangleListFromTiles(Rectangle *rect, Grid *grid)
                * RectangleTileCollision(Rectangle *rect, Vector2 *velocity, RectList *list)
            * UpdateCoin();

        * GameDraw()
            * DrawTileGrid()
            * DrawTileMap()
            * DrawCoins()
            * DrawPlayer()
            * DrawScoreText()

********************************************************************************************/

#include "raylib.h"
#include "math.h"

//#define PLATFORM_WEB
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

bool PLATFORMER = true; // toggle to top-down movement with TAB

//tile collision types
#define EMPTY 0
#define BLOCK 1
typedef struct{
    float x;
    float y;
    int w;      // width
    int h;      // height
    int s;      // cell size (square cells)
    int *cell;  // array of tiles
}Grid;

// List of Rectangles to check for a collision.
typedef struct{
    Rectangle *rect;
    int size;
}RectList;

void GameInit();
void GameUpdate();
void GameDraw();
void GameLoop(){GameUpdate(); GameDraw();}
void Reset();

void DrawTileGrid();
void DrawTileMap();
void DrawCoins();
void DrawPlayer();
void DrawScoreText();

void UpdateScreen();
void UpdatePlayer();
void UpdateCoin();

void        RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity);
Rectangle   RectangleResize(Rectangle *rect, Vector2 *size);
RectList*   RectangleListFromTiles(Rectangle *rect, Grid *grid);
void        RectangleTileCollision(Rectangle *rect, Vector2 *velocity, RectList *list);

#define MAP_W 20
#define MAP_H 12
int screenWidth = 32*MAP_W;
int screenHeight = 32*MAP_H;
const int gameWidth = 32*MAP_W;
const int gameHeight = 32*MAP_H;
RenderTexture viewport;
int scale = 1;
Vector2 vpOffset = (Vector2){0.0f, 0.0f};

Rectangle player = {32.0f * 2, 32.0f * 8, 32.0f, 32.0f};

#define COIN_COUNT 10
Rectangle coins[COIN_COUNT] = {0};
bool visible[COIN_COUNT] = {0};
int points = 0;
int time = 0;       // For animation

Grid map;
int tiles[] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

int main(void){
    GameInit();
    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(GameLoop, 0, 1);
    #else
    SetTargetFPS(60);
    while (!WindowShouldClose()){
        GameLoop();
    }
    UnloadRenderTexture(viewport);
    #endif
    CloseWindow();
    return 0;
}

void GameInit() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "classic game: platformer");
    viewport = LoadRenderTexture(gameWidth, gameHeight);
    map.x = 0.0f;
    map.y = 0.0f;
    map.w = MAP_W;
    map.h = MAP_H;
    map.s = 32;
    map.cell = tiles;
    Reset();
}

void Reset(){
    const float s = 32.0f;
    player = (Rectangle){s * 2, s * 8, s, s};
    points = 0;
    time = 0;
    
    coins[0] = (Rectangle){s * 1.5f, s * 8, 10.0f, 10.0f};
    coins[1] = (Rectangle){s * 3.5f, s * 6, 10.0f, 10.0f};
    coins[2] = (Rectangle){s * 4.5f, s * 6, 10.0f, 10.0f};
    coins[3] = (Rectangle){s * 5.5f, s * 6, 10.0f, 10.0f};
    coins[4] = (Rectangle){s * 8.5f, s * 3, 10.0f, 10.0f};
    coins[5] = (Rectangle){s * 9.5f, s * 3, 10.0f, 10.0f};
    coins[6] = (Rectangle){s * 10.5f, s * 3, 10.0f, 10.0f};
    coins[7] = (Rectangle){s * 14.5f, s * 4, 10.0f, 10.0f};
    coins[8] = (Rectangle){s * 15.5f, s * 4, 10.0f, 10.0f};
    coins[9] = (Rectangle){s * 17.5f, s * 2, 10.0f, 10.0f};
    
    for (int i = 0; i < COIN_COUNT; i++){visible[i] = true;}
}

void GameUpdate(){
    
    UpdateScreen();// Adapt to resolution
    UpdatePlayer();
    UpdateCoin();
}

void UpdateScreen(){
    // Adapt to resolution
    if (IsWindowResized()){
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        scale = MAX(1, MIN((screenWidth/gameWidth), (screenHeight/gameHeight)));
        vpOffset.x = (screenWidth - (gameWidth * scale)) / 2;
        vpOffset.y = (screenHeight - (gameHeight * scale)) / 2;
    }
}

void UpdatePlayer(){
    const float maxSpd = 6.0f;
    const float acc = 0.1f;
    const float grav = 0.5f;
    const float jmpImpulse = -10.0f;
    const int jmpBufferTime = 30;
    static bool isGrounded = false;
    static int jmpBuffer = 0;
    static int dirX = 0;
    static int dirY = 0;
    static Vector2 vel = {0};
    static Vector2 prevVel = {0};
    
    // INPUT
    dirX = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    dirY = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W));
    if(IsKeyPressed(KEY_TAB)){PLATFORMER = !PLATFORMER;}
    
    // HORIZONTAL SPEED
    vel.x += (dirX * maxSpd - vel.x) * acc;
    if (vel.x < -maxSpd){
        vel.x = -maxSpd;
    }
    else if (vel.x > maxSpd){
        vel.x = maxSpd;
    }
    
    // VERTICAL SPEED
    if (PLATFORMER){
        if (isGrounded && jmpBuffer != jmpBufferTime){
            jmpBuffer = jmpBufferTime;
        }
        if (isGrounded && IsKeyPressed(KEY_SPACE)){
            vel.y = jmpImpulse;
            jmpBuffer = 0;
        }
        else if (jmpBuffer > 0 && IsKeyPressed(KEY_SPACE)){
            vel.y = jmpImpulse;
            jmpBuffer = 0;
        }
        else{
            if (!IsKeyDown(KEY_SPACE) && vel.y < jmpImpulse * 0.2){
                vel.y = jmpImpulse * 0.2;
            }
            else{
                vel.y += grav;
                if (vel.y > -jmpImpulse){
                    vel.y = -jmpImpulse;
                }
                if (jmpBuffer > 0){
                    jmpBuffer -= 1;
                }
            }
        }
        prevVel = vel;   // for ground check
    }
    else{
        // TOP-DOWN
        vel.y += (dirY * maxSpd - vel.y) * acc;
        if (vel.y < -maxSpd){
            vel.y = -maxSpd;
        }
        else if (vel.y > maxSpd){
            vel.y = maxSpd;
        }
    }
    
    
    RectangleCollisionUpdate(&player, &vel);
    isGrounded = prevVel.y > 0.0f && vel.y <= 0.0001f;  // naive way to check grounded state
    player.x += vel.x;
    player.y += vel.y;
}

void UpdateCoin(){
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]){
            if (CheckCollisionRecs(coins[i], player)){
                visible[i] = false;
                points += 1;
            }
        }
    }
    
    if (points == COIN_COUNT && IsKeyPressed(KEY_ENTER)){
        Reset();
    }
}

void GameDraw(){
    // Viewport scaling
    const Vector2 origin = (Vector2){0.0f, 0.0f};
    const Rectangle vp_r = (Rectangle){0.0f,gameHeight,gameWidth, -gameHeight}; // flip vertically: position = left-bottom
    Rectangle out_r = (Rectangle){vpOffset.x, vpOffset.y, gameWidth * scale, gameHeight * scale};
    
    // Render game's viewport
    BeginTextureMode(viewport);
        DrawRectangle(0, 0, gameWidth, gameHeight, SKYBLUE); // Background
        DrawTileMap();
        DrawTileGrid();
        DrawScoreText();
        DrawCoins();
        DrawPlayer();
    EndTextureMode();
    
    // Draw the viewport
    BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(viewport.texture, vp_r, out_r, origin, 0.0f, WHITE);
    EndDrawing();
}

void DrawTileMap(){
    for (int y = 0; y < map.h; y++){
        for (int x = 0; x < map.w; x++){
            int i = x + y * map.w;
            int tile = map.cell[i];
            if (tile){
                float cellX = (map.x + map.s * x);
                float cellY = (map.y + map.s * y);
                DrawRectangle((int)cellX, (int)cellY, map.s, map.s, LIME);
                // check tile above
                if (i - map.w >= 0 && !map.cell[i - map.w]){
                    DrawLineEx((Vector2){cellX, cellY + 3}, (Vector2){cellX + map.s, cellY + 3}, 6.0f, GREEN);
                }
            }
        }
    }
}

void DrawTileGrid(){
    Color c = (Color){255,255,255,25};
    
    for (int y = 0; y < map.h + 1; y++){
        int x1 = map.x;
        int x2 = map.x + map.w * map.s;
        int Y = map.y + map.s * y;
        DrawLine(x1, Y, x2, Y, c);
    }
    for (int x = 0; x < map.w + 1; x++){
        int y1 = map.y;
        int y2 = map.y + map.h * map.s;
        int X = map.x + map.s * x;
        DrawLine(X, y1, X, y2, c);
    }
}

void DrawPlayer(){
    DrawRectangle((int)player.x, (int)player.y, (int)player.width, (int)player.height, WHITE);
    DrawRectangleLinesEx(player, 2, BLACK);
    
    // Artistic touch
    static int dirX = 0;
    dirX = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)) * 4;
    Vector2 L1 = (Vector2){player.x + 12 + dirX, player.y + 4};
    Vector2 R1 = (Vector2){player.x + 20 + dirX, player.y + 4};
    Vector2 L2 = L1;
    L2.y += 8;
    Vector2 R2 = R1;
    R2.y += 8;
    DrawLineEx(L1, L2, 2.0f, BLACK);
    DrawLineEx(R1, R2, 2.0f, BLACK);
}

void DrawCoins(){
    time += 1;
    
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]){
            Rectangle c = coins[i];
            float y = (float)sin(2 * PI * (time / 60.0f * 0.5) + (c.x * 5)) * 4; // pseudo random offset floating
            float x = (float)sin(2 * PI * (time / 60.0f * 2)) * 4;
            DrawRectangle((int)(c.x + 4 + x * 0.5), (int)(c.y + y), (int)(c.width - 4 - x), (int)c.height, GOLD);
        }
    }
}

void DrawScoreText(){
    const char *text;
    if (points == COIN_COUNT){
        text = TextFormat("Pres 'ENTER' to restart!");
    }
    else{
        text = TextFormat("Score: %d", points);
    }
    
    const int size = 24;
    int x = gameWidth /2 - MeasureText(text, size) / 2;
    int y = 48;
    
    DrawText(text, x, y+1, size, BLACK);
    DrawText(text, x, y, size, WHITE);
    
}

void RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity){
    Rectangle colArea = RectangleResize(rect, velocity);
    RectList *tiles = RectangleListFromTiles(&colArea, &map);
    RectangleTileCollision(rect, velocity, tiles);
    // free allocated RectList memory
    MemFree(tiles->rect);
    MemFree(tiles);
}

Rectangle RectangleResize(Rectangle *rect, Vector2 *size){
    return (Rectangle){
        size->x > 0 ? rect->x : rect->x + size->x,
        size->y > 0 ? rect->y : rect->y + size->y,
        size->x > 0 ? rect->width + size->x : rect->width - size->x,
        size->y > 0 ? rect->height + size->y : rect->height - size->y
        };
}

RectList* RectangleListFromTiles(Rectangle *rect, Grid *grid){
    float offX = rect->x - grid->x;
    float offY = rect->y - grid->y;
    // compensate flooring
    if (offX < 0.0f){offX -= grid->s;}
    if (offY < 0.0f){offY -= grid->s;}
    
    // grid coordinates
    int X = (int)(offX / grid->s);
    int sizeX = (int)((offX + rect->width) / grid->s) + 1;
    int Y = (int)(offY / grid->s);
    int sizeY = (int)((offY + rect->height) / grid->s) + 1;
    
    RectList *list = MemAlloc(sizeof(RectList));
    list->rect = MemAlloc(sizeof(Rectangle) * sizeX * sizeY);
    list->size = 0;
    
    for (int y = Y; y < sizeY; y++){
        if (y >= 0 && y < grid->h){
            for (int x = X; x < sizeX; x++){
                if (x >= 0 && x < grid->w){
                    int tile = grid->cell[x + y * grid->w];
                    if (tile){
                        list->rect[list->size] = (Rectangle){
                            grid->x + x * grid->s,
                            grid->y + y * grid->s,
                            grid->s,
                            grid->s
                            };
                        list->size += 1;
                    }
                }
            }
        }
    }
    return list;
}

void RectangleTileCollision(Rectangle *rect, Vector2 *velocity, RectList *list){
    Rectangle *a = rect;
    float *spdX = &velocity->x;
    float *spdY = &velocity->y;
    //Because of this logic it's necessary to check in vertical order of velocity.y direction
    bool down = velocity->y > 0; // Reverse list reading order
    
    for (int i = 0; i < list->size; i++){
        Rectangle *b = &list->rect[down ? i : list->size -1 -i];
        Rectangle c = (Rectangle){a->x + *spdX, a->y, a->width, a->height};

        if (CheckCollisionRecs(c, *b)) {
            if (*spdX > 0.0f) {
                *spdX = (b->x - a->width) - a->x;
            }
            else if (*spdX < 0.0f) {
                *spdX = (b->x + b->width) - a->x;
            }
        }
        c.x = a->x + *spdX;
        c.y += *spdY;

        if (CheckCollisionRecs(c, *b)) {
            if (*spdY > 0.0f) {
                *spdY = (b->y - a->height) - a->y;
            }
            else if (*spdY < 0.0f) {
                *spdY = (b->y + b->height) - a->y;
            }
        }
    }
}


