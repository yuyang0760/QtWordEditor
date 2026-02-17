#ifndef PARAGRAPHDIALOG_H
#define PARAGRAPHDIALOG_H

#include <QDialog>
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief The ParagraphDialog class provides a dialog for setting paragraph style.
 */
class ParagraphDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParagraphDialog(QWidget *parent = nullptr);
    ~ParagraphDialog() override;

    // Static convenience method to get a paragraph style.
    static ParagraphStyle getStyle(const ParagraphStyle &initial, QWidget *parent = nullptr);

    // Get the selected style.
    ParagraphStyle selectedStyle() const;

private:
    class Private;
    QScopedPointer<Private> d;
};

} // namespace QtWordEditor

#endif // PARAGRAPHDIALOG_H