# QtWordEditor 详细文档

## 目录
1. [项目概述](#项目概述)
2. [项目结构](#项目结构)
3. [核心模块](#核心模块)
4. [文档模型](#文档模型)
5. [数学公式系统](#数学公式系统)
6. [图形系统](#图形系统)
7. [编辑控制](#编辑控制)
8. [用户界面](#用户界面)
9. [输入输出](#输入输出)
10. [常量定义](#常量定义)
11. [构建系统](#构建系统)

---

## 项目概述

### 项目简介
QtWordEditor 是一个基于 Qt6 框架开发的现代化文字处理软件，采用模块化架构设计，具有丰富的文档编辑和格式化功能。项目新增了强大的数学公式编辑系统，支持分数、根号、上下标等复杂数学表达式的编辑和渲染。

### 技术栈
- **编程语言**: C++17
- **GUI 框架**: Qt 6.5+
- **构建系统**: CMake 3.16+
- **编译器支持**: MinGW, MSVC
- **生成器**: Ninja

### 主要功能
- 富文本编辑
- 字符和段落格式化
- 多页文档支持
- 页面设置（尺寸、边距、方向）
- 撤销/重做功能
- 缩放和滚动
- 实时坐标显示
- Ribbon 风格用户界面
- **数学公式编辑**（新增）
  - 分数支持
  - 数字/变量输入
  - 行容器
  - 公式内光标导航
  - 复合设计模式支持嵌套公式
- **统一光标系统**（新增）
- **文档标尺**（新增）
- **调试控制台**（新增）
- **样式管理**（新增）

---

## 项目结构

### 目录结构
```
QtWordEditor/
├── include/                    # 头文件
│   ├── app/                   # 应用程序入口
│   ├── core/                  # 核心模块
│   │   ├── commands/          # 命令系统
│   │   ├── document/          # 文档模型
│   │   │   └── math/          # 数学公式数据模型
│   │   ├── layout/            # 布局引擎
│   │   ├── styles/            # 样式管理
│   │   └── utils/             # 工具类
│   ├── editcontrol/           # 编辑控制
│   │   ├── cursor/            # 光标控制
│   │   ├── formatting/        # 格式化控制
│   │   ├── handlers/          # 事件处理
│   │   └── selection/         # 选择控制
│   ├── graphics/              # 图形渲染
│   │   ├── factory/           # 工厂类
│   │   ├── formula/           # 公式渲染
│   │   ├── items/             # 图形项
│   │   ├── scene/             # 场景
│   │   └── view/              # 视图
│   ├── io/                    # 输入输出
│   └── ui/                    # 用户界面
│       ├── dialogs/           # 对话框
│       ├── mainwindow/        # 主窗口
│       ├── ribbon/            # Ribbon栏
│       └── widgets/           # 自定义控件
├── src/                       # 源文件
│   └── (对应 include 的实现)
├── translations/              # 翻译文件
├── u文档/                    # 文档目录
├── CMakeLists.txt            # CMake 构建配置
├── CMakePresets.json         # CMake 预设
└── *.md                      # 文档文件
```

### CMake 模块划分

项目采用模块化设计，分为以下静态库：

| 模块 | 说明 | 依赖 |
|------|------|------|
| QtWordEditorCore | 核心文档模型、命令、布局 | Qt::Core, Qt::Gui |
| QtWordEditorEditControl | 光标、选择、编辑处理 | Qt::Core, Qt::Gui, QtWordEditorCore |
| QtWordEditorGraphics | 图形项、场景、视图、公式渲染 | Qt::Core, Qt::Gui, Qt::Widgets, QtWordEditorCore |
| QtWordEditorIO | 导入导出、序列化 | Qt::Core, Qt::Gui, Qt::PrintSupport, QtWordEditorCore |
| QtWordEditorUI | 主窗口、对话框、控件 | Qt::Core, Qt::Gui, Qt::Widgets, Qt::PrintSupport, 其他所有模块 |

---

## 核心模块 (Core Module)

### 文档模型层级（新版）

```
Document (文档根)
  └── Section (节)
        └── Block (块)
              ├── ParagraphBlock (段落块)
              │     └── InlineSpan (内联元素基类)
              │           ├── TextSpan (文本)
              │           └── MathSpan (公式)
              │                 ├── NumberMathSpan (数字/变量)
              │                 ├── RowContainerMathSpan (行容器)
              │                 ├── FractionMathSpan (分数)
              │                 └── ... (其他公式类型)
              ├── ImageBlock (图片块)
              └── TableBlock (表格块)
```

### InlineSpan 类 (`include/core/document/InlineSpan.h`)

所有内联内容的基类（文本和公式）。这是内联内容系统的基石，TextSpan和MathSpan都继承自此，使得文本和公式可以在ParagraphBlock中统一管理。

#### 类型枚举
```cpp
enum Type {
    Text,    ///< 文本
    Math     ///< 公式
};
```

#### 公共方法
```cpp
// 获取内联内容类型
virtual Type type() const = 0;

// 获取占用的字符数（TextSpan返回文本长度，MathSpan返回1）
virtual int length() const = 0;

// 克隆当前内联内容
virtual InlineSpan *clone() const = 0;
```

### TextSpan 类 (`include/core/document/TextSpan.h`)

文本内联元素数据类，继承自 InlineSpan，用于存储文本内容和样式。替换原来的 Span 类，但保留相似的接口。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_text` | `QString` | 文本内容 |
| `m_styleName` | `QString` | 命名样式名称 |
| `m_directStyle` | `CharacterStyle` | 直接样式（覆盖命名样式） |

#### 公共方法
```cpp
// InlineSpan 接口实现
Type type() const override { return Text; }
int length() const override;
InlineSpan *clone() const override;

// 文本内容
QString text() const;
void setText(const QString &text);
void append(const QString &text);
void insert(int position, const QString &text);
void remove(int position, int length);

// 样式 - 传统接口（向后兼容）
CharacterStyle style() const;
void setStyle(const CharacterStyle &style);

// 命名样式
QString styleName() const;
void setStyleName(const QString &styleName);

// 直接样式（覆盖命名样式）
CharacterStyle directStyle() const;
void setDirectStyle(const CharacterStyle &style);

// 获取最终生效的样式（命名样式 + 直接样式）
CharacterStyle effectiveStyle(const StyleManager *styleManager) const;
```

### ParagraphBlock 类（更新） (`include/core/document/ParagraphBlock.h`)

段落块，继承自 Block，现在使用统一的 InlineSpan 管理文本和公式。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_inlineSpans` | `QList&lt;InlineSpan*&gt;` | 统一管理文本和公式 |
| `m_paragraphStyle` | `ParagraphStyle` | 段落样式 |

#### 公共方法
```cpp
// 文本操作
QString text() const;
void setText(const QString &text);
void insert(int position, const QString &text, const CharacterStyle &style);
void remove(int position, int length);

// InlineSpan 访问
int inlineSpanCount() const;
InlineSpan *inlineSpan(int index) const;
void addInlineSpan(InlineSpan *span);
void insertInlineSpan(int index, InlineSpan *span);
void insertInlineSpanAtPosition(int position, InlineSpan *span);
void removeInlineSpan(InlineSpan *span);
void removeInlineSpanAt(int index);
void clearInlineSpans();

// 段落样式
ParagraphStyle paragraphStyle() const;
void setParagraphStyle(const ParagraphStyle &style);

// 重写 Block 方法
int length() const override;
bool isEmpty() const override;
Block *clone() const override;

// 辅助方法
int findInlineSpanIndex(int globalPosition, int *positionInSpan = nullptr) const;
CharacterStyle styleAt(int position) const;
void setStyle(int start, int length, const CharacterStyle &style);
```

### Document 类 (`include/core/document/Document.h`)

文档的根容器，管理所有节、元数据和撤销栈。（保持原有结构）

---

## 数学公式系统

### MathSpan 类 (`include/core/document/MathSpan.h`)

公式内联元素数据基类，所有公式元素都继承自此类，采用 Composite 设计模式处理嵌套结构。MathSpan 只负责数据存储，不负责渲染。

#### 公式元素类型枚举
```cpp
enum MathType {
    Number,         ///< 数字/变量
    Operator,       ///< 运算符
    RowContainer,   ///< 水平行容器
    Fraction,       ///< 分数
    Radical,        ///< 根号
    SubSup,         ///< 上下标
    Bracket         ///< 括号
};
```

#### InlineSpan 接口实现
```cpp
Type type() const override { return Math; }
int length() const override { return 1; } // 公式占用1个字符位置
```

#### 子项管理（容器类型）
```cpp
// 在指定位置插入子元素
virtual void insertChild(int index, MathSpan *item);

// 在末尾添加子元素
virtual void appendChild(MathSpan *item);

// 移除指定的子元素
virtual void removeChild(MathSpan *item);

// 获取所有子元素列表
virtual QList&lt;MathSpan*&gt; children() const;

// 获取子元素数量
virtual int childCount() const;

// 获取指定位置的子元素
virtual MathSpan *childAt(int index) const;

// 获取子元素在父元素中的索引
virtual int indexOfChild(MathSpan *child) const;
```

### NumberMathSpan 类 (`include/core/document/math/NumberMathSpan.h`)

数字/变量数据类，存储数字或变量的文本内容。

```cpp
class NumberMathSpan : public MathSpan
{
public:
    MathType mathType() const override { return Number; }
    bool isContainer() const override { return false; }
    InlineSpan *clone() const override;

    QString text() const;
    void setText(const QString &text);
    void insert(int position, const QString &text);
    void remove(int position, int length);
};
```

### RowContainerMathSpan 类 (`include/core/document/math/RowContainerMathSpan.h`)

行容器数据类，用于水平排列多个公式子元素。

```cpp
class RowContainerMathSpan : public MathSpan
{
public:
    MathType mathType() const override { return RowContainer; }
    bool isContainer() const override { return true; }
    InlineSpan *clone() const override;
};
```

### FractionMathSpan 类 (`include/core/document/math/FractionMathSpan.h`)

分数数据类，包含分子和分母两个子元素。

```cpp
class FractionMathSpan : public MathSpan
{
public:
    MathType mathType() const override { return Fraction; }
    bool isContainer() const override { return true; }
    InlineSpan *clone() const override;

    // 子项管理（分子和分母）
    QList&lt;MathSpan*&gt; children() const override;
    int childCount() const override { return 2; }
    MathSpan *childAt(int index) const override;

    // Fraction 特有接口
    MathSpan *numerator() const;
    void setNumerator(MathSpan *numerator);
    MathSpan *denominator() const;
    void setDenominator(MathSpan *denominator);
};
```

---

## 编辑控制

### UnifiedCursor 类 (`include/editcontrol/cursor/UnifiedCursor.h`)

统一光标类，整合了普通文档光标和数学公式光标的功能，提供统一的光标管理接口。

#### 光标模式枚举
```cpp
enum class CursorMode {
    DocumentMode,       // 普通文档模式
    MathContainerMode,  // 公式容器模式
    MathNumberMode      // 公式数字模式
};
```

#### UnifiedCursorPosition 结构体
```cpp
struct UnifiedCursorPosition {
    // 文档模式字段
    int blockIndex = -1;
    int offset = 0;

    // 公式模式字段
    bool inMathSpan = false;
    InlineSpan *mathSpan = nullptr;
    MathItem *mathItem = nullptr;
    RowContainerItem *mathContainer = nullptr;
    NumberItem *mathNumberItem = nullptr;
    int mathChildIndex = -1;
    int mathChildOffset = 0;

    // 当前模式
    CursorMode mode = CursorMode::DocumentMode;
};
```

#### 模式切换
```cpp
void setMode(CursorMode mode);
CursorMode mode() const;
```

#### 文档模式方法
```cpp
void setDocumentPosition(int blockIndex, int offset);
void moveLeft();
void moveRight();
void moveUp();
void moveDown();
void moveToStartOfLine();
void moveToEndOfLine();
void moveToStartOfDocument();
void moveToEndOfDocument();
void insertText(const QString &text, const CharacterStyle &style);
void deletePreviousChar();
void deleteNextChar();
```

#### 公式模式方法
```cpp
void setMathContainerPosition(RowContainerItem *container, int position);
void setMathNumberPosition(NumberItem *numberItem, int position);
void mathMoveLeft();
void mathMoveRight();
void mathMoveUp();
void mathMoveDown();
void mathMoveToParent();
void exitMathMode();
```

#### 信号
```cpp
void positionChanged(const UnifiedCursorPosition &pos);
void modeChanged(CursorMode newMode, CursorMode oldMode);
```

---

## 图形系统

### 架构概述（更新）

图形系统使用 Qt Graphics Framework，采用 Model-View-Controller 模式，新增公式渲染模块：

```
DocumentModel (数据)
    ↓
DocumentScene (场景)
    ↓
DocumentView (视图)
    ↓
    ├─ TextBlockItem
    │    └─ TextBlockLayoutEngine
    │         └─ LayoutItem (Text / Math)
    │              ├─ Text (QPainter绘制)
    │              └─ MathItem (公式渲染)
    │                   ├─ NumberItem
    │                   ├─ RowContainerItem
    │                   └─ FractionItem
    ├─ ImageBlockItem
    └─ TableBlockItem
```

### TextBlockLayoutEngine 类 (`include/graphics/items/TextBlockLayoutEngine.h`)

文本块布局引擎类，负责文本的测量、换行、对齐和基线对齐等排版工作，直接基于 InlineSpan 工作。

#### 换行模式
```cpp
enum class WrapMode {
    NoWrap,
    WrapAtWordBoundary,
    WrapAnywhere
};
```

#### LayoutItem 结构体
```cpp
struct LayoutItem {
    int spanIndex;
    InlineSpan *inlineSpan;
    QString text;
    QString fullSpanText;
    CharacterStyle style;
    qreal width;
    qreal height;
    qreal ascent;
    qreal descent;
    QPointF position;
    QFont font;
    int startOffsetInSpan;
    int endOffsetInSpan;
    int globalStartOffset;
    int globalEndOffset;
    QGraphicsItem *graphicsItem;  // MathItem 或 nullptr
};
```

#### 公共方法
```cpp
void setAvailableWidth(qreal width);
void setParagraphStyle(const ParagraphStyle &style);
void setWrapMode(WrapMode mode);

// 执行布局（支持MathSpan的真实尺寸）
void layout(const QList&lt;InlineSpan*&gt; &spans);
void layout(const QList&lt;InlineSpan*&gt; &spans,
            const QHash&lt;InlineSpan*, QSizeF&gt; &mathSizeMap,
            const QHash&lt;InlineSpan*, qreal&gt; &mathBaselineMap);

const QList&lt;LayoutItem&gt; &layoutItems() const;
const QList&lt;LineInfo&gt; &lines() const;
qreal totalWidth() const;
qreal totalHeight() const;

// 光标位置计算
CursorHitResult hitTest(const QPointF &amp;localPos, const QList&lt;InlineSpan*&gt; &spans) const;
CursorVisualResult cursorPositionAt(int globalOffset, const QList&lt;InlineSpan*&gt; &spans) const;
QList&lt;QRectF&gt; selectionRects(int startOffset, int endOffset, const QList&lt;InlineSpan*&gt; &spans) const;
```

### MathItem 类 (`include/graphics/formula/MathItem.h`)

所有公式视图元素的基类，继承自 QGraphicsItem，可以接收鼠标和键盘事件，实现原地编辑。每个 MathItem 对应一个 MathSpan 数据对象。

```cpp
class MathItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 2000 };

    explicit MathItem(MathSpan *span, MathItem *parent = nullptr);

    // 核心布局接口
    virtual void updateLayout();
    virtual qreal baseline() const;

    // 子项管理
    virtual void insertChild(int index, MathItem *item);
    virtual void appendChild(MathItem *item);
    virtual void removeChild(MathItem *item);
    virtual QList&lt;MathItem*&gt; children() const;
    virtual int childCount() const;
    virtual MathItem *childAt(int index) const;
    virtual int indexOfChild(MathItem *child) const;

    // Qt标准接口
    int type() const override { return Type; }
    QRectF boundingRect() const override = 0;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override = 0;

    // 数据关联
    MathSpan *mathSpan() const;
    MathItem *parentMathItem() const;
    virtual bool isContainer() const { return false; }

    // 编辑支持
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual int hitTestRegion(const QPointF &amp;localPos) const;
};
```

### 具体公式图形项

| 类名 | 说明 | 位置 |
|------|------|------|
| `NumberItem` | 数字/变量渲染 | `include/graphics/formula/NumberItem.h` |
| `RowContainerItem` | 行容器渲染 | `include/graphics/formula/RowContainerItem.h` |
| `FractionItem` | 分数渲染 | `include/graphics/formula/FractionItem.h` |
| `MathCursor` | 公式内光标 | `include/graphics/formula/MathCursor.h` |

### MathItemFactory 类 (`include/graphics/factory/MathItemFactory.h`)

公式项工厂，用于创建公式图形项。

```cpp
class MathItemFactory
{
public:
    static MathItem *createItem(MathSpan *span, MathItem *parent = nullptr);
};
```

---

## 用户界面

### Ruler 类 (`include/ui/widgets/Ruler.h`)

标尺类，显示水平标尺，显示页边距、段落缩进和制表位。

```cpp
class Ruler : public QWidget
{
public:
    explicit Ruler(QWidget *parent = nullptr);

    // 设置用于坐标转换的视图
    void setView(QWidget *view);

    // 设置页边距（毫米）
    void setMargins(qreal left, qreal right);

    // 设置段落缩进（毫米）
    void setIndents(qreal firstLine, qreal left);

    // 获取当前设置
    qreal leftMargin() const;
    qreal rightMargin() const;
    qreal firstLineIndent() const;
    qreal leftIndent() const;
};
```

### DebugConsole 类 (`include/ui/widgets/DebugConsole.h`)

调试控制台控件，用于显示调试信息。

### FormatToolBar 类 (`include/ui/widgets/FormatToolBar.h`)

格式工具栏，提供常用的文本格式化按钮。

### 新增对话框

| 对话框 | 说明 | 位置 |
|--------|------|------|
| `FontDialog` | 字体对话框 | `include/ui/dialogs/FontDialog.h` |
| `InsertImageDialog` | 插入图片对话框 | `include/ui/dialogs/InsertImageDialog.h` |
| `ParagraphDialog` | 段落设置对话框 | `include/ui/dialogs/ParagraphDialog.h` |
| `StyleManagerDialog` | 样式管理对话框 | `include/ui/dialogs/StyleManagerDialog.h` |

### MainWindow 类（更新）

主窗口类新增成员：
- `m_debugConsoleDock` - 调试控制台停靠窗口
- `m_debugConsoleTextEdit` - 调试控制台文本编辑器
- `m_ruler` - 标尺控件
- `m_currentCursorPos` - 统一光标位置

### StyleManager 类 (`include/core/styles/StyleManager.h`)

样式管理器，管理命名样式。

---

## 核心技术实现细节（更新）

### 内联内容系统架构

项目采用统一的 InlineSpan 抽象作为文本和公式的基类：

```
InlineSpan (抽象基类)
  ├── TextSpan (文本)
  └── MathSpan (公式，Composite模式)
       ├── NumberMathSpan (叶子节点)
       ├── RowContainerMathSpan (容器)
       └── FractionMathSpan (容器，分子+分母)
```

### 文本与公式混排布局

TextBlockLayoutEngine 支持文本和公式的统一布局：

1. 为每个 InlineSpan 计算尺寸
2. 对于 MathSpan，从对应的 MathItem 获取真实尺寸和基线
3. 按基线对齐排列内联元素
4. 处理换行和段落对齐

```cpp
// 文本和公式的基线对齐
qreal lineMaxBaseline = qMax(textAscent, mathBaseline);
```

### 统一光标切换流程

```
DocumentMode ←→ MathContainerMode ←→ MathNumberMode
     ↑              ↑                    ↑
  普通文本      公式容器              数字编辑
```

### 数学公式的 Composite 模式

公式系统采用 Composite 设计模式：
- 叶子节点：NumberMathSpan
- 容器节点：RowContainerMathSpan、FractionMathSpan 等

这使得复杂的嵌套公式（如分数内还有分数）可以轻松实现。

---

## 输入输出 (IO Module)

保持原有结构不变。

---

## 常量定义 (Constants)

保持原有结构不变。

---

## 构建系统 (Build System)

保持原有结构不变，使用 CMakePresets.json 的 Debug 预设编译。
