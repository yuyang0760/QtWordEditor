#include "ui/widgets/FormatToolBar.h"
#include "core/styles/StyleManager.h"
#include <QFontComboBox>
#include <QSpinBox>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDebug>

namespace QtWordEditor {

class FormatToolBarPrivate
{
public:
    StyleManager *styleManager = nullptr;
    
    QComboBox *characterStyleCombo = nullptr;
    QComboBox *paragraphStyleCombo = nullptr;
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

FormatToolBar::FormatToolBar(StyleManager *styleManager, QWidget *parent)
    : QToolBar(parent)
    , d(new FormatToolBarPrivate)
{
    d->styleManager = styleManager;
    
    setWindowTitle(tr("Format"));
    setIconSize(QSize(16, 16));

    // 段落样式下拉框
    QLabel *paragraphStyleLabel = new QLabel(tr("段落样式:"), this);
    addWidget(paragraphStyleLabel);
    
    d->paragraphStyleCombo = new QComboBox(this);
    d->paragraphStyleCombo->setMaximumWidth(120);
    d->paragraphStyleCombo->setEditable(false);
    addWidget(d->paragraphStyleCombo);
    
    addSeparator();
    
    // 字符样式下拉框
    QLabel *characterStyleLabel = new QLabel(tr("字符样式:"), this);
    addWidget(characterStyleLabel);
    
    d->characterStyleCombo = new QComboBox(this);
    d->characterStyleCombo->setMaximumWidth(120);
    d->characterStyleCombo->setEditable(false);
    addWidget(d->characterStyleCombo);
    
    addSeparator();

    // 字体选择下拉框
    d->fontCombo = new QFontComboBox(this);
    d->fontCombo->setMaximumWidth(150);
    addWidget(d->fontCombo);

    // 字号选择框
    d->fontSizeSpin = new QSpinBox(this);
    d->fontSizeSpin->setRange(6, 72);
    d->fontSizeSpin->setValue(12);
    d->fontSizeSpin->setMaximumWidth(60);
    addWidget(d->fontSizeSpin);

    addSeparator();

    // 粗体、斜体、下划线按钮
    d->boldAction = new QAction(tr("粗体"), this);
    d->boldAction->setCheckable(true);
    d->boldAction->setIcon(QIcon::fromTheme("format-text-bold"));
    addAction(d->boldAction);

    d->italicAction = new QAction(tr("斜体"), this);
    d->italicAction->setCheckable(true);
    d->italicAction->setIcon(QIcon::fromTheme("format-text-italic"));
    addAction(d->italicAction);

    d->underlineAction = new QAction(tr("下划线"), this);
    d->underlineAction->setCheckable(true);
    d->underlineAction->setIcon(QIcon::fromTheme("format-text-underline"));
    addAction(d->underlineAction);

    addSeparator();

    // 对齐方式按钮组
    QMenu *alignMenu = new QMenu(this);
    d->alignLeftAction = alignMenu->addAction(tr("左对齐"));
    d->alignLeftAction->setCheckable(true);
    d->alignLeftAction->setIcon(QIcon::fromTheme("format-justify-left"));
    d->alignCenterAction = alignMenu->addAction(tr("居中"));
    d->alignCenterAction->setCheckable(true);
    d->alignCenterAction->setIcon(QIcon::fromTheme("format-justify-center"));
    d->alignRightAction = alignMenu->addAction(tr("右对齐"));
    d->alignRightAction->setCheckable(true);
    d->alignRightAction->setIcon(QIcon::fromTheme("format-justify-right"));
    d->alignJustifyAction = alignMenu->addAction(tr("两端对齐"));
    d->alignJustifyAction->setCheckable(true);
    d->alignJustifyAction->setIcon(QIcon::fromTheme("format-justify-fill"));

    QToolButton *alignButton = new QToolButton(this);
    alignButton->setText(tr("对齐"));
    alignButton->setMenu(alignMenu);
    alignButton->setPopupMode(QToolButton::InstantPopup);
    addWidget(alignButton);
    
    addSeparator();
    
    // 样式管理按钮
    d->styleManagerAction = new QAction(tr("样式管理..."), this);
    d->styleManagerAction->setIcon(QIcon::fromTheme("configure"));
    addAction(d->styleManagerAction);

    // 连接信号
    connect(d->paragraphStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (index >= 0) {
            emit paragraphStyleChanged(d->paragraphStyleCombo->itemText(index));
        }
    });
    
    connect(d->characterStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (index >= 0) {
            emit characterStyleChanged(d->characterStyleCombo->itemText(index));
        }
    });
    
    connect(d->styleManagerAction, &QAction::triggered,
            this, &FormatToolBar::openStyleManagerRequested);
    
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
    
    // 初始化样式列表
    refreshStyleLists();
}

FormatToolBar::~FormatToolBar()
{
}

void FormatToolBar::updateFromSelection(const QString &characterStyleName,
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

void FormatToolBar::refreshStyleLists()
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