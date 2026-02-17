#include "ui/dialogs/FontDialog.h"
#include <QFontDialog>
#include <QPushButton>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

namespace QtWordEditor {

class FontDialog::Private
{
public:
    QFontDialog *fontDialog = nullptr;
    QPushButton *colorButton = nullptr;
    QColor textColor;
};

FontDialog::FontDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private)
{
    setWindowTitle(tr("Font"));

    d->fontDialog = new QFontDialog(this);
    d->fontDialog->setOption(QFontDialog::NoButtons, true);
    d->fontDialog->setOption(QFontDialog::DontUseNativeDialog, true);

    d->colorButton = new QPushButton(tr("Color..."), this);
    d->textColor = Qt::black;

    QPushButton *okButton = new QPushButton(tr("OK"), this);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(d->fontDialog);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel(tr("Text Color:"), this));
    colorLayout->addWidget(d->colorButton);
    colorLayout->addStretch();
    mainLayout->addLayout(colorLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(d->fontDialog, &QFontDialog::currentFontChanged,
            this, &FontDialog::onFontSelected);
    connect(d->colorButton, &QPushButton::clicked,
            this, &FontDialog::onColorClicked);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

FontDialog::~FontDialog()
{
}

CharacterStyle FontDialog::getStyle(const CharacterStyle &initial, QWidget *parent)
{
    FontDialog dialog(parent);
    dialog.d->fontDialog->setCurrentFont(initial.font());
    dialog.d->textColor = initial.textColor();
    // For simplicity, just return a style with selected font and color
    CharacterStyle style;
    style.setFont(dialog.d->fontDialog->currentFont());
    style.setTextColor(dialog.d->textColor);
    // TODO: set other properties like background color, letter spacing
    return style;
}

CharacterStyle FontDialog::selectedStyle() const
{
    CharacterStyle style;
    style.setFont(d->fontDialog->currentFont());
    style.setTextColor(d->textColor);
    return style;
}

void FontDialog::onFontSelected(const QFont &font)
{
    // placeholder
    Q_UNUSED(font);
}

void FontDialog::onColorClicked()
{
    QColor color = QColorDialog::getColor(d->textColor, this);
    if (color.isValid()) {
        d->textColor = color;
    }
}

} // namespace QtWordEditor