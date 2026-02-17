#include "ui/ribbon/RibbonBar.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QDebug>
#include <QFontComboBox>
#include <QSpinBox>
#include <QMenu>
#include <QFrame>

namespace QtWordEditor {

class RibbonBarPrivate
{
public:
    int currentTab = -1;
    QHash<int, QStringList> tabGroups;
    QWidget *currentContentWidget = nullptr;
    
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

RibbonBar::RibbonBar(QWidget *parent)
    : QTabWidget(parent)
    , d(new RibbonBarPrivate)
{
    setDocumentMode(true);
    setTabPosition(QTabWidget::North);
    
    int homeIndex = addTab(tr("Home"));
    
    addGroup(tr("Font"));
    
    d->fontCombo = new QFontComboBox(this);
    d->fontCombo->setMaximumWidth(140);
    d->fontCombo->setMinimumHeight(24);
    d->fontCombo->setMaximumHeight(24);
    connect(d->fontCombo, &QFontComboBox::currentFontChanged,
            this, &RibbonBar::fontChanged);
    addWidget(d->fontCombo);
    
    d->fontSizeSpin = new QSpinBox(this);
    d->fontSizeSpin->setRange(6, 72);
    d->fontSizeSpin->setValue(12);
    d->fontSizeSpin->setMaximumWidth(55);
    d->fontSizeSpin->setMinimumHeight(24);
    d->fontSizeSpin->setMaximumHeight(24);
    d->fontSizeSpin->setButtonSymbols(QSpinBox::UpDownArrows);
    connect(d->fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &RibbonBar::fontSizeChanged);
    addWidget(d->fontSizeSpin);
    
    QFrame *separator1 = new QFrame(this);
    separator1->setFrameShape(QFrame::VLine);
    separator1->setFrameShadow(QFrame::Sunken);
    addWidget(separator1);
    
    d->boldAction = new QAction(tr("B"), this);
    d->boldAction->setCheckable(true);
    connect(d->boldAction, &QAction::toggled,
            this, &RibbonBar::boldChanged);
    addAction(d->boldAction);
    
    d->italicAction = new QAction(tr("I"), this);
    d->italicAction->setCheckable(true);
    connect(d->italicAction, &QAction::toggled,
            this, &RibbonBar::italicChanged);
    addAction(d->italicAction);
    
    d->underlineAction = new QAction(tr("U"), this);
    d->underlineAction->setCheckable(true);
    connect(d->underlineAction, &QAction::toggled,
            this, &RibbonBar::underlineChanged);
    addAction(d->underlineAction);
    
    addGroup(tr("Paragraph"));
    
    d->alignLeftAction = new QAction(tr("Left"), this);
    d->alignLeftAction->setCheckable(true);
    connect(d->alignLeftAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignLeft); });
    addAction(d->alignLeftAction);
    
    d->alignCenterAction = new QAction(tr("Center"), this);
    d->alignCenterAction->setCheckable(true);
    connect(d->alignCenterAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignCenter); });
    addAction(d->alignCenterAction);
    
    d->alignRightAction = new QAction(tr("Right"), this);
    d->alignRightAction->setCheckable(true);
    connect(d->alignRightAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignRight); });
    addAction(d->alignRightAction);
    
    d->alignJustifyAction = new QAction(tr("Justify"), this);
    d->alignJustifyAction->setCheckable(true);
    connect(d->alignJustifyAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignJustify); });
    addAction(d->alignJustifyAction);
}

RibbonBar::~RibbonBar()
{
}

int RibbonBar::addTab(const QString &title)
{
    QWidget *tabWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(tabWidget);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    tabWidget->setLayout(layout);

    int index = QTabWidget::addTab(tabWidget, title);
    d->currentTab = index;
    return index;
}

void RibbonBar::addGroup(const QString &groupName)
{
    if (d->currentTab < 0 || d->currentTab >= count())
        return;

    QWidget *tabWidget = widget(d->currentTab);
    if (!tabWidget || !tabWidget->layout())
        return;

    QHBoxLayout *tabLayout = qobject_cast<QHBoxLayout*>(tabWidget->layout());
    if (!tabLayout)
        return;

    QWidget *groupWidget = new QWidget(tabWidget);
    QVBoxLayout *groupLayout = new QVBoxLayout(groupWidget);
    groupLayout->setContentsMargins(0, 0, 0, 0);
    groupLayout->setSpacing(3);
    groupLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QLabel *label = new QLabel(groupName, groupWidget);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setStyleSheet("font-weight: 600; color: #666; font-size: 10px; padding-left: 2px;");
    label->setMinimumHeight(16);
    label->setMaximumHeight(16);
    groupLayout->addWidget(label);

    QWidget *contentWidget = new QWidget(groupWidget);
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(3);
    contentLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    contentWidget->setLayout(contentLayout);
    groupLayout->addWidget(contentWidget);
    groupLayout->addStretch(1);

    tabLayout->addWidget(groupWidget);

    d->currentContentWidget = contentWidget;
    d->tabGroups[d->currentTab].append(groupName);
}

void RibbonBar::addAction(QAction *action)
{
    if (!d->currentContentWidget)
        return;

    QHBoxLayout *contentLayout = qobject_cast<QHBoxLayout*>(d->currentContentWidget->layout());
    if (!contentLayout)
        return;

    QToolButton *button = new QToolButton(d->currentContentWidget);
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setMinimumSize(32, 24);
    button->setMaximumSize(45, 24);
    button->setCheckable(action->isCheckable());
    button->setStyleSheet(R"(
        QToolButton {
            border: 1px solid transparent;
            border-radius: 2px;
            padding: 2px 6px;
            font-weight: bold;
            font-size: 12px;
        }
        QToolButton:hover {
            border: 1px solid #b8d6fb;
            background-color: #e5f3ff;
        }
        QToolButton:checked {
            border: 1px solid #7aa7e2;
            background-color: #cce8ff;
        }
        QToolButton:pressed {
            border: 1px solid #5c99d2;
            background-color: #b8d6fb;
        }
    )");
    contentLayout->addWidget(button);
}

void RibbonBar::addWidget(QWidget *widget)
{
    if (!d->currentContentWidget)
        return;

    QHBoxLayout *contentLayout = qobject_cast<QHBoxLayout*>(d->currentContentWidget->layout());
    if (!contentLayout)
        return;

    contentLayout->addWidget(widget);
}

void RibbonBar::updateFromSelection()
{
    qDebug() << "RibbonBar::updateFromSelection() placeholder";
}

} // namespace QtWordEditor
