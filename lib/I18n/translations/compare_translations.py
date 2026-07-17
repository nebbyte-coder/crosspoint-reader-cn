#!/usr/bin/env python3
"""
compare_translations.py - 对比英文和中文 YAML 翻译文件，输出英文新增的键。
用法：
    python compare_translations.py english.yaml chinese.yaml
"""

import sys
import yaml

def load_yaml(path):
    """加载 YAML 文件，返回字典。"""
    with open(path, 'r', encoding='utf-8') as f:
        data = yaml.safe_load(f)
    if not isinstance(data, dict):
        raise ValueError(f"{path} 不是一个有效的键值对 YAML 文件")
    return data

def main():
    if len(sys.argv) != 3:
        print("用法: python compare_translations.py english.yaml chinese.yaml")
        sys.exit(1)

    en_path = sys.argv[1]
    cn_path = sys.argv[2]

    try:
        en_dict = load_yaml(en_path)
        cn_dict = load_yaml(cn_path)
    except Exception as e:
        print(f"读取文件失败: {e}")
        sys.exit(1)

    # 获取所有键（忽略元信息键，如 _language_name, _language_code, _order）
    meta_keys = {'_language_name', '_language_code', '_order'}
    en_keys = set(en_dict.keys()) - meta_keys
    cn_keys = set(cn_dict.keys()) - meta_keys

    # 英文有但中文没有的键
    missing_keys = en_keys - cn_keys

    if not missing_keys:
        print("✅ 中文翻译已覆盖所有英文键，无需补充。")
        return

    print(f"⚠️  发现 {len(missing_keys)} 个英文键在中文翻译中缺失：\n")
    for key in sorted(missing_keys):
        print(f"  {key}: \"{en_dict[key]}\"")

    print("\n💡 建议添加到 chinese.yaml 中的内容：")
    for key in sorted(missing_keys):
        print(f"{key}: \"{en_dict[key]}\"   # TODO: 替换为中文翻译")

if __name__ == "__main__":
    main()