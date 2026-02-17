#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include <QTabWidget>
#include <QHash>
#include <QAction>
#include <QFontComboBox>
#include <QSpinBox>
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

    int addTab(const QString &title);
    void addGroup(const QString &groupName);
    void addAction(QAction *action);
    void addWidget(QWidget *widget);
    void updateFromSelection();

signals:
    void fontChanged(const QFont &font);
    void fontSizeChanged(int size);
    void boldChanged(bool bold);
    void italicChanged(bool italic);
    void underlineChanged(bool underline);
    void alignmentChanged(Qt::Alignment alignment);

private:
    QScopedPointer<RibbonBarPrivate> d;
};

} // namespace QtWordEditor

#endif // RIBBONBAR_H
