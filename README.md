# CrossPoint Reader 中文版 (chinese-support)

本固件基于 crosspoint-reader/crosspoint-reader上游源码，适配了**简体中文界面**与**CJK 点阵字体**，使设备能够完整显示中文菜单、提示以及中文 EPUB 书籍。

以后更新以同步上游为主，不会增加太多功能，保持纯粹。

## 功能亮点

- 全简体中文界面（菜单、提示、设置）
- 内置 CJK 点阵字体（Noto Sans CJK 8～18pt），中文 EPUB 完美渲染
- 伪粗体支持，增强阅读体验
- 阅读菜单增加抗锯齿开关
- XTC 格式书籍支持跳转进度
- OTA 更新地址指向本仓库，可自行发布固件更新
- 精简语言列表（仅保留英文和简体中文），减小固件体积
- 精简一个主题（RoundedRaff），进一步压缩固件

## 主要改动

- 新增 `notosans_cjk_8~18`六套 CJK 点阵字体
- 所有内置字体均指向 CJK 点阵，确保中文正常渲染
- 添加简体中文翻译（覆盖全部 UI 字符串）
- 增加了伪粗体
- 在阅读菜单里增加了抗锯齿开关
- 在阅读 XTC 书籍时支持跳转进度
- 精简语言列表，仅保留英文和简体中文（减小固件体积）
- 精简了一个主题
- 修改了 OTA 地址为本仓库
- 修改 `gen_i18n.py`中的 V1 迁移表以适配精简后的语言枚举

## 致谢

中文适配的实现参考了 0x1abin/crossmux的开源成果，包括：

- CJK 点阵字体文件（Noto Sans CJK 8～18pt）
- 简体中文翻译文件 (`chinese.yaml`)
- 字体替换方案

在此对 crossmux 作者表示衷心感谢 🙏

## 使用说明

1. 下载本 Release 中的 `firmware.bin`
2. 使用 Web 刷机工具 https://crosspointreader.com/#flash-tools
3. 开机后在 **设置 → 语言** 中选择 `简体中文`

## 构建指南

本项目使用 PlatformIO 构建。克隆仓库后，在项目根目录执行：

```bash
# 编译
pio run -e slim 

# 编译并烧录
pio run -e slim -t upload 
```

编译产物位于 `.pio/build/slim/firmware.bin`。

## 许可证

本项目基于 MIT License开源。上游 CrossPoint Reader 亦采用 MIT 许可。

---
