#include "SokobanBoard.h"

#include <cstdio>

void SokobanBoard::clear() {
  memset(cells, 0, sizeof(cells));
  rows = 0;
  cols = 0;
  playerR = 0;
  playerC = 0;
  pushes = 0;
}

bool SokobanBoard::loadFromStrings(const char* const* levelData, int numRows) {
  clear();
  if (numRows <= 0 || numRows > MAX_ROWS) {
    printf("SOK: invalid rows %d\n", numRows);
    return false;
  }
  rows = numRows;
  cols = 0;
  for (int r = 0; r < rows; ++r) {
    const char* line = levelData[r];
    int c = 0;
    while (line[c] != '\0' && c < MAX_COLS) {
      char ch = line[c];
      switch (ch) {
        case '#':
          cells[r][c] = WALL;
          break;
        case ' ':
          cells[r][c] = FLOOR;
          break;
        case '.':
          cells[r][c] = TARGET;
          break;
        case '$':
          cells[r][c] = BOX;
          break;
        case '*':
          cells[r][c] = BOX_ON_TARGET;
          break;
        case '@':
          cells[r][c] = PLAYER;
          playerR = r;
          playerC = c;
          break;
        case '+':
          cells[r][c] = PLAYER_ON_TARGET;
          playerR = r;
          playerC = c;
          break;
        default:
          cells[r][c] = EMPTY;
          break;
      }
      ++c;
    }
    if (c > cols) cols = c;
  }
  // 将所有EMPTY视为FLOOR
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (cells[r][c] == EMPTY) cells[r][c] = FLOOR;
    }
  }
  printf("SOK: loaded %dx%d, player at (%d,%d)\n", rows, cols, playerR, playerC);
  return true;
}

bool SokobanBoard::movePlayer(int dr, int dc) {
  int nr = playerR + dr;
  int nc = playerC + dc;
  if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) return false;

  Cell target = cells[nr][nc];
  if (target == WALL) return false;

  if (target == BOX || target == BOX_ON_TARGET) {
    int nnr = nr + dr;
    int nnc = nc + dc;
    if (nnr < 0 || nnr >= rows || nnc < 0 || nnc >= cols) return false;
    Cell behind = cells[nnr][nnc];
    if (behind == WALL || behind == BOX || behind == BOX_ON_TARGET) return false;

    // 移动箱子
    if (behind == TARGET) {
      cells[nnr][nnc] = BOX_ON_TARGET;
    } else {
      cells[nnr][nnc] = BOX;
    }
    // 箱子原来位置
    if (target == BOX_ON_TARGET) {
      cells[nr][nc] = TARGET;
    } else {
      cells[nr][nc] = FLOOR;
    }
    pushes++;
  }

  // 移动玩家
  Cell oldPos = cells[playerR][playerC];
  if (oldPos == PLAYER_ON_TARGET) {
    cells[playerR][playerC] = TARGET;
  } else {
    cells[playerR][playerC] = FLOOR;
  }

  Cell newCell = cells[nr][nc];
  if (newCell == TARGET) {
    cells[nr][nc] = PLAYER_ON_TARGET;
  } else {
    cells[nr][nc] = PLAYER;
  }

  playerR = nr;
  playerC = nc;
  return true;
}

bool SokobanBoard::isWin() const {
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (cells[r][c] == BOX) return false;
    }
  }
  return true;
}
