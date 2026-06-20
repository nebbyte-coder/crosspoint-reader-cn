#pragma once

#include <string>
#include <vector>

#include "EpubReaderMenuActivity.h"
#include "activities/Activity.h"
#include "util/ButtonNavigator.h"

class XtcReaderMenuActivity final : public Activity {
 public:
  static constexpr int ACTION_SELECT_CHAPTER = 100;
  static constexpr int ACTION_GO_TO_PERCENT = 101;

  explicit XtcReaderMenuActivity(GfxRenderer& renderer, MappedInputManager& mappedInput, const std::string& title);

  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  struct MenuItem {
    int action;
    StrId labelId;
  };

  static std::vector<MenuItem> buildMenuItems();

  std::vector<MenuItem> menuItems;

  std::string title;
  int selectedIndex = 0;
  ButtonNavigator buttonNavigator;
};