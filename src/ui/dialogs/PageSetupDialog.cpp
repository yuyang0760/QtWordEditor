#include "ui/dialogs/PageSetupDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDebug>

namespace QtWordEditor {

class PageSetupDialog::Private
{
public:
    QComboBox *pageSizeCombo = nullptr;
    QDoubleSpinBox *pageWidthSpin = nullptr;
    QDoubleSpinBox *pageHeightSpin = nullptr;
    QRadioButton *portraitRadio = nullptr;
    QRadioButton *landscapeRadio = nullptr;
    QComboBox *marginPresetCombo = nullptr;
    QDoubleSpinBox *marginLeftSpin = nullptr;
    QDoubleSpinBox *marginRightSpin = nullptr;
    QDoubleSpinBox *marginTopSpin = nullptr;
    QDoubleSpinBox *marginBottomSpin = nullptr;
    
    bool isSettingFromPreset = false;
};

PageSetupDialog::PageSetupDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private)
{
    setWindowTitle(tr("Page Setup"));
    setMinimumSize(400, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Page size group
    QGroupBox *sizeGroup = new QGroupBox(tr("Page Size"), this);
    QGridLayout *sizeLayout = new QGridLayout(sizeGroup);
    
    sizeLayout->addWidget(new QLabel(tr("Preset:"), sizeGroup), 0, 0);
    d->pageSizeCombo = new QComboBox(sizeGroup);
    d->pageSizeCombo->addItem(tr("A4"), QVariant::fromValue(PageSizePreset::A4));
    d->pageSizeCombo->addItem(tr("A3"), QVariant::fromValue(PageSizePreset::A3));
    d->pageSizeCombo->addItem(tr("A5"), QVariant::fromValue(PageSizePreset::A5));
    d->pageSizeCombo->addItem(tr("Letter"), QVariant::fromValue(PageSizePreset::Letter));
    d->pageSizeCombo->addItem(tr("Legal"), QVariant::fromValue(PageSizePreset::Legal));
    d->pageSizeCombo->addItem(tr("Tabloid"), QVariant::fromValue(PageSizePreset::Tabloid));
    d->pageSizeCombo->addItem(tr("Custom"), QVariant::fromValue(PageSizePreset::Custom));
    sizeLayout->addWidget(d->pageSizeCombo, 0, 1, 1, 2);
    
    sizeLayout->addWidget(new QLabel(tr("Width:"), sizeGroup), 1, 0);
    d->pageWidthSpin = new QDoubleSpinBox(sizeGroup);
    d->pageWidthSpin->setRange(50.0, 500.0);
    d->pageWidthSpin->setValue(210.0);
    d->pageWidthSpin->setSuffix(" mm");
    d->pageWidthSpin->setDecimals(1);
    d->pageWidthSpin->setSingleStep(1.0);
    sizeLayout->addWidget(d->pageWidthSpin, 1, 1);
    
    sizeLayout->addWidget(new QLabel(tr("Height:"), sizeGroup), 2, 0);
    d->pageHeightSpin = new QDoubleSpinBox(sizeGroup);
    d->pageHeightSpin->setRange(50.0, 500.0);
    d->pageHeightSpin->setValue(297.0);
    d->pageHeightSpin->setSuffix(" mm");
    d->pageHeightSpin->setDecimals(1);
    d->pageHeightSpin->setSingleStep(1.0);
    sizeLayout->addWidget(d->pageHeightSpin, 2, 1);
    
    sizeGroup->setLayout(sizeLayout);
    mainLayout->addWidget(sizeGroup);

    // Orientation group
    QGroupBox *orientGroup = new QGroupBox(tr("Orientation"), this);
    QHBoxLayout *orientLayout = new QHBoxLayout(orientGroup);
    QButtonGroup *orientGroupBtn = new QButtonGroup(this);
    
    d->portraitRadio = new QRadioButton(tr("Portrait"), orientGroup);
    d->landscapeRadio = new QRadioButton(tr("Landscape"), orientGroup);
    d->portraitRadio->setChecked(true);
    
    orientGroupBtn->addButton(d->portraitRadio);
    orientGroupBtn->addButton(d->landscapeRadio);
    
    orientLayout->addWidget(d->portraitRadio);
    orientLayout->addWidget(d->landscapeRadio);
    orientLayout->addStretch();
    orientGroup->setLayout(orientLayout);
    mainLayout->addWidget(orientGroup);

    // Margins group
    QGroupBox *marginGroup = new QGroupBox(tr("Margins"), this);
    QGridLayout *marginLayout = new QGridLayout(marginGroup);
    
    marginLayout->addWidget(new QLabel(tr("Preset:"), marginGroup), 0, 0);
    d->marginPresetCombo = new QComboBox(marginGroup);
    d->marginPresetCombo->addItem(tr("Normal"), QVariant::fromValue(MarginPreset::Normal));
    d->marginPresetCombo->addItem(tr("Narrow"), QVariant::fromValue(MarginPreset::Narrow));
    d->marginPresetCombo->addItem(tr("Moderate"), QVariant::fromValue(MarginPreset::Moderate));
    d->marginPresetCombo->addItem(tr("Wide"), QVariant::fromValue(MarginPreset::Wide));
    d->marginPresetCombo->addItem(tr("Custom"), QVariant::fromValue(MarginPreset::Custom));
    marginLayout->addWidget(d->marginPresetCombo, 0, 1, 1, 2);
    
    marginLayout->addWidget(new QLabel(tr("Left:"), marginGroup), 1, 0);
    d->marginLeftSpin = new QDoubleSpinBox(marginGroup);
    d->marginLeftSpin->setRange(0.0, 100.0);
    d->marginLeftSpin->setValue(20.0);
    d->marginLeftSpin->setSuffix(" mm");
    d->marginLeftSpin->setDecimals(1);
    d->marginLeftSpin->setSingleStep(1.0);
    marginLayout->addWidget(d->marginLeftSpin, 1, 1);
    
    marginLayout->addWidget(new QLabel(tr("Right:"), marginGroup), 2, 0);
    d->marginRightSpin = new QDoubleSpinBox(marginGroup);
    d->marginRightSpin->setRange(0.0, 100.0);
    d->marginRightSpin->setValue(20.0);
    d->marginRightSpin->setSuffix(" mm");
    d->marginRightSpin->setDecimals(1);
    d->marginRightSpin->setSingleStep(1.0);
    marginLayout->addWidget(d->marginRightSpin, 2, 1);
    
    marginLayout->addWidget(new QLabel(tr("Top:"), marginGroup), 3, 0);
    d->marginTopSpin = new QDoubleSpinBox(marginGroup);
    d->marginTopSpin->setRange(0.0, 100.0);
    d->marginTopSpin->setValue(20.0);
    d->marginTopSpin->setSuffix(" mm");
    d->marginTopSpin->setDecimals(1);
    d->marginTopSpin->setSingleStep(1.0);
    marginLayout->addWidget(d->marginTopSpin, 3, 1);
    
    marginLayout->addWidget(new QLabel(tr("Bottom:"), marginGroup), 4, 0);
    d->marginBottomSpin = new QDoubleSpinBox(marginGroup);
    d->marginBottomSpin->setRange(0.0, 100.0);
    d->marginBottomSpin->setValue(20.0);
    d->marginBottomSpin->setSuffix(" mm");
    d->marginBottomSpin->setDecimals(1);
    d->marginBottomSpin->setSingleStep(1.0);
    marginLayout->addWidget(d->marginBottomSpin, 4, 1);
    
    marginGroup->setLayout(marginLayout);
    mainLayout->addWidget(marginGroup);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *okButton = new QPushButton(tr("OK"), this);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    okButton->setDefault(true);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    connect(d->pageSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PageSetupDialog::onPageSizePresetChanged);
    connect(d->portraitRadio, &QRadioButton::toggled,
            this, &PageSetupDialog::onOrientationChanged);
    connect(d->landscapeRadio, &QRadioButton::toggled,
            this, &PageSetupDialog::onOrientationChanged);
    connect(d->marginPresetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PageSetupDialog::onMarginPresetChanged);
    
    connect(d->pageWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomSizeChanged);
    connect(d->pageHeightSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomSizeChanged);
    connect(d->marginLeftSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomMarginChanged);
    connect(d->marginRightSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomMarginChanged);
    connect(d->marginTopSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomMarginChanged);
    connect(d->marginBottomSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomMarginChanged);
}

PageSetupDialog::~PageSetupDialog()
{
}

void PageSetupDialog::onPageSizePresetChanged(int index)
{
    if (d->isSettingFromPreset) return;
    
    PageSizePreset preset = d->pageSizeCombo->itemData(index).value<PageSizePreset>();
    d->isSettingFromPreset = true;
    
    qreal width = 210.0;
    qreal height = 297.0;
    
    switch (preset) {
        case PageSizePreset::A4:
            width = 210.0;
            height = 297.0;
            break;
        case PageSizePreset::A3:
            width = 297.0;
            height = 420.0;
            break;
        case PageSizePreset::A5:
            width = 148.0;
            height = 210.0;
            break;
        case PageSizePreset::Letter:
            width = 215.9;
            height = 279.4;
            break;
        case PageSizePreset::Legal:
            width = 215.9;
            height = 355.6;
            break;
        case PageSizePreset::Tabloid:
            width = 279.4;
            height = 431.8;
            break;
        case PageSizePreset::Custom:
        default:
            break;
    }
    
    if (preset != PageSizePreset::Custom) {
        if (!d->portraitRadio->isChecked()) {
            qSwap(width, height);
        }
        d->pageWidthSpin->setValue(width);
        d->pageHeightSpin->setValue(height);
    }
    
    d->isSettingFromPreset = false;
}

void PageSetupDialog::onOrientationChanged()
{
    if (d->isSettingFromPreset) return;
    
    d->isSettingFromPreset = true;
    
    qreal width = d->pageWidthSpin->value();
    qreal height = d->pageHeightSpin->value();
    d->pageWidthSpin->setValue(height);
    d->pageHeightSpin->setValue(width);
    
    d->isSettingFromPreset = false;
}

void PageSetupDialog::onMarginPresetChanged(int index)
{
    if (d->isSettingFromPreset) return;
    
    MarginPreset preset = d->marginPresetCombo->itemData(index).value<MarginPreset>();
    d->isSettingFromPreset = true;
    
    qreal left = 20.0;
    qreal right = 20.0;
    qreal top = 20.0;
    qreal bottom = 20.0;
    
    switch (preset) {
        case MarginPreset::Normal:
            left = 20.0;
            right = 20.0;
            top = 20.0;
            bottom = 20.0;
            break;
        case MarginPreset::Narrow:
            left = 12.7;
            right = 12.7;
            top = 12.7;
            bottom = 12.7;
            break;
        case MarginPreset::Moderate:
            left = 25.4;
            right = 25.4;
            top = 25.4;
            bottom = 25.4;
            break;
        case MarginPreset::Wide:
            left = 50.8;
            right = 50.8;
            top = 50.8;
            bottom = 50.8;
            break;
        case MarginPreset::Custom:
        default:
            break;
    }
    
    if (preset != MarginPreset::Custom) {
        d->marginLeftSpin->setValue(left);
        d->marginRightSpin->setValue(right);
        d->marginTopSpin->setValue(top);
        d->marginBottomSpin->setValue(bottom);
    }
    
    d->isSettingFromPreset = false;
}

void PageSetupDialog::onCustomSizeChanged()
{
    if (d->isSettingFromPreset) return;
    
    d->isSettingFromPreset = true;
    d->pageSizeCombo->setCurrentIndex(d->pageSizeCombo->findData(QVariant::fromValue(PageSizePreset::Custom)));
    d->isSettingFromPreset = false;
}

void PageSetupDialog::onCustomMarginChanged()
{
    if (d->isSettingFromPreset) return;
    
    d->isSettingFromPreset = true;
    d->marginPresetCombo->setCurrentIndex(d->marginPresetCombo->findData(QVariant::fromValue(MarginPreset::Custom)));
    d->isSettingFromPreset = false;
}

PageSetup PageSetupDialog::getPageSetup(const PageSetup &initial, QWidget *parent)
{
    PageSetupDialog dialog(parent);
    
    dialog.d->isSettingFromPreset = true;
    
    dialog.d->pageWidthSpin->setValue(initial.pageWidth);
    dialog.d->pageHeightSpin->setValue(initial.pageHeight);
    dialog.d->marginLeftSpin->setValue(initial.marginLeft);
    dialog.d->marginRightSpin->setValue(initial.marginRight);
    dialog.d->marginTopSpin->setValue(initial.marginTop);
    dialog.d->marginBottomSpin->setValue(initial.marginBottom);
    
    if (initial.portrait) {
        dialog.d->portraitRadio->setChecked(true);
    } else {
        dialog.d->landscapeRadio->setChecked(true);
    }
    
    dialog.d->isSettingFromPreset = false;
    
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedSetup();
    }
    
    return initial;
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