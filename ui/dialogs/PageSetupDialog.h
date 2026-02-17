#ifndef PAGESETUPDIALOG_H
#define PAGESETUPDIALOG_H

#include <QDialog>
#include "core/Global.h"

namespace QtWordEditor {

struct PageSetup
{
    qreal pageWidth = 210.0;   // millimeters
    qreal pageHeight = 297.0;
    qreal marginLeft = 20.0;
    qreal marginRight = 20.0;
    qreal marginTop = 20.0;
    qreal marginBottom = 20.0;
    bool portrait = true;
};

/**
 * @brief The PageSetupDialog class provides a dialog for setting page dimensions and margins.
 */
class PageSetupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PageSetupDialog(QWidget *parent = nullptr);
    ~PageSetupDialog() override;

    // Static convenience method to get page setup.
    static PageSetup getPageSetup(const PageSetup &initial, QWidget *parent = nullptr);

    // Get the selected setup.
    PageSetup selectedSetup() const;

private:
    class Private;
    QScopedPointer<Private> d;
};

} // namespace QtWordEditor

#endif // PAGESETUPDIALOG_H