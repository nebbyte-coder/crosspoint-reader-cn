#pragma once

#include <cstdint>

#include "../../Activity.h"
#include "../GameSaveDebouncer.h"
#include "SokobanBoard.h"
#include "SokobanStore.h"

class SokobanGameActivity final : public Activity {
 public:
  SokobanGameActivity(GfxRenderer& renderer, MappedInputManager& mappedInput);
  ~SokobanGameActivity() override = default;

  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  enum class State : uint8_t { Playing, Won };

  State state = State::Playing;
  SokobanBoard board;
  int currentLevel = 0;
  int moves = 0;
  GameSaveDebouncer saveDebouncer;

  static constexpr uint32_t kInitialHoldDelayMs = 350;
  static constexpr uint32_t kRepeatMoveIntervalMs = 280;
  uint32_t lastCursorMoveTime = 0;
  bool isFirstMoveAfterHold = false;
  int heldDr = 0, heldDc = 0;

  void handleInput();
  void move(int dr, int dc);
  void resetLevel();
  void nextLevel();
  void prevLevel();
  void loadLevel(int idx);
  void onWin();
  void scheduleSave();
  void flushSave();

  void drawBoard();
  void drawHUD();
  void drawFooter();
  void drawWinScreen();

  static constexpr int TITLE_BAR_H = 38;
  static constexpr int FOOTER_H = 36;
  static constexpr int BOARD_TOP = TITLE_BAR_H + 4;
  static constexpr int BOARD_BOTTOM = 768;  // 适当调整
};