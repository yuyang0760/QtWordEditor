#ifndef PAGEITEM_H
#define PAGEITEM_H

#include <QGraphicsRectItem>
#include "core/Global.h"

namespace QtWordEditor {

class Page;

/**
 * @brief The PageItem class renders a page background and serves as a container
 * for block items belonging to that page.
 */
class PageItem : public QGraphicsRectItem
{
public:
    explicit PageItem(Page *page, QGraphicsItem *parent = nullptr);
    ~PageItem() override;

    Page *page() const;
    void updatePage();

private:
    Page *m_page;
};

} // namespace QtWordEditor

#endif // PAGEITEM_H