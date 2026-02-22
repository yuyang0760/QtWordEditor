#include "ui/ribbon/RibbonBar.h"
#include "core/styles/StyleManager.h"
// 移除 Logger 头文件，使用 Qt 内置日志函数
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QAction>
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
    QComboBox *fontSizeCombo = nullptr;  // 改为 QComboBox
    QAction *boldAction = nullptr;
    QAction *italicAction = nullptr;
    QAction *underlineAction = nullptr;
    QAction *alignLeftAction = nullptr;
    QAction *alignCenterAction = nullptr;
    QAction *alignRightAction = nullptr;
    QAction *alignJustifyAction = nullptr;
    QAction *styleManagerAction = nullptr;
    
    // ========== 公式工具栏 ==========
    QAction *insertFormulaAction = nullptr;
    QAction *insertFractionAction = nullptr;
    QAction *insertNumberAction = nullptr;
    QAction *insertRadicalAction = nullptr;
    QAction *insertSubscriptAction = nullptr;
    QAction *insertSuperscriptAction = nullptr;
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
    
    d->fontSizeCombo = new QComboBox(this);
    d->fontSizeCombo->setMaximumWidth(55);
    d->fontSizeCombo->setMinimumHeight(24);
    d->fontSizeCombo->setMaximumHeight(24);
    d->fontSizeCombo->setEditable(true);
    
    // 添加常见字号选项
    QStringList fontSizeList = QStringList() 
        << "6" << "7" << "8" << "9" << "10" << "11" << "12" 
        << "14" << "16" << "18" << "20" << "22" << "24" << "26" 
        << "28" << "36" << "48" << "72";
    d->fontSizeCombo->addItems(fontSizeList);
    d->fontSizeCombo->setCurrentText("12");
    
    connect(d->fontSizeCombo, &QComboBox::currentTextChanged,
            this, [this](const QString &text) {
        bool ok = false;
        int size = text.toInt(&ok);
        if (ok && size > 0) {
            emit fontSizeChanged(size);
        }
    });
    addWidget(d->fontSizeCombo);
    
    QFrame *separator1 = new QFrame(this);
    separator1->setFrameShape(QFrame::VLine);
    separator1->setFrameShadow(QFrame::Sunken);
    addWidget(separator1);
    
    d->boldAction = new QAction(tr("B"), this);
    d->boldAction->setCheckable(true);
    // 使用 triggered 信号，不依赖按钮的 toggled 状态
    connect(d->boldAction, &QAction::triggered,
            this, [this]() {
                // 使用 QSignalBlocker 防止 setChecked 触发信号
                QSignalBlocker blocker(d->boldAction);
                // 发出信号，由 MainWindow 处理实际切换
                emit boldChanged(false);  // 参数值会被忽略
            });
    addAction(d->boldAction);
    
    d->italicAction = new QAction(tr("I"), this);
    d->italicAction->setCheckable(true);
    connect(d->italicAction, &QAction::triggered,
            this, [this]() {
                QSignalBlocker blocker(d->italicAction);
                emit italicChanged(false);  // 参数值会被忽略
            });
    addAction(d->italicAction);
    
    d->underlineAction = new QAction(tr("U"), this);
    d->underlineAction->setCheckable(true);
    connect(d->underlineAction, &QAction::triggered,
            this, [this]() {
                QSignalBlocker blocker(d->underlineAction);
                emit underlineChanged(false);  // 参数值会被忽略
            });
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
    
    // ========== 添加"插入"标签页 ==========
    int insertIndex = addTab(tr("插入"));
    
    // 公式组
    addGroup(tr("公式"));
    
    d->insertFormulaAction = new QAction(tr("π"), this);
    d->insertFormulaAction->setToolTip(tr("插入公式"));
    connect(d->insertFormulaAction, &QAction::triggered,
            this, &RibbonBar::insertFormulaRequested);
    addAction(d->insertFormulaAction);
    
    d->insertFractionAction = new QAction(tr("½"), this);
    d->insertFractionAction->setToolTip(tr("插入分数"));
    connect(d->insertFractionAction, &QAction::triggered,
            this, &RibbonBar::insertFractionRequested);
    addAction(d->insertFractionAction);
    
    d->insertNumberAction = new QAction(tr("123"), this);
    d->insertNumberAction->setToolTip(tr("插入公式文本"));
    connect(d->insertNumberAction, &QAction::triggered,
            this, &RibbonBar::insertNumberRequested);
    addAction(d->insertNumberAction);
    
    d->insertRadicalAction = new QAction(tr("√"), this);
    d->insertRadicalAction->setToolTip(tr("插入根号"));
    connect(d->insertRadicalAction, &QAction::triggered,
            this, &RibbonBar::insertRadicalRequested);
    addAction(d->insertRadicalAction);
    
    d->insertSubscriptAction = new QAction(tr("X₂"), this);
    d->insertSubscriptAction->setToolTip(tr("插入下标"));
    connect(d->insertSubscriptAction, &QAction::triggered,
            this, &RibbonBar::insertSubscriptRequested);
    addAction(d->insertSubscriptAction);
    
    d->insertSuperscriptAction = new QAction(tr("X²"), this);
    d->insertSuperscriptAction->setToolTip(tr("插入上标"));
    connect(d->insertSuperscriptAction, &QAction::triggered,
            this, &RibbonBar::insertSuperscriptRequested);
    addAction(d->insertSuperscriptAction);
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
    // 默认调用带一致性参数的版本，假设样式一致
    updateFromSelection(style, true);
}

void RibbonBar::updateFromSelection(const CharacterStyle &style, bool styleConsistent)
{
    // 使用 QSignalBlocker 防止信号循环
    QSignalBlocker fontBlocker(d->fontCombo);
    QSignalBlocker fontSizeBlocker(d->fontSizeCombo);
    QSignalBlocker boldBlocker(d->boldAction);
    QSignalBlocker italicBlocker(d->italicAction);
    QSignalBlocker underlineBlocker(d->underlineAction);
    
    if (styleConsistent) {
        // ========== 样式一致：正常显示样式 ==========
        // 更新字体下拉框
        QString fontFamily = style.fontFamily();
        
        // 通过字体族名称查找并设置（比直接用 setCurrentFont 更可靠）
        bool found = false;
        for (int i = 0; i < d->fontCombo->count(); ++i) {
            if (d->fontCombo->itemText(i) == fontFamily) {
                d->fontCombo->setCurrentIndex(i);
                found = true;
                break;
            }
        }
        
        // 如果找不到，尝试使用 setCurrentFont() 作为备选
        if (!found) {
            d->fontCombo->setCurrentFont(style.font());
        }
        
        // 更新字号下拉框
        int fontSize = style.fontSize();
        d->fontSizeCombo->setCurrentText(QString::number(fontSize));
        
        // 更新粗体按钮
        bool bold = style.bold();
        d->boldAction->setChecked(bold);
        
        // 更新斜体按钮
        bool italic = style.italic();
        d->italicAction->setChecked(italic);
        
        // 更新下划线按钮
        bool underline = style.underline();
        d->underlineAction->setChecked(underline);
    } else {
        // ========== 样式不一致：不显示样式（混合状态） ==========
        // 清空字体选择
        d->fontCombo->setCurrentIndex(-1);
        
        // 清空字号
        d->fontSizeCombo->setCurrentIndex(-1);
        
        // 取消按钮选中状态
        d->boldAction->setChecked(false);
        d->italicAction->setChecked(false);
        d->underlineAction->setChecked(false);
    }
    
    // 注意：我们仍然调用旧的 updateFromSelection 来更新样式名称下拉框
    // 但由于我们现在没有样式名称，暂时不调用
}

void RibbonBar::updateFromSelection(const CharacterStyle &style, const StyleConsistency &consistency)
{
    qDebug() << QString("RibbonBar::updateFromSelection - 各属性一致性：字体=%1, 字号=%2, 粗体=%3, 斜体=%4, 下划线=%5")
        .arg(consistency.fontFamilyConsistent)
        .arg(consistency.fontSizeConsistent)
        .arg(consistency.boldConsistent)
        .arg(consistency.italicConsistent)
        .arg(consistency.underlineConsistent);

    // 使用 QSignalBlocker 防止信号循环
    QSignalBlocker fontBlocker(d->fontCombo);
    QSignalBlocker fontSizeBlocker(d->fontSizeCombo);
    QSignalBlocker boldBlocker(d->boldAction);
    QSignalBlocker italicBlocker(d->italicAction);
    QSignalBlocker underlineBlocker(d->underlineAction);
    
    // ========== 更新字体 ==========
    if (consistency.fontFamilyConsistent) {
        QString fontFamily = consistency.consistentFontFamily;
        bool found = false;
        for (int i = 0; i < d->fontCombo->count(); ++i) {
            if (d->fontCombo->itemText(i) == fontFamily) {
                d->fontCombo->setCurrentIndex(i);
                found = true;
                break;
            }
        }
        if (!found) {
            QFont font;
            font.setFamily(fontFamily);
            d->fontCombo->setCurrentFont(font);
        }
        qDebug() << QString("  字体一致，设置字体:%1").arg(fontFamily);
    } else {
        d->fontCombo->setCurrentIndex(-1);
        qDebug() << "  字体不一致，清空";
    }
    
    // ========== 更新字号 ==========
    if (consistency.fontSizeConsistent) {
        int fontSize = consistency.consistentFontSize;
        d->fontSizeCombo->setCurrentText(QString::number(fontSize));
        qDebug() << QString("  字号一致，设置字号:%1").arg(fontSize);
    } else {
        d->fontSizeCombo->setCurrentIndex(-1);
        qDebug() << "  字号不一致，清空";
    }
    
    // ========== 更新粗体 ==========
    if (consistency.boldConsistent) {
        bool bold = consistency.consistentBold;
        d->boldAction->setChecked(bold);
        qDebug() << QString("  粗体一致，设置:%1").arg(bold);
    } else {
        d->boldAction->setChecked(false);
        qDebug() << "  粗体不一致，取消选中";
    }
    
    // ========== 更新斜体 ==========
    if (consistency.italicConsistent) {
        bool italic = consistency.consistentItalic;
        d->italicAction->setChecked(italic);
        qDebug() << QString("  斜体一致，设置:%1").arg(italic);
    } else {
        d->italicAction->setChecked(false);
        qDebug() << "  斜体不一致，取消选中";
    }
    
    // ========== 更新下划线 ==========
    if (consistency.underlineConsistent) {
        bool underline = consistency.consistentUnderline;
        d->underlineAction->setChecked(underline);
        qDebug() << QString("  下划线一致，设置:%1").arg(underline);
    } else {
        d->underlineAction->setChecked(false);
        qDebug() << "  下划线不一致，取消选中";
    }
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
