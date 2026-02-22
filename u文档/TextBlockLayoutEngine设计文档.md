# TextBlockLayoutEngine 设计文档

## 1. 核心功能
TextBlockLayoutEngine 是 QtWordEditor 中负责文本块布局的核心组件，统一处理文本和数学公式的布局、基线对齐和换行逻辑。

## 2. 类结构设计

### 2.1 头文件 (TextBlockLayoutEngine.h)
```cpp
#ifndef TEXTBLOCKLAYOUTENGINE_H
#define TEXTBLOCKLAYOUTENGINE_H

#include <QList>
#include <QRectF>
#include <QFont>
#include "core/document/ParagraphBlock.h"
#include "core/Global.h"

namespace QtWordEditor {

class TextBlockItem;
class InlineSpan;

/**
 * @brief 文本块布局引擎
 * 
 * 负责：
 * 1. 计算文本和公式的布局位置
 * 2. 处理自动换行
 * 3. 基线对齐
 * 4. 行高计算
 */
class TextBlockLayoutEngine
{
public:
    /**
     * @brief 行布局信息
     */
    struct LineLayout {
        int startSpanIndex;          // 起始Span索引
        int endSpanIndex;            // 结束Span索引
        qreal width;                 // 行宽度
        qreal height;                // 行高度
        qreal baseline;              // 基线位置
        QList<QRectF> spanRects;     // 每个Span的矩形位置
    };

    explicit TextBlockLayoutEngine(TextBlockItem* textBlockItem);
    ~TextBlockLayoutEngine();

    /**
     * @brief 执行布局
     */
    void layout();

    /**
     * @brief 获取行布局信息
     */
    QList<LineLayout> lineLayouts() const;

    /**
     * @brief 获取总行数
     */
    int lineCount() const;

    /**
     * @brief 获取指定行的布局信息
     */
    LineLayout lineLayout(int lineIndex) const;

    /**
     * @brief 获取指定Span的矩形位置
     */
    QRectF spanRect(int spanIndex) const;

    /**
     * @brief 获取总高度
     */
    qreal totalHeight() const;

private:
    /**
     * @brief 计算Span的宽度
     */
    qreal calculateSpanWidth(InlineSpan* span) const;

    /**
     * @brief 计算Span的高度和基线
     */
    void calculateSpanMetrics(InlineSpan* span, qreal& height, qreal& baseline) const;

    /**
     * @brief 创建新行
     */
    void createNewLine();

    /**
     * @brief 添加到当前行
     */
    void addToCurrentLine(InlineSpan* span);

    /**
     * @brief 计算文本宽度
     */
    qreal calculateTextWidth(const QString& text, const QFont& font) const;

private:
    TextBlockItem* m_textBlockItem;
    ParagraphBlock* m_paragraphBlock;
    QList<LineLayout> m_lineLayouts;
    LineLayout m_currentLine;
    qreal m_availableWidth;
    qreal m_charSpacing;
    qreal m_lineHeight;
};

} // namespace QtWordEditor

#endif // TEXTBLOCKLAYOUTENGINE_H
```

### 2.2 实现文件 (TextBlockLayoutEngine.cpp)
```cpp
#include "TextBlockLayoutEngine.h"
#include "TextBlockItem.h"
#include "core/document/TextSpan.h"
#include "core/document/MathSpan.h"
#include "core/document/ParagraphStyle.h"
#include "graphics/formula/MathFormulaItem.h"
#include <QFontMetricsF>
#include <QDebug>

namespace QtWordEditor {

TextBlockLayoutEngine::TextBlockLayoutEngine(TextBlockItem* textBlockItem)
    : m_textBlockItem(textBlockItem)
    , m_paragraphBlock(nullptr)
    , m_availableWidth(0)
    , m_charSpacing(0)
    , m_lineHeight(1.2)
{
}

TextBlockLayoutEngine::~TextBlockLayoutEngine()
{
}

void TextBlockLayoutEngine::layout()
{
    m_lineLayouts.clear();
    m_currentLine = LineLayout();
    
    if (!m_paragraphBlock) {
        return;
    }
    
    m_availableWidth = m_textBlockItem->width();
    ParagraphStyle style = m_paragraphBlock->paragraphStyle();
    m_charSpacing = style.charSpacing();
    m_lineHeight = style.lineHeight();
    
    qreal currentLineWidth = 0;
    
    for (int i = 0; i < m_paragraphBlock->inlineSpanCount(); ++i) {
        InlineSpan* span = m_paragraphBlock->inlineSpan(i);
        qreal spanWidth = calculateSpanWidth(span);
        
        if (currentLineWidth + spanWidth > m_availableWidth && currentLineWidth > 0) {
            // 需要自动换行
            createNewLine();
            currentLineWidth = 0;
        }
        
        // 添加到当前行
        addToCurrentLine(span);
        currentLineWidth += spanWidth + m_charSpacing;
    }
    
    // 添加最后一行
    if (m_currentLine.startSpanIndex != -1) {
        m_lineLayouts.append(m_currentLine);
    }
}

qreal TextBlockLayoutEngine::calculateSpanWidth(InlineSpan* span) const
{
    if (span->type() == InlineSpan::Text) {
        TextSpan* textSpan = static_cast<TextSpan*>(span);
        return calculateTextWidth(textSpan->text(), textSpan->font());
    } else if (span->type() == InlineSpan::Math) {
        MathSpan* mathSpan = static_cast<MathSpan*>(span);
        // 假设MathSpan有width()方法
        return mathSpan->width();
    }
    return 0;
}

void TextBlockLayoutEngine::calculateSpanMetrics(InlineSpan* span, qreal& height, qreal& baseline) const
{
    if (span->type() == InlineSpan::Text) {
        TextSpan* textSpan = static_cast<TextSpan*>(span);
        QFontMetricsF fm(textSpan->font());
        height = fm.height() * m_lineHeight;
        baseline = fm.ascent();
    } else if (span->type() == InlineSpan::Math) {
        MathSpan* mathSpan = static_cast<MathSpan*>(span);
        // 假设MathSpan有metrics()方法
        height = mathSpan->height();
        baseline = mathSpan->baseline();
    }
}

void TextBlockLayoutEngine::createNewLine()
{
    if (m_currentLine.startSpanIndex != -1) {
        m_lineLayouts.append(m_currentLine);
    }
    m_currentLine = LineLayout();
    m_currentLine.startSpanIndex = -1;
}

void TextBlockLayoutEngine::addToCurrentLine(InlineSpan* span)
{
    if (m_currentLine.startSpanIndex == -1) {
        m_currentLine.startSpanIndex = m_paragraphBlock->indexOfInlineSpan(span);
    }
    m_currentLine.endSpanIndex = m_paragraphBlock->indexOfInlineSpan(span);
    
    qreal spanWidth = calculateSpanWidth(span);
    qreal spanHeight, spanBaseline;
    calculateSpanMetrics(span, spanHeight, spanBaseline);
    
    // 更新行高度和基线
    if (spanHeight > m_currentLine.height) {
        m_currentLine.height = spanHeight;
    }
    if (spanBaseline > m_currentLine.baseline) {
        m_currentLine.baseline = spanBaseline;
    }
    
    m_currentLine.width += spanWidth + m_charSpacing;
    m_currentLine.spanRects.append(QRectF(m_currentLine.width - spanWidth, 0, spanWidth, spanHeight));
}

qreal TextBlockLayoutEngine::calculateTextWidth(const QString& text, const QFont& font) const
{
    QFontMetricsF fm(font);
    return fm.horizontalAdvance(text);
}

QList<TextBlockLayoutEngine::LineLayout> TextBlockLayoutEngine::lineLayouts() const
{
    return m_lineLayouts;
}

int TextBlockLayoutEngine::lineCount() const
{
    return m_lineLayouts.count();
}

TextBlockLayoutEngine::LineLayout TextBlockLayoutEngine::lineLayout(int lineIndex) const
{
    if (lineIndex >= 0 && lineIndex < m_lineLayouts.count()) {
        return m_lineLayouts[lineIndex];
    }
    return LineLayout();
}

QRectF TextBlockLayoutEngine::spanRect(int spanIndex) const
{
    for (const LineLayout& line : m_lineLayouts) {
        if (spanIndex >= line.startSpanIndex && spanIndex <= line.endSpanIndex) {
            int localIndex = spanIndex - line.startSpanIndex;
            if (localIndex >= 0 && localIndex < line.spanRects.count()) {
                return line.spanRects[localIndex];
            }
        }
    }
    return QRectF();
}

qreal TextBlockLayoutEngine::totalHeight() const
{
    qreal total = 0;
    for (const LineLayout& line : m_lineLayouts) {
        total += line.height;
    }
    return total;
}

} // namespace QtWordEditor
```

## 3. 关键实现细节

### 3.1 自动换行逻辑
- 计算每个Span的宽度
- 当行宽超过可用宽度时自动换行
- 保持公式作为整体不跨多行

### 3.2 基线对齐
- 计算每个Span的基线位置
- 确保所有Span在同一行内基线对齐
- 符合数学排版规范

### 3.3 行高计算
- 根据段落样式设置行高
- 自动调整行高以容纳较大的公式

## 4. 与TextBlockItem的集成
```cpp
// 在TextBlockItem中使用TextBlockLayoutEngine
void TextBlockItem::updateLayout()
{
    m_layoutEngine->layout();
    setHeight(m_layoutEngine->totalHeight());
    
    // 更新子项位置
    for (int i = 0; i < m_paragraphBlock->inlineSpanCount(); ++i) {
        InlineSpan* span = m_paragraphBlock->inlineSpan(i);
        QRectF rect = m_layoutEngine->spanRect(i);
        
        // 更新TextFragment或MathFormulaItem的位置
        if (span->type() == InlineSpan::Text) {
            TextFragment* fragment = findTextFragment(span);
            if (fragment) {
                fragment->setPos(rect.x(), rect.y());
            }
        } else if (span->type() == InlineSpan::Math) {
            MathFormulaItem* formulaItem = findMathFormulaItem(span);
            if (formulaItem) {
                formulaItem->setPos(rect.x(), rect.y());
            }
        }
    }
}
```

## 5. 性能优化
- 缓存Span的宽度和基线信息
- 只在数据变化时重新布局
- 避免重复计算

---
**创建日期**: 2026-02-22  
**最后更新**: 2026-02-22  
**作者**: QtWordEditor开发团队