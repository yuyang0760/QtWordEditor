#include "PageItem.h"
#include "core/document/Page.h"
#include <QBrush>
#include <QPen>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

namespace QtWordEditor {

PageItem::PageItem(Page *page, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_page(page)
{
    if (page) {
        setRect(page->pageRect());
        setBrush(QBrush(Qt::white));
        setPen(QPen(Qt::lightGray, 1));
        // Add a subtle shadow
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(10);
        shadow->setOffset(2, 2);
        setGraphicsEffect(shadow);
    }
}

PageItem::~PageItem()
{
}

Page *PageItem::page() const
{
    return m_page;
}

void PageItem::updatePage()
{
    if (m_page) {
        setRect(m_page->pageRect());
    }
}

} // namespace QtWordEditor