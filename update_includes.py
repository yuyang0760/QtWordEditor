#!/usr/bin/env python3
import os
import re

def update_include_paths(root_dir):
    """批量更新源文件中的#include路径 - 移除多余的include/前缀"""
    
    # 遍历目录下的所有.cpp和.h文件
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.h'):
                file_path = os.path.join(root, file)
                print(f"Processing {file_path}")
                
                # 读取文件内容
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                # 移除 #include "include/..." 中的 include/ 前缀
                modified = False
                new_content = re.sub(r'#include\s+"include/(.*?)"', r'#include "\1"', content)
                if new_content != content:
                    modified = True
                    content = new_content
                
                # 如果有修改，写回文件
                if modified:
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print(f"  Updated {file}")

if __name__ == "__main__":
    src_root = r"d:\vscodeproject\QtWordEditor\src"
    include_root = r"d:\vscodeproject\QtWordEditor\include"
    update_include_paths(src_root)
    update_include_paths(include_root)
    print("All include paths updated successfully!")