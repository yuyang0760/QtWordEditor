# QtWordEditor 详细文档

## 目录
1. [项目概述](#项目概述)
2. [项目结构](#项目结构)
3. [核心模块](#核心模块)
4. [文档模型](#文档模型)
5. [图形系统](#图形系统)
6. [编辑控制](#编辑控制)
7. [用户界面](#用户界面)
8. [输入输出](#输入输出)
9. [常量定义](#常量定义)
10. [构建系统](#构建系统)

---

## 项目概述

### 项目简介
QtWordEditor 是一个基于 Qt6 框架开发的现代化文字处理软件，采用模块化架构设计，具有丰富的文档编辑和格式化功能。

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

---

## 项目结构

### 目录结构
```
QtWordEditor/
├── include/                    # 头文件
│   ├── app/                   # 应用程序入口
│   ├── core/                  # 核心模块
│   ├── editcontrol/           # 编辑控制
│   ├── graphics/              # 图形渲染
│   ├── io/                    # 输入输出
│   └── ui/                    # 用户界面
├── src/                       # 源文件
│   ├── app/
│   ├── core/
│   ├── editcontrol/
│   ├── graphics/
│   ├── io/
│   └── ui/
├── translations/              # 翻译文件
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
| QtWordEditorGraphics | 图形项、场景、视图 | Qt::Core, Qt::Gui, Qt::Widgets, QtWordEditorCore |
| QtWordEditorIO | 导入导出、序列化 | Qt::Core, Qt::Gui, Qt::PrintSupport, QtWordEditorCore |
| QtWordEditorUI | 主窗口、对话框、控件 | Qt::Core, Qt::Gui, Qt::Widgets, Qt::PrintSupport, 其他所有模块 |

---

## 核心模块 (Core Module)

### 文档模型层级

```
Document (文档根)
  └── Section (节)
        └── Block (块)
              ├── ParagraphBlock (段落块)
              ├── ImageBlock (图片块)
              ├── TableBlock (表格块)
              └── ... (其他块类型)
                    └── Span (文本跨度)
                          └── CharacterStyle (字符样式)
```

### Document 类 (`include/core/document/Document.h`)

文档的根容器，管理所有节、元数据和撤销栈。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_documentId` | `int` | 文档 ID |
| `m_title` | `QString` | 文档标题 |
| `m_author` | `QString` | 作者 |
| `m_created` | `QDateTime` | 创建时间 |
| `m_modified` | `QDateTime` | 修改时间 |
| `m_sections` | `QList<Section*>` | 节列表 |
| `m_undoStack` | `QUndoStack*` | 撤销栈 |

#### 公共方法
```cpp
// 文档标识
int documentId() const;
void setDocumentId(int id);

// 元数据
QString title() const;
void setTitle(const QString &title);
QString author() const;
void setAuthor(const QString &author);
QDateTime created() const;
QDateTime modified() const;
void setModified(const QDateTime &modified);

// 节管理
int sectionCount() const;
Section *section(int index) const;
void addSection(Section *section);
void insertSection(int index, Section *section);
void removeSection(int index);

// 块管理（全局索引）
int blockCount() const;
Block *block(int globalIndex) const;

// 撤销栈
QUndoStack *undoStack() const;

// 纯文本导出
QString plainText() const;
```

#### 信号
```cpp
void documentChanged();
void sectionAdded(int index);
void sectionRemoved(int index);
void blockAdded(int globalIndex);
void blockRemoved(int globalIndex);
void layoutChanged();
```

### Section 类 (`include/core/document/Section.h`)

文档的节，包含多个块。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_blocks` | `QList<Block*>` | 块列表 |
| `m_pages` | `QList<Page*>` | 页列表 |

#### 公共方法
```cpp
// 块管理
int blockCount() const;
Block *block(int index) const;
void addBlock(Block *block);
void insertBlock(int index, Block *block);
void removeBlock(int index);
void clearBlocks();

// 页管理
int pageCount() const;
Page *page(int index) const;
void addPage(Page *page);
void clearPages();
```

### Block 类 (`include/core/document/Block.h`)

块的基类，所有块类型都继承自此类。

#### 块类型枚举
```cpp
enum class BlockType {
    Paragraph,
    Image,
    Table
};
```

#### 公共方法
```cpp
BlockType type() const;
int blockIndex() const;
void setBlockIndex(int index);
qreal height() const;
void setHeight(qreal height);
qreal y() const;
void setY(qreal y);
```

### ParagraphBlock 类 (`include/core/document/ParagraphBlock.h`)

段落块，继承自 Block。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_text` | `QString` | 段落文本 |
| `m_spans` | `QList<Span*>` | 文本跨度列表 |
| `m_style` | `ParagraphStyle` | 段落样式 |

#### 公共方法
```cpp
QString text() const;
void setText(const QString &text);

// 跨度管理
int spanCount() const;
Span *span(int index) const;
void addSpan(Span *span);
void clearSpans();

// 段落样式
ParagraphStyle &style();
const ParagraphStyle &style() const;
```

### Span 类 (`include/core/document/Span.h`)

文本跨度，代表具有相同字符样式的一段文本。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_start` | `int` | 起始位置 |
| `m_length` | `int` | 长度 |
| `m_style` | `CharacterStyle` | 字符样式 |

#### 公共方法
```cpp
int start() const;
void setStart(int start);
int length() const;
void setLength(int length);
int end() const;

CharacterStyle &style();
const CharacterStyle &style() const;
```

### CharacterStyle 类 (`include/core/document/CharacterStyle.h`)

字符样式，使用隐式共享（copy-on-write）。

#### 字体属性
| 属性 | 类型 | 说明 |
|------|------|------|
| `font` | `QFont` | 完整字体 |
| `fontFamily` | `QString` | 字体族 |
| `fontSize` | `int` | 字号 |
| `bold` | `bool` | 加粗 |
| `italic` | `bool` | 斜体 |
| `underline` | `bool` | 下划线 |
| `strikeOut` | `bool` | 删除线 |

#### 颜色属性
| 属性 | 类型 | 说明 |
|------|------|------|
| `textColor` | `QColor` | 文本颜色 |
| `backgroundColor` | `QColor` | 背景颜色 |

#### 其他属性
| 属性 | 类型 | 说明 |
|------|------|------|
| `letterSpacing` | `qreal` | 字间距 |

#### 公共方法
```cpp
// 字体
QFont font() const;
void setFont(const QFont &font);
QString fontFamily() const;
void setFontFamily(const QString &family);
int fontSize() const;
void setFontSize(int size);
bool bold() const;
void setBold(bool bold);
bool italic() const;
void setItalic(bool italic);
bool underline() const;
void setUnderline(bool underline);
bool strikeOut() const;
void setStrikeOut(bool strikeOut);

// 颜色
QColor textColor() const;
void setTextColor(const QColor &color);
QColor backgroundColor() const;
void setBackgroundColor(const QColor &color);

// 字间距
qreal letterSpacing() const;
void setLetterSpacing(qreal spacing);

// 比较
bool operator==(const CharacterStyle &other) const;
bool operator!=(const CharacterStyle &other) const;

// 重置
void reset();
```

### ParagraphStyle 类 (`include/core/document/ParagraphStyle.h`)

段落样式，使用隐式共享（copy-on-write）。

#### 对齐方式枚举
```cpp
enum class ParagraphAlignment {
    AlignLeft,
    AlignCenter,
    AlignRight,
    AlignJustify
};
```

#### 属性
| 属性 | 类型 | 说明 |
|------|------|------|
| `alignment` | `ParagraphAlignment` | 对齐方式 |
| `firstLineIndent` | `qreal` | 首行缩进 |
| `leftIndent` | `qreal` | 左缩进 |
| `rightIndent` | `qreal` | 右缩进 |
| `spaceBefore` | `qreal` | 段前间距 |
| `spaceAfter` | `qreal` | 段后间距 |
| `lineHeight` | `int` | 行高（百分比） |

#### 公共方法
```cpp
// 对齐
ParagraphAlignment alignment() const;
void setAlignment(ParagraphAlignment align);

// 缩进
qreal firstLineIndent() const;
void setFirstLineIndent(qreal indent);
qreal leftIndent() const;
void setLeftIndent(qreal indent);
qreal rightIndent() const;
void setRightIndent(qreal indent);

// 间距
qreal spaceBefore() const;
void setSpaceBefore(qreal space);
qreal spaceAfter() const;
void setSpaceAfter(qreal space);
int lineHeight() const;
void setLineHeight(int percent);

// 比较
bool operator==(const ParagraphStyle &other) const;
bool operator!=(const ParagraphStyle &other) const;

// 重置
void reset();
```

### Page 类 (`include/core/document/Page.h`)

页面，由布局引擎生成。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_pageNumber` | `int` | 页码 |
| `m_pageRect` | `QRectF` | 页面矩形 |
| `m_contentRect` | `QRectF` | 内容矩形 |
| `m_blocks` | `QList<Block*>` | 块列表（非拥有） |

#### 公共方法
```cpp
int pageNumber() const;
void setPageNumber(int number);
QRectF pageRect() const;
QRectF contentRect() const;

// 块管理
int blockCount() const;
Block *block(int index) const;
void addBlock(Block *block);
void clearBlocks();
bool isEmpty() const;
```

### PageBuilder 类 (`include/core/layout/PageBuilder.h`)

页面构建器，协助布局引擎将块分配到页面。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_pageWidth` | `qreal` | 页面宽度 |
| `m_pageHeight` | `qreal` | 页面高度 |
| `m_margin` | `qreal` | 页边距 |
| `m_contentWidth` | `qreal` | 内容宽度 |
| `m_contentHeight` | `qreal` | 内容高度 |
| `m_currentY` | `qreal` | 当前 Y 位置 |
| `m_currentPageBlocks` | `QList<Block*>` | 当前页块列表 |

#### 公共方法
```cpp
PageBuilder(qreal pageWidth, qreal pageHeight, qreal margin);

bool tryAddBlock(Block *block);
Page *finishPage();
void reset();
```

---

## 图形系统 (Graphics Module)

### 架构概述

图形系统使用 Qt Graphics Framework，采用 Model-View-Controller 模式：

```
DocumentModel (数据)
    ↓
DocumentScene (场景)
    ↓
DocumentView (视图)
    ↓
用户界面
```

### PageItem 类 (`include/graphics/items/PageItem.h`)

页面图形项，继承自 QGraphicsRectItem。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_page` | `Page*` | 页面对象 |

#### 公共方法
```cpp
explicit PageItem(Page *page, QGraphicsItem *parent = nullptr);
Page *page() const;
void updatePage();
```

### BaseBlockItem 类 (`include/graphics/items/BaseBlockItem.h`)

块图形项基类，所有块项都继承自此类。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_block` | `Block*` | 块对象 |

### TextBlockItem 类 (`include/graphics/items/TextBlockItem.h`)

文本块图形项，继承自 BaseBlockItem。

### ImageBlockItem 类 (`include/graphics/items/ImageBlockItem.h`)

图片块图形项，继承自 BaseBlockItem。

### TableBlockItem 类 (`include/graphics/items/TableBlockItem.h`)

表格块图形项，继承自 BaseBlockItem。

### CursorItem 类 (`include/graphics/items/CursorItem.h`)

光标图形项。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_position` | `QPointF` | 位置 |
| `m_height` | `qreal` | 高度 |
| `m_visible` | `bool` | 是否可见 |
| `m_blinkTimer` | `QTimer*` | 闪烁定时器 |

### SelectionItem 类 (`include/graphics/items/SelectionItem.h`)

选择图形项。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_rects` | `QList<QRectF>` | 选择矩形列表 |

### DocumentScene 类 (`include/graphics/scene/DocumentScene.h`)

文档场景，继承自 QGraphicsScene。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_document` | `Document*` | 文档 |
| `m_blockItems` | `QHash<Block*, BaseBlockItem*>` | 块项哈希表 |
| `m_pageItems` | `QList<PageItem*>` | 页项列表 |
| `m_cursorItem` | `CursorItem*` | 光标项 |
| `m_selectionItem` | `SelectionItem*` | 选择项 |

#### 公共方法
```cpp
explicit DocumentScene(QObject *parent = nullptr);

void setDocument(Document *document);
Document *document() const;

void rebuildFromDocument();
void updateCursor(const QPointF &pos, qreal height);
void updateSelection(const QList<QRectF> &rects);
void clearSelection();

// 页管理
void clearPages();
void addPage(Page *page);
Page *pageAt(const QPointF &scenePos) const;
```

#### 公共槽
```cpp
void onBlockAdded(int globalIndex);
void onBlockRemoved(int globalIndex);
void onLayoutChanged();
```

### DocumentView 类 (`include/graphics/view/DocumentView.h`)

文档视图，继承自 QGraphicsView。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_zoom` | `qreal` | 缩放比例 |
| `m_lastMousePos` | `QPoint` | 最后鼠标位置 |

#### 公共方法
```cpp
explicit DocumentView(QWidget *parent = nullptr);

void setScene(DocumentScene *scene);

qreal zoom() const;
void setZoom(qreal zoom);
void zoomIn();
void zoomOut();
void zoomToFit();
void updateMousePosition();
```

#### 信号
```cpp
void keyPressed(QKeyEvent *event);
void mousePressed(QMouseEvent *event);
void mouseMoved(QMouseEvent *event);
void mouseReleased(QMouseEvent *event);
void inputMethodReceived(QInputMethodEvent *event);
void mousePositionChanged(const QPointF &scenePos, const QPoint &viewPos);
void zoomChanged(qreal zoom);
```

#### 保护方法
```cpp
void keyPressEvent(QKeyEvent *event) override;
void keyReleaseEvent(QKeyEvent *event) override;
void mousePressEvent(QMouseEvent *event) override;
void mouseMoveEvent(QMouseEvent *event) override;
void mouseReleaseEvent(QMouseEvent *event) override;
void wheelEvent(QWheelEvent *event) override;
void inputMethodEvent(QInputMethodEvent *event) override;
void scrollContentsBy(int dx, int dy) override;
void resizeEvent(QResizeEvent *event) override;
```

### BlockItemFactory 类 (`include/graphics/factory/BlockItemFactory.h`)

块项工厂，用于创建块图形项。

#### 公共方法
```cpp
static BaseBlockItem *createItem(Block *block, QGraphicsItem *parent = nullptr);
```

---

## 编辑控制 (Edit Control Module)

### Cursor 类 (`include/editcontrol/cursor/Cursor.h`)

光标类，管理文档中的插入点位置和选择。

#### CursorPosition 结构体
```cpp
struct CursorPosition
{
    int blockIndex = -1;    ///< 块索引
    int offset = 0;         ///< 块内的字符偏移量
    
    bool operator==(const CursorPosition &other) const;
    bool operator!=(const CursorPosition &other) const;
};
```

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_document` | `Document*` | 关联的文档 |
| `m_position` | `CursorPosition` | 当前光标位置 |

#### 公共方法
```cpp
// ========== 位置查询方法 ==========
/**
 * @brief 获取当前光标位置
 * @return 当前光标位置结构体
 */
CursorPosition position() const;

/**
 * @brief 设置光标位置
 * @param blockIndex 块索引
 * @param offset 块内偏移量
 */
void setPosition(int blockIndex, int offset);

/**
 * @brief 设置光标位置
 * @param pos 光标位置结构体
 */
void setPosition(const CursorPosition &pos);

// ========== 光标移动方法 ==========
/**
 * @brief 向左移动光标
 * 移动到前一个字符位置
 */
void moveLeft();

/**
 * @brief 向右移动光标
 * 移动到后一个字符位置
 */
void moveRight();

/**
 * @brief 向上移动光标
 * 移动到上一行相同水平位置
 */
void moveUp();

/**
 * @brief 向下移动光标
 * 移动到下一行相同水平位置
 */
void moveDown();

/**
 * @brief 移动到行首
 */
void moveToStartOfLine();

/**
 * @brief 移动到行尾
 */
void moveToEndOfLine();

/**
 * @brief 移动到文档开头
 */
void moveToStartOfDocument();

/**
 * @brief 移动到文档结尾
 */
void moveToEndOfDocument();

// ========== 编辑操作方法（会创建相应命令）==========

/**
 * @brief 在光标位置插入文本
 * @param text 要插入的文本
 * @param style 文本的字符样式
 */
void insertText(const QString &text, const CharacterStyle &style);

/**
 * @brief 删除光标前一个字符
 */
void deletePreviousChar();

/**
 * @brief 删除光标后一个字符
 */
void deleteNextChar();
```

#### 信号
```cpp
/**
 * @brief 光标位置发生变化时发出的信号
 * @param pos 新的光标位置
 */
void positionChanged(const CursorPosition &pos);
```

### Selection 类 (`include/editcontrol/selection/Selection.h`)

选择类。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_start` | `int` | 起始位置 |
| `m_end` | `int` | 结束位置 |

#### 公共方法
```cpp
int start() const;
void setStart(int start);
int end() const;
void setEnd(int end);
bool isEmpty() const;
void clear();
bool contains(int pos) const;
```

### EditEventHandler 类 (`include/editcontrol/handlers/EditEventHandler.h`)

编辑事件处理器，将用户输入事件转换为具体的编辑操作。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_document` | `Document*` | 关联的文档 |
| `m_cursor` | `Cursor*` | 光标控制器 |
| `m_selection` | `Selection*` | 选择控制器 |

#### 公共方法
```cpp
/**
 * @brief 处理键盘按下事件
 * @param event 键盘事件对象
 * @return 是否处理了该事件
 */
bool handleKeyPress(QKeyEvent *event);

/**
 * @brief 处理鼠标按下事件
 * @param event 鼠标事件对象
 * @return 是否处理了该事件
 */
bool handleMousePress(QMouseEvent *event);

/**
 * @brief 处理鼠标移动事件
 * @param event 鼠标事件对象
 * @return 是否处理了该事件
 */
bool handleMouseMove(QMouseEvent *event);

/**
 * @brief 处理鼠标释放事件
 * @param event 鼠标事件对象
 * @return 是否处理了该事件
 */
bool handleMouseRelease(QMouseEvent *event);

/**
 * @brief 处理输入法事件
 * @param event 输入法事件对象
 * @return 是否处理了该事件
 */
bool handleInputMethod(QInputMethodEvent *event);
```

#### 实现要点
- 支持方向键移动光标
- 支持 Backspace/Delete 删除文本
- 支持普通字符输入
- 支持输入法输入（中文等）
- 通过 Undo/Redo 命令系统执行编辑操作

### FormatController 类 (`include/editcontrol/formatting/FormatController.h`)

格式化控制器，管理字符和段落样式的应用。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_document` | `Document*` | 关联的文档 |
| `m_selection` | `Selection*` | 选择控制器 |

#### 公共方法
```cpp
// 字符样式设置
void setBold(bool bold);
void setItalic(bool italic);
void setUnderline(bool underline);
void setFontFamily(const QString &family);
void setFontSize(int size);
void setTextColor(const QColor &color);

// 段落样式设置
void setAlignment(ParagraphAlignment align);
void setLineSpacing(int percent);
void setFirstLineIndent(qreal indent);
void setLeftIndent(qreal indent);

// 样式获取
CharacterStyle currentCharacterStyle() const;
ParagraphStyle currentParagraphStyle() const;
```

---

## 用户界面 (UI Module)

### MainWindow 类 (`include/ui/mainwindow/MainWindow.h`)

主窗口类，应用程序的主要用户界面。

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_document` | `Document*` | 当前文档 |
| `m_scene` | `DocumentScene*` | 文档场景 |
| `m_view` | `DocumentView*` | 文档视图 |
| `m_cursor` | `Cursor*` | 光标控制器 |
| `m_selection` | `Selection*` | 选择控制器 |
| `m_editEventHandler` | `EditEventHandler*` | 编辑事件处理器 |
| `m_formatController` | `FormatController*` | 格式控制器 |
| `m_styleManager` | `StyleManager*` | 样式管理器 |
| `m_ribbonBar` | `RibbonBar*` | Ribbon 栏 |
| `m_currentFile` | `QString` | 当前文件 |
| `m_isModified` | `bool` | 是否已修改 |
| `m_currentZoom` | `qreal` | 当前缩放 |
| `m_lastScenePos` | `QPointF` | 上次场景位置 |
| `m_lastViewPos` | `QPoint` | 上次视图位置 |
| `m_pageSetup` | `PageSetup` | 页面设置 |
| `m_currentCursorPos` | `CursorPosition` | 当前光标位置 |
| `m_debugConsoleDock` | `QDockWidget*` | 调试控制台停靠窗口 |
| `m_debugConsoleTextEdit` | `QPlainTextEdit*` | 调试控制台文本编辑器 |

#### 公共方法
```cpp
explicit MainWindow(QWidget *parent = nullptr);

Document *document() const;
void setDocument(Document *document);
```

#### 私有槽
```cpp
// 文件操作
void newDocument();
void openDocument();
bool saveDocument();
bool saveAsDocument();
void about();

// 编辑操作
void undo();
void redo();
void cut();
void copy();
void paste();

// 视图操作
void zoomIn();
void zoomOut();
void zoomToFit();
void pageSetup();

// 界面更新
void updateWindowTitle();
void updateUI();
void updateStatusBar(const QPointF &scenePos, const QPoint &viewPos);
void updateCursorPosition(const CursorPosition &pos);

// 语言切换
void switchToChinese();
void switchToEnglish();
```

#### 保护方法
```cpp
void closeEvent(QCloseEvent *event) override;
```

#### 私有方法
```cpp
void setupUi();
void createActions();
bool maybeSave();
void retranslateUi();
QPointF calculateCursorVisualPosition(const CursorPosition &pos);
void setupDebugConsole();
```

#### 成员变量
| 变量 | 类型 | 说明 |
|------|------|------|
| `m_document` | `Document*` | 文档 |
| `m_scene` | `DocumentScene*` | 场景 |
| `m_view` | `DocumentView*` | 视图 |
| `m_cursor` | `Cursor*` | 光标 |
| `m_selection` | `Selection*` | 选择 |
| `m_editEventHandler` | `EditEventHandler*` | 编辑事件处理器 |
| `m_formatController` | `FormatController*` | 格式化控制器 |
| `m_styleManager` | `StyleManager*` | 样式管理器 |
| `m_ribbonBar` | `RibbonBar*` | Ribbon 栏 |
| `m_currentFile` | `QString` | 当前文件 |
| `m_isModified` | `bool` | 是否已修改 |
| `m_currentZoom` | `qreal` | 当前缩放 |
| `m_lastScenePos` | `QPointF` | 最后场景位置 |
| `m_lastViewPos` | `QPoint` | 最后视图位置 |
| `m_pageSetup` | `PageSetup` | 页面设置 |

#### 公共方法
```cpp
explicit MainWindow(QWidget *parent = nullptr);

Document *document() const;
void setDocument(Document *document);
```

#### 私有槽
```cpp
void newDocument();
void openDocument();
bool saveDocument();
bool saveAsDocument();
void about();

void undo();
void redo();
void cut();
void copy();
void paste();

void zoomIn();
void zoomOut();
void zoomToFit();
void pageSetup();
void updateWindowTitle();
void updateUI();
void updateStatusBar(const QPointF &scenePos, const QPoint &viewPos);
```

#### 保护方法
```cpp
void closeEvent(QCloseEvent *event) override;
```

#### 私有方法
```cpp
void setupUi();
void createActions();
bool maybeSave();
```

### RibbonBar 类 (`include/ui/ribbon/RibbonBar.h`)

Ribbon 栏类，继承自 QTabWidget。

#### 公共方法
```cpp
explicit RibbonBar(QWidget *parent = nullptr);

int addTab(const QString &title);
QWidget *currentTabWidget() const;
void addGroup(const QString &title, QWidget *widget);
void addWidget(QWidget *widget);
void updateFromSelection();
```

#### 信号
```cpp
void fontChanged(const QString &family);
void sizeChanged(int size);
void boldChanged(bool bold);
void italicChanged(bool italic);
void underlineChanged(bool underline);
void alignmentChanged(ParagraphAlignment align);
```

### PageSetupDialog 类 (`include/ui/dialogs/PageSetupDialog.h`)

页面设置对话框。

#### PageSetup 结构体
```cpp
struct PageSetup {
    qreal pageWidth = 210.0;   // 毫米
    qreal pageHeight = 297.0;
    qreal marginLeft = 20.0;
    qreal marginRight = 20.0;
    qreal marginTop = 20.0;
    qreal marginBottom = 20.0;
    bool portrait = true;
};
```

#### PageSizePreset 枚举
```cpp
enum class PageSizePreset {
    A4,
    A3,
    A5,
    Letter,
    Legal,
    Tabloid,
    Custom
};
```

#### MarginPreset 枚举
```cpp
enum class MarginPreset {
    Normal,
    Narrow,
    Moderate,
    Wide,
    Custom
};
```

#### 公共方法
```cpp
explicit PageSetupDialog(QWidget *parent = nullptr);

static PageSetup getPageSetup(const PageSetup &initial, QWidget *parent = nullptr);
PageSetup selectedSetup() const;
```

#### 私有槽
```cpp
void onPageSizePresetChanged(int index);
void onOrientationChanged();
void onMarginPresetChanged(int index);
void onCustomSizeChanged();
void onCustomMarginChanged();
```

#### Private 内部类
```cpp
class Private {
public:
    QComboBox *pageSizeCombo = nullptr;
    QDoubleSpinBox *pageWidthSpin = nullptr;
    QDoubleSpinBox *pageHeightSpin = nullptr;
    QRadioButton *portraitRadio = nullptr;
    QRadioButton *landscapeRadio = nullptr;
    QComboBox *marginPresetCombo = nullptr;
    QDoubleSpinBox *marginLeftSpin = nullptr;
    QDoubleSpinBox *marginRightSpin = nullptr;
    QDoubleSpinBox *marginTopSpin = nullptr;
    QDoubleSpinBox *marginBottomSpin = nullptr;
    bool isSettingFromPreset = false;
};
```

---

## 输入输出 (IO Module)

### TxtImporter 类 (`include/io/importers/TxtImporter.h`)

文本导入器。

### XmlSerializer 类 (`include/io/serializers/XmlSerializer.h`)

XML 序列化器。

### PdfExporter 类 (`include/io/exporters/PdfExporter.h`)

PDF 导出器。

---

## 常量定义 (Constants)

### Constants 命名空间 (`include/core/utils/Constants.h`)

#### 页面尺寸相关常量
| 常量 | 值 | 说明 |
|------|-----|------|
| `PAGE_WIDTH` | 595.0 | A4 纸宽度（点） |
| `PAGE_HEIGHT` | 842.0 | A4 纸高度（点） |
| `PAGE_MARGIN` | 72.0 | 页边距（1 英寸 = 72 点） |

#### 界面布局相关常量
| 常量 | 值 | 说明 |
|------|-----|------|
| `RIBBON_BAR_HEIGHT` | 140 | RibbonBar 固定高度（像素） |
| `PAGE_TOP_SPACING` | 20 | 页面与 RibbonBar 之间的间距（像素） |

#### 字体相关常量
| 常量 | 值 | 说明 |
|------|-----|------|
| `DefaultFontFamily` | "Times New Roman" | 默认字体族 |
| `DefaultFontSize` | 12 | 默认字号 |

#### 缩放相关常量
| 常量 | 值 | 说明 |
|------|-----|------|
| `ZOOM_MIN` | 0.5 | 最小缩放 |
| `ZOOM_MAX` | 3.0 | 最大缩放 |
| `ZOOM_STEP` | 0.1 | 缩放步长 |

#### 光标相关常量
| 常量 | 值 | 说明 |
|------|-----|------|
| `CURSOR_BLINK_INTERVAL` | 500 | 光标闪烁间隔（毫秒） |
| `CURSOR_WIDTH` | 2.0 | 光标宽度 |

#### 选择相关常量
| 常量 | 值 | 说明 |
|------|-----|------|
| `SELECTION_COLOR_RGB` | 0xffadd8e6 | 选择颜色（浅蓝色） |

#### 单位转换相关常量
| 常量 | 值 | 说明 |
|------|-----|------|
| `MillimetersToPoints` | 2.83464567 | 毫米转点的系数 |

---

## 构建系统 (Build System)

### CMakeLists.txt

#### 项目配置
```cmake
cmake_minimum_required(VERSION 3.16)
project(QtWordEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

#### Qt6 查找
```cmake
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Gui PrintSupport LinguistTools)
```

#### 自动 MOC/UIC/RCC
```cmake
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
```

#### 静态库创建
```cmake
add_library(QtWordEditorCore STATIC ${CORE_SOURCES} ${CORE_HEADERS})
add_library(QtWordEditorEditControl STATIC ${EDITCONTROL_SOURCES} ${EDITCONTROL_HEADERS})
add_library(QtWordEditorGraphics STATIC ${GRAPHICS_SOURCES} ${GRAPHICS_HEADERS})
add_library(QtWordEditorIO STATIC ${IO_SOURCES} ${IO_HEADERS})
add_library(QtWordEditorUI STATIC ${UI_SOURCES} ${UI_HEADERS})
```

#### 可执行文件创建
```cmake
add_executable(QtWordEditor ${APP_SOURCES} ${APP_HEADERS})
```

### CMakePresets.json

#### Debug 预设
```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "Debug",
            "displayName": "Debug",
            "description": "Debug build using MinGW and Ninja",
            "binaryDir": "${sourceDir}/cmake-build-debug",
            "generator": "Ninja",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_C_COMPILER": "gcc",
                "CMAKE_CXX_COMPILER": "g++"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "Debug",
            "configurePreset": "Debug",
            "jobs": 6
        }
    ]
}
```

## 核心技术实现细节

### 文本布局系统

QtWordEditor 使用 Qt 的 QTextLayout 系统实现精确的文本布局：

#### QTextLayout 在项目中的应用

1. **光标位置计算**
```cpp
// DocumentScene::calculateCursorVisualPosition()
QTextCursor cursor(doc);
cursor.setPosition(charOffset);

QTextBlock block = cursor.block();
QTextLayout *layout = block.layout();

// 计算光标在行中的X坐标
qreal x = line.cursorToX(positionInBlock);
// 获取行的Y坐标
qreal y = line.y();
```

2. **点击定位**
```cpp
// DocumentScene::cursorPositionAt()
int offset = textItem->document()->documentLayout()->hitTest(
    localPos, Qt::FuzzyHit);
```

3. **块高度计算**
```cpp
// LayoutEngine::calculateBlockHeight()
QTextLayout textLayout(text);
textLayout.setAdditionalFormats(formatRanges);
textLayout.setTextOption(option);

textLayout.beginLayout();
qreal y = 0;
while (true) {
    QTextLine line = textLayout.createLine();
    if (!line.isValid())
        break;
    line.setLineWidth(maxWidth - leftIndent - rightIndent);
    y += line.height() + style.lineSpacing();
}
textLayout.endLayout();
```

### 坐标系统转换

项目实现了5层坐标系统的精确转换：

```
屏幕坐标 → 视图坐标 → 场景坐标 → 页面坐标 → 块坐标
```

#### 关键转换方法
```cpp
// DocumentView 中的坐标转换
QPointF scenePos = mapToScene(viewPos);  // 视图→场景

// DocumentScene 中的位置计算
Page *page = pageAt(scenePos);           // 找到所在页面
QPointF relativePos = scenePos - pageItem->pos();  // 页面相对坐标
```

### Undo/Redo 系统

基于 Qt 的 QUndoStack 实现完整的撤销重做功能：

#### 命令执行流程
```
用户操作 → Cursor 方法 → 创建 Command → Push 到 UndoStack → 执行 redo()

撤销: UndoStack::undo() → 执行 undo() 方法
重做: UndoStack::redo() → 执行 redo() 方法
```

#### 命令合并优化
```cpp
// 连续的字符输入可以合并为一个命令
void InsertTextCommand::mergeWith(const QUndoCommand *command)
{
    const InsertTextCommand *other = static_cast<const InsertTextCommand*>(command);
    if (m_blockIndex == other->m_blockIndex && 
        m_offset + m_text.length() == other->m_offset) {
        m_text += other->m_text;
        return true;
    }
    return false;
}
```

### 样式系统

支持字符样式和段落样式的完整实现：

#### 字符样式继承
```cpp
// CharacterStyle 使用隐式共享（copy-on-write）
class CharacterStyle {
private:
    struct CharacterStyleData : public QSharedData {
        QFont font;
        QColor textColor;
        QColor backgroundColor;
        // ... 其他属性
    };
    QSharedDataPointer<CharacterStyleData> d;
};
```

#### 样式应用优先级
1. 直接字符样式（最高优先级）
2. 段落默认样式
3. 文档默认样式
4. 系统默认样式（最低优先级）

### 性能优化策略

1. **增量布局**：只重新计算受影响的块
2. **缓存机制**：缓存块高度和位置信息
3. **懒加载**：延迟创建图形项直到需要显示
4. **批量更新**：合并多个小更新为一次大更新

项目使用命令模式实现撤销/重做功能。

### 命令基类 (`include/core/commands/EditCommand.h`)

所有命令继承自 QUndoCommand，实现撤销/重做功能。

#### 命令层次结构
```
QUndoCommand
├── InsertBlockCommand    - 插入块命令
├── RemoveBlockCommand    - 删除块命令
├── InsertTextCommand     - 插入文本命令
├── RemoveTextCommand     - 删除文本命令
├── SetCharacterStyleCommand - 设置字符样式命令
└── SetParagraphStyleCommand - 设置段落样式命令
```

#### InsertTextCommand 示例
```cpp
class InsertTextCommand : public QUndoCommand
{
public:
    InsertTextCommand(Document *document, int blockIndex, int offset,
                     const QString &text, const CharacterStyle &style);
    
    void undo() override;
    void redo() override;
    
private:
    Document *m_document;
    int m_blockIndex;
    int m_offset;
    QString m_text;
    CharacterStyle m_style;
};
```

#### 使用方式
```cpp
// 在 Cursor::insertText() 中
QUndoStack *stack = m_document->undoStack();
if (stack) {
    InsertTextCommand *cmd = new InsertTextCommand(
        m_document, m_position.blockIndex, m_position.offset, text, style);
    stack->push(cmd);
    // 更新光标位置
    m_position.offset += text.length();
    emit positionChanged(m_position);
}
```

### 具体命令
- `InsertBlockCommand` - 插入块
- `RemoveBlockCommand` - 删除块
- `InsertTextCommand` - 插入文本
- `RemoveTextCommand` - 删除文本
- `SetCharacterStyleCommand` - 设置字符样式
- `SetParagraphStyleCommand` - 设置段落样式

---

## 状态栏显示

### 状态栏信息
状态栏实时显示以下信息：

```
光标: 块0, 偏移5  |  缩放: 100%  |  页码: 1  |  场景坐标: (100.50, 200.30)  |  相对坐标: (100.50, 200.30)  |  视图坐标: (150, 250)
```

| 信息 | 说明 |
|------|------|
| 光标 | 当前光标位置（块索引和偏移量） |
| 缩放 | 当前缩放百分比 |
| 页码 | 当前鼠标所在页码 |
| 场景坐标 | 相对于整个场景的绝对坐标 |
| 相对坐标 | 相对于当前页面的坐标 |
| 视图坐标 | 相对于视图控件的坐标 |

#### 实现方式
```cpp
void MainWindow::updateStatusBar(const QPointF &scenePos, const QPoint &viewPos)
{
    Page *page = m_scene->pageAt(scenePos);
    
    QString cursorInfo = QString("光标: 块%1, 偏移%2")
        .arg(m_currentCursorPos.blockIndex)
        .arg(m_currentCursorPos.offset);
    
    if (page) {
        // 计算相对坐标
        qreal pageSpacing = 30.0;
        qreal pageHeight = Constants::PAGE_HEIGHT;
        int pageIndex = page->pageNumber() - 1;
        qreal yOffset = pageIndex * (pageHeight + pageSpacing);
        
        qreal relativeX = scenePos.x();
        qreal relativeY = scenePos.y() - yOffset;
        
        QString statusText = QString("%1  |  缩放: %2%  |  页码: %3  |  场景坐标: (%4, %5)  |  相对坐标: (%6, %7)  |  视图坐标: (%8, %9)")
            .arg(cursorInfo)
            .arg(m_currentZoom, 0, 'f', 0)
            .arg(page->pageNumber())
            .arg(scenePos.x(), 0, 'f', 2)
            .arg(scenePos.y(), 0, 'f', 2)
            .arg(relativeX, 0, 'f', 2)
            .arg(relativeY, 0, 'f', 2)
            .arg(viewPos.x())
            .arg(viewPos.y());
        statusBar()->showMessage(statusText);
    }
}
```

---

## 技术架构总结

### 系统架构图

```
┌─────────────────────────────────────────────────────────┐
│                    用户界面层                            │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐    │
│  │ MainWindow  │  │ RibbonBar   │  │ Dialogs     │    │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘    │
└─────────┼─────────────────┼─────────────────┼───────────┘
          │                 │                 │
          ▼                 ▼                 ▼
┌─────────────────────────────────────────────────────────┐
│                    控制层                                │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │ EditEventHandler│  │ FormatController│              │
│  └────────┬────────┘  └────────┬────────┘              │
│           │                    │                         │
│           ▼                    ▼                         │
│  ┌──────────────────────────────────────┐              │
│  │              Cursor                  │              │
│  └──────────────────────────────────────┘              │
└─────────────────────┼───────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────┐
│                    模型层                                │
│  ┌──────────────────────────────────────────────────┐   │
│  │                    Document                      │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────────────┐  │   │
│  │  │Section 1│  │Section 2│  │ Undo/Redo Stack │  │   │
│  │  └────┬────┘  └────┬────┘  └─────────────────┘  │   │
│  │       │            │                             │   │
│  │  ┌────▼────┐  ┌────▼────┐                        │   │
│  │  │ Block 1 │  │ Block 2 │                        │   │
│  │  └─────────┘  └─────────┘                        │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────┼───────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────┐
│                    布局层                                │
│  ┌──────────────────────────────────────────────────┐   │
│  │                 LayoutEngine                     │   │
│  │  - calculateBlockHeight()                        │   │
│  │  - 分页算法                                       │   │
│  │  - 坐标计算                                       │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────┼───────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────┐
│                    渲染层                                │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │ DocumentScene   │  │ DocumentView    │              │
│  │ (QGraphicsScene)│  │ (QGraphicsView) │              │
│  └──────┬──────────┘  └────────┬────────┘              │
│         │                      │                         │
│  ┌──────▼──────────────────────▼──────┐                 │
│  │         Graphics Items              │                 │
│  │  - PageItem                         │                 │
│  │  - TextBlockItem                    │                 │
│  │  - CursorItem                       │                 │
│  │  - SelectionItem                    │                 │
│  └─────────────────────────────────────┘                 │
└─────────────────────────────────────────────────────────┘
```

### 数据流向

1. **输入处理流程**：
```
用户输入 → DocumentView → EditEventHandler → Cursor → Command → Document
     ↑                                                    ↓
     └────────────── 视图更新 ← LayoutEngine ←─────────────┘
```

2. **显示更新流程**：
```
Document 变化 → LayoutEngine 重新布局 → DocumentScene 重建 → Graphics Items 更新
```

### 关键设计模式

1. **命令模式**：实现 Undo/Redo 功能
2. **观察者模式**：信号槽机制实现组件间通信
3. **工厂模式**：BlockItemFactory 创建不同类型块项
4. **单例模式**：Application 类管理全局状态
5. **享元模式**：CharacterStyle 使用隐式共享减少内存占用

### 性能优化要点

1. **增量更新**：只重新计算和渲染变化的部分
2. **对象池**：重用 QGraphicsItem 对象
3. **延迟加载**：只在需要时创建图形项
4. **批处理**：合并多个小操作为一次大更新
5. **空间分区**：使用 Qt Graphics View 的内置优化

---

## 开发指南

### 代码规范

1. **命名约定**：
   - 类名：PascalCase（如 `DocumentScene`）
   - 方法名：camelCase（如 `calculateHeight`）
   - 成员变量：m_前缀 + camelCase（如 `m_document`）
   - 常量：UPPER_SNAKE_CASE（如 `PAGE_WIDTH`）

2. **注释规范**：
   - 使用 Doxygen 风格注释
   - 所有公共接口必须有详细注释
   - 复杂算法需要添加实现说明

3. **内存管理**：
   - 使用 Qt 的父子对象系统自动管理内存
   - 避免手动 delete，使用 deleteLater()
   - 注意循环引用问题

### 扩展建议

1. **添加新块类型**：
   - 继承 Block 基类
   - 实现对应的 Graphics Item
   - 在 BlockItemFactory 中注册

2. **添加新命令**：
   - 继承 QUndoCommand
   - 实现 undo() 和 redo() 方法
   - 在适当位置创建并执行命令

3. **性能优化**：
   - 使用 profiler 分析热点
   - 考虑多线程布局计算
   - 实现更智能的缓存策略

## 总结

QtWordEditor 是一个功能完善的文字处理软件，具有以下特点：

1. **模块化架构**：清晰的模块划分，易于维护和扩展
2. **文档模型**：完整的文档层级结构（Document → Section → Block → Span）
3. **样式系统**：支持字符样式和段落样式
4. **图形渲染**：基于 Qt Graphics Framework 的高性能渲染
5. **编辑功能**：支持撤销/重做、光标、选择等
6. **用户界面**：现代化的 Ribbon 风格界面
7. **页面设置**：支持多种页面尺寸、边距预设和方向
8. **实时反馈**：状态栏实时显示缩放、页码和坐标信息

项目使用 CMake 构建系统，支持跨平台编译，并提供了完整的翻译支持。
