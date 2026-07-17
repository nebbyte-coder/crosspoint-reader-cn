#!/usr/bin/env python3
"""
将 Microban.sok 转换为 C++ 头文件和源文件，用于嵌入式推箱子游戏。
用法: python convert_levels.py
输出: levels.h, levels.cpp
"""

import os

INPUT_FILE = "Microban.sok"
OUTPUT_H = "levels.h"
OUTPUT_CPP = "levels.cpp"
MAX_LEVELS = 155  # 设为较大值以提取所有关卡，也可限制如 50

def parse_sok(filepath):
    """解析 .sok 文件，返回关卡列表（每个关卡是字符串列表）"""
    levels = []
    current = []
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.rstrip('\n\r')
            # 跳过注释和元数据行
            if line.startswith(';') or line.startswith('Title:') or \
               line.startswith('Author:') or line.startswith('Date:') or \
               line.startswith('Set:') or line.startswith('Copyright:') or \
               line.startswith('Email:') or line.startswith('Homepage:'):
                continue
            # 空行分隔关卡
            if not line.strip():
                if current:
                    levels.append(current)
                    current = []
                continue
            # 只保留有效的关卡行（以 # 或空格或 .$@*+ 开头）
            if line and line[0] in ('#', ' ', '.', '$', '@', '*', '+', '-'):
                current.append(line)
        if current:
            levels.append(current)
    return levels

def escape_string(s):
    """转义字符串中的双引号和反斜杠"""
    return s.replace('\\', '\\\\').replace('"', '\\"')

def generate_header(levels, max_count):
    """生成 levels.h"""
    actual = min(len(levels), max_count)
    lines = []
    lines.append("#pragma once\n")
    lines.append(f"// 自动生成，共 {actual} 关\n")
    lines.append(f"#define TOTAL_LEVELS {actual}\n")
    lines.append("extern const char* const* const levels[TOTAL_LEVELS];")
    lines.append("extern const int levelHeights[TOTAL_LEVELS];\n")
    return '\n'.join(lines)

def generate_source(levels, max_count):
    """生成 levels.cpp"""
    actual = min(len(levels), max_count)
    lines = []
    lines.append('#include "levels.h"\n')
    lines.append(f"// 自动生成，共 {actual} 关\n")

    # 定义每个关卡数组
    for i in range(actual):
        lvl = levels[i]
        lines.append(f"// 第 {i+1} 关")
        lines.append(f"static const char* level{i}[] = {{")
        for row in lvl:
            escaped = escape_string(row)
            lines.append(f'    "{escaped}",')
        lines.append("    nullptr")
        lines.append("};\n")

    # 定义 levels 指针数组
    lines.append("const char* const* const levels[TOTAL_LEVELS] = {")
    for i in range(actual):
        lines.append(f"    level{i},")
    lines.append("};\n")

    # 定义 levelHeights 数组
    lines.append("const int levelHeights[TOTAL_LEVELS] = {")
    for i in range(actual):
        lines.append(f"    {len(levels[i])},")
    lines.append("};\n")

    return '\n'.join(lines)

def main():
    if not os.path.exists(INPUT_FILE):
        print(f"❌ 未找到关卡文件 '{INPUT_FILE}'，请将其放在当前目录。")
        return

    print(f"正在解析 {INPUT_FILE}...")
    levels = parse_sok(INPUT_FILE)
    print(f"✅ 共发现 {len(levels)} 个关卡")

    if len(levels) == 0:
        print("❌ 没有解析到任何关卡，请检查文件格式。")
        return

    # 限制数量
    actual = min(len(levels), MAX_LEVELS)
    if actual < len(levels):
        print(f"⚠️ 只取前 {actual} 关（MAX_LEVELS={MAX_LEVELS}）")

    # 生成文件
    header = generate_header(levels, MAX_LEVELS)
    source = generate_source(levels, MAX_LEVELS)

    with open(OUTPUT_H, 'w', encoding='utf-8') as f:
        f.write(header)
    print(f"✅ 已生成 {OUTPUT_H}")

    with open(OUTPUT_CPP, 'w', encoding='utf-8') as f:
        f.write(source)
    print(f"✅ 已生成 {OUTPUT_CPP}")
    print("完成！请将这两个文件放到 src/activities/apps/sokoban/ 目录下。")

if __name__ == "__main__":
    main()