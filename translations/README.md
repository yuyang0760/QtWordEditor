# QtWordEditor 翻译支持说明

## 概述

本项目现已支持多语言翻译功能，目前包含中文(zh_CN)和英文(en_US)两种语言。

## 文件结构

```
translations/
├── QtWordEditor_zh_CN.ts    # 中文翻译源文件
├── QtWordEditor_en_US.ts    # 英文翻译源文件
├── QtWordEditor_zh_CN.qm    # 编译后的中文翻译文件
└── QtWordEditor_en_US.qm    # 编译后的英文翻译文件
```

## 使用方法

### 1. 更新翻译源文件

运行 `update_translations.bat` 脚本，该脚本会：
- 扫描源代码中的所有 `tr()` 和 `QObject::tr()` 字符串
- 更新 `.ts` 翻译源文件
- 自动检测新增或删除的可翻译字符串

### 2. 编辑翻译内容

使用 Qt Linguist 工具打开 `.ts` 文件进行翻译：
- Windows: `%QT_PATH%\bin\linguist.exe`
- 或者直接双击 `.ts` 文件（如果已关联）

### 3. 编译翻译文件

运行 `compile_translations.bat` 脚本，该脚本会：
- 将 `.ts` 文件编译为 `.qm` 二进制文件
- 这些文件会被程序自动加载

## 开发者指南

### 在代码中添加可翻译字符串

```cpp
// 方法1：使用QObject::tr()（推荐用于非QObject类）
QString text = QObject::tr("Hello World");

// 方法2：使用tr()（适用于继承自QObject的类）
QString text = tr("Hello World");

// 带参数的翻译
QString text = tr("Welcome, %1!").arg(userName);

// 复数形式处理
QString text = tr("%n file(s) processed", "", fileCount);
```

### 添加新的语言支持

1. 复制现有的 `.ts` 文件并重命名为新语言代码
2. 修改 `<TS version="2.1" language="xx_XX">` 中的语言代码
3. 使用 Qt Linguist 翻译新语言
4. 运行编译脚本生成 `.qm` 文件

## 自动化构建

CMake配置已集成翻译支持：
- 构建时自动处理翻译文件
- 生成的 `.qm` 文件会自动包含在可执行文件中
- 安装时会将翻译文件复制到正确位置

## 注意事项

1. 请确保Qt安装路径正确（在bat脚本中修改）
2. 翻译文件应与源代码保持同步
3. 建议定期运行更新脚本以捕获新的可翻译字符串
4. 不同操作系统可能需要调整脚本中的路径分隔符

## 测试翻译效果

程序会自动根据系统语言加载对应翻译。如需测试不同语言：
1. 在控制面板中更改系统语言
2. 或在代码中调用 `switchLanguage("zh_CN")` 方法