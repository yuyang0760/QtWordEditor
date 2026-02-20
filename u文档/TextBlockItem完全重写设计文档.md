---
title: "TextBlockItem 完全重写详细设计文档"
date: "2026-02-20"
---

# TextBlockItem 完全重写详细设计文档

## 1. 设计概述

### 1.1 目标
完全重写 `TextBlockItem`，抛弃现有的 `QGraphicsTextItem`，自己实现整个文本和对象布局系统，实现像 Word 一样专业的排版效果，支持内联公式的完美嵌入。

### 1.2 核心原则
1. **完全继承 QGraphicsItem**：不继承 QGraphicsRectItem，直接继承 QGraphicsItem
2. **自主布局引擎**：自己实现文本测量、换行、对齐
3. **基线对齐**：专业排版的核心，文本和对象都按基线对齐
4. **模块化设计**：每个功能都独立成类，便于维护和扩展
5. **与现有架构兼容**：保持与项目现有架构的无缝集成

---

## 2. 整体架构设计

### 2.1 架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                    DocumentScene (场景)                   │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │           PageItem (页面)               │
        └─────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│           BaseBlockItem (基类，需修改)                 │
│  (继承自 QGraphicsItem，不再继承 QGraphicsRectItem)        │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│              TextBlockItem (完全重写)                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │           TextBlockLayoutEngine (布局引擎)          │  │
│  │  - 文本测量                         │  │
│  │  - 文本换行                         │  │
│  │  - 文本对齐                         │  │
│  │  - 基线对齐                         │  │
│  │  - 对象定位                         │  │
│  └─────────────────────────────────────────────────────────┘  │
│                              │                          │
│  ┌───────────────────────────┼───────────────────────┐  │
│  │                           │                       │  │
│  ▼                           ▼                       ▼  │
│  ┌──────────────┐  ┌──────────────────┐  ┌──────────────┐ │
│  │ TextFragment │  │MathFormula-      │  │ TextFragment │ │
│  │  (文本片段1)  │  │GraphicsItem1     │  │  (文本片段2)  │ │
│  │              │  │  (内联公式1)      │  │              │ │
│  └──────────────┘  └──────────────────┘  └──────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 继承链（修改后）

```
QGraphicsItem (Qt 最基类)
    │
    ├── BaseBlockItem (修改为继承 QGraphicsItem)
    │       │
    │       ├── TextBlockItem (完全重写)
    │       ├── ImageBlockItem
    │       ├── TableBlockItem
    │       └── MathFormulaBlockItem
    │
    ├── QGraphicsTextItem (保留，但 TextBlockItem 不再使用)
    └── MathItem (公式元素基类)
```

---

## 3. 核心类设计

### 3.1 BaseBlockItem 类修改

**文件位置**：`include/graphics/items/BaseBlockItem.h`

#### 3.1.1 修改内容

1. **继承关系**：从 `QGraphicsRectItem` 改为 `QGraphicsItem`
2. **新增方法**：
   - `boundingRect()` - 自己实现
   - `paint()` - 如果需要背景，自己绘制
3. **保留方法**：
   - `block()` - 获取关联的数据块
   - `updateBlock()` - 纯虚函数，子类必须实现

#### 3.1.2 代码示例

```cpp
#ifndef BASEBLOCKITEM_H
#define BASEBLOCKITEM_H

#include <QGraphicsItem>  // 改为 QGraphicsItem
#include "core/Global.h"

namespace QtWordEditor {

class Block;

/**
 * @brief 块图形项基类，是所有块图形项的基类
 *
 * 该类继承自 QGraphicsItem（不再继承 QGraphicsRectItem），
 * 作为文档中各种块类型（如文本块、图片块、表格块等）
 * 在图形场景中的可视化表示基类。
 */
class BaseBlockItem : public QGraphicsItem
{
public:
    /**
     * @brief 构造函数
     * @param block 关联的数据块对象
     * @param parent 父图形项指针，默认为 nullptr
     */
    explicit BaseBlockItem(Block *block, QGraphicsItem *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~BaseBlockItem() override;

    // ========== QGraphicsItem 接口 ==========
    
    /**
     * @brief 获取边界矩形
     * @return 边界矩形
     */
    QRectF boundingRect() const override = 0;
    
    /**
     * @brief 绘制图形项
     * @param painter 画家指针
     * @param option 样式选项
     * @param widget 窗口指针
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override = 0;

    // ========== 块相关方法 ==========
    
    /**
     * @brief 获取关联的数据块
     * @return 指向关联块对象的指针
     */
    Block *block() const;
    
    /**
     * @brief 更新图形项显示
     * 纯虚函数，子类必须实现具体的更新逻辑
     */
    virtual void updateBlock() = 0;

protected:
    Block *m_block;  ///< 关联的数据块对象
    QRectF m_boundingRect;  ///< 边界矩形（自己管理）
};

} // namespace QtWordEditor

#endif // BASEBLOCKITEM_H
```

---

### 3.2 TextFragment 类（文本片段）

**文件位置**：`include/graphics/items/TextFragment.h` & `src/graphics/items/TextFragment.cpp`

#### 3.2.1 类职责
- 自己渲染文本，不使用 QGraphicsTextItem
- 计算文本的宽度、高度、基线
- 支持字符样式（字体、颜色、粗体、斜体、下划线等）
- 继承自 QGraphicsItem

#### 3.2.2 类设计

```cpp
#ifndef TEXTFRAGMENT_H
#define TEXTFRAGMENT_H

#include <QGraphicsItem>
#include <QString>
#include <QFont>
#include <QColor>
#include "core/document/CharacterStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 文本片段类
 *
 * 自己渲染文本，不使用 QGraphicsTextItem。
 * 负责：
 * 1. 文本测量（宽度、高度、基线）
 * 2. 文本渲染（使用 QPainter）
 * 3. 字符样式应用
 */
class TextFragment : public QGraphicsItem
{
public:
    // 类型 ID
    enum { Type = UserType + 1010 };

    /**
     * @brief 构造函数
     * @param text 文本内容
     * @param style 字符样式
     * @param parent 父图形项指针
     */
    explicit TextFragment(const QString &text, const CharacterStyle &style, 
                          QGraphicsItem *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~TextFragment() override;

    // ========== 文本内容访问 ==========
    
    /**
     * @brief 获取文本内容
     * @return 文本
     */
    QString text() const;
    
    /**
     * @brief 设置文本内容
     * @param text 文本
     */
    void setText(const QString &text);
    
    /**
     * @brief 获取字符样式
     * @return 字符样式
     */
    CharacterStyle style() const;
    
    /**
     * @brief 设置字符样式
     * @param style 字符样式
     */
    void setStyle(const CharacterStyle &style);

    // ========== 尺寸和基线 ==========
    
    /**
     * @brief 获取文本宽度
     * @return 宽度
     */
    qreal width() const;
    
    /**
     * @brief 获取文本高度
     * @return 高度
     */
    qreal height() const;
    
    /**
     * @brief 获取基线位置（从顶部到基线的距离）
     * @return 基线位置
     */
    qreal baseline() const;

    // ========== QGraphicsItem 接口 ==========
    
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    /**
     * @brief 计算文本尺寸（内部方法）
     */
    void calculateSize();
    
    /**
     * @brief 从 CharacterStyle 创建 QFont
     * @param style 字符样式
     * @return QFont
     */
    QFont createFontFromStyle(const CharacterStyle &style) const;

private:
    QString m_text;           ///< 文本内容
    CharacterStyle m_style;   ///< 字符样式
    QFont m_font;             ///< 字体
    QColor m_textColor;       ///< 文本颜色
    QRectF m_boundingRect;    ///< 边界矩形
    qreal m_baseline;         ///< 基线位置
};

} // namespace QtWordEditor

#endif // TEXTFRAGMENT_H
```

---

### 3.3 TextBlockLayoutEngine 类（布局引擎）

**文件位置**：`include/graphics/items/TextBlockLayoutEngine.h` & `src/graphics/items/TextBlockLayoutEngine.cpp`

#### 3.3.1 类职责
这是**核心的核心**！负责：
1. 文本测量
2. 文本换行（单词换行、字符换行）
3. 文本对齐（左对齐、居中、右对齐、两端对齐）
4. 基线对齐（专业排版的核心）
5. 对象定位（定位嵌入的公式对象）
6. 计算整体边界矩形

#### 3.3.2 类设计

```cpp
#ifndef TEXTBLOCKLAYOUTENGINE_H
#define TEXTBLOCKLAYOUTENGINE_H

#include <QList>
#include <QRectF>
#include <QPointF>
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class TextFragment;
class MathFormulaGraphicsItem;
class QGraphicsItem;

/**
 * @brief 文本块布局引擎
 *
 * 这是完全重写 TextBlockItem 的核心！
 * 负责：
 * 1. 文本测量和换行
 * 2. 文本对齐
 * 3. 基线对齐（专业排版）
 * 4. 对象定位（嵌入公式）
 * 5. 计算整体尺寸
 */
class TextBlockLayoutEngine
{
public:
    /**
     * @brief 换行模式
     */
    enum class WrapMode {
        NoWrap,         ///< 不换行
        WordWrap,       ///< 单词换行
        WrapAnywhere    ///< 字符换行
    };

    /**
     * @brief 构造函数
     */
    TextBlockLayoutEngine();
    
    /**
     * @brief 析构函数
     */
    ~TextBlockLayoutEngine();

    // ========== 布局设置 ==========
    
    /**
     * @brief 设置可用宽度
     * @param width 宽度
     */
    void setAvailableWidth(qreal width);
    
    /**
     * @brief 设置段落样式
     * @param style 段落样式
     */
    void setParagraphStyle(const ParagraphStyle &style);
    
    /**
     * @brief 设置换行模式
     * @param mode 换行模式
     */
    void setWrapMode(WrapMode mode);

    // ========== 布局执行 ==========
    
    /**
     * @brief 执行布局
     * @param items 所有内容项（TextFragment + MathFormulaGraphicsItem）
     */
    void layout(const QList<QGraphicsItem*> &items);
    
    /**
     * @brief 清除布局结果
     */
    void clear();

    // ========== 布局结果获取 ==========
    
    /**
     * @brief 获取计算后的总高度
     * @return 高度
     */
    qreal totalHeight() const;
    
    /**
     * @brief 获取计算后的总宽度
     * @return 宽度
     */
    qreal totalWidth() const;
    
    /**
     * @brief 获取某个项的位置
     * @param item 项指针
     * @return 位置（相对于 TextBlockItem）
     */
    QPointF positionForItem(QGraphicsItem *item) const;
    
    /**
     * @brief 获取所有行的信息
     * @return 行列表
     */
    QList<QRectF> lines() const;

private:
    /**
     * @brief 一行的信息
     */
    struct LineInfo {
        QRectF rect;              ///< 行的矩形
        qreal maxBaseline;        ///< 该行的最大基线（用于对齐）
        QList<QGraphicsItem*> items;  ///< 该行的所有项
    };

    // ========== 内部布局方法 ==========
    
    /**
     * @brief 将项分配到行
     */
    void assignItemsToLines(const QList<QGraphicsItem*> &items);
    
    /**
     * @brief 计算每一行的基线
     */
    void calculateLineBaselines();
    
    /**
     * @brief 应用文本对齐
     */
    void applyAlignment();
    
    /**
     * @brief 定位所有项
     */
    void positionItems();
    
    /**
     * @brief 计算整体尺寸
     */
    void calculateTotalSize();

private:
    qreal m_availableWidth;          ///< 可用宽度
    ParagraphStyle m_paragraphStyle; ///< 段落样式
    WrapMode m_wrapMode;             ///< 换行模式
    
    QList<LineInfo> m_lines;         ///< 所有行的信息
    QHash<QGraphicsItem*, QPointF> m_itemPositions;  ///< 项的位置
    qreal m_totalWidth;              ///< 总宽度
    qreal m_totalHeight;             ///< 总高度
};

} // namespace QtWordEditor

#endif // TEXTBLOCKLAYOUTENGINE_H
```

---

### 3.4 TextBlockItem 类（完全重写）

**文件位置**：`include/graphics/items/TextBlockItem.h` & `src/graphics/items/TextBlockItem.cpp`

#### 3.4.1 类职责
1. 从 ParagraphBlock 读取数据
2. 创建内容项（TextFragment + MathFormulaGraphicsItem）
3. 使用 TextBlockLayoutEngine 进行布局
4. 绘制背景（如果需要）
5. 管理内容项的生命周期

#### 3.4.2 类设计

```cpp
#ifndef TEXTBLOCKITEM_H
#define TEXTBLOCKITEM_H

#include "BaseBlockItem.h"
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

class ParagraphBlock;
class TextFragment;
class MathFormulaGraphicsItem;
class TextBlockLayoutEngine;

/**
 * @brief 文本块图形项类（完全重写版）
 *
 * 不再使用 QGraphicsTextItem！完全自己实现：
 * - 文本渲染（TextFragment）
 * - 文本布局（TextBlockLayoutEngine）
 * - 内联公式嵌入（MathFormulaGraphicsItem）
 * - 基线对齐（专业排版）
 */
class TextBlockItem : public BaseBlockItem
{
public:
    // 类型 ID
    enum { Type = UserType + 1001 };

    /**
     * @brief 构造函数
     * @param block 关联的段落块
     * @param parent 父图形项指针
     */
    explicit TextBlockItem(ParagraphBlock *block, QGraphicsItem *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~TextBlockItem() override;

    // ========== BaseBlockItem 接口 ==========
    
    void updateBlock() override;

    // ========== QGraphicsItem 接口 ==========
    
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    // ========== 内部方法 ==========
    
    /**
     * @brief 从 ParagraphBlock 构建内容项
     */
    void buildContentItems();
    
    /**
     * @brief 清除所有内容项
     */
    void clearContentItems();
    
    /**
     * @brief 执行布局
     */
    void performLayout();

private:
    QList<QGraphicsItem*> m_contentItems;  ///< 所有内容项（TextFragment + MathFormulaGraphicsItem）
    TextBlockLayoutEngine *m_layoutEngine;  ///< 布局引擎
};

} // namespace QtWordEditor

#endif // TEXTBLOCKITEM_H
```

---

## 4. 核心算法设计

### 4.1 文本测量算法

**文件位置**：`TextFragment::calculateSize()`

#### 算法步骤
1. 根据 CharacterStyle 创建 QFont
2. 使用 QFontMetricsF 测量文本宽度
3. 使用 QFontMetricsF 获取字体的 ascent 和 descent
4. 计算基线位置（ascent）
5. 计算边界矩形

#### 代码示例
```cpp
void TextFragment::calculateSize()
{
    QFontMetricsF fm(m_font);
    
    // 计算宽度（使用 horizontalAdvance 更准确）
    qreal width = fm.horizontalAdvance(m_text);
    
    // 计算高度
    qreal height = fm.height();
    
    // 计算基线（ascent 是从顶部到基线的距离）
    m_baseline = fm.ascent();
    
    // 设置边界矩形
    m_boundingRect = QRectF(0, 0, width, height);
}
```

---

### 4.2 文本换行算法

**文件位置**：`TextBlockLayoutEngine::assignItemsToLines()`

#### 算法步骤
1. 初始化当前行
2. 遍历所有内容项（文本片段 + 公式）
3. 对于每个项：
   a. 计算如果加入当前行的总宽度
   b. 如果超过可用宽度：
      i. 完成当前行
      ii. 开始新行
   c. 将项加入当前行
4. 完成最后一行

#### 换行模式
- **WordWrap**：在单词边界换行（优先）
- **WrapAnywhere**：在任意字符边界换行
- **NoWrap**：不换行

---

### 4.3 基线对齐算法

**文件位置**：`TextBlockLayoutEngine::calculateLineBaselines()`

#### 原理
基线对齐是专业排版的核心！所有项（文本和公式）都按它们自己的基线对齐。

#### 算法步骤
1. 对于每一行：
   a. 找出该行所有项的最大基线值
   b. 对于每个项，计算它的垂直位置：
      `y = lineY + (maxBaseline - itemBaseline)`
2. 这样所有项的基线都会对齐！

#### 图示
```
行最大基线位置 → ──────────┐
                    │
文本项基线 → ──────┼───────── (对齐)
                    │
公式项基线 → ──────┼───────── (对齐)
                    │
                    └───────── 行底部
```

---

### 4.4 文本对齐算法

**文件位置**：`TextBlockLayoutEngine::applyAlignment()`

#### 支持的对齐方式
1. **左对齐（AlignLeft）**：所有行靠左
2. **居中（AlignCenter）**：所有行居中
3. **右对齐（AlignRight）**：所有行靠右
4. **两端对齐（AlignJustify）**：每行撑满可用宽度（最后一行除外）

---

## 5. 实现步骤计划

### 5.1 阶段一：修改 BaseBlockItem
1. 修改 BaseBlockItem，继承 QGraphicsItem
2. 实现 boundingRect() 和 paint() 接口
3. 更新所有子类（TextBlockItem、ImageBlockItem 等）

### 5.2 阶段二：实现 TextFragment
1. 创建 TextFragment 类
2. 实现文本测量（calculateSize）
3. 实现文本渲染（paint）
4. 支持字符样式（粗体、斜体、下划线等）

### 5.3 阶段三：实现 TextBlockLayoutEngine
1. 创建 TextBlockLayoutEngine 类
2. 实现文本换行算法
3. 实现基线对齐算法
4. 实现文本对齐算法
5. 实现对象定位

### 5.4 阶段四：完全重写 TextBlockItem
1. 创建新的 TextBlockItem（完全重写）
2. 实现 buildContentItems()（从 ParagraphBlock 构建）
3. 实现 performLayout()（使用布局引擎）
4. 实现 updateBlock()
5. 实现 paint()（绘制背景）

### 5.5 阶段五：集成和测试
1. 更新 DocumentScene 使用新的 TextBlockItem
2. 编译测试
3. 调试和优化

---

## 6. 与现有架构的集成

### 6.1 数据层（保持不变）
- ParagraphBlock - 不变
- Span - 不变
- MathFormulaSpan - 不变（已实现）
- CharacterStyle - 不变
- ParagraphStyle - 不变

### 6.2 图形层（需要修改）
- BaseBlockItem - ✅ 需要修改
- TextBlockItem - ✅ 需要完全重写
- DocumentScene - ✅ 需要更新（使用新 TextBlockItem）
- 其他 BlockItem（ImageBlockItem 等）- ✅ 需要更新（适配新 BaseBlockItem）

### 6.3 其他层（保持不变）
- 编辑控制层 - 不变
- 命令层 - 不变
- UI 层 - 不变

---

## 7. 优点总结

| 优点 | 说明 |
|------|------|
| 1. **完全可控** | 一切都在掌控之中，没有黑盒 |
| 2. **专业排版** | 基线对齐，像 Word 一样 |
| 3. **完美的内联公式** | 公式和文本完美融合 |
| 4. **性能最优** | 没有额外的 QGraphicsTextItem 开销 |
| 5. **没有占位符问题** | 不需要任何占位符 |
| 6. **完美的文本选择** | 可以选择包含公式的任何内容 |
| 7. **模块化设计** | 每个功能都独立成类，易于维护 |

---

## 8. 风险和注意事项

| 风险 | 缓解方案 |
|------|---------|
| 1. 工作量巨大 | 分阶段实现，每个阶段都测试 |
| 2. 容易出错 | 充分测试，特别是布局算法 |
| 3. 开发时间长 | 先保留旧的 TextBlockItem 作为备份 |
| 4. 需要排版知识 | 参考 Word、LaTeX 的排版算法 |

---

## 9. 文件清单

### 新增文件
1. `include/graphics/items/TextFragment.h`
2. `src/graphics/items/TextFragment.cpp`
3. `include/graphics/items/TextBlockLayoutEngine.h`
4. `src/graphics/items/TextBlockLayoutEngine.cpp`

### 修改文件
1. `include/graphics/items/BaseBlockItem.h`
2. `src/graphics/items/BaseBlockItem.cpp`
3. `include/graphics/items/TextBlockItem.h`
4. `src/graphics/items/TextBlockItem.cpp`
5. `include/graphics/items/ImageBlockItem.h`（需要适配）
6. `include/graphics/items/TableBlockItem.h`（需要适配）
7. `src/graphics/scene/DocumentScene.cpp`（需要更新）

---

## 10. 总结

本设计文档详细说明了如何完全重写 TextBlockItem，抛弃 QGraphicsTextItem，自己实现整个文本和对象布局系统。

**核心理念**：
- 直接继承 QGraphicsItem
- 模块化设计（TextFragment、TextBlockLayoutEngine）
- 基线对齐（专业排版）
- 与现有架构无缝兼容

**预计开发时间**：2-4 周（取决于复杂度和测试）

**推荐的开发方式**：分阶段实现，每个阶段都充分测试！
