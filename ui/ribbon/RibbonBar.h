#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include <QTabWidget>
#include <QHash>
#include "core/Global.h"

namespace QtWordEditor {

class RibbonBarPrivate;

/**
 * @brief The RibbonBar class provides a Ribbon-style interface similar to Microsoft Office.
 *
 * This is a simplified implementation that uses QTabWidget as base.
 * For a production application, consider using a dedicated Ribbon library.
 */
class RibbonBar : public QTabWidget
{
    Q_OBJECT
public:
    explicit RibbonBar(QWidget *parent = nullptr);
    ~RibbonBar() override;

    // Add a new tab with given title. Returns the tab index.
    int addTab(const QString &title);

    // Add a group to the current tab. Groups are visual separators.
    void addGroup(const QString &groupName);

    // Add an action to the current group.
    void addAction(QAction *action);

    // Update UI state based on current document selection (e.g., bold button).
    void updateFromSelection();

private:
    QScopedPointer<RibbonBarPrivate> d;
};

} // namespace QtWordEditor

#endif // RIBBONBAR_H