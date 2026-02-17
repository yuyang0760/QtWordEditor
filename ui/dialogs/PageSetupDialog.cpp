#include "PageSetupDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDebug>

namespace QtWordEditor {

class PageSetupDialog::Private
{
public:
    QDoubleSpinBox *pageWidthSpin = nullptr;
    QDoubleSpinBox *pageHeightSpin = nullptr;
    QRadioButton *portraitRadio = nullptr;
    QRadioButton *landscapeRadio = nullptr;
    QDoubleSpinBox *marginLeftSpin = nullptr;
    QDoubleSpinBox *marginRightSpin = nullptr;
    QDoubleSpinBox *marginTopSpin = nullptr;
    QDoubleSpinBox *marginBottomSpin = nullptr;
};

PageSetupDialog::PageSetupDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private)
{
    setWindowTitle(tr("Page Setup"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Page size group
    QGroupBox *sizeGroup = new QGroupBox(tr("Page Size (mm)"), this);
    QGridLayout *sizeLayout = new QGridLayout(sizeGroup);
    sizeLayout->addWidget(new QLabel(tr("Width:"), sizeGroup), 0, 0);
    d->pageWidthSpin = new QDoubleSpinBox(sizeGroup);
    d->pageWidthSpin->setRange(50, 500);
    d->pageWidthSpin->setValue(210);
    d->pageWidthSpin->setSuffix(" mm");
    sizeLayout->addWidget(d->pageWidthSpin, 0, 1);
    sizeLayout->addWidget(new QLabel(tr("Height:"), sizeGroup), 1, 0);
    d->pageHeightSpin = new QDoubleSpinBox(sizeGroup);
    d->pageHeightSpin->setRange(50, 500);
    d->pageHeightSpin->setValue(297);
    d->pageHeightSpin->setSuffix(" mm");
    sizeLayout->addWidget(d->pageHeightSpin, 1, 1);
    sizeGroup->setLayout(sizeLayout);
    mainLayout->addWidget(sizeGroup);

    // Orientation group
    QGroupBox *orientGroup = new QGroupBox(tr("Orientation"), this);
    QHBoxLayout *orientLayout = new QHBoxLayout(orientGroup);
    d->portraitRadio = new QRadioButton(tr("Portrait"), orientGroup);
    d->landscapeRadio = new QRadioButton(tr("Landscape"), orientGroup);
    d->portraitRadio->setChecked(true);
    orientLayout->addWidget(d->portraitRadio);
    orientLayout->addWidget(d->landscapeRadio);
    orientGroup->setLayout(orientLayout);
    mainLayout->addWidget(orientGroup);

    // Margins group
    QGroupBox *marginGroup = new QGroupBox(tr("Margins (mm)"), this);
    QGridLayout *marginLayout = new QGridLayout(marginGroup);
    marginLayout->addWidget(new QLabel(tr("Left:"), marginGroup), 0, 0);
    d->marginLeftSpin = new QDoubleSpinBox(marginGroup);
    d->marginLeftSpin->setRange(0, 100);
    d->marginLeftSpin->setValue(20);
    d->marginLeftSpin->setSuffix(" mm");
    marginLayout->addWidget(d->marginLeftSpin, 0, 1);
    marginLayout->addWidget(new QLabel(tr("Right:"), marginGroup), 1, 0);
    d->marginRightSpin = new QDoubleSpinBox(marginGroup);
    d->marginRightSpin->setRange(0, 100);
    d->marginRightSpin->setValue(20);
    d->marginRightSpin->setSuffix(" mm");
    marginLayout->addWidget(d->marginRightSpin, 1, 1);
    marginLayout->addWidget(new QLabel(tr("Top:"), marginGroup), 2, 0);
    d->marginTopSpin = new QDoubleSpinBox(marginGroup);
    d->marginTopSpin->setRange(0, 100);
    d->marginTopSpin->setValue(20);
    d->marginTopSpin->setSuffix(" mm");
    marginLayout->addWidget(d->marginTopSpin, 2, 1);
    marginLayout->addWidget(new QLabel(tr("Bottom:"), marginGroup), 3, 0);
    d->marginBottomSpin = new QDoubleSpinBox(marginGroup);
    d->marginBottomSpin->setRange(0, 100);
    d->marginBottomSpin->setValue(20);
    d->marginBottomSpin->setSuffix(" mm");
    marginLayout->addWidget(d->marginBottomSpin, 3, 1);
    marginGroup->setLayout(marginLayout);
    mainLayout->addWidget(marginGroup);

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

PageSetupDialog::~PageSetupDialog()
{
}

PageSetup PageSetupDialog::getPageSetup(const PageSetup &initial, QWidget *parent)
{
    PageSetupDialog dialog(parent);
    // TODO: set initial values
    // For now, return default setup
    return dialog.selectedSetup();
}

PageSetup PageSetupDialog::selectedSetup() const
{
    PageSetup setup;
    setup.pageWidth = d->pageWidthSpin->value();
    setup.pageHeight = d->pageHeightSpin->value();
    setup.marginLeft = d->marginLeftSpin->value();
    setup.marginRight = d->marginRightSpin->value();
    setup.marginTop = d->marginTopSpin->value();
    setup.marginBottom = d->marginBottomSpin->value();
    setup.portrait = d->portraitRadio->isChecked();
    return setup;
}

} // namespace QtWordEditor