#ifndef SELECTIONITEM_H
#define SELECTIONITEM_H

#include <QGraphicsItem>
#include <QList>
#include <QRectF>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief The SelectionItem class draws multiple rectangles representing text selections.
 */
class SelectionItem : public QGraphicsItem
{
public:
    explicit SelectionItem(QGraphicsItem *parent = nullptr);
    ~SelectionItem() override;

    void setRects(const QList<QRectF> &rects);
    void clear();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    QList<QRectF> m_rects;
};

} // namespace QtWordEditor

#endif // SELECTIONITEM_H