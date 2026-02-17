#ifndef FORMATTOOLBAR_H
#define FORMATTOOLBAR_H

#include <QToolBar>
#include "core/Global.h"

namespace QtWordEditor {

class FormatToolBarPrivate;

/**
 * @brief The FormatToolBar class provides a toolbar for formatting controls.
 */
class FormatToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit FormatToolBar(QWidget *parent = nullptr);
    ~FormatToolBar() override;

    // Update controls based on current selection (e.g., bold button state).
    void updateFromSelection();

public slots:
    void onFontChanged(const QFont &font);
    void onFontSizeChanged(int size);
    void onBoldChanged(bool bold);
    void onItalicChanged(bool italic);
    void onUnderlineChanged(bool underline);
    void onAlignmentChanged(Qt::Alignment alignment);

signals:
    void fontChanged(const QFont &font);
    void fontSizeChanged(int size);
    void boldChanged(bool bold);
    void italicChanged(bool italic);
    void underlineChanged(bool underline);
    void alignmentChanged(Qt::Alignment alignment);

private:
    QScopedPointer<FormatToolBarPrivate> d;
};

} // namespace QtWordEditor

#endif // FORMATTOOLBAR_H