#include "ui/ribbon/RibbonBar.h"
#include "core/styles/StyleManager.h"
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
#include <QComboBox>

namespace QtWordEditor {

class RibbonBarPrivate
{
public:
    StyleManager *styleManager = nullptr;
    int currentTab = -1;
    QHash<int, QStringList> tabGroups;
    QWidget *currentContentWidget = nullptr;
    
    QComboBox *paragraphStyleCombo = nullptr;
    QComboBox *characterStyleCombo = nullptr;
    QFontComboBox *fontCombo = nullptr;
    QSpinBox *fontSizeSpin = nullptr;
    QAction *boldAction = nullptr;
    QAction *italicAction = nullptr;
    QAction *underlineAction = nullptr;
    QAction *alignLeftAction = nullptr;
    QAction *alignCenterAction = nullptr;
    QAction *alignRightAction = nullptr;
    QAction *alignJustifyAction = nullptr;
    QAction *styleManagerAction = nullptr;
};

RibbonBar::RibbonBar(StyleManager *styleManager, QWidget *parent)
    : QTabWidget(parent)
    , d(new RibbonBarPrivate)
{
    d->styleManager = styleManager;
    
    setDocumentMode(true);
    setTabPosition(QTabWidget::North);
    
    int homeIndex = addTab(tr("开始"));
    
    // 样式组
    addGroup(tr("样式"));
    
    QLabel *paragraphStyleLabel = new QLabel(tr("段落:"), this);
    addWidget(paragraphStyleLabel);
    
    d->paragraphStyleCombo = new QComboBox(this);
    d->paragraphStyleCombo->setMaximumWidth(100);
    d->paragraphStyleCombo->setMinimumHeight(24);
    d->paragraphStyleCombo->setMaximumHeight(24);
    connect(d->paragraphStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (index >= 0) {
            emit paragraphStyleChanged(d->paragraphStyleCombo->itemText(index));
        }
    });
    addWidget(d->paragraphStyleCombo);
    
    QFrame *styleSeparator1 = new QFrame(this);
    styleSeparator1->setFrameShape(QFrame::VLine);
    styleSeparator1->setFrameShadow(QFrame::Sunken);
    addWidget(styleSeparator1);
    
    QLabel *characterStyleLabel = new QLabel(tr("字符:"), this);
    addWidget(characterStyleLabel);
    
    d->characterStyleCombo = new QComboBox(this);
    d->characterStyleCombo->setMaximumWidth(100);
    d->characterStyleCombo->setMinimumHeight(24);
    d->characterStyleCombo->setMaximumHeight(24);
    connect(d->characterStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (index >= 0) {
            emit characterStyleChanged(d->characterStyleCombo->itemText(index));
        }
    });
    addWidget(d->characterStyleCombo);
    
    QFrame *styleSeparator2 = new QFrame(this);
    styleSeparator2->setFrameShape(QFrame::VLine);
    styleSeparator2->setFrameShadow(QFrame::Sunken);
    addWidget(styleSeparator2);
    
    d->styleManagerAction = new QAction(tr("样式管理"), this);
    connect(d->styleManagerAction, &QAction::triggered,
            this, &RibbonBar::openStyleManagerRequested);
    addAction(d->styleManagerAction);
    
    // 字体组
    addGroup(tr("字体"));
    
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
    
    // 段落组
    addGroup(tr("段落"));
    
    d->alignLeftAction = new QAction(tr("左"), this);
    d->alignLeftAction->setCheckable(true);
    connect(d->alignLeftAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignLeft); });
    addAction(d->alignLeftAction);
    
    d->alignCenterAction = new QAction(tr("中"), this);
    d->alignCenterAction->setCheckable(true);
    connect(d->alignCenterAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignCenter); });
    addAction(d->alignCenterAction);
    
    d->alignRightAction = new QAction(tr("右"), this);
    d->alignRightAction->setCheckable(true);
    connect(d->alignRightAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignRight); });
    addAction(d->alignRightAction);
    
    d->alignJustifyAction = new QAction(tr("两"), this);
    d->alignJustifyAction->setCheckable(true);
    connect(d->alignJustifyAction, &QAction::toggled,
            this, [this](bool checked) { if (checked) emit alignmentChanged(Qt::AlignJustify); });
    addAction(d->alignJustifyAction);
    
    // 初始化样式列表
    refreshStyleLists();
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

void RibbonBar::updateFromSelection(const QString &characterStyleName,
                                    const QString &paragraphStyleName)
{
    // 更新段落样式下拉框
    int paraIndex = d->paragraphStyleCombo->findText(paragraphStyleName);
    if (paraIndex >= 0) {
        QSignalBlocker blocker(d->paragraphStyleCombo);
        d->paragraphStyleCombo->setCurrentIndex(paraIndex);
    } else {
        QSignalBlocker blocker(d->paragraphStyleCombo);
        d->paragraphStyleCombo->setCurrentIndex(-1);
    }
    
    // 更新字符样式下拉框
    int charIndex = d->characterStyleCombo->findText(characterStyleName);
    if (charIndex >= 0) {
        QSignalBlocker blocker(d->characterStyleCombo);
        d->characterStyleCombo->setCurrentIndex(charIndex);
    } else {
        QSignalBlocker blocker(d->characterStyleCombo);
        d->characterStyleCombo->setCurrentIndex(-1);
    }
}

void RibbonBar::updateFromSelection(const CharacterStyle &style)
{
    // 使用 QSignalBlocker 防止信号循环
    QSignalBlocker fontBlocker(d->fontCombo);
    QSignalBlocker fontSizeBlocker(d->fontSizeSpin);
    QSignalBlocker boldBlocker(d->boldAction);
    QSignalBlocker italicBlocker(d->italicAction);
    QSignalBlocker underlineBlocker(d->underlineAction);
    
    // 更新字体下拉框（同时检查字体族和字体大小属性）
    if (style.isPropertySet(CharacterStyleProperty::FontFamily) || 
        style.isPropertySet(CharacterStyleProperty::FontSize)) {
        QFont font = style.font();
        d->fontCombo->setCurrentFont(font);
    }
    
    // 更新字号下拉框
    if (style.isPropertySet(CharacterStyleProperty::FontSize)) {
        int fontSize = style.fontSize();
        d->fontSizeSpin->setValue(fontSize);
    }
    
    // 更新粗体按钮
    if (style.isPropertySet(CharacterStyleProperty::Bold)) {
        bool bold = style.bold();
        d->boldAction->setChecked(bold);
    }
    
    // 更新斜体按钮
    if (style.isPropertySet(CharacterStyleProperty::Italic)) {
        bool italic = style.italic();
        d->italicAction->setChecked(italic);
    }
    
    // 更新下划线按钮
    if (style.isPropertySet(CharacterStyleProperty::Underline)) {
        bool underline = style.underline();
        d->underlineAction->setChecked(underline);
    }
    
    // 注意：我们仍然调用旧的 updateFromSelection 来更新样式名称下拉框
    // 但由于我们现在没有样式名称，暂时不调用
}

void RibbonBar::refreshStyleLists()
{
    if (!d->styleManager)
        return;
    
    // 临时保存当前选中的样式
    QString currentParaStyle = d->paragraphStyleCombo->currentText();
    QString currentCharStyle = d->characterStyleCombo->currentText();
    
    // 更新段落样式列表
    QSignalBlocker paraBlocker(d->paragraphStyleCombo);
    d->paragraphStyleCombo->clear();
    
    QStringList paraStyles = d->styleManager->paragraphStyleNames();
    d->paragraphStyleCombo->addItems(paraStyles);
    
    // 恢复之前选中的样式
    int paraIndex = d->paragraphStyleCombo->findText(currentParaStyle);
    if (paraIndex >= 0) {
        d->paragraphStyleCombo->setCurrentIndex(paraIndex);
    }
    
    // 更新字符样式列表
    QSignalBlocker charBlocker(d->characterStyleCombo);
    d->characterStyleCombo->clear();
    
    QStringList charStyles = d->styleManager->characterStyleNames();
    d->characterStyleCombo->addItems(charStyles);
    
    // 恢复之前选中的样式
    int charIndex = d->characterStyleCombo->findText(currentCharStyle);
    if (charIndex >= 0) {
        d->characterStyleCombo->setCurrentIndex(charIndex);
    }
}

} // namespace QtWordEditor
