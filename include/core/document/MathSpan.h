#ifndef MATHSPAN_H
#define MATHSPAN_H

#include "InlineSpan.h"
#include "core/Global.h"
#include <QObject>

namespace QtWordEditor {

/**
 * @brief 公式内联元素数据基类
 * 
 * 所有公式元素都继承自此类，采用Composite设计模式处理嵌套结构。
 * MathSpan只负责数据存储，不负责渲染。
 */
class MathSpan : public InlineSpan
{
    Q_OBJECT
public:
    enum MathType {
        Number,
        Operator,
        RowContainer,
        Fraction,
        Radical,
        SubSup,
        Bracket
    };
    
    MathSpan(QObject *parent = nullptr);
    ~MathSpan() override;

    Type type() const override { return Math; }
    int length() const override { return 1; }
    
    virtual MathType mathType() const = 0;
    virtual bool isContainer() const { return false; }

    // ========== 子项管理 ==========
    virtual void insertChild(int index, MathSpan *item);
    virtual void appendChild(MathSpan *item);
    virtual void removeChild(MathSpan *item);
    virtual QList<MathSpan*> children() const;
    virtual int childCount() const;
    virtual MathSpan *childAt(int index) const;
    virtual int indexOfChild(MathSpan *child) const;
    
    // ========== 父项管理 ==========
    MathSpan *parentMathSpan() const;
    void setParentMathSpan(MathSpan *parent);

signals:
    /**
     * @brief 公式内容发生变化
     */
    void contentChanged();

private:
    MathSpan *m_parent;
};

} // namespace QtWordEditor

#endif // MATHSPAN_H
