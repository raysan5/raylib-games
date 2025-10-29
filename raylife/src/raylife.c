/*******************************************************************************************
 *
 *   Controls:
 *   - Right Mouse Button: Drag to move the camera
 *   - Left Mouse Button: Toggle cell state in Draw mode
 *   - Mouse Wheel: Zoom in/out
 *   - Space Key: Toggle between Play/Draw mode (only works if cells are alive)
 *   - UP Arrow: Increase generations interval
 *   - DOWN Arrow: Decrease generations interval
 *   - R Key: Reset the grid
 *
 *******************************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

//------------------------------------------------------------------------------------------
// Constants Definition
//------------------------------------------------------------------------------------------
#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1000
#define INITIAL_CAMERA_ZOOM 1.0f
#define ZOOM_SCALE 0.25f
#define BOARD_SPACING 50
#define BOARD_ROWS 500
#define BOARD_COLS 500
#define INIT_INTERVAL 0.2f
#define MAX_GENERATIONS 500

typedef struct {
  int isAlive;
  Vector2 pos;
  Vector2 size;
} Cell;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static Cell board[BOARD_ROWS][BOARD_COLS];
static int aliveCells = 0;
static int generations = 0;
static int playMode = 0;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void initGrid(void);
static void drawBoard(void);
static void drawCells(void);
static void updateBoard(void);
static int countAliveNeighbors(int x, int y);
static void toggleCells(int x, int y);
static void nextGeneration(double *lastGenerationTime,
                           float *generationInterval);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char **argv) {
  // Parse command line arguments
  int maxGenerations = argv[1] ? atoi(argv[1]) : MAX_GENERATIONS;
  if (maxGenerations < 1)
    maxGenerations = MAX_GENERATIONS;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RayLife");
  SetTargetFPS(60);

  // Camera initialization
  Camera2D camera = {0};
  camera.zoom = INITIAL_CAMERA_ZOOM;
  camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
  camera.target = (Vector2){(BOARD_COLS * BOARD_SPACING) / 2.0f,
                            (BOARD_ROWS * BOARD_SPACING) / 2.0f};

  double lastGenerationTime = 0.0;
  float generations_interval = INIT_INTERVAL;

  initGrid();

  while (!WindowShouldClose()) {
    if (aliveCells == 0) {
      playMode = 0;
      generations = 0;
    }

    // Toggle play/draw mode with space key
    if (IsKeyPressed(KEY_SPACE) && aliveCells > 0 &&
        generations < maxGenerations)
      playMode = !playMode;

    // Move camera with right mouse button
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      Vector2 delta = GetMouseDelta();
      delta = Vector2Scale(delta, -INITIAL_CAMERA_ZOOM / camera.zoom);
      camera.target = Vector2Add(camera.target, delta);
    }

    // Zoom in/out with mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel) {
      Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
      camera.offset = GetMousePosition();
      camera.target = mousePos;
      float scaleFactor = INITIAL_CAMERA_ZOOM + (ZOOM_SCALE * fabsf(wheel));
      if (wheel < 0)
        scaleFactor = INITIAL_CAMERA_ZOOM / scaleFactor;
      camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f);
    }

    // Reset all with R key
    if (IsKeyPressed(KEY_R)) {
      playMode = 0;
      generations = 0;
      aliveCells = 0;
      generations_interval = INIT_INTERVAL;
      initGrid();
    }

    // Increase generations interval with UP arrow
    if (IsKeyPressed(KEY_UP)) {
      generations_interval += 0.1f;
      if (generations_interval > maxGenerations)
        generations_interval = maxGenerations;
    }

    // Decrease generations interval with DOWN arrow
    if (IsKeyPressed(KEY_DOWN)) {
      generations_interval -= 0.1f;
      if (generations_interval < 0.0)
        generations_interval = 0.0;
    }

    // Draw mode actions with left mouse button
    if (!playMode && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
      int boardX = (int)(mousePos.x / BOARD_SPACING);
      int boardY = (int)(mousePos.y / BOARD_SPACING);
      toggleCells(boardX, boardY);
    }

    if (!playMode && IsKeyPressed(KEY_SPACE))
      nextGeneration(&lastGenerationTime, &generations_interval);

    // Start generations with space key
    if (playMode && aliveCells > 0) {
      nextGeneration(&lastGenerationTime, &generations_interval);
      if (generations >= maxGenerations)
        playMode = 0;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    DrawRectangle(5, 5, 300, 110, Fade(RAYWHITE, 0.9f));
    DrawText(playMode ? "Play mode" : "Draw mode", 10, 10, 20, BLACK);
    DrawText(
        TextFormat("Generation: %d (Max: %d)", generations, maxGenerations), 10,
        35, 20, BLACK);
    DrawText(TextFormat("Cells: %d", aliveCells), 10, 60, 20, BLACK);
    DrawText(TextFormat("Generation interval: %.1fs", generations_interval), 10,
             85, 20, BLACK);

    BeginMode2D(camera);
    drawBoard();
    drawCells();
    EndMode2D();
    EndDrawing();
  }

  CloseWindow();

  return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
static void initGrid(void) {
  for (int i = 0; i < BOARD_ROWS; i++) {
    for (int j = 0; j < BOARD_COLS; j++) {
      board[i][j].isAlive = 0;
      board[i][j].pos.x = j * BOARD_SPACING;
      board[i][j].pos.y = i * BOARD_SPACING;
      board[i][j].size.x = BOARD_SPACING;
      board[i][j].size.y = BOARD_SPACING;
    }
  }
}

static void drawBoard(void) {
  for (int i = 0; i <= BOARD_ROWS; i++) {
    Vector2 start = {0, i * BOARD_SPACING};
    Vector2 end = {BOARD_COLS * BOARD_SPACING, i * BOARD_SPACING};
    DrawLineV(start, end, GRAY);
  }

  for (int j = 0; j <= BOARD_COLS; j++) {
    Vector2 start = {j * BOARD_SPACING, 0};
    Vector2 end = {j * BOARD_SPACING, BOARD_ROWS * BOARD_SPACING};
    DrawLineV(start, end, GRAY);
  }
}

static int countAliveNeighbors(int x, int y) {
  int count = 0;

  // This is going to check the 8 surrounding cells
  // If any of them are alive, then the cell is alive
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0)
        continue;

      // Calculate the neighbor's coordinates
      int nx = (x + dx + BOARD_COLS) % BOARD_COLS;
      int ny = (y + dy + BOARD_ROWS) % BOARD_ROWS;

      count += board[ny][nx].isAlive;
    }
  }

  return count;
}

static void toggleCells(int x, int y) {
  if (x >= 0 && x < BOARD_COLS && y >= 0 && y < BOARD_ROWS) {
    board[y][x].isAlive = !board[y][x].isAlive;
    aliveCells += (board[y][x].isAlive ? 1 : -1);
  }
}

static void updateBoard(void) {
  Cell nextGrid[BOARD_ROWS][BOARD_COLS];
  int newAliveCells = 0;

  for (int i = 0; i < BOARD_ROWS; i++) {
    for (int j = 0; j < BOARD_COLS; j++) {
      int aliveNeighbors = countAliveNeighbors(j, i);
      nextGrid[i][j] = board[i][j];
      nextGrid[i][j].isAlive = (board[i][j].isAlive &&
                                (aliveNeighbors == 2 || aliveNeighbors == 3)) ||
                               (!board[i][j].isAlive && aliveNeighbors == 3);
      if (nextGrid[i][j].isAlive)
        newAliveCells++;
    }
  }

  memcpy(board, nextGrid, sizeof(board));
  aliveCells = newAliveCells;
}

static void drawCells(void) {
  for (int i = 0; i < BOARD_ROWS; i++) {
    for (int j = 0; j < BOARD_COLS; j++) {
      if (board[i][j].isAlive) {
        DrawRectangleV(board[i][j].pos, board[i][j].size, WHITE);
        Rectangle rec = {board[i][j].pos.x, board[i][j].pos.y,
                         board[i][j].size.x, board[i][j].size.y};
        DrawRectangleLinesEx(rec, 1, BLACK);
      }
    }
  }
}

static void nextGeneration(double *lastGenerationTime,
                           float *generations_interval) {
  double time = GetTime();

  if (time - *lastGenerationTime >= *generations_interval) {
    updateBoard();
    generations++;
    *lastGenerationTime = time;
  }
}
