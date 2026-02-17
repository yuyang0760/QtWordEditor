#include "ui/dialogs/ParagraphDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QDebug>

namespace QtWordEditor {

class ParagraphDialog::Private
{
public:
    QComboBox *alignmentCombo = nullptr;
    QDoubleSpinBox *firstLineIndentSpin = nullptr;
    QDoubleSpinBox *leftIndentSpin = nullptr;
    QDoubleSpinBox *rightIndentSpin = nullptr;
    QDoubleSpinBox *spaceBeforeSpin = nullptr;
    QDoubleSpinBox *spaceAfterSpin = nullptr;
    QDoubleSpinBox *lineHeightSpin = nullptr;
};

ParagraphDialog::ParagraphDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private)
{
    setWindowTitle(tr("Paragraph"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Alignment group
    QGroupBox *alignmentGroup = new QGroupBox(tr("Alignment"), this);
    QHBoxLayout *alignLayout = new QHBoxLayout(alignmentGroup);
    d->alignmentCombo = new QComboBox(alignmentGroup);
    d->alignmentCombo->addItem(tr("Left"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignLeft));
    d->alignmentCombo->addItem(tr("Center"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignCenter));
    d->alignmentCombo->addItem(tr("Right"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignRight));
    d->alignmentCombo->addItem(tr("Justify"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignJustify));
    alignLayout->addWidget(d->alignmentCombo);
    alignmentGroup->setLayout(alignLayout);
    mainLayout->addWidget(alignmentGroup);

    // Indentation group
    QGroupBox *indentGroup = new QGroupBox(tr("Indentation (mm)"), this);
    QGridLayout *indentLayout = new QGridLayout(indentGroup);
    indentLayout->addWidget(new QLabel(tr("First line:"), indentGroup), 0, 0);
    d->firstLineIndentSpin = new QDoubleSpinBox(indentGroup);
    d->firstLineIndentSpin->setRange(-100, 100);
    d->firstLineIndentSpin->setValue(0);
    d->firstLineIndentSpin->setSuffix(" mm");
    indentLayout->addWidget(d->firstLineIndentSpin, 0, 1);
    indentLayout->addWidget(new QLabel(tr("Left:"), indentGroup), 1, 0);
    d->leftIndentSpin = new QDoubleSpinBox(indentGroup);
    d->leftIndentSpin->setRange(0, 100);
    d->leftIndentSpin->setValue(0);
    d->leftIndentSpin->setSuffix(" mm");
    indentLayout->addWidget(d->leftIndentSpin, 1, 1);
    indentLayout->addWidget(new QLabel(tr("Right:"), indentGroup), 2, 0);
    d->rightIndentSpin = new QDoubleSpinBox(indentGroup);
    d->rightIndentSpin->setRange(0, 100);
    d->rightIndentSpin->setValue(0);
    d->rightIndentSpin->setSuffix(" mm");
    indentLayout->addWidget(d->rightIndentSpin, 2, 1);
    indentGroup->setLayout(indentLayout);
    mainLayout->addWidget(indentGroup);

    // Spacing group
    QGroupBox *spacingGroup = new QGroupBox(tr("Spacing (mm)"), this);
    QGridLayout *spacingLayout = new QGridLayout(spacingGroup);
    spacingLayout->addWidget(new QLabel(tr("Before:"), spacingGroup), 0, 0);
    d->spaceBeforeSpin = new QDoubleSpinBox(spacingGroup);
    d->spaceBeforeSpin->setRange(0, 100);
    d->spaceBeforeSpin->setValue(0);
    d->spaceBeforeSpin->setSuffix(" mm");
    spacingLayout->addWidget(d->spaceBeforeSpin, 0, 1);
    spacingLayout->addWidget(new QLabel(tr("After:"), spacingGroup), 1, 0);
    d->spaceAfterSpin = new QDoubleSpinBox(spacingGroup);
    d->spaceAfterSpin->setRange(0, 100);
    d->spaceAfterSpin->setValue(0);
    d->spaceAfterSpin->setSuffix(" mm");
    spacingLayout->addWidget(d->spaceAfterSpin, 1, 1);
    spacingLayout->addWidget(new QLabel(tr("Line height (%):"), spacingGroup), 2, 0);
    d->lineHeightSpin = new QDoubleSpinBox(spacingGroup);
    d->lineHeightSpin->setRange(50, 300);
    d->lineHeightSpin->setValue(100);
    d->lineHeightSpin->setSuffix(" %");
    spacingLayout->addWidget(d->lineHeightSpin, 2, 1);
    spacingGroup->setLayout(spacingLayout);
    mainLayout->addWidget(spacingGroup);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *okButton = new QPushButton(tr("OK"), this);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

ParagraphDialog::~ParagraphDialog()
{
}

ParagraphStyle ParagraphDialog::getStyle(const ParagraphStyle &initial, QWidget *parent)
{
    ParagraphDialog dialog(parent);
    // TODO: set initial values
    // For now, return default style
    return dialog.selectedStyle();
}

ParagraphStyle ParagraphDialog::selectedStyle() const
{
    ParagraphStyle style;
    style.setAlignment(static_cast<QtWordEditor::ParagraphAlignment>(
        d->alignmentCombo->currentData().toInt()));
    style.setFirstLineIndent(d->firstLineIndentSpin->value());
    style.setLeftIndent(d->leftIndentSpin->value());
    style.setRightIndent(d->rightIndentSpin->value());
    style.setSpaceBefore(d->spaceBeforeSpin->value());
    style.setSpaceAfter(d->spaceAfterSpin->value());
    style.setLineHeight(d->lineHeightSpin->value());
    return style;
}

} // namespace QtWordEditor