#include "XtcReaderMenuActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "MappedInputManager.h"
#include "components/UITheme.h"
#include "fontIds.h"

XtcReaderMenuActivity::XtcReaderMenuActivity(GfxRenderer& renderer, MappedInputManager& mappedInput,
                                             const std::string& title)
    : Activity("XtcReaderMenu", renderer, mappedInput), menuItems(buildMenuItems()), title(title) {}

std::vector<XtcReaderMenuActivity::MenuItem> XtcReaderMenuActivity::buildMenuItems() {
  std::vector<MenuItem> items;
  items.reserve(2);
  items.push_back({ACTION_SELECT_CHAPTER, StrId::STR_SELECT_CHAPTER});
  items.push_back({ACTION_GO_TO_PERCENT, StrId::STR_GO_TO_PERCENT});
  return items;
}

void XtcReaderMenuActivity::onEnter() {
  Activity::onEnter();
  requestUpdate();
}

void XtcReaderMenuActivity::onExit() { Activity::onExit(); }

void XtcReaderMenuActivity::loop() {
  buttonNavigator.onNext([this] {
    selectedIndex = ButtonNavigator::nextIndex(selectedIndex, static_cast<int>(menuItems.size()));
    requestUpdate();
  });

  buttonNavigator.onPrevious([this] {
    selectedIndex = ButtonNavigator::previousIndex(selectedIndex, static_cast<int>(menuItems.size()));
    requestUpdate();
  });

  if (mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
    setResult(MenuResult{menuItems[selectedIndex].action, 0, 0});
    finish();
    return;
  } else if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
    ActivityResult result;
    result.isCancelled = true;
    result.data = MenuResult{-1, 0, 0};
    setResult(std::move(result));
    finish();
    return;
  }
}

void XtcReaderMenuActivity::render(RenderLock&&) {
  renderer.clearScreen();

  auto metrics = UITheme::getInstance().getMetrics();
  Rect screen = UITheme::getInstance().getScreenSafeArea(renderer, true, false);

  GUI.drawHeader(renderer, Rect{screen.x, screen.y + metrics.topPadding, screen.width, metrics.headerHeight},
                 title.c_str());

  const int contentTop = screen.y + metrics.topPadding + metrics.headerHeight + metrics.verticalSpacing;
  const int contentHeight = screen.height - contentTop - metrics.verticalSpacing;

  GUI.drawList(
      renderer, Rect{screen.x, contentTop, screen.width, contentHeight}, menuItems.size(), selectedIndex,
      [this](int index) { return I18N.get(menuItems[index].labelId); },  // ★ 已捕获 this
      nullptr, nullptr, [](int) { return ""; }, true);

  const auto labels = mappedInput.mapLabels(tr(STR_BACK), tr(STR_SELECT), tr(STR_DIR_UP), tr(STR_DIR_DOWN));
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);

  renderer.displayBuffer();
}