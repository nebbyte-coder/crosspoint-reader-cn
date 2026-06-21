// AUTO-GENERATED — font ID registry
// If you change any font .h content, these IDs will change (that's intentional: runtime uses them for freshness).
#pragma once

#include "notosans_8_regular.h"   // SMALL_FONT
#include "notosans_12_regular.h"
#include "notosans_12_bold.h"
#include "notosans_12_italic.h"
#include "notosans_12_bolditalic.h"
#include "notosans_14_regular.h"
#include "notosans_14_bold.h"
#include "notosans_14_italic.h"
#include "notosans_14_bolditalic.h"
#include "notosans_16_regular.h"
#include "notosans_16_bold.h"
#include "notosans_16_italic.h"
#include "notosans_16_bolditalic.h"
#include "notosans_18_regular.h"
#include "notosans_18_bold.h"
#include "notosans_18_italic.h"
#include "notosans_18_bolditalic.h"
#include "ubuntu_10_regular.h"
#include "ubuntu_10_bold.h"
#include "ubuntu_12_regular.h"
#include "ubuntu_12_bold.h"
#include "lxgw_cjk_8.h"
#include "lxgw_cjk_10.h"
#include "lxgw_cjk_12.h"
#include "lxgw_cjk_14.h"
#include "lxgw_cjk_16.h"
#include "lxgw_cjk_18.h"

// ---- FONT IDs (signed int32, unique per font *content*) ----
#define UI_10_FONT_ID        ((int32_t)-1021)
#define UI_12_FONT_ID        ((int32_t)-1022)
#define SMALL_FONT_ID        ((int32_t)-1023)

#define CJK_8_FONT_ID        ((int32_t)-2008)
#define CJK_10_FONT_ID       ((int32_t)-2010)
#define CJK_12_FONT_ID       ((int32_t)-2012)
#define CJK_14_FONT_ID       ((int32_t)-2014)
#define CJK_16_FONT_ID       ((int32_t)-2016)
#define CJK_18_FONT_ID       ((int32_t)-2018)

#define CHINESE_CHESS_FONT_ID ((int32_t)-3000)

// Sentinel guard (ID 0 means "not found")
static_assert(SMALL_FONT_ID != 0, "font ID 0 collides with sentinel");