#ifndef STYLEMANAGERDIALOG_H
#define STYLEMANAGERDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include "core/styles/StyleManager.h"

namespace QtWordEditor {

class StyleManagerDialogPrivate;

/**
 * @brief 样式管理对话框类，用于管理命名样式
 *
 * 该对话框提供以下功能：
 * 1. 显示所有可用的字符样式和段落样式
 * 2. 添加新样式
 * 3. 编辑现有样式
 * 4. 删除样式
 * 5. 设置样式继承关系
 */
class StyleManagerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StyleManagerDialog(StyleManager *styleManager, QWidget *parent = nullptr);
    ~StyleManagerDialog() override;

private slots:
    void onCharacterStyleSelected();
    void onParagraphStyleSelected();
    void onAddCharacterStyle();
    void onAddParagraphStyle();
    void onEditCharacterStyle();
    void onEditParagraphStyle();
    void onDeleteCharacterStyle();
    void onDeleteParagraphStyle();
    void onCharacterStyleParentChanged();
    void onParagraphStyleParentChanged();

private:
    void refreshCharacterStyleList();
    void refreshParagraphStyleList();
    void refreshCharacterStyleParentCombo();
    void refreshParagraphStyleParentCombo();
    QScopedPointer<StyleManagerDialogPrivate> d;
};

} // namespace QtWordEditor

#endif // STYLEMANAGERDIALOG_H
