#ifndef RAYSWEEP_H
#define RAYSWEEP_H

#include <stdbool.h>

#include "raylib.h"

#define WIDTH 1280
#define HEIGHT 720
#define TITLE "Minesweeper"
#define FPS 60

#define BOARD_WIDTH 30
#define BOARD_HEIGHT 16
#define BOARD_MINES 99

#define TILE_SIZE 30
#define TILE_GAP 2
#define LINE_THICKNESS 2
#define TILE_FONT_SIZE 20

#define UI_HEIGHT 50
#define UI_FONT_SIZE 30

typedef enum TileKind {
  SAFE,
  MINE,
} TileKind;

typedef struct Tile {
  bool open, flag, mine;
  int mineCount, flagCount;
} Tile;

typedef enum GameState {
  UNSTARTED,
  PLAYING,
  WON,
  LOST,
} GameState;

typedef struct Game {
  GameState state;
  double startTime;
  int openCount, flagCount;
  Tile board[BOARD_HEIGHT][BOARD_WIDTH];
} Game;

void prepareGame(Game *game);
void startGame(Game *game, int firstClickX, int firstClickY);
void endGame(Game *game);
void generateBoard(Game *game, int firstClickX, int firstClickY);

void openTile(Game *game, int x, int y, bool cascading);
void toggleFlag(Game *game, int x, int y);

void drawGame(Game *game);
void drawUI(Game *game);

void drawBoard(Game *game);
void drawTile(Game *game, int x, int y);

Rectangle tileRect(int tx, int ty);
Color getTileColor(Tile tile);

#endif
