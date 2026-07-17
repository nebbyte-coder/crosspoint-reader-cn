#include "levels.h"

// 第1关：3x3，一个箱子一个目标，一步解决
static const char* level0[] = {"####", "#. #", "# $#", "# @#", "####", nullptr};

// 第2关：4x4，两个箱子两个目标，两步
static const char* level1[] = {"#####", "#   #", "# $.#", "# $.#", "# @ #", "#####", nullptr};

const char* const* const levels[TOTAL_LEVELS] = {
    level0,
    level1,
};

const int levelHeights[TOTAL_LEVELS] = {
    5,
    6,
};