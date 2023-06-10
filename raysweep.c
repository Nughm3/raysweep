#include <stdbool.h>
#include <stdio.h>

#include "raylib.h"
#include "raysweep.h"

int main() {
  InitWindow(WIDTH, HEIGHT, TITLE);
  SetTargetFPS(FPS);

  Game game;
  prepareGame(&game);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    drawGame(&game);

    if ((game.state == WON || game.state == LOST) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      prepareGame(&game);

    Vector2 pos = GetMousePosition();
    for (int y = 0; y < BOARD_HEIGHT; y++) {
      for (int x = 0; x < BOARD_WIDTH; x++) {
        Rectangle rec = tileRect(x, y);
        if (CheckCollisionPointRec(pos, rec)) {
          if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (game.state == PLAYING) {
              openTile(&game, x, y, false);
              if (game.openCount == BOARD_WIDTH * BOARD_HEIGHT - BOARD_MINES)
                game.state = WON;
            } else if (game.state == UNSTARTED) {
              startGame(&game, x, y);
            }
          }

          if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && game.state == PLAYING)
            toggleFlag(&game, x, y);
        }
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

void prepareGame(Game *game) {
  game->state = UNSTARTED;
  for (int y = 0; y < BOARD_HEIGHT; y++)
    for (int x = 0; x < BOARD_WIDTH; x++) {
      Tile *tile = &game->board[y][x];
      tile->mineCount = tile->flagCount = 0;
      tile->open = tile->mine = tile->flag = false;
    }
}

void startGame(Game *game, int firstClickX, int firstClickY) {
  game->state = PLAYING;
  game->startTime = GetTime();
  game->openCount = game->flagCount = 0;
  generateBoard(game, firstClickX, firstClickY);
}

void endGame(Game *game) {
  game->state = LOST;
  for (int y = 0; y < BOARD_HEIGHT; y++)
    for (int x = 0; x < BOARD_WIDTH; x++)
      game->board[y][x].open = true;
}

void generateBoard(Game *game, int firstClickX, int firstClickY) {
  int minesLeft = BOARD_MINES;

  while (minesLeft > 0) {
    int x = GetRandomValue(0, BOARD_WIDTH - 1),
        y = GetRandomValue(0, BOARD_HEIGHT - 1);

    Tile *tile = &game->board[y][x];

    bool setMine = !tile->mine && !(x == firstClickX && y == firstClickY);

    for (int dy = -1; dy <= 1; dy++)
      for (int dx = -1; dx <= 1; dx++)
        if (x + dx == firstClickX && y + dy == firstClickY)
          setMine = false;

    if (!setMine)
      continue;

    tile->mine = true;
    minesLeft--;

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        int ax = x + dx, ay = y + dy;
        if (ax < 0 || ax >= BOARD_WIDTH || ay < 0 || ay >= BOARD_HEIGHT)
          continue;

        Tile *adjTile = &game->board[ay][ax];
        if (!adjTile->mine)
          adjTile->mineCount++;
      }
    }
  }

  openTile(game, firstClickX, firstClickY, false);
}

void openTile(Game *game, int x, int y, bool cascading) {
  Tile *tile = &game->board[y][x];

  if (tile->flag)
    return;

  if (!tile->mine) {
    tile->open = true;
    game->openCount++;

    bool fullyFlagged =
        tile->open && tile->flagCount == tile->mineCount && !cascading;

    if (tile->mineCount == 0 || fullyFlagged)
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;

          int ax = x + dx, ay = y + dy;
          if (ax < 0 || ax >= BOARD_WIDTH || ay < 0 || ay >= BOARD_HEIGHT)
            continue;

          Tile tile = game->board[ay][ax];
          if (tile.open || tile.flag)
            continue;

          openTile(game, ax, ay, true);
        }
      }
  } else {
    endGame(game);
  }
}

void toggleFlag(Game *game, int x, int y) {
  Tile *tile = &game->board[y][x];

  if (tile->open)
    return;

  bool flagged = !tile->flag;
  tile->flag = !tile->flag;

  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      int ax = x + dx, ay = y + dy;
      if (ax < 0 || ax >= BOARD_WIDTH || ay < 0 || ay >= BOARD_HEIGHT)
        continue;

      Tile *tile = &game->board[ay][ax];
      if (flagged)
        tile->flagCount++;
      else
        tile->flagCount--;
    }
  }

  if (flagged)
    game->flagCount++;
  else
    game->flagCount--;
}

void drawGame(Game *game) {
  drawBoard(game);
  drawUI(game);
}

void drawUI(Game *game) {
  int totalSize = TILE_SIZE + TILE_GAP;

  int left = (WIDTH - BOARD_WIDTH * totalSize + TILE_GAP) / 2,
      right = (WIDTH + BOARD_WIDTH * totalSize - TILE_GAP) / 2;

  int x;
  int y = (HEIGHT + BOARD_HEIGHT * totalSize + UI_HEIGHT) / 2 - UI_FONT_SIZE;

  const char *msg;
  Color color;

  switch (game->state) {
  case PLAYING: {
    x = left;
    DrawText("expert/standard", x, y, UI_FONT_SIZE, DARKGRAY);

    double elapsedTime = GetTime() - game->startTime;
    char timer[12];
    sprintf(timer, "%.0f", elapsedTime);

    char flagCount[5 + 1]; // safety: flags <= 99
    sprintf(flagCount, "%d/%d", game->flagCount, BOARD_MINES);
    int len = MeasureText(flagCount, UI_FONT_SIZE);
    x = right - len;
    DrawText(flagCount, x, y, UI_FONT_SIZE, DARKGRAY);

    len = MeasureText(timer, UI_FONT_SIZE);
    x -= len + 20;
    DrawText(timer, x, y, UI_FONT_SIZE, DARKGRAY);

    return; // the PLAYING UI is drawn separately
  }
  case UNSTARTED: {
    msg = "click anywhere to start...";
    color = DARKGREEN;
    break;
  }
  case WON: {
    msg = "you won! click anywhere to continue...";
    color = RED;
    break;
  }
  case LOST: {
    msg = "you died! click anywhere to continue...";
    color = RED;
    break;
  }
  }

  int len = MeasureText(msg, UI_FONT_SIZE);
  x = (WIDTH - len) / 2;
  DrawText(msg, x, y, UI_FONT_SIZE, color);
}

void drawBoard(Game *game) {
  for (int y = 0; y < BOARD_HEIGHT; y++)
    for (int x = 0; x < BOARD_WIDTH; x++)
      drawTile(game, x, y);
}

void drawTile(Game *game, int x, int y) {
  Tile tile = game->board[y][x];
  Rectangle rec = tileRect(x, y);

  if (tile.open) {
    if (!tile.mine) {
      if (tile.flag) {
        DrawRectangleRec(rec, PINK);
        DrawRectangleLinesEx(rec, LINE_THICKNESS, RED);

        int len = MeasureText("!", TILE_FONT_SIZE);
        int fx = (int)rec.x + (TILE_SIZE - len) / 2,
            fy = (int)rec.y + (TILE_SIZE - TILE_FONT_SIZE) / 2;

        DrawText("!", fx, fy, TILE_FONT_SIZE, DARKGRAY);
      } else if (tile.mineCount == 0) {
        DrawRectangleRec(rec, RAYWHITE);
      } else {
        DrawRectangleRec(rec, LIGHTGRAY);

        Color color = getTileColor(tile);
        DrawRectangleLinesEx(rec, LINE_THICKNESS, color);

        char s[1 + 1]; // safey: tile.count <= 8
        sprintf(s, "%d", tile.mineCount);

        int len = MeasureText(s, TILE_FONT_SIZE);
        int fx = (int)rec.x + (TILE_SIZE - len) / 2,
            fy = (int)rec.y + (TILE_SIZE - TILE_FONT_SIZE) / 2;

        DrawText(s, fx, fy, TILE_FONT_SIZE, color);
      }
    } else {
      if (tile.flag) {
        DrawRectangleLinesEx(rec, LINE_THICKNESS, ORANGE);
        int len = MeasureText("!", TILE_FONT_SIZE);
        int fx = (int)rec.x + (TILE_SIZE - len) / 2,
            fy = (int)rec.y + (TILE_SIZE - TILE_FONT_SIZE) / 2;
        DrawText("!", fx, fy, TILE_FONT_SIZE, RED);
      } else {
        DrawRectangleRec(rec, GRAY);
        DrawRectangleLinesEx(rec, LINE_THICKNESS, DARKGRAY);

        int len = MeasureText("#", TILE_FONT_SIZE);
        int fx = (int)rec.x + (TILE_SIZE - len) / 2,
            fy = (int)rec.y + (TILE_SIZE - TILE_FONT_SIZE) / 2;

        DrawText("#", fx, fy, TILE_FONT_SIZE, DARKGRAY);
      }
    }
  } else if (tile.flag) {
    DrawRectangleLinesEx(rec, LINE_THICKNESS, ORANGE);
    int len = MeasureText("!", TILE_FONT_SIZE);
    int fx = (int)rec.x + (TILE_SIZE - len) / 2,
        fy = (int)rec.y + (TILE_SIZE - TILE_FONT_SIZE) / 2;
    DrawText("!", fx, fy, TILE_FONT_SIZE, RED);
  } else {
    DrawRectangleRec(rec, LIGHTGRAY);
  }
}

Rectangle tileRect(int tx, int ty) {
  int totalSize = TILE_SIZE + TILE_GAP;
  int w = BOARD_WIDTH * totalSize, h = BOARD_HEIGHT * totalSize;

  int x = (WIDTH - w + TILE_GAP) / 2 + totalSize * tx,
      y = (HEIGHT - UI_HEIGHT - h + TILE_GAP) / 2 + totalSize * ty,
      size = TILE_SIZE - TILE_GAP / 2;

  return (Rectangle){(float)x, (float)y, (float)size, (float)size};
}

Color getTileColor(Tile tile) {
  switch (tile.mineCount) {
  case 0:
    return BLANK;
  case 1:
    return BLUE;
  case 2:
    return GREEN;
  case 3:
    return RED;
  case 4:
    return DARKBLUE;
  case 5:
    return DARKBROWN;
  case 6:
    return SKYBLUE;
  default:
    return BLACK;
  }
}
