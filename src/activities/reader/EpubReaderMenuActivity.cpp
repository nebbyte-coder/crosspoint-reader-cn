#include "EpubReaderMenuActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "CrossPointSettings.h"
#include "MappedInputManager.h"
#include "SdCardFontSystem.h"
#include "components/UITheme.h"
#include "fontIds.h"

EpubReaderMenuActivity::EpubReaderMenuActivity(GfxRenderer& renderer, MappedInputManager& mappedInput,
                                               const std::string& title, const int currentPage, const int totalPages,
                                               const int bookProgressPercent, const uint8_t currentOrientation,
                                               const bool hasFootnotes, const bool hasBookmarks)
    : Activity("EpubReaderMenu", renderer, mappedInput),
      menuItems(buildMenuItems(hasFootnotes, hasBookmarks)),
      title(title),
      pendingOrientation(currentOrientation),
      currentPage(currentPage),
      totalPages(totalPages),
      bookProgressPercent(bookProgressPercent) {}

std::vector<EpubReaderMenuActivity::MenuItem> EpubReaderMenuActivity::buildMenuItems(bool hasFootnotes,
                                                                                     bool hasBookmarks) {
  std::vector<MenuItem> items;
  items.reserve(15);
  items.push_back({MenuAction::SELECT_CHAPTER, StrId::STR_SELECT_CHAPTER});
  if (hasFootnotes) {
    items.push_back({MenuAction::FOOTNOTES, StrId::STR_FOOTNOTES});
  }
  if (hasBookmarks) {
    items.push_back({MenuAction::BOOKMARKS, StrId::STR_BOOKMARKS});
  }
  items.push_back({MenuAction::TOGGLE_BOOKMARK, StrId::STR_TOGGLE_BOOKMARK});
  items.push_back({MenuAction::ROTATE_SCREEN, StrId::STR_ORIENTATION});
  items.push_back({MenuAction::AUTO_PAGE_TURN, StrId::STR_AUTO_TURN_PAGES_PER_MIN});
  items.push_back({MenuAction::GO_TO_PERCENT, StrId::STR_GO_TO_PERCENT});
  items.push_back({MenuAction::SCREENSHOT, StrId::STR_SCREENSHOT_BUTTON});
  items.push_back({MenuAction::DISPLAY_QR, StrId::STR_DISPLAY_QR});
  items.push_back({MenuAction::GO_HOME, StrId::STR_GO_HOME_BUTTON});
  items.push_back({MenuAction::SYNC, StrId::STR_SYNC_PROGRESS});
  items.push_back({MenuAction::DELETE_CACHE, StrId::STR_DELETE_CACHE});
  items.push_back({MenuAction::FONT_SELECTION, StrId::STR_FONT_FAMILY});
  items.push_back({MenuAction::TEXT_AA, StrId::STR_TEXT_AA});
  items.push_back({MenuAction::FAKE_BOLD, StrId::STR_FAKE_BOLD});
  return items;
}

void EpubReaderMenuActivity::onEnter() {
  Activity::onEnter();
  requestUpdate();
}

void EpubReaderMenuActivity::onExit() { Activity::onExit(); }

void EpubReaderMenuActivity::loop() {
  // Handle navigation
  buttonNavigator.onNext([this] {
    selectedIndex = ButtonNavigator::nextIndex(selectedIndex, static_cast<int>(menuItems.size()));
    requestUpdate();
  });

  buttonNavigator.onPrevious([this] {
    selectedIndex = ButtonNavigator::previousIndex(selectedIndex, static_cast<int>(menuItems.size()));
    requestUpdate();
  });

  if (mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
    const auto selectedAction = menuItems[selectedIndex].action;
    if (selectedAction == MenuAction::ROTATE_SCREEN) {
      // Cycle orientation preview locally; actual rotation happens on menu exit.
      pendingOrientation = (pendingOrientation + 1) % orientationLabels.size();
      requestUpdate();
      return;
    }

    if (selectedAction == MenuAction::AUTO_PAGE_TURN) {
      selectedPageTurnOption = (selectedPageTurnOption + 1) % pageTurnLabels.size();
      requestUpdate();
      return;
    } else if (selectedAction == MenuAction::TEXT_AA) {
      SETTINGS.textAntiAliasing = !SETTINGS.textAntiAliasing;
      fontChanged = false;
      requestUpdate();
      return;
    } else if (selectedAction == MenuAction::FONT_SELECTION) {
      sdFontSystem.refreshIfDirty();
      const auto& families = sdFontSystem.registry().getFamilies();
      int sdCount = static_cast<int>(families.size());
      int total = 2 + sdCount;
      if (total == 0) {
        return;
      }

      int displayIndex;
      if (SETTINGS.sdFontFamilyName[0] != '\0') {
        displayIndex = 2;
        for (int i = 0; i < sdCount; i++) {
          if (families[i].name == SETTINGS.sdFontFamilyName) {
            displayIndex = 2 + i;
            break;
          }
        }
      } else {
        displayIndex = SETTINGS.fontFamily < 2 ? SETTINGS.fontFamily : 0;
      }

      int next = (displayIndex + 1) % total;
      if (next < 2) {
        SETTINGS.fontFamily = next;
        SETTINGS.sdFontFamilyName[0] = '\0';
      } else {
        int sdIdx = next - 2;
        if (sdIdx < sdCount) {
          strncpy(SETTINGS.sdFontFamilyName, families[sdIdx].name.c_str(), sizeof(SETTINGS.sdFontFamilyName) - 1);
          SETTINGS.sdFontFamilyName[sizeof(SETTINGS.sdFontFamilyName) - 1] = '\0';
        }
      }
      SETTINGS.saveToFile();
      fontChanged = true;
      requestUpdate();
      return;
    } else if (selectedAction == MenuAction::FAKE_BOLD) {
      uint8_t current = SETTINGS.fakeBold;
      SETTINGS.fakeBold = (current + 1) % 3;
      fontChanged = false;
      requestUpdate();
      return;
    }

    setResult(MenuResult{static_cast<int>(selectedAction), pendingOrientation, selectedPageTurnOption});
    finish();
    return;
  } else if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
    int actionCode = fontChanged ? static_cast<int>(MenuAction::FONT_SELECTION) : -1;
    ActivityResult result;
    result.isCancelled = true;
    result.data = MenuResult{actionCode, pendingOrientation, selectedPageTurnOption};
    setResult(std::move(result));
    finish();
    return;
  }
}

void EpubReaderMenuActivity::render(RenderLock&&) {
  renderer.clearScreen();

  auto metrics = UITheme::getInstance().getMetrics();
  Rect screen = UITheme::getInstance().getScreenSafeArea(renderer, true, false);

  GUI.drawHeader(renderer, Rect{screen.x, screen.y + metrics.topPadding, screen.width, metrics.headerHeight},
                 title.c_str());

  // Progress summary
  std::string progressLine;
  if (totalPages > 0) {
    progressLine = std::string(tr(STR_CHAPTER_PREFIX)) + std::to_string(currentPage) + "/" +
                   std::to_string(totalPages) + std::string(tr(STR_PAGES_SEPARATOR));
  }
  progressLine += std::string(tr(STR_BOOK_PREFIX)) + std::to_string(bookProgressPercent) + "%";
  GUI.drawSubHeader(
      renderer,
      Rect{screen.x, screen.y + metrics.topPadding + metrics.headerHeight, screen.width, metrics.tabBarHeight},
      progressLine.c_str());

  const int contentTop =
      screen.y + metrics.topPadding + metrics.headerHeight + metrics.tabBarHeight + metrics.verticalSpacing;
  const int contentHeight = screen.height - contentTop - metrics.verticalSpacing;

  GUI.drawList(
      renderer, Rect{screen.x, contentTop, screen.width, contentHeight}, menuItems.size(), selectedIndex,
      [this](int index) { return I18N.get(menuItems[index].labelId); }, nullptr, nullptr,
      [this](int index) -> std::string {
        const auto value = menuItems[index].action;
        if (value == MenuAction::ROTATE_SCREEN) {
          return std::string(I18N.get(orientationLabels[pendingOrientation]));
        } else if (value == MenuAction::AUTO_PAGE_TURN) {
          return std::string(pageTurnLabels[selectedPageTurnOption]);
        } else if (value == MenuAction::TEXT_AA) {
          return std::string(I18N.get(SETTINGS.textAntiAliasing ? StrId::STR_STATE_ON : StrId::STR_STATE_OFF));
        } else if (value == MenuAction::FONT_SELECTION) {
          if (SETTINGS.sdFontFamilyName[0] != '\0') {
            return std::string(SETTINGS.sdFontFamilyName);
          } else {
            return std::string(I18N.get(SETTINGS.fontFamily == 0 ? StrId::STR_NOTO_SERIF : StrId::STR_NOTO_SANS));
          }
        } else if (value == MenuAction::FAKE_BOLD) {
          uint8_t fb = SETTINGS.fakeBold;
          if (fb == 0)
            return std::string(I18N.get(StrId::STR_FAKE_BOLD_OFF));
          else if (fb == 1)
            return std::string(I18N.get(StrId::STR_FAKE_BOLD_ON));
          else
            return std::string(I18N.get(StrId::STR_FAKE_BOLD_EXTRA));
        } else {
          return std::string();
        }
      },
      true);

  // Footer / Hints
  const auto labels = mappedInput.mapLabels(tr(STR_BACK), tr(STR_SELECT), tr(STR_DIR_UP), tr(STR_DIR_DOWN));
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);

  renderer.displayBuffer();
}
