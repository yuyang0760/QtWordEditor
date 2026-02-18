#include "ui/dialogs/StyleManagerDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

namespace QtWordEditor {

class StyleManagerDialogPrivate
{
public:
    StyleManager *styleManager = nullptr;
    
    QTabWidget *tabWidget = nullptr;
    
    // 字符样式标签页
    QWidget *characterTab = nullptr;
    QListWidget *characterStyleList = nullptr;
    QPushButton *addCharacterBtn = nullptr;
    QPushButton *editCharacterBtn = nullptr;
    QPushButton *deleteCharacterBtn = nullptr;
    QComboBox *characterParentCombo = nullptr;
    
    // 段落样式标签页
    QWidget *paragraphTab = nullptr;
    QListWidget *paragraphStyleList = nullptr;
    QPushButton *addParagraphBtn = nullptr;
    QPushButton *editParagraphBtn = nullptr;
    QPushButton *deleteParagraphBtn = nullptr;
    QComboBox *paragraphParentCombo = nullptr;
};

StyleManagerDialog::StyleManagerDialog(StyleManager *styleManager, QWidget *parent)
    : QDialog(parent)
    , d(new StyleManagerDialogPrivate)
{
    d->styleManager = styleManager;
    
    setWindowTitle(tr("样式管理"));
    resize(600, 500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    d->tabWidget = new QTabWidget(this);
    
    // 创建字符样式标签页
    d->characterTab = new QWidget(this);
    QVBoxLayout *charLayout = new QVBoxLayout(d->characterTab);
    
    QGroupBox *charListGroup = new QGroupBox(tr("字符样式列表"), d->characterTab);
    QVBoxLayout *charListLayout = new QVBoxLayout(charListGroup);
    
    d->characterStyleList = new QListWidget(charListGroup);
    charListLayout->addWidget(d->characterStyleList);
    
    QHBoxLayout *charBtnLayout = new QHBoxLayout();
    d->addCharacterBtn = new QPushButton(tr("添加..."), charListGroup);
    d->editCharacterBtn = new QPushButton(tr("编辑..."), charListGroup);
    d->deleteCharacterBtn = new QPushButton(tr("删除"), charListGroup);
    charBtnLayout->addWidget(d->addCharacterBtn);
    charBtnLayout->addWidget(d->editCharacterBtn);
    charBtnLayout->addWidget(d->deleteCharacterBtn);
    charListLayout->addLayout(charBtnLayout);
    
    charLayout->addWidget(charListGroup);
    
    QGroupBox *charParentGroup = new QGroupBox(tr("继承设置"), d->characterTab);
    QVBoxLayout *charParentLayout = new QVBoxLayout(charParentGroup);
    
    QHBoxLayout *charParentComboLayout = new QHBoxLayout();
    charParentComboLayout->addWidget(new QLabel(tr("继承自:"), charParentGroup));
    d->characterParentCombo = new QComboBox(charParentGroup);
    d->characterParentCombo->setEditable(false);
    charParentComboLayout->addWidget(d->characterParentCombo);
    charParentLayout->addLayout(charParentComboLayout);
    
    charLayout->addWidget(charParentGroup);
    
    // 创建段落样式标签页
    d->paragraphTab = new QWidget(this);
    QVBoxLayout *paraLayout = new QVBoxLayout(d->paragraphTab);
    
    QGroupBox *paraListGroup = new QGroupBox(tr("段落样式列表"), d->paragraphTab);
    QVBoxLayout *paraListLayout = new QVBoxLayout(paraListGroup);
    
    d->paragraphStyleList = new QListWidget(paraListGroup);
    paraListLayout->addWidget(d->paragraphStyleList);
    
    QHBoxLayout *paraBtnLayout = new QHBoxLayout();
    d->addParagraphBtn = new QPushButton(tr("添加..."), paraListGroup);
    d->editParagraphBtn = new QPushButton(tr("编辑..."), paraListGroup);
    d->deleteParagraphBtn = new QPushButton(tr("删除"), paraListGroup);
    paraBtnLayout->addWidget(d->addParagraphBtn);
    paraBtnLayout->addWidget(d->editParagraphBtn);
    paraBtnLayout->addWidget(d->deleteParagraphBtn);
    paraListLayout->addLayout(paraBtnLayout);
    
    paraLayout->addWidget(paraListGroup);
    
    QGroupBox *paraParentGroup = new QGroupBox(tr("继承设置"), d->paragraphTab);
    QVBoxLayout *paraParentLayout = new QVBoxLayout(paraParentGroup);
    
    QHBoxLayout *paraParentComboLayout = new QHBoxLayout();
    paraParentComboLayout->addWidget(new QLabel(tr("继承自:"), paraParentGroup));
    d->paragraphParentCombo = new QComboBox(paraParentGroup);
    d->paragraphParentCombo->setEditable(false);
    paraParentComboLayout->addWidget(d->paragraphParentCombo);
    paraParentLayout->addLayout(paraParentComboLayout);
    
    paraLayout->addWidget(paraParentGroup);
    
    // 添加标签页
    d->tabWidget->addTab(d->characterTab, tr("字符样式"));
    d->tabWidget->addTab(d->paragraphTab, tr("段落样式"));
    
    mainLayout->addWidget(d->tabWidget);
    
    // 按钮栏
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *closeBtn = new QPushButton(tr("关闭"), this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeBtn);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
    
    // 初始化列表
    refreshCharacterStyleList();
    refreshParagraphStyleList();
    
    // 连接信号
    connect(d->characterStyleList, &QListWidget::itemSelectionChanged,
            this, &StyleManagerDialog::onCharacterStyleSelected);
    connect(d->paragraphStyleList, &QListWidget::itemSelectionChanged,
            this, &StyleManagerDialog::onParagraphStyleSelected);
    connect(d->addCharacterBtn, &QPushButton::clicked,
            this, &StyleManagerDialog::onAddCharacterStyle);
    connect(d->addParagraphBtn, &QPushButton::clicked,
            this, &StyleManagerDialog::onAddParagraphStyle);
    connect(d->editCharacterBtn, &QPushButton::clicked,
            this, &StyleManagerDialog::onEditCharacterStyle);
    connect(d->editParagraphBtn, &QPushButton::clicked,
            this, &StyleManagerDialog::onEditParagraphStyle);
    connect(d->deleteCharacterBtn, &QPushButton::clicked,
            this, &StyleManagerDialog::onDeleteCharacterStyle);
    connect(d->deleteParagraphBtn, &QPushButton::clicked,
            this, &StyleManagerDialog::onDeleteParagraphStyle);
    connect(d->characterParentCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StyleManagerDialog::onCharacterStyleParentChanged);
    connect(d->paragraphParentCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StyleManagerDialog::onParagraphStyleParentChanged);
}

StyleManagerDialog::~StyleManagerDialog()
{
}

void StyleManagerDialog::refreshCharacterStyleList()
{
    if (!d->styleManager)
        return;
    
    QString currentStyle;
    if (d->characterStyleList->currentItem()) {
        currentStyle = d->characterStyleList->currentItem()->text();
    }
    
    d->characterStyleList->clear();
    
    QStringList styles = d->styleManager->characterStyleNames();
    d->characterStyleList->addItems(styles);
    
    // 恢复选择
    if (!currentStyle.isEmpty()) {
        QList<QListWidgetItem*> items = d->characterStyleList->findItems(currentStyle, Qt::MatchExactly);
        if (!items.isEmpty()) {
            d->characterStyleList->setCurrentItem(items.first());
        }
    }
    
    // 更新父样式下拉框
    d->characterParentCombo->clear();
    d->characterParentCombo->addItem(QString());
    for (const QString &style : styles) {
        d->characterParentCombo->addItem(style);
    }
}

void StyleManagerDialog::refreshParagraphStyleList()
{
    if (!d->styleManager)
        return;
    
    QString currentStyle;
    if (d->paragraphStyleList->currentItem()) {
        currentStyle = d->paragraphStyleList->currentItem()->text();
    }
    
    d->paragraphStyleList->clear();
    
    QStringList styles = d->styleManager->paragraphStyleNames();
    d->paragraphStyleList->addItems(styles);
    
    // 恢复选择
    if (!currentStyle.isEmpty()) {
        QList<QListWidgetItem*> items = d->paragraphStyleList->findItems(currentStyle, Qt::MatchExactly);
        if (!items.isEmpty()) {
            d->paragraphStyleList->setCurrentItem(items.first());
        }
    }
    
    // 更新父样式下拉框
    d->paragraphParentCombo->clear();
    d->paragraphParentCombo->addItem(QString());
    for (const QString &style : styles) {
        d->paragraphParentCombo->addItem(style);
    }
}

void StyleManagerDialog::onCharacterStyleSelected()
{
    QListWidgetItem *item = d->characterStyleList->currentItem();
    bool hasSelection = (item != nullptr);
    
    d->editCharacterBtn->setEnabled(hasSelection);
    d->deleteCharacterBtn->setEnabled(hasSelection);
    
    if (hasSelection && d->styleManager) {
        QString styleName = item->text();
        QString parentName = d->styleManager->characterStyleParent(styleName);
        
        QSignalBlocker blocker(d->characterParentCombo);
        int index = d->characterParentCombo->findText(parentName);
        d->characterParentCombo->setCurrentIndex(index >= 0 ? index : 0);
    }
}

void StyleManagerDialog::onParagraphStyleSelected()
{
    QListWidgetItem *item = d->paragraphStyleList->currentItem();
    bool hasSelection = (item != nullptr);
    
    d->editParagraphBtn->setEnabled(hasSelection);
    d->deleteParagraphBtn->setEnabled(hasSelection);
    
    if (hasSelection && d->styleManager) {
        QString styleName = item->text();
        QString parentName = d->styleManager->paragraphStyleParent(styleName);
        
        QSignalBlocker blocker(d->paragraphParentCombo);
        int index = d->paragraphParentCombo->findText(parentName);
        d->paragraphParentCombo->setCurrentIndex(index >= 0 ? index : 0);
    }
}

void StyleManagerDialog::onAddCharacterStyle()
{
    if (!d->styleManager)
        return;
    
    bool ok;
    QString styleName = QInputDialog::getText(this, tr("添加字符样式"),
                                               tr("样式名称:"), QLineEdit::Normal,
                                               QString(), &ok);
    if (!ok || styleName.isEmpty())
        return;
    
    if (d->styleManager->hasCharacterStyle(styleName)) {
        QMessageBox::warning(this, tr("警告"),
                            tr("样式名称已存在，请使用其他名称。"));
        return;
    }
    
    CharacterStyle style;
    d->styleManager->addCharacterStyle(styleName, style);
    
    refreshCharacterStyleList();
}

void StyleManagerDialog::onAddParagraphStyle()
{
    if (!d->styleManager)
        return;
    
    bool ok;
    QString styleName = QInputDialog::getText(this, tr("添加段落样式"),
                                               tr("样式名称:"), QLineEdit::Normal,
                                               QString(), &ok);
    if (!ok || styleName.isEmpty())
        return;
    
    if (d->styleManager->hasParagraphStyle(styleName)) {
        QMessageBox::warning(this, tr("警告"),
                            tr("样式名称已存在，请使用其他名称。"));
        return;
    }
    
    ParagraphStyle style;
    d->styleManager->addParagraphStyle(styleName, style);
    
    refreshParagraphStyleList();
}

void StyleManagerDialog::onEditCharacterStyle()
{
    QListWidgetItem *item = d->characterStyleList->currentItem();
    if (!item)
        return;
    
    QString oldName = item->text();
    
    bool ok;
    QString newName = QInputDialog::getText(this, tr("编辑字符样式"),
                                             tr("样式名称:"), QLineEdit::Normal,
                                             oldName, &ok);
    if (!ok || newName.isEmpty())
        return;
    
    if (newName != oldName) {
        if (d->styleManager->hasCharacterStyle(newName)) {
            QMessageBox::warning(this, tr("警告"),
                                tr("样式名称已存在，请使用其他名称。"));
            return;
        }
        
        // 重命名样式
        CharacterStyle style = d->styleManager->characterStyle(oldName);
        QString parentName = d->styleManager->characterStyleParent(oldName);
        d->styleManager->removeCharacterStyle(oldName);
        d->styleManager->addCharacterStyle(newName, style);
        if (!parentName.isEmpty()) {
            d->styleManager->setCharacterStyleParent(newName, parentName);
        }
    }
    
    refreshCharacterStyleList();
}

void StyleManagerDialog::onEditParagraphStyle()
{
    QListWidgetItem *item = d->paragraphStyleList->currentItem();
    if (!item)
        return;
    
    QString oldName = item->text();
    
    bool ok;
    QString newName = QInputDialog::getText(this, tr("编辑段落样式"),
                                             tr("样式名称:"), QLineEdit::Normal,
                                             oldName, &ok);
    if (!ok || newName.isEmpty())
        return;
    
    if (newName != oldName) {
        if (d->styleManager->hasParagraphStyle(newName)) {
            QMessageBox::warning(this, tr("警告"),
                                tr("样式名称已存在，请使用其他名称。"));
            return;
        }
        
        // 重命名样式
        ParagraphStyle style = d->styleManager->paragraphStyle(oldName);
        QString parentName = d->styleManager->paragraphStyleParent(oldName);
        d->styleManager->removeParagraphStyle(oldName);
        d->styleManager->addParagraphStyle(newName, style);
        if (!parentName.isEmpty()) {
            d->styleManager->setParagraphStyleParent(newName, parentName);
        }
    }
    
    refreshParagraphStyleList();
}

void StyleManagerDialog::onDeleteCharacterStyle()
{
    QListWidgetItem *item = d->characterStyleList->currentItem();
    if (!item)
        return;
    
    QString styleName = item->text();
    
    if (styleName == QLatin1String("Default")) {
        QMessageBox::warning(this, tr("警告"),
                            tr("不能删除默认样式。"));
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("确认删除"),
                                                               tr("确定要删除字符样式 \"%1\" 吗？").arg(styleName),
                                                               QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;
    
    d->styleManager->removeCharacterStyle(styleName);
    refreshCharacterStyleList();
}

void StyleManagerDialog::onDeleteParagraphStyle()
{
    QListWidgetItem *item = d->paragraphStyleList->currentItem();
    if (!item)
        return;
    
    QString styleName = item->text();
    
    if (styleName == QLatin1String("Default")) {
        QMessageBox::warning(this, tr("警告"),
                            tr("不能删除默认样式。"));
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("确认删除"),
                                                               tr("确定要删除段落样式 \"%1\" 吗？").arg(styleName),
                                                               QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;
    
    d->styleManager->removeParagraphStyle(styleName);
    refreshParagraphStyleList();
}

void StyleManagerDialog::onCharacterStyleParentChanged()
{
    QListWidgetItem *item = d->characterStyleList->currentItem();
    if (!item || !d->styleManager)
        return;
    
    QString styleName = item->text();
    QString parentName = d->characterParentCombo->currentText();
    
    if (parentName == styleName) {
        // 不能继承自己
        QSignalBlocker blocker(d->characterParentCombo);
        d->characterParentCombo->setCurrentIndex(0);
        return;
    }
    
    d->styleManager->setCharacterStyleParent(styleName, parentName);
}

void StyleManagerDialog::onParagraphStyleParentChanged()
{
    QListWidgetItem *item = d->paragraphStyleList->currentItem();
    if (!item || !d->styleManager)
        return;
    
    QString styleName = item->text();
    QString parentName = d->paragraphParentCombo->currentText();
    
    if (parentName == styleName) {
        // 不能继承自己
        QSignalBlocker blocker(d->paragraphParentCombo);
        d->paragraphParentCombo->setCurrentIndex(0);
        return;
    }
    
    d->styleManager->setParagraphStyleParent(styleName, parentName);
}

} // namespace QtWordEditor
