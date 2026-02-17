#ifndef INSERTIMAGEDIALOG_H
#define INSERTIMAGEDIALOG_H

#include <QDialog>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief The InsertImageDialog class provides a dialog for selecting an image file.
 */
class InsertImageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InsertImageDialog(QWidget *parent = nullptr);
    ~InsertImageDialog() override;

    // Static convenience method to get an image file path.
    static QString getImagePath(QWidget *parent = nullptr);

    // Get the selected file path.
    QString selectedPath() const;

private:
    class Private;
    QScopedPointer<Private> d;
};

} // namespace QtWordEditor

#endif // INSERTIMAGEDIALOG_H