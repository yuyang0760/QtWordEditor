#ifndef ROWCONTAINERMATHSPAN_H
#define ROWCONTAINERMATHSPAN_H

#include "core/document/MathSpan.h"
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

class RowContainerMathSpan : public MathSpan
{
    Q_OBJECT
public:
    RowContainerMathSpan(QObject *parent = nullptr);
    ~RowContainerMathSpan() override;

    MathType mathType() const override { return RowContainer; }
    bool isContainer() const override { return true; }
    InlineSpan *clone() const override;

    void insertChild(int index, MathSpan *item) override;
    void appendChild(MathSpan *item) override;
    void removeChild(MathSpan *item) override;
    QList<MathSpan*> children() const override;
    int childCount() const override;
    MathSpan *childAt(int index) const override;
    int indexOfChild(MathSpan *child) const override;

    qreal spacing() const;
    void setSpacing(qreal spacing);

private:
    QList<MathSpan*> m_children;
    qreal m_spacing;
};

} // namespace QtWordEditor

#endif // ROWCONTAINERMATHSPAN_H
