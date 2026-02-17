#!/usr/bin/env python3
import os
import re

def update_include_paths(root_dir):
    """批量更新源文件中的#include路径"""
    
    # 定义路径映射关系
    path_mappings = {
        # core/document
        r'#include "Block.h"': '#include "core/document/Block.h"',
        r'#include "CharacterStyle.h"': '#include "core/document/CharacterStyle.h"',
        r'#include "Document.h"': '#include "core/document/Document.h"',
        r'#include "ImageBlock.h"': '#include "core/document/ImageBlock.h"',
        r'#include "Page.h"': '#include "core/document/Page.h"',
        r'#include "ParagraphBlock.h"': '#include "core/document/ParagraphBlock.h"',
        r'#include "ParagraphStyle.h"': '#include "core/document/ParagraphStyle.h"',
        r'#include "Section.h"': '#include "core/document/Section.h"',
        r'#include "Span.h"': '#include "core/document/Span.h"',
        r'#include "TableBlock.h"': '#include "core/document/TableBlock.h"',
        
        # core/commands
        r'#include "EditCommand.h"': '#include "core/commands/EditCommand.h"',
        r'#include "InsertBlockCommand.h"': '#include "core/commands/InsertBlockCommand.h"',
        r'#include "InsertTextCommand.h"': '#include "core/commands/InsertTextCommand.h"',
        r'#include "RemoveBlockCommand.h"': '#include "core/commands/RemoveBlockCommand.h"',
        r'#include "RemoveTextCommand.h"': '#include "core/commands/RemoveTextCommand.h"',
        r'#include "SetCharacterStyleCommand.h"': '#include "core/commands/SetCharacterStyleCommand.h"',
        r'#include "SetParagraphStyleCommand.h"': '#include "core/commands/SetParagraphStyleCommand.h"',
        
        # core/layout
        r'#include "LayoutEngine.h"': '#include "core/layout/LayoutEngine.h"',
        r'#include "PageBuilder.h"': '#include "core/layout/PageBuilder.h"',
        
        # core/styles
        r'#include "StyleManager.h"': '#include "core/styles/StyleManager.h"',
        
        # core/utils
        r'#include "Constants.h"': '#include "core/utils/Constants.h"',
        r'#include "FontUtils.h"': '#include "core/utils/FontUtils.h"',
        
        # editcontrol
        r'#include "Cursor.h"': '#include "editcontrol/cursor/Cursor.h"',
        r'#include "FormatController.h"': '#include "editcontrol/formatting/FormatController.h"',
        r'#include "EditEventHandler.h"': '#include "editcontrol/handlers/EditEventHandler.h"',
        r'#include "Selection.h"': '#include "editcontrol/selection/Selection.h"',
        
        # graphics
        r'#include "BlockItemFactory.h"': '#include "graphics/factory/BlockItemFactory.h"',
        r'#include "BaseBlockItem.h"': '#include "graphics/items/BaseBlockItem.h"',
        r'#include "CursorItem.h"': '#include "graphics/items/CursorItem.h"',
        r'#include "ImageBlockItem.h"': '#include "graphics/items/ImageBlockItem.h"',
        r'#include "PageItem.h"': '#include "graphics/items/PageItem.h"',
        r'#include "SelectionItem.h"': '#include "graphics/items/SelectionItem.h"',
        r'#include "TableBlockItem.h"': '#include "graphics/items/TableBlockItem.h"',
        r'#include "TextBlockItem.h"': '#include "graphics/items/TextBlockItem.h"',
        r'#include "DocumentScene.h"': '#include "graphics/scene/DocumentScene.h"',
        r'#include "DocumentView.h"': '#include "graphics/view/DocumentView.h"',
        
        # io
        r'#include "XmlSerializer.h"': '#include "io/serializers/XmlSerializer.h"',
        r'#include "PdfExporter.h"': '#include "io/exporters/PdfExporter.h"',
        r'#include "TxtImporter.h"': '#include "io/importers/TxtImporter.h"',
        
        # ui
        r'#include "FontDialog.h"': '#include "ui/dialogs/FontDialog.h"',
        r'#include "InsertImageDialog.h"': '#include "ui/dialogs/InsertImageDialog.h"',
        r'#include "PageSetupDialog.h"': '#include "ui/dialogs/PageSetupDialog.h"',
        r'#include "ParagraphDialog.h"': '#include "ui/dialogs/ParagraphDialog.h"',
        r'#include "MainWindow.h"': '#include "ui/mainwindow/MainWindow.h"',
        r'#include "RibbonBar.h"': '#include "ui/ribbon/RibbonBar.h"',
        r'#include "FormatToolBar.h"': '#include "ui/widgets/FormatToolBar.h"',
        r'#include "Ruler.h"': '#include "ui/widgets/Ruler.h"',
        
        # app
        r'#include "Application.h"': '#include "app/Application.h"',
    }
    
    # 遍历src目录下的所有.cpp文件
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                print(f"Processing {file_path}")
                
                # 读取文件内容
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                # 应用所有路径映射
                modified = False
                for old_pattern, new_path in path_mappings.items():
                    # 使用正则表达式匹配精确的#include行
                    pattern = old_pattern.replace('"', r'\"')
                    regex = rf'^({pattern})'
                    
                    def replace_func(match):
                        nonlocal modified
                        modified = True
                        return new_path
                    
                    new_content = re.sub(regex, replace_func, content, flags=re.MULTILINE)
                    if modified:
                        content = new_content
                
                # 如果有修改，写回文件
                if modified:
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print(f"  Updated {file}")

if __name__ == "__main__":
    project_root = r"d:\vscodeproject\QtWordEditor\src"
    update_include_paths(project_root)
    print("All include paths updated successfully!")