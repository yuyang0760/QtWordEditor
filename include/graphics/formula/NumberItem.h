#ifndef NUMBERITEM_H
#define NUMBERITEM_H

#include "graphics/formula/MathFormulaItem.h"
#include "core/document/math/NumberMathSpan.h"
#include <QString>
#include <QFont>
#include <QColor>
#include "core/Global.h"

namespace QtWordEditor {

class NumberItem : public MathFormulaItem
{
public:
    NumberItem(NumberMathSpan *dataSpan, QGraphicsItem *parent = nullptr);
    ~NumberItem() override = default;

    void relayout() override;
    void bindData(MathSpan *data) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    // ========== 光标定位（重写基类方法）==========
    /**
     * @brief 获取指定文本偏移处的光标位置
     * @param charOffset 字符偏移量（0 <= charOffset <= textLength()）
     * @return 光标位置（局部坐标）
     */
    QPointF cursorPosition(int charOffset) const override;
    
    /**
     * @brief 获取光标高度
     * @return 光标高度
     */
    qreal cursorHeight() const override;

    // ========== 样式设置 ==========
    void setFont(const QFont &font);
    void setTextColor(const QColor &color);

private:
    void calculateSize();
    void updateBoundingRect();

    QString m_text;
    QFont m_font;
    QColor m_textColor;
};

} // namespace QtWordEditor

#endif // NUMBERITEM_H
