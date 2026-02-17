#ifndef FONTDIALOG_H
#define FONTDIALOG_H

#include <QDialog>
#include "core/document/CharacterStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief The FontDialog class provides a dialog for selecting character style.
 *
 * This wraps QFontDialog and adds color selection.
 */
class FontDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FontDialog(QWidget *parent = nullptr);
    ~FontDialog() override;

    // Static convenience method to get a character style.
    static CharacterStyle getStyle(const CharacterStyle &initial, QWidget *parent = nullptr);

    // Get the selected style.
    CharacterStyle selectedStyle() const;

private slots:
    void onFontSelected(const QFont &font);
    void onColorClicked();

private:
    class Private;
    QScopedPointer<Private> d;
};

} // namespace QtWordEditor

#endif // FONTDIALOG_H