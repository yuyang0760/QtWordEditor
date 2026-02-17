#include "ui/widgets/FormatToolBar.h"
#include <QFontComboBox>
#include <QSpinBox>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QHBoxLayout>
#include <QDebug>

namespace QtWordEditor {

class FormatToolBarPrivate
{
public:
    QFontComboBox *fontCombo = nullptr;
    QSpinBox *fontSizeSpin = nullptr;
    QAction *boldAction = nullptr;
    QAction *italicAction = nullptr;
    QAction *underlineAction = nullptr;
    QAction *alignLeftAction = nullptr;
    QAction *alignCenterAction = nullptr;
    QAction *alignRightAction = nullptr;
    QAction *alignJustifyAction = nullptr;
};

FormatToolBar::FormatToolBar(QWidget *parent)
    : QToolBar(parent)
    , d(new FormatToolBarPrivate)
{
    setWindowTitle(tr("Format"));
    setIconSize(QSize(16, 16));

    // Font family combo
    d->fontCombo = new QFontComboBox(this);
    d->fontCombo->setMaximumWidth(150);
    addWidget(d->fontCombo);

    // Font size spin
    d->fontSizeSpin = new QSpinBox(this);
    d->fontSizeSpin->setRange(6, 72);
    d->fontSizeSpin->setValue(12);
    d->fontSizeSpin->setMaximumWidth(60);
    addWidget(d->fontSizeSpin);

    addSeparator();

    // Bold, italic, underline
    d->boldAction = new QAction(tr("Bold"), this);
    d->boldAction->setCheckable(true);
    d->boldAction->setIcon(QIcon::fromTheme("format-text-bold"));
    addAction(d->boldAction);

    d->italicAction = new QAction(tr("Italic"), this);
    d->italicAction->setCheckable(true);
    d->italicAction->setIcon(QIcon::fromTheme("format-text-italic"));
    addAction(d->italicAction);

    d->underlineAction = new QAction(tr("Underline"), this);
    d->underlineAction->setCheckable(true);
    d->underlineAction->setIcon(QIcon::fromTheme("format-text-underline"));
    addAction(d->underlineAction);

    addSeparator();

    // Alignment actions (left, center, right, justify)
    QMenu *alignMenu = new QMenu(this);
    d->alignLeftAction = alignMenu->addAction(tr("Left"));
    d->alignLeftAction->setCheckable(true);
    d->alignLeftAction->setIcon(QIcon::fromTheme("format-justify-left"));
    d->alignCenterAction = alignMenu->addAction(tr("Center"));
    d->alignCenterAction->setCheckable(true);
    d->alignCenterAction->setIcon(QIcon::fromTheme("format-justify-center"));
    d->alignRightAction = alignMenu->addAction(tr("Right"));
    d->alignRightAction->setCheckable(true);
    d->alignRightAction->setIcon(QIcon::fromTheme("format-justify-right"));
    d->alignJustifyAction = alignMenu->addAction(tr("Justify"));
    d->alignJustifyAction->setCheckable(true);
    d->alignJustifyAction->setIcon(QIcon::fromTheme("format-justify-fill"));

    QToolButton *alignButton = new QToolButton(this);
    alignButton->setText(tr("Align"));
    alignButton->setMenu(alignMenu);
    alignButton->setPopupMode(QToolButton::InstantPopup);
    addWidget(alignButton);

    // Connect signals
    connect(d->fontCombo, &QFontComboBox::currentFontChanged,
            this, &FormatToolBar::fontChanged);
    connect(d->fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FormatToolBar::fontSizeChanged);
    connect(d->boldAction, &QAction::toggled,
            this, &FormatToolBar::boldChanged);
    connect(d->italicAction, &QAction::toggled,
            this, &FormatToolBar::italicChanged);
    connect(d->underlineAction, &QAction::toggled,
            this, &FormatToolBar::underlineChanged);
    connect(d->alignLeftAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignLeft); });
    connect(d->alignCenterAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignCenter); });
    connect(d->alignRightAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignRight); });
    connect(d->alignJustifyAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignJustify); });
}

FormatToolBar::~FormatToolBar()
{
}

void FormatToolBar::updateFromSelection()
{
    // Placeholder: update control states based on current selection
    qDebug() << "FormatToolBar::updateFromSelection() placeholder";
}

void FormatToolBar::onFontChanged(const QFont &font)
{
    d->fontCombo->setCurrentFont(font);
}

void FormatToolBar::onFontSizeChanged(int size)
{
    d->fontSizeSpin->setValue(size);
}

void FormatToolBar::onBoldChanged(bool bold)
{
    d->boldAction->setChecked(bold);
}

void FormatToolBar::onItalicChanged(bool italic)
{
    d->italicAction->setChecked(italic);
}

void FormatToolBar::onUnderlineChanged(bool underline)
{
    d->underlineAction->setChecked(underline);
}

void FormatToolBar::onAlignmentChanged(Qt::Alignment alignment)
{
    d->alignLeftAction->setChecked(alignment == Qt::AlignLeft);
    d->alignCenterAction->setChecked(alignment == Qt::AlignCenter);
    d->alignRightAction->setChecked(alignment == Qt::AlignRight);
    d->alignJustifyAction->setChecked(alignment == Qt::AlignJustify);
}

} // namespace QtWordEditor