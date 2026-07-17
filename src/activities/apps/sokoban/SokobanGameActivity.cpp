#include "SokobanGameActivity.h"

#include <I18n.h>
#include <Logging.h>

#include <cstdio>

#include "../../../components/UITheme.h"
#include "../../../fontIds.h"
#include "../GameUi.h"
#include "levels.h"

SokobanGameActivity::SokobanGameActivity(GfxRenderer& renderer, MappedInputManager& mappedInput)
    : Activity("Sokoban", renderer, mappedInput) {}

void SokobanGameActivity::onEnter() {
  Activity::onEnter();
  renderer.setOrientation(GfxRenderer::Orientation::Portrait);

  // 只读取关卡索引
  SokobanSaveSlot slot;
  if (SokobanStore::load(slot)) {
    currentLevel = slot.currentLevel;
    if (currentLevel < 0 || currentLevel >= TOTAL_LEVELS) currentLevel = 0;
    moves = slot.moves;
    // 注意：pushes 不再保存，从 board.pushes 获取
  } else {
    currentLevel = 0;
    moves = 0;
  }
  loadLevel(currentLevel);
  requestUpdate();
}

void SokobanGameActivity::onExit() {
  flushSave();
  Activity::onExit();
}

void SokobanGameActivity::loadLevel(int idx) {
  if (idx < 0 || idx >= TOTAL_LEVELS) idx = 0;
  if (levelHeights[idx] <= 0 || levelHeights[idx] > SokobanBoard::MAX_ROWS) {
    LOG_ERR("SOK", "Invalid level height %d", levelHeights[idx]);
    idx = 0;
  }
  currentLevel = idx;
  board.loadFromStrings(levels[idx], levelHeights[idx]);
  moves = 0;
  state = State::Playing;
  heldDr = heldDc = 0;
}

void SokobanGameActivity::loop() {
  if (state == State::Won) {
    if (mappedInput.wasReleased(MappedInputManager::Button::Confirm) ||
        mappedInput.wasReleased(MappedInputManager::Button::Back)) {
      if (currentLevel + 1 < TOTAL_LEVELS) {
        loadLevel(currentLevel + 1);
        scheduleSave();
      } else {
        activityManager.goToApps();
        return;
      }
      requestUpdate();
    }
    return;
  }

  handleInput();
  if (saveDebouncer.consumeIfDue(millis())) {
    flushSave();
  }
}

void SokobanGameActivity::handleInput() {
  const uint32_t now = millis();

  if (mappedInput.wasPressed(MappedInputManager::Button::Up)) {
    move(-1, 0);
    heldDr = -1;
    heldDc = 0;
    lastCursorMoveTime = now;
    isFirstMoveAfterHold = true;
  } else if (mappedInput.wasPressed(MappedInputManager::Button::Down)) {
    move(1, 0);
    heldDr = 1;
    heldDc = 0;
    lastCursorMoveTime = now;
    isFirstMoveAfterHold = true;
  } else if (mappedInput.wasPressed(MappedInputManager::Button::Left)) {
    move(0, -1);
    heldDr = 0;
    heldDc = -1;
    lastCursorMoveTime = now;
    isFirstMoveAfterHold = true;
  } else if (mappedInput.wasPressed(MappedInputManager::Button::Right)) {
    move(0, 1);
    heldDr = 0;
    heldDc = 1;
    lastCursorMoveTime = now;
    isFirstMoveAfterHold = true;
  }

  if (heldDr != 0 || heldDc != 0) {
    bool held = false;
    if (heldDr == -1 && mappedInput.isHeld(MappedInputManager::Button::Up))
      held = true;
    else if (heldDr == 1 && mappedInput.isHeld(MappedInputManager::Button::Down))
      held = true;
    else if (heldDc == -1 && mappedInput.isHeld(MappedInputManager::Button::Left))
      held = true;
    else if (heldDc == 1 && mappedInput.isHeld(MappedInputManager::Button::Right))
      held = true;

    if (held) {
      uint32_t delay = isFirstMoveAfterHold ? kInitialHoldDelayMs : kRepeatMoveIntervalMs;
      if (now - lastCursorMoveTime >= delay) {
        move(heldDr, heldDc);
        lastCursorMoveTime = now;
        isFirstMoveAfterHold = false;
      }
    } else {
      heldDr = heldDc = 0;
    }
  }

  // 返回键：无操作（或可设为重置）
  if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
    // 简单起见：重置当前关卡
    resetLevel();
  }
}

void SokobanGameActivity::move(int dr, int dc) {
  if (state != State::Playing) return;
  if (board.movePlayer(dr, dc)) {
    moves++;
    scheduleSave();
    requestUpdate();
    if (board.isWin()) {
      onWin();
    }
  }
}

void SokobanGameActivity::resetLevel() {
  loadLevel(currentLevel);
  requestUpdate();
}

void SokobanGameActivity::nextLevel() {
  if (currentLevel + 1 < TOTAL_LEVELS) {
    loadLevel(currentLevel + 1);
    scheduleSave();
    requestUpdate();
  }
}

void SokobanGameActivity::prevLevel() {
  if (currentLevel > 0) {
    loadLevel(currentLevel - 1);
    scheduleSave();
    requestUpdate();
  }
}

void SokobanGameActivity::onWin() {
  state = State::Won;
  if (currentLevel + 1 < TOTAL_LEVELS) {
    SokobanStore::saveLevel(currentLevel + 1, 0);
  }
  requestUpdate();
}

void SokobanGameActivity::scheduleSave() { saveDebouncer.schedule(millis()); }

void SokobanGameActivity::flushSave() {
  if (state != State::Playing) return;
  SokobanSaveSlot slot;
  slot.currentLevel = currentLevel;
  slot.moves = moves;
  // 不保存棋盘
  slot.hasBoard = false;
  SokobanStore::save(slot);
}

void SokobanGameActivity::render(RenderLock&&) {
  renderer.setOrientation(GfxRenderer::Orientation::Portrait);
  renderer.clearScreen();

  if (state == State::Won) {
    drawWinScreen();
  } else {
    drawHUD();
    drawBoard();
    drawFooter();
  }

  renderer.displayBuffer(HalDisplay::FAST_REFRESH);
}

void SokobanGameActivity::drawHUD() {
  const int sw = renderer.getScreenWidth();
  renderer.drawLine(0, TITLE_BAR_H, sw, TITLE_BAR_H, true);

  char buf[64];
  snprintf(buf, sizeof(buf), "%s %d/%d", tr(STR_SOKOBAN_TITLE), currentLevel + 1, TOTAL_LEVELS);
  renderer.drawText(UI_12_FONT_ID, 12, 10, buf);

  snprintf(buf, sizeof(buf), "%s:%d %s:%d", tr(STR_SOKOBAN_MOVES), moves, tr(STR_SOKOBAN_PUSHES), board.pushes);
  int tw = renderer.getTextWidth(UI_12_FONT_ID, buf);
  renderer.drawText(UI_12_FONT_ID, sw - 12 - tw, 10, buf);
}

void SokobanGameActivity::drawBoard() {
  if (board.rows == 0 || board.cols == 0) {
    LOG_ERR("SOK", "Empty board, skip drawing");
    return;
  }

  int availH = BOARD_BOTTOM - BOARD_TOP;
  int availW = renderer.getScreenWidth() - 16;
  int cellSize = 48;
  int maxCellByRows = availH / board.rows;
  int maxCellByCols = availW / board.cols;
  cellSize = (maxCellByRows < maxCellByCols) ? maxCellByRows : maxCellByCols;
  if (cellSize > 48) cellSize = 48;
  if (cellSize < 8) cellSize = 8;

  int boardPixelW = board.cols * cellSize;
  int boardPixelH = board.rows * cellSize;
  int startX = (renderer.getScreenWidth() - boardPixelW) / 2;
  int startY = BOARD_TOP + (availH - boardPixelH) / 2;

  for (int r = 0; r < board.rows; ++r) {
    for (int c = 0; c < board.cols; ++c) {
      int x = startX + c * cellSize;
      int y = startY + r * cellSize;
      SokobanBoard::Cell cell = board.cells[r][c];

      switch (cell) {
        case SokobanBoard::WALL:
          renderer.fillRect(x, y, cellSize, cellSize, true);
          break;
        case SokobanBoard::FLOOR:
          break;
        case SokobanBoard::TARGET:
          renderer.drawRect(x + cellSize / 4, y + cellSize / 4, cellSize / 2, cellSize / 2, 2, true);
          break;
        case SokobanBoard::BOX:
          renderer.drawRect(x + 2, y + 2, cellSize - 4, cellSize - 4, 3, true);
          renderer.drawLine(x + 2, y + 2, x + cellSize - 4, y + cellSize - 4, true);
          renderer.drawLine(x + cellSize - 4, y + 2, x + 2, y + cellSize - 4, true);
          break;
        case SokobanBoard::BOX_ON_TARGET:
          renderer.fillRect(x + 2, y + 2, cellSize - 4, cellSize - 4, true);
          renderer.drawRect(x + cellSize / 4, y + cellSize / 4, cellSize / 2, cellSize / 2, 2, false);
          break;
        case SokobanBoard::PLAYER:
          renderer.fillRect(x + 4, y + 4, cellSize - 8, cellSize - 8, true);
          renderer.drawRect(x + cellSize / 3 - 2, y + cellSize / 4, 3, 3, false);
          renderer.drawRect(x + 2 * cellSize / 3 - 1, y + cellSize / 4, 3, 3, false);
          break;
        case SokobanBoard::PLAYER_ON_TARGET:
          renderer.fillRect(x + 4, y + 4, cellSize - 8, cellSize - 8, true);
          renderer.drawRect(x + cellSize / 4, y + cellSize / 4, cellSize / 2, cellSize / 2, 2, false);
          break;
        default:
          break;
      }
    }
  }
}

void SokobanGameActivity::drawFooter() {
  const int sw = renderer.getScreenWidth();
  const int sh = renderer.getScreenHeight();
  renderer.drawLine(0, sh - FOOTER_H, sw, sh - FOOTER_H, true);
  renderer.drawText(UI_10_FONT_ID, 12, sh - FOOTER_H + 6, tr(STR_SOKOBAN_HINT_MOVE));
  renderer.drawText(UI_10_FONT_ID, 150, sh - FOOTER_H + 6, tr(STR_SOKOBAN_HINT_UNDO));  // 提示改为重置
}

void SokobanGameActivity::drawWinScreen() {
  const int sw = renderer.getScreenWidth();
  const int sh = renderer.getScreenHeight();
  renderer.drawCenteredText(NOTOSERIF_14_FONT_ID, sh / 2 - 30, tr(STR_SOKOBAN_WIN));
  char buf[64];
  snprintf(buf, sizeof(buf), "%s %d %s %d", tr(STR_SOKOBAN_MOVES), moves, tr(STR_SOKOBAN_PUSHES), board.pushes);
  renderer.drawCenteredText(UI_12_FONT_ID, sh / 2 + 10, buf);
  renderer.drawCenteredText(UI_12_FONT_ID, sh / 2 + 35, tr(STR_SOKOBAN_WIN_HINT));
}