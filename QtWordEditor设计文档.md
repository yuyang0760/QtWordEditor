# QtWordEditor 文字处理器完整设计文档

## 1\. 系统概述

### 1.1 项目目标

开发一个功能类似 Microsoft Word 的跨平台文字处理器，基于 Qt 6 框架和 C++17 标准。支持多页文档、富文本编辑、段落样式、图片/表格插入、撤销/重做、文件保存/加载等核心功能。

### 1.2 设计原则

-   **模块化**：将系统划分为独立模块（文档模型、视图、编辑控制、IO、UI），降低耦合。
    
-   **可扩展性**：通过工厂模式、插件机制支持未来新增内容类型（图表、公式等）。
    
-   **高性能**：支持大文档（数百页）的流畅编辑，采用增量渲染和缓存技术。
    
-   **跨平台**：充分利用 Qt 的跨平台特性，确保在 Windows、macOS、Linux 上行为一致。
    
-   **用户友好**：提供直观的 Ribbon 风格界面（可选），支持自定义主题。
    

### 1.3 技术栈

-   **Qt 6.5+**：核心框架（Core、Gui、Widgets、PrintSupport、OpenGL 等）。
    
-   **C++17**：现代 C++ 特性（智能指针、lambda、optional 等）。
    
-   **CMake**：构建系统。
    
-   **Google Test**：单元测试（可选）。
    
-   **SQlite**：用于缓存或元数据存储（可选）。
    

---

## 2\. 系统架构

### 2.1 分层架构

采用经典的三层架构：

-   **表示层（UI）**：主窗口、工具栏、状态栏、对话框等，负责与用户交互。
    
-   **业务逻辑层（Core）**：文档模型、编辑命令、样式管理、布局引擎等，处理核心编辑功能。
    
-   **数据层（IO）**：文件读写、序列化、导入/导出过滤器。
    

### 2.2 模块划分

```

QtWordEditor/
├── app/                # 应用程序入口、全局对象
├── core/               # 核心业务逻辑
│   ├── document/       # 文档模型（Document, Section, Block, Span, Style）
│   ├── commands/       # 撤销/重做命令基类及具体实现
│   ├── layout/         # 布局引擎（LayoutEngine, PageBuilder）
│   ├── styles/         # 样式管理（样式集、样式继承）
│   └── utils/          # 工具类（字体转换、度量计算）
├── editcontrol/        # 编辑控制
│   ├── cursor/         # 光标管理
│   ├── selection/      # 选区管理
│   ├── handlers/       # 事件处理器（键盘、鼠标、输入法）
│   └── formatting/     # 格式控制器（应用字符/段落样式）
├── graphics/           # 图形视图
│   ├── scene/          # 文档场景
│   ├── view/           # 文档视图
│   ├── items/          # 图形项基类及具体实现（页面、块、光标、选区）
│   └── factory/        # 图形项工厂
├── io/                  # 输入输出
│   ├── serializers/     # 序列化器（XML、二进制）
│   ├── exporters/       # 导出器（PDF、HTML、纯文本）
│   └── importers/       # 导入器（DOCX、ODT、TXT）
├── ui/                  # 用户界面
│   ├── mainwindow/      # 主窗口
│   ├── ribbon/          # 功能区（类似 Word Ribbon）
│   ├── dialogs/         # 对话框（字体、段落、页面设置）
│   └── widgets/         # 自定义控件（格式工具栏、标尺）
├── plugins/             # 插件接口（可选）
└── resources/           # 资源文件（图标、样式表、翻译）

```

---

## 3\. 核心类设计

### 3.1 文档模型（document 模块）

#### 3.1.1 类图

```

Document (1) ——— (n) Section
Section (1) ——— (n) Block (abstract)
Block ——— ParagraphBlock
Block ——— TableBlock
Block ——— ImageBlock
ParagraphBlock ——— (n) Span
Span ——— (1) CharacterStyle
ParagraphBlock ——— (1) ParagraphStyle
Section ——— (n) Page (运行时由布局引擎生成)

```

#### 3.1.2 主要类职责

-   **Document**：文档根容器，包含多个节（Section），管理文档元数据（标题、作者、创建时间等），持有撤销栈（QUndoStack）。
    
-   **Section**：节，代表文档的一个逻辑分区（如章节），包含块列表和页面列表（由布局引擎填充）。
    
-   **Block**：块基类，所有内容块的抽象。子类包括：
    
    -   **ParagraphBlock**：段落块，包含 Span 列表，持有段落样式。
        
    -   **TableBlock**：表格块，包含行、列、单元格。
        
    -   **ImageBlock**：图片块，包含图片数据、缩放信息。
    
-   **Span**：文本片段，包含文本内容和字符样式。
    
-   **CharacterStyle**：字符样式（字体、字号、颜色、粗体/斜体/下划线等）。
    
-   **ParagraphStyle**：段落样式（对齐、缩进、行距、段前/段后间距等）。
    

#### 3.1.3 关键设计

-   所有模型类使用隐式共享（QSharedData）以提高复制效率。
    
-   块和 span 支持增量修改，通过发出信号通知视图更新。
    
-   文档通过 QUndoStack 管理所有编辑命令，实现撤销/重做。
    

### 3.2 编辑命令（commands 模块）

#### 3.2.1 命令基类

```

EditCommand : public QUndoCommand
  - m_document : Document*
  - virtual void redo() = 0;
  - virtual void undo() = 0;

```

#### 3.2.2 具体命令

-   **InsertTextCommand**：插入文本。
    
-   **RemoveTextCommand**：删除文本。
    
-   **InsertBlockCommand**：插入块。
    
-   **RemoveBlockCommand**：删除块。
    
-   **SetCharacterStyleCommand**：设置字符样式。
    
-   **SetParagraphStyleCommand**：设置段落样式。
    

#### 3.2.3 设计要点

-   命令内部保存足够的状态以实现撤销/重做。
    
-   命令执行后自动更新文档并触发相关信号。
    
-   命令可以合并（如连续键入合并为一个命令），通过 `mergeWith` 实现。
    

### 3.3 布局引擎（layout 模块）

#### 3.3.1 类设计

-   **LayoutEngine**：负责将文档内容分页，计算每个块的位置和大小。
    
-   **PageBuilder**：辅助类，将块分配到页面，处理跨页断行。
    

#### 3.3.2 工作原理

1.  遍历所有节和块。
    
2.  对于每个段落块，测量其高度（根据字体、行距、宽度）。
    
3.  将块放入当前页面，如果放不下则新建页面。
    
4.  记录每个块在页面中的位置（相对于页面原点的坐标）。
    
5.  布局完成后更新每个块的 boundingRect，并发出 `layoutChanged` 信号。
    

#### 3.3.3 性能优化

-   增量布局：只重新布局受影响的块。
    
-   缓存块高度：对于未变化的块，直接使用缓存值。
    
-   多线程布局（可选）：将不同节的布局任务分配到线程池。
    

### 3.4 编辑控制（editcontrol 模块）

#### 3.4.1 光标管理（Cursor）

-   **CursorPosition**：包含块索引和块内偏移。
    
-   **Cursor**：持有当前位置，提供移动、插入、删除等操作（实际调用命令）。
    
-   **VisualCursor**：管理光标的屏幕位置，由视图更新。
    

#### 3.4.2 选区管理（Selection）

-   **SelectionRange**：包含起始和结束的块索引与偏移。
    
-   **Selection**：持有选区范围，提供扩展、收缩、清空等方法。
    
-   支持多段选区（多个不相邻的选区），用 `QList<SelectionRange>` 表示。
    

#### 3.4.3 事件处理器（EditEventHandler）

-   接收来自视图的事件（键盘、鼠标、输入法）。
    
-   根据当前光标/选区状态，调用相应的命令或更新光标/选区。
    
-   不直接修改视图，只修改模型，视图通过监听模型信号更新。
    

#### 3.4.4 格式控制器（FormatController）

-   提供应用字符样式和段落样式的方法。
    
-   内部遍历选区或整个文档，创建样式命令并推入撤销栈。
    

### 3.5 图形视图（graphics 模块）

采用之前重构的设计，核心类包括：

-   **DocumentScene**：图形项容器，维护块到图形项的映射，响应模型信号增量更新。
    
-   **DocumentView**：视图控件，处理缩放和事件转发。
    
-   **PageItem**：页面背景容器，自动管理子块项。
    
-   **TextBlockItem**：使用 QGraphicsTextItem 绘制富文本。
    
-   **CursorItem**：QGraphicsLineItem 实现闪烁光标。
    
-   **SelectionItem**：支持多矩形绘制。
    
-   **BlockItemFactory**：根据块类型创建对应图形项。
    

### 3.6 IO 模块

#### 3.6.1 序列化器

-   **XmlSerializer**：将文档保存为自定义 XML 格式（.qtdoc）。
    
-   **BinarySerializer**（可选）：紧凑的二进制格式，提高读写速度。
    

#### 3.6.2 导出器

-   **PdfExporter**：使用 QPrinter 将文档渲染为 PDF。
    
-   **HtmlExporter**：将文档转换为 HTML，保留基本格式。
    
-   **PlainTextExporter**：导出纯文本。
    

#### 3.6.3 导入器

-   **DocxImporter**：解析 Office Open XML 格式（需处理 ZIP 和 XML）。
    
-   **OdtImporter**：解析 OpenDocument 格式。
    
-   **TxtImporter**：导入纯文本，自动检测编码。
    

### 3.7 UI 模块

#### 3.7.1 主窗口（MainWindow）

-   包含菜单栏、Ribbon 工具栏、状态栏。
    
-   持有 DocumentView 和 DocumentScene。
    
-   管理文档打开/保存、缩放、打印等操作。
    
-   监听文档修改状态，更新标题栏的“\*”标记。
    

#### 3.7.2 格式工具栏（FormatToolBar）

-   提供字体选择、字号、粗体/斜体/下划线、对齐方式等控件。
    
-   直接与 FormatController 交互。
    

#### 3.7.3 标尺（Ruler）

-   显示页边距、段落缩进、制表位。
    
-   支持拖动修改缩进和制表位，实时更新段落样式。
    

#### 3.7.4 对话框

-   **FontDialog**：封装 QFontDialog，可设置字符样式。
    
-   **ParagraphDialog**：设置段落缩进、间距、对齐。
    
-   **PageSetupDialog**：设置页面尺寸、边距、纸张方向。
    
-   **InsertImageDialog**：选择图片并插入。
    

---

## 4\. 数据流与事件处理

### 4.1 编辑操作流程（以键入字符为例）

1.  用户在 DocumentView 中按下键盘，视图发射 `keyPressed` 信号。
    
2.  MainWindow 接收到信号，转发给 EditEventHandler。
    
3.  EditEventHandler 根据当前光标位置，创建 `InsertTextCommand` 并推入文档的撤销栈。
    
4.  命令执行：向文档的指定块插入文本，并发出 `blockChanged` 信号。
    
5.  DocumentScene 监听到 `blockChanged` 信号，找到对应的 TextBlockItem 并调用 `updateBlock()`。
    
6.  TextBlockItem 更新其内部的 QGraphicsTextItem 内容。
    
7.  光标位置变化后，Cursor 发出 `positionChanged` 信号。
    
8.  MainWindow 计算光标新位置的世界坐标，调用 DocumentScene::updateCursor。
    
9.  场景更新光标项的位置，视图自动重绘。
    

### 4.2 文件打开流程

1.  用户选择“打开”，MainWindow 调用 DocumentIO::load。
    
2.  DocumentIO 根据文件后缀选择合适的导入器。
    
3.  导入器解析文件，构建 Document 对象。
    
4.  新文档设置为主窗口的当前文档。
    
5.  调用 DocumentScene::rebuildFromDocument 构建场景。
    
6.  布局引擎重新布局文档（如有必要）。
    
7.  状态栏显示“打开成功”。
    

---

## 5\. 扩展性设计

### 5.1 支持新内容类型

-   在 `Block` 基类下派生子类，如 `EquationBlock`、`ChartBlock`。
    
-   在 `BlockItemFactory` 中添加对应的创建分支。
    
-   在 UI 中添加插入该类型块的动作。
    
-   在序列化器中添加该块的序列化代码。
    

### 5.2 支持新文件格式

-   实现新的导入器/导出器类，继承自 `Importer`/`Exporter` 基类。
    
-   在 DocumentIO 中根据扩展名动态选择。
    

### 5.3 插件系统（高级）

-   定义插件接口，允许第三方开发扩展。
    
-   使用 Qt 的插件机制（QPluginLoader）加载动态库。
    
-   插件可以注册新的块类型、导入器/导出器、UI 组件等。
    

---

## 6\. 性能优化策略

### 6.1 文档模型优化

-   使用 QSharedData 实现写时复制，减少大文档复制开销。
    
-   段落块中的 Span 列表使用 QVector，预分配内存。
    

### 6.2 布局引擎优化

-   缓存块高度，避免重复测量。
    
-   增量布局：只重新布局受编辑影响的块，使用脏区域标记。
    
-   懒加载：只布局当前视图可见的页面。
    

### 6.3 图形渲染优化

-   使用 QGraphicsView 的缓存模式（ItemCoordinateCache）。
    
-   对静态背景（如页面阴影）使用 QPixmap 缓存。
    
-   文本块使用 QStaticText 或 QTextDocument 的缓存。
    

### 6.4 内存管理

-   利用 Qt 对象树自动释放图形项。
    
-   对大型图片使用异步加载和缩略图。
    

---

## 7\. 技术选型说明

-   **Qt 6**：提供强大的跨平台 GUI 和图形视图框架，内置 Unicode 支持、打印、OpenGL 加速。
    
-   **C++17**：使用标准库智能指针、可选值等，提升代码安全性。
    
-   **CMake**：跨平台构建，易于集成第三方库。
    
-   **Google Test**：单元测试框架，保证核心逻辑正确性。
    
-   **libzip**（可选）：用于处理 DOCX 等 ZIP 压缩格式。
    
-   **poppler**（可选）：用于 PDF 导入。
    

---

## 8\. 后续开发计划

### 第一阶段：核心框架（1-2个月）

-   完成文档模型、基本编辑命令（插入/删除文本）。
    
-   实现简单的布局引擎（单页、固定高度）。
    
-   实现基本图形视图（显示文本块）。
    
-   实现 XML 序列化。
    
-   完成主窗口和基本菜单。
    

### 第二阶段：富文本与样式（1个月）

-   实现字符样式（字体、颜色、粗体等）。
    
-   实现段落样式（对齐、缩进、行距）。
    
-   添加格式工具栏。
    
-   实现撤销/重做栈的集成。
    

### 第三阶段：高级功能（2个月）

-   支持图片插入。
    
-   支持表格。
    
-   实现多页布局和分页。
    
-   实现标尺和制表位。
    
-   实现 PDF 导出。
    

### 第四阶段：完善与优化（1个月）

-   实现 DOCX 导入/导出。
    
-   性能调优（大文档测试）。
    
-   国际化（多语言翻译）。
    
-   错误处理和用户提示。
    

---

## 9\. 目录结构（最终版）

```

QtWordEditor/
├── CMakeLists.txt
├── README.md
├── app/
│   ├── Application.cpp
│   ├── Application.h
│   ├── Global.h
│   └── main.cpp
├── core/
│   ├── CMakeLists.txt
│   ├── document/
│   │   ├── Block.cpp
│   │   ├── Block.h
│   │   ├── CharacterStyle.cpp
│   │   ├── CharacterStyle.h
│   │   ├── Document.cpp
│   │   ├── Document.h
│   │   ├── ImageBlock.cpp
│   │   ├── ImageBlock.h
│   │   ├── ParagraphBlock.cpp
│   │   ├── ParagraphBlock.h
│   │   ├── ParagraphStyle.cpp
│   │   ├── ParagraphStyle.h
│   │   ├── Section.cpp
│   │   ├── Section.h
│   │   ├── Span.cpp
│   │   ├── Span.h
│   │   ├── TableBlock.cpp
│   │   └── TableBlock.h
│   ├── commands/
│   │   ├── EditCommand.h
│   │   ├── InsertTextCommand.cpp
│   │   ├── InsertTextCommand.h
│   │   ├── RemoveTextCommand.cpp
│   │   ├── RemoveTextCommand.h
│   │   ├── SetStyleCommand.cpp
│   │   └── SetStyleCommand.h
│   ├── layout/
│   │   ├── LayoutEngine.cpp
│   │   ├── LayoutEngine.h
│   │   ├── PageBuilder.cpp
│   │   └── PageBuilder.h
│   ├── styles/
│   │   ├── StyleManager.cpp
│   │   └── StyleManager.h
│   └── utils/
│       ├── Constants.h
│       ├── FontUtils.cpp
│       └── FontUtils.h
├── editcontrol/
│   ├── cursor/
│   │   ├── Cursor.cpp
│   │   └── Cursor.h
│   ├── selection/
│   │   ├── Selection.cpp
│   │   └── Selection.h
│   ├── handlers/
│   │   ├── EditEventHandler.cpp
│   │   └── EditEventHandler.h
│   └── formatting/
│       ├── FormatController.cpp
│       └── FormatController.h
├── graphics/
│   ├── scene/
│   │   ├── DocumentScene.cpp
│   │   └── DocumentScene.h
│   ├── view/
│   │   ├── DocumentView.cpp
│   │   └── DocumentView.h
│   ├── items/
│   │   ├── BaseBlockItem.cpp
│   │   ├── BaseBlockItem.h
│   │   ├── CursorItem.cpp
│   │   ├── CursorItem.h
│   │   ├── PageItem.cpp
│   │   ├── PageItem.h
│   │   ├── SelectionItem.cpp
│   │   ├── SelectionItem.h
│   │   ├── TextBlockItem.cpp
│   │   ├── TextBlockItem.h
│   │   ├── ImageBlockItem.cpp
│   │   ├── ImageBlockItem.h
│   │   ├── TableBlockItem.cpp
│   │   └── TableBlockItem.h
│   └── factory/
│       ├── BlockItemFactory.cpp
│       └── BlockItemFactory.h
├── io/
│   ├── DocumentIO.cpp
│   ├── DocumentIO.h
│   ├── serializers/
│   │   ├── XmlSerializer.cpp
│   │   └── XmlSerializer.h
│   ├── exporters/
│   │   ├── PdfExporter.cpp
│   │   ├── PdfExporter.h
│   │   ├── HtmlExporter.cpp
│   │   └── HtmlExporter.h
│   └── importers/
│       ├── DocxImporter.cpp
│       ├── DocxImporter.h
│       ├── TxtImporter.cpp
│       └── TxtImporter.h
├── ui/
│   ├── mainwindow/
│   │   ├── MainWindow.cpp
│   │   └── MainWindow.h
│   ├── ribbon/
│   │   ├── RibbonBar.cpp
│   │   └── RibbonBar.h
│   ├── dialogs/
│   │   ├── FontDialog.cpp
│   │   ├── FontDialog.h
│   │   ├── ParagraphDialog.cpp
│   │   ├── ParagraphDialog.h
│   │   ├── PageSetupDialog.cpp
│   │   └── PageSetupDialog.h
│   └── widgets/
│       ├── FormatToolBar.cpp
│       ├── FormatToolBar.h
│       ├── Ruler.cpp
│       └── Ruler.h
├── plugins/             # 插件目录
├── resources/
│   ├── icons/
│   ├── styles/
│   │   └── default.qss
│   └── translations/
└── tests/
    ├── core/
    ├── editcontrol/
    └── graphics/

```

---

## 10\. 总结

本设计文档提供了一个完整、可扩展的 Qt Word 文字处理器架构。通过分层模块、职责清晰的类设计、灵活的扩展机制和性能优化策略，能够满足类似 Word 的复杂功能需求，并为未来迭代奠定坚实基础。后续开发可按计划分阶段推进，确保项目稳步前进。

---


*由 [NousSave Ai Chat Exporter](https://www.noussave.com) 生成 | deepseek | 2026/2/17*
